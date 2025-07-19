#include "module.h"
#include "backend.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Forward declarations
static int dfs_visit(Module **modules, int index, int total_count, 
                    int *visited, int *in_stack, Module **order, int *count, int max_modules);
static int find_module_index(Module **modules, int count, const char *name);

ModuleManager *module_manager_create(BackendType backend_type, const char *output_dir) {
    ModuleManager *manager = malloc(sizeof(ModuleManager));
    if (!manager) return NULL;
    
    manager->modules = NULL;
    manager->backend = backend_create(backend_type, "-");
    manager->output_dir = strdup(output_dir);
    manager->module_count = 0;
    
    if (!manager->backend || !manager->output_dir) {
        module_manager_destroy(manager);
        return NULL;
    }
    
    return manager;
}

void module_manager_destroy(ModuleManager *manager) {
    if (!manager) return;
    
    Module *current = manager->modules;
    while (current) {
        Module *next = current->next;
        
        free(current->name);
        free(current->file_path);
        
        for (size_t i = 0; i < current->import_count; i++) {
            free(current->imports[i]);
        }
        free(current->imports);
        
        for (size_t i = 0; i < current->export_count; i++) {
            free(current->exports[i]);
        }
        free(current->exports);
        
        free(current);
        current = next;
    }
    
    if (manager->backend) {
        backend_destroy(manager->backend);
    }
    
    free(manager->output_dir);
    free(manager);
}

Module *module_manager_add_module(ModuleManager *manager, const char *name, 
                                  const char *file_path, Node *ast) {
    if (!manager || !name || !file_path || !ast) return NULL;
    
    Module *module = malloc(sizeof(Module));
    if (!module) return NULL;
    
    module->name = strdup(name);
    module->file_path = strdup(file_path);
    module->ast = ast;
    module->imports = NULL;
    module->import_count = 0;
    module->exports = NULL;
    module->export_count = 0;
    module->compiled = 0;
    module->next = manager->modules;
    
    if (!module->name || !module->file_path) {
        free(module->name);
        free(module->file_path);
        free(module);
        return NULL;
    }
    
    manager->modules = module;
    manager->module_count++;
    
    return module;
}

Module *module_manager_find_module(ModuleManager *manager, const char *name) {
    if (!manager || !name) return NULL;
    
    Module *current = manager->modules;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

int module_add_import(Module *module, const char *import_name) {
    if (!module || !import_name) return -1;
    
    // Resize imports array
    char **new_imports = realloc(module->imports, 
                                (module->import_count + 1) * sizeof(char*));
    if (!new_imports) return -1;
    
    module->imports = new_imports;
    module->imports[module->import_count] = strdup(import_name);
    
    if (!module->imports[module->import_count]) return -1;
    
    module->import_count++;
    return 0;
}

int module_add_export(Module *module, const char *export_name) {
    if (!module || !export_name) return -1;
    
    // Resize exports array
    char **new_exports = realloc(module->exports, 
                                (module->export_count + 1) * sizeof(char*));
    if (!new_exports) return -1;
    
    module->exports = new_exports;
    module->exports[module->export_count] = strdup(export_name);
    
    if (!module->exports[module->export_count]) return -1;
    
    module->export_count++;
    return 0;
}

int module_manager_compile_all(ModuleManager *manager) {
    if (!manager) return -1;
    
    // Resolve dependency order
    Module *order[256];  // Maximum 256 modules
    int count = module_resolve_dependencies(manager, order, 256);
    if (count < 0) {
        fprintf(stderr, "Error: Circular dependency detected\n");
        return -1;
    }
    
    // Compile modules in dependency order
    for (int i = 0; i < count; i++) {
        if (module_compile(manager, order[i]) != 0) {
            fprintf(stderr, "Error: Failed to compile module '%s'\n", order[i]->name);
            return -1;
        }
    }
    
    return 0;
}

int module_compile(ModuleManager *manager, Module *module) {
    if (!manager || !module || module->compiled) return 0;
    
    // Generate output file path
    char output_path[512];
    snprintf(output_path, sizeof(output_path), "%s/%s.c", 
             manager->output_dir, module->name);
    
    // Create backend for this module
    Backend *backend = backend_create(BACKEND_C, output_path);
    if (!backend) return -1;
    
    // Begin module compilation
    if (BACKEND_CALL(backend, begin_module, module->name) != 0) {
        backend_destroy(backend);
        return -1;
    }
    
    // Emit imports
    for (size_t i = 0; i < module->import_count; i++) {
        BACKEND_CALL(backend, emit_import, module->imports[i]);
    }
    
    // Compile AST (this would integrate with existing codegen)
    // For now, this is a placeholder
    BACKEND_CALL(backend, emit_function, module->ast);
    
    // End module compilation
    if (BACKEND_CALL(backend, end_module) != 0) {
        backend_destroy(backend);
        return -1;
    }
    
    backend_destroy(backend);
    
    // Generate header file for exports
    char header_path[512];
    snprintf(header_path, sizeof(header_path), "%s/%s.h", 
             manager->output_dir, module->name);
    
    if (module_generate_header(module, header_path) != 0) {
        return -1;
    }
    
    module->compiled = 1;
    return 0;
}

int module_generate_header(Module *module, const char *output_path) {
    if (!module || !output_path) return -1;
    
    FILE *f = fopen(output_path, "w");
    if (!f) return -1;
    
    // Generate header guard
    fprintf(f, "#ifndef %s_H\n", module->name);
    fprintf(f, "#define %s_H\n\n", module->name);
    
    fprintf(f, "#ifdef __cplusplus\n");
    fprintf(f, "extern \"C\" {\n");
    fprintf(f, "#endif\n\n");
    
    // Generate extern declarations for exports
    for (size_t i = 0; i < module->export_count; i++) {
        // This would analyze the AST to generate proper declarations
        fprintf(f, "extern void %s(void);  // TODO: Generate proper signature\n", 
                module->exports[i]);
    }
    
    fprintf(f, "\n#ifdef __cplusplus\n");
    fprintf(f, "}\n");
    fprintf(f, "#endif\n\n");
    
    fprintf(f, "#endif // %s_H\n", module->name);
    
    fclose(f);
    return 0;
}

int module_manager_link(ModuleManager *manager, Module *main_module, const char *output_path) {
    if (!manager || !main_module || !output_path) return -1;
    
    // This would invoke the linker to combine all compiled modules
    // For now, this is a placeholder that would call zig cc or gcc
    
    char link_cmd[2048];
    snprintf(link_cmd, sizeof(link_cmd), "zig cc -o %s ", output_path);
    
    // Add all compiled C files
    Module *current = manager->modules;
    while (current) {
        if (current->compiled) {
            char module_path[512];
            snprintf(module_path, sizeof(module_path), "%s/%s.c ", 
                     manager->output_dir, current->name);
            strcat(link_cmd, module_path);
        }
        current = current->next;
    }
    
    // Add runtime library
    strcat(link_cmd, "-L. -ldreamrt");
    
    // Execute link command
    return system(link_cmd);
}

int module_resolve_dependencies(ModuleManager *manager, Module **order, int max_modules) {
    if (!manager || !order) return -1;
    
    // Simple topological sort for dependency resolution
    int count = 0;
    int *visited = calloc(manager->module_count, sizeof(int));
    int *in_stack = calloc(manager->module_count, sizeof(int));
    
    if (!visited || !in_stack) {
        free(visited);
        free(in_stack);
        return -1;
    }
    
    // Convert linked list to array for easier processing
    Module **modules = malloc(manager->module_count * sizeof(Module*));
    if (!modules) {
        free(visited);
        free(in_stack);
        return -1;
    }
    
    Module *current = manager->modules;
    for (int i = 0; i < manager->module_count; i++) {
        modules[i] = current;
        current = current->next;
    }
    
    // Perform topological sort
    for (int i = 0; i < manager->module_count; i++) {
        if (!visited[i]) {
            if (dfs_visit(modules, i, manager->module_count, visited, in_stack, order, &count, max_modules) != 0) {
                free(visited);
                free(in_stack);
                free(modules);
                return -1; // Circular dependency
            }
        }
    }
    
    free(visited);
    free(in_stack);
    free(modules);
    
    return count;
}

// Helper function for DFS-based topological sort
static int dfs_visit(Module **modules, int index, int total_count, 
                    int *visited, int *in_stack, Module **order, int *count, int max_modules) {
    if (*count >= max_modules) return -1;
    
    visited[index] = 1;
    in_stack[index] = 1;
    
    Module *module = modules[index];
    
    // Visit dependencies (imported modules)
    for (size_t i = 0; i < module->import_count; i++) {
        int dep_index = find_module_index(modules, total_count, module->imports[i]);
        if (dep_index >= 0) {
            if (in_stack[dep_index]) {
                return -1; // Circular dependency
            }
            if (!visited[dep_index]) {
                if (dfs_visit(modules, dep_index, total_count, visited, in_stack, order, count, max_modules) != 0) {
                    return -1;
                }
            }
        }
    }
    
    in_stack[index] = 0;
    order[(*count)++] = module;
    
    return 0;
}

// Helper function to find module index by name
static int find_module_index(Module **modules, int count, const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(modules[i]->name, name) == 0) {
            return i;
        }
    }
    return -1;
}