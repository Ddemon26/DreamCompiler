#ifndef MODULE_H
#define MODULE_H

#include "../parser/ast.h"
#include "backend.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Module compilation context for multi-file support
 * 
 * This structure manages the compilation of multiple Dream modules
 * and their interdependencies.
 */

typedef struct Module Module;
typedef struct ModuleManager ModuleManager;

/**
 * @brief Represents a single Dream module
 */
struct Module {
    char *name;                    // Module name
    char *file_path;               // Source file path
    Node *ast;                     // Parsed AST
    char **imports;                // List of imported modules
    size_t import_count;           // Number of imports
    char **exports;                // List of exported symbols
    size_t export_count;           // Number of exports
    int compiled;                  // Compilation status
    Module *next;                  // Linked list for dependencies
};

/**
 * @brief Manages multiple modules and their compilation
 */
struct ModuleManager {
    Module *modules;               // Linked list of modules
    Backend *backend;              // Code generation backend
    char *output_dir;              // Output directory for generated files
    int module_count;              // Total number of modules
};

/**
 * @brief Create a new module manager
 * @param backend_type Type of backend to use
 * @param output_dir Directory for output files
 * @return Pointer to module manager, or NULL on failure
 */
ModuleManager *module_manager_create(BackendType backend_type, const char *output_dir);

/**
 * @brief Destroy a module manager and all its modules
 * @param manager Module manager to destroy
 */
void module_manager_destroy(ModuleManager *manager);

/**
 * @brief Add a module to the manager
 * @param manager Module manager
 * @param name Module name
 * @param file_path Path to source file
 * @param ast Parsed AST
 * @return Pointer to added module, or NULL on failure
 */
Module *module_manager_add_module(ModuleManager *manager, const char *name, 
                                  const char *file_path, Node *ast);

/**
 * @brief Find a module by name
 * @param manager Module manager
 * @param name Module name to find
 * @return Pointer to module, or NULL if not found
 */
Module *module_manager_find_module(ModuleManager *manager, const char *name);

/**
 * @brief Add an import dependency to a module
 * @param module Target module
 * @param import_name Name of module to import
 * @return 0 on success, -1 on failure
 */
int module_add_import(Module *module, const char *import_name);

/**
 * @brief Add an export to a module
 * @param module Target module
 * @param export_name Name of symbol to export
 * @return 0 on success, -1 on failure
 */
int module_add_export(Module *module, const char *export_name);

/**
 * @brief Compile all modules in dependency order
 * @param manager Module manager
 * @return 0 on success, -1 on failure
 */
int module_manager_compile_all(ModuleManager *manager);

/**
 * @brief Compile a single module
 * @param manager Module manager
 * @param module Module to compile
 * @return 0 on success, -1 on failure
 */
int module_compile(ModuleManager *manager, Module *module);

/**
 * @brief Generate header file for a module's exports
 * @param module Module to generate header for
 * @param output_path Path for header file
 * @return 0 on success, -1 on failure
 */
int module_generate_header(Module *module, const char *output_path);

/**
 * @brief Link compiled modules into final executable
 * @param manager Module manager
 * @param main_module Main module containing entry point
 * @param output_path Path for final executable
 * @return 0 on success, -1 on failure
 */
int module_manager_link(ModuleManager *manager, Module *main_module, const char *output_path);

/**
 * @brief Resolve dependencies and determine compilation order
 * @param manager Module manager
 * @param order Output array for compilation order
 * @param max_modules Maximum number of modules
 * @return Number of modules in order, or -1 on circular dependency
 */
int module_resolve_dependencies(ModuleManager *manager, Module **order, int max_modules);

#ifdef __cplusplus
}
#endif

#endif // MODULE_H