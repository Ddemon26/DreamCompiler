#include "modules.h"
#include "../parser/diagnostic.h"
#include "../parser/warnings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct SourceModule SourceModule;
typedef struct FunctionBinding {
  Node *node;
  Slice name;
  Slice lowered_name;
  bool exported;
} FunctionBinding;

struct SourceModule {
  const char *path;
  const char *source;
  Slice name;
  Node *root;
  SourceModule **imports;
  Slice *import_names;
  size_t import_count;
  FunctionBinding *functions;
  size_t function_count;
  size_t id;
  int state;
  bool entry;
  SourceModule *next;
};

typedef struct {
  Arena *arena;
  WarningConfig warnings;
  SourceModule *modules;
  size_t count;
  bool failed;
} Loader;

static bool same_name(Slice a, Slice b) {
  return a.len == b.len && (!a.len || memcmp(a.start, b.start, a.len) == 0);
}

static const char *copy_text(Loader *loader, const char *text) {
  size_t len = strlen(text);
  char *copy = arena_alloc(loader->arena, len + 1);
  memcpy(copy, text, len + 1);
  return copy;
}

static void module_error(Loader *loader, SourceModule *module, Node *node, const char *format, ...) {
  loader->failed = true;
  fprintf(stderr, "%s:%zu:%zu: error: ", module ? module->path : "module", node ? node->pos.line : 1, node ? node->pos.column : 1);
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputc('\n', stderr);
}

static const char *canonical_path(Loader *loader, const char *path) {
#ifdef _WIN32
  char *absolute = _fullpath(NULL, path, 0);
#else
  char *absolute = realpath(path, NULL);
#endif
  if (!absolute) return NULL;
  for (char *p = absolute; *p; ++p) if (*p == '\\') *p = '/';
  const char *result = copy_text(loader, absolute);
  free(absolute);
  return result;
}

static const char *read_source(Loader *loader, const char *path) {
  FILE *file = fopen(path, "rb");
  if (!file) return NULL;
  if (fseek(file, 0, SEEK_END) != 0) { fclose(file); return NULL; }
  long size = ftell(file);
  if (size < 0 || fseek(file, 0, SEEK_SET) != 0) { fclose(file); return NULL; }
  char *source = arena_alloc(loader->arena, (size_t)size + 2);
  size_t read = fread(source, 1, (size_t)size, file);
  bool failed = ferror(file) != 0;
  fclose(file);
  if (failed) return NULL;
  source[read] = '\0';
  char *input = source, *output = source;
  while (*input) {
    if (input[0] == '\r' && input[1] == '\n') ++input;
    *output++ = *input++;
  }
  *output = '\0';
  return source;
}

static FunctionBinding *find_function(SourceModule *module, Slice name) {
  for (size_t i = 0; i < module->function_count; ++i)
    if (same_name(module->functions[i].name, name)) return &module->functions[i];
  return NULL;
}

static SourceModule *find_import(SourceModule *module, Slice name) {
  for (size_t i = 0; i < module->import_count; ++i)
    if (same_name(module->import_names[i], name)) return module->imports[i];
  return NULL;
}

static SourceModule *load_module(Loader *loader, const char *path, Slice expected_name,
                                  SourceModule *importer, Node *import_node,
                                  Node *entry, const char *entry_source) {
  const char *absolute = canonical_path(loader, path);
  if (!absolute) {
    module_error(loader, importer, import_node, "cannot load module '%.*s' from %s", (int)expected_name.len, expected_name.start, path);
    return NULL;
  }
  for (SourceModule *known = loader->modules; known; known = known->next) {
    if (strcmp(known->path, absolute) == 0) {
      if (known->state == 1) module_error(loader, importer, import_node, "circular import involving '%.*s'", (int)known->name.len, known->name.start);
      return loader->failed ? NULL : known;
    }
  }
  SourceModule *module = arena_alloc(loader->arena, sizeof(SourceModule));
  module->path = absolute;
  module->id = loader->count++;
  module->entry = entry != NULL;
  module->state = 1;
  module->next = loader->modules;
  loader->modules = module;
  module->source = entry ? entry_source : read_source(loader, absolute);
  if (!module->source) {
    module_error(loader, importer, import_node, "cannot load module '%.*s' from %s", (int)expected_name.len, expected_name.start, path);
    return NULL;
  }
  if (entry) module->root = entry;
  else {
    Parser parser;
    parser_init(&parser, loader->arena, module->source);
    parser.warn_config = loader->warnings;
    module->root = parse_program(&parser);
    analyze_warnings(&parser, module->root);
    if (parser.diags.len) fprintf(stderr, "In module %s:\n", module->path);
    print_diagnostics(module->source, &parser.diags);
    bool failed = diagnostics_have_errors(&parser.diags);
    free(parser.types);
    free(parser.diags.data);
    if (failed) { loader->failed = true; return NULL; }
  }

  size_t count = module->root->as.block.len;
  module->functions = arena_alloc(loader->arena, count * sizeof(FunctionBinding));
  module->imports = arena_alloc(loader->arena, count * sizeof(SourceModule *));
  module->import_names = arena_alloc(loader->arena, count * sizeof(Slice));
  for (size_t i = 0; i < count; ++i) {
    Node *node = module->root->as.block.items[i];
    if (node->kind == ND_MODULE) {
      if (module->name.len) module_error(loader, module, node, "multiple module declarations");
      module->name = node->as.module.name;
    }
  }
  if (!module->entry && !same_name(module->name, expected_name)) {
    module_error(loader, module, import_node, "module declaration must match import '%.*s'", (int)expected_name.len, expected_name.start);
    return NULL;
  }
  for (SourceModule *known = module->next; known; known = known->next) {
    if (module->name.len && same_name(known->name, module->name)) {
      module_error(loader, module, NULL, "duplicate module '%.*s'", (int)module->name.len, module->name.start);
      return NULL;
    }
  }
  for (size_t i = 0; i < count; ++i) {
    Node *node = module->root->as.block.items[i];
    bool exported = node->kind == ND_EXPORT;
    Node *declaration = exported ? node->as.export.decl : node;
    if (exported && declaration->kind != ND_FUNC) {
      module_error(loader, module, node, "only functions can be exported in this module implementation");
      continue;
    }
    if (declaration->kind == ND_FUNC) {
      Slice name = declaration->as.func.name;
      if (find_function(module, name)) {
        module_error(loader, module, node, "duplicate function '%.*s'", (int)name.len, name.start);
        continue;
      }
      bool main_function = name.len == 4 && memcmp(name.start, "main", 4) == 0;
      if (!module->entry && main_function) {
        module_error(loader, module, node, "imported modules cannot define main");
        continue;
      }
      Slice lowered = name;
      if (!main_function) {
        int length = snprintf(NULL, 0, "__dream_module_%zu_%.*s", module->id, (int)name.len, name.start);
        char *symbol = arena_alloc(loader->arena, (size_t)length + 1);
        snprintf(symbol, (size_t)length + 1, "__dream_module_%zu_%.*s", module->id, (int)name.len, name.start);
        lowered = (Slice){symbol, (size_t)length};
      }
      module->functions[module->function_count++] = (FunctionBinding){declaration, name, lowered, exported};
    } else if (!module->entry && declaration->kind != ND_MODULE && declaration->kind != ND_IMPORT) {
      module_error(loader, module, node, "imported modules currently support function declarations only");
    }
  }
  if (loader->failed) return NULL;
  for (size_t i = 0; i < count; ++i) {
    Node *node = module->root->as.block.items[i];
    if (node->kind != ND_IMPORT) continue;
    Slice name = node->as.import.path;
    if (find_import(module, name)) continue;
    const char *slash = strrchr(module->path, '/');
    size_t directory_len = slash ? (size_t)(slash - module->path + 1) : 0;
    size_t path_len = directory_len + name.len + 3;
    char *dependency = arena_alloc(loader->arena, path_len + 1);
    memcpy(dependency, module->path, directory_len);
    memcpy(dependency + directory_len, name.start, name.len);
    memcpy(dependency + directory_len + name.len, ".dr", 4);
    SourceModule *imported = load_module(loader, dependency, name, module, node, NULL, NULL);
    if (!imported) return NULL;
    module->import_names[module->import_count] = name;
    module->imports[module->import_count++] = imported;
  }
  module->state = 2;
  return module;
}

static void rewrite_node(Loader *loader, SourceModule *module, Node *node) {
  if (!node || loader->failed) return;
  node->source_file = module->path;
  node->source_text = module->source;
#define WALK(child) rewrite_node(loader, module, (child))
  switch (node->kind) {
  case ND_CALL: {
    Node *callee = node->as.call.callee;
    FunctionBinding *binding = NULL;
    if (callee->kind == ND_FIELD && callee->as.field.object->kind == ND_IDENT) {
      Slice name = callee->as.field.object->as.ident;
      SourceModule *imported = find_import(module, name);
      if (imported) {
        binding = find_function(imported, callee->as.field.name);
        if (!binding || !binding->exported) {
          module_error(loader, module, node, "module '%.*s' has no exported function '%.*s'", (int)name.len, name.start, (int)callee->as.field.name.len, callee->as.field.name.start);
          return;
        }
      }
    } else if (callee->kind == ND_IDENT) binding = find_function(module, callee->as.ident);
    if (binding) {
      callee->kind = ND_IDENT;
      callee->as.ident = binding->lowered_name;
    }
    WALK(callee);
    for (size_t i = 0; i < node->as.call.len; ++i) WALK(node->as.call.args[i]);
    break;
  }
  case ND_FUNC:
    for (size_t i = 0; i < node->as.func.param_len; ++i) WALK(node->as.func.params[i]);
    WALK(node->as.func.body);
    break;
  case ND_VAR_DECL:
    if (find_import(module, node->as.var_decl.name) || find_function(module, node->as.var_decl.name))
      module_error(loader, module, node, "variable '%.*s' conflicts with a module function or import", (int)node->as.var_decl.name.len, node->as.var_decl.name.start);
    WALK(node->as.var_decl.init);
    break;
  case ND_FIELD:
    if (node->as.field.object->kind == ND_IDENT && find_import(module, node->as.field.object->as.ident))
      module_error(loader, module, node, "module members must be called as functions");
    WALK(node->as.field.object);
    break;
  case ND_BLOCK:
    for (size_t i = 0; i < node->as.block.len; ++i) WALK(node->as.block.items[i]);
    break;
  case ND_CLASS_DECL: case ND_STRUCT_DECL:
    for (size_t i = 0; i < node->as.type_decl.len; ++i) WALK(node->as.type_decl.members[i]);
    break;
  case ND_UNARY: case ND_POST_UNARY: WALK(node->as.unary.expr); break;
  case ND_BINOP: WALK(node->as.bin.lhs); WALK(node->as.bin.rhs); break;
  case ND_COND: WALK(node->as.cond.cond); WALK(node->as.cond.then_expr); WALK(node->as.cond.else_expr); break;
  case ND_INDEX: WALK(node->as.index.array); WALK(node->as.index.index); break;
  case ND_IF: WALK(node->as.if_stmt.cond); WALK(node->as.if_stmt.then_br); WALK(node->as.if_stmt.else_br); break;
  case ND_WHILE: WALK(node->as.while_stmt.cond); WALK(node->as.while_stmt.body); break;
  case ND_DO_WHILE: WALK(node->as.do_while_stmt.body); WALK(node->as.do_while_stmt.cond); break;
  case ND_FOR: WALK(node->as.for_stmt.init); WALK(node->as.for_stmt.cond); WALK(node->as.for_stmt.update); WALK(node->as.for_stmt.body); break;
  case ND_RETURN: WALK(node->as.ret.expr); break;
  case ND_EXPR_STMT: WALK(node->as.expr_stmt.expr); break;
  case ND_CONSOLE_CALL: WALK(node->as.console.arg); break;
  case ND_SWITCH:
    WALK(node->as.switch_stmt.expr);
    for (size_t i = 0; i < node->as.switch_stmt.len; ++i) { WALK(node->as.switch_stmt.cases[i].value); WALK(node->as.switch_stmt.cases[i].body); }
    break;
  case ND_TRY: WALK(node->as.try_stmt.body); WALK(node->as.try_stmt.catch_body); WALK(node->as.try_stmt.finally_body); break;
  case ND_THROW: WALK(node->as.throw_stmt.expr); break;
  case ND_AWAIT: WALK(node->as.await_expr.expr); break;
  case ND_NEW:
    for (size_t i = 0; i < node->as.new_expr.arg_len; ++i) WALK(node->as.new_expr.args[i]);
    break;
  case ND_ENUM_DECL:
    for (size_t i = 0; i < node->as.enum_decl.len; ++i) WALK(node->as.enum_decl.members[i]);
    break;
  default: break;
  }
#undef WALK
}

bool modules_expand_program(Arena *arena, const char *entry_path, Node *entry,
                            const char *source, WarningConfig warnings, Node **expanded) {
  Loader loader = {.arena = arena, .warnings = warnings};
  SourceModule *main_module = load_module(&loader, entry_path, (Slice){0}, NULL, NULL, entry, source);
  if (!main_module || loader.failed) return false;
  size_t total = 0;
  for (SourceModule *module = loader.modules; module; module = module->next) {
    total += module->entry ? module->root->as.block.len : module->function_count;
    for (size_t i = 0; i < module->root->as.block.len; ++i) {
      Node *node = module->root->as.block.items[i];
      if (node->kind == ND_EXPORT) node = node->as.export.decl;
      rewrite_node(&loader, module, node);
    }
  }
  if (loader.failed) return false;
  Node *program = node_new(arena, ND_BLOCK);
  program->as.block.items = arena_alloc(arena, total * sizeof(Node *));
  for (SourceModule *module = loader.modules; module; module = module->next) {
    for (size_t i = 0; i < module->function_count; ++i)
      module->functions[i].node->as.func.name = module->functions[i].lowered_name;
    for (size_t i = 0; i < module->root->as.block.len; ++i) {
      Node *node = module->root->as.block.items[i];
      if (node->kind == ND_EXPORT) node = node->as.export.decl;
      if (node->kind == ND_MODULE || node->kind == ND_IMPORT) continue;
      program->as.block.items[program->as.block.len++] = node;
    }
  }
  *expanded = program;
  return true;
}
