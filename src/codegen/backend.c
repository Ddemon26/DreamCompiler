#include "backend.h"
#include "c_emit.h"
#include <stdlib.h>
#include <string.h>

// Forward declarations for C backend implementation
static int c_backend_init(Backend *backend, const char *output_path);
static void c_backend_cleanup(Backend *backend);
static int c_backend_begin_module(Backend *backend, const char *module_name);
static int c_backend_end_module(Backend *backend);
static int c_backend_emit_function(Backend *backend, Node *func_node);
static int c_backend_emit_global(Backend *backend, Node *global_node);
static int c_backend_emit_type_decl(Backend *backend, Node *type_node);
static int c_backend_emit_expression(Backend *backend, Node *expr_node);
static int c_backend_emit_statement(Backend *backend, Node *stmt_node);
static int c_backend_emit_main_wrapper(Backend *backend);
static int c_backend_finalize_output(Backend *backend);
static int c_backend_emit_extern_decl(Backend *backend, const char *name, const char *type);
static int c_backend_emit_import(Backend *backend, const char *module_name);

// C backend vtable
static const BackendVTable c_backend_vtable = {
    .name = "C",
    .init = c_backend_init,
    .cleanup = c_backend_cleanup,
    .begin_module = c_backend_begin_module,
    .end_module = c_backend_end_module,
    .emit_function = c_backend_emit_function,
    .emit_global = c_backend_emit_global,
    .emit_type_decl = c_backend_emit_type_decl,
    .emit_expression = c_backend_emit_expression,
    .emit_statement = c_backend_emit_statement,
    .emit_main_wrapper = c_backend_emit_main_wrapper,
    .finalize_output = c_backend_finalize_output,
    .emit_extern_decl = c_backend_emit_extern_decl,
    .emit_import = c_backend_emit_import,
};

Backend *backend_create(BackendType type, const char *output_path) {
    Backend *backend = malloc(sizeof(Backend));
    if (!backend) return NULL;
    
    backend->type = type;
    backend->output = NULL;
    backend->output_path = strdup(output_path);
    backend->context = NULL;
    
    switch (type) {
        case BACKEND_C:
            backend->vtable = &c_backend_vtable;
            break;
        default:
            free(backend->output_path);
            free(backend);
            return NULL;
    }
    
    if (backend->vtable->init && backend->vtable->init(backend, output_path) != 0) {
        free(backend->output_path);
        free(backend);
        return NULL;
    }
    
    return backend;
}

void backend_destroy(Backend *backend) {
    if (!backend) return;
    
    if (backend->vtable->cleanup) {
        backend->vtable->cleanup(backend);
    }
    
    if (backend->output && backend->output != stdout) {
        fclose(backend->output);
    }
    
    free(backend->output_path);
    free(backend);
}

const BackendVTable *backend_get_c_vtable(void) {
    return &c_backend_vtable;
}

// C Backend Implementation
static int c_backend_init(Backend *backend, const char *output_path) {
    if (strcmp(output_path, "-") == 0) {
        backend->output = stdout;
    } else {
        backend->output = fopen(output_path, "w");
        if (!backend->output) {
            return -1;
        }
    }
    
    // Initialize C output context
    COut *c_out = malloc(sizeof(COut));
    if (!c_out) return -1;
    
    c_out_init(c_out);
    backend->context = c_out;
    
    return 0;
}

static void c_backend_cleanup(Backend *backend) {
    if (backend->context) {
        COut *c_out = (COut *)backend->context;
        c_out_free(c_out);
        free(c_out);
        backend->context = NULL;
    }
}

static int c_backend_begin_module(Backend *backend, const char *module_name) {
    COut *c_out = (COut *)backend->context;
    if (!c_out) return -1;
    
    // Emit standard includes and module header
    c_out_write(c_out, "#ifndef DREAM_GENERATED\n");
    c_out_write(c_out, "#define DREAM_GENERATED\n");
    c_out_write(c_out, "#include <stdio.h>\n");
    c_out_write(c_out, "#include <string.h>\n");
    c_out_write(c_out, "#include <stdlib.h>\n");
    c_out_write(c_out, "#include <setjmp.h>\n");
    
    // Include runtime headers
    c_out_write(c_out, "#include \"../runtime/console.h\"\n");
    c_out_write(c_out, "#include \"../runtime/custom.h\"\n");
    c_out_write(c_out, "#include \"../runtime/memory.h\"\n");
    c_out_newline(c_out);
    
    // Module comment
    if (module_name) {
        c_out_write(c_out, "// Generated from Dream module: %s\n", module_name);
        c_out_newline(c_out);
    }
    
    return 0;
}

static int c_backend_end_module(Backend *backend) {
    COut *c_out = (COut *)backend->context;
    if (!c_out) return -1;
    
    c_out_write(c_out, "#endif /* DREAM_GENERATED */\n");
    c_out_dump(backend->output, c_out);
    
    return 0;
}

// Stub implementations for now - these would call into existing codegen functions
static int c_backend_emit_function(Backend *backend, Node *func_node) {
    // TODO: Integrate with existing function emission logic
    return 0;
}

static int c_backend_emit_global(Backend *backend, Node *global_node) {
    // TODO: Integrate with existing global emission logic
    return 0;
}

static int c_backend_emit_type_decl(Backend *backend, Node *type_node) {
    // TODO: Integrate with existing type declaration logic
    return 0;
}

static int c_backend_emit_expression(Backend *backend, Node *expr_node) {
    // TODO: Integrate with existing expression emission logic
    return 0;
}

static int c_backend_emit_statement(Backend *backend, Node *stmt_node) {
    // TODO: Integrate with existing statement emission logic
    return 0;
}

static int c_backend_emit_main_wrapper(Backend *backend) {
    COut *c_out = (COut *)backend->context;
    if (!c_out) return -1;
    
    // Emit main function wrapper with cleanup
    c_out_write(c_out, "int main(void) {\n");
    c_out_indent(c_out);
    c_out_write(c_out, "// Main program logic will be inserted here\n");
    c_out_write(c_out, "dr_release_all();\n");
    c_out_write(c_out, "return 0;\n");
    c_out_dedent(c_out);
    c_out_write(c_out, "}\n");
    
    return 0;
}

static int c_backend_finalize_output(Backend *backend) {
    // Flush any remaining output
    if (backend->output) {
        fflush(backend->output);
    }
    return 0;
}

static int c_backend_emit_extern_decl(Backend *backend, const char *name, const char *type) {
    COut *c_out = (COut *)backend->context;
    if (!c_out) return -1;
    
    c_out_write(c_out, "extern %s %s;\n", type, name);
    return 0;
}

static int c_backend_emit_import(Backend *backend, const char *module_name) {
    COut *c_out = (COut *)backend->context;
    if (!c_out) return -1;
    
    c_out_write(c_out, "#include \"%s.h\"\n", module_name);
    return 0;
}