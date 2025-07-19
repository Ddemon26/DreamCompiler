#ifndef BACKEND_H
#define BACKEND_H

#include "../parser/ast.h"
#include "../ir/ir.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Backend abstraction for code generation
 * 
 * This interface allows different code generation backends
 * (C, assembly, LLVM IR, etc.) to be plugged into the compiler.
 */

typedef enum {
    BACKEND_C,          // C code generation
    BACKEND_ASM,        // Assembly generation (future)
    BACKEND_LLVM,       // LLVM IR generation (future)
    BACKEND_WASM        // WebAssembly generation (future)
} BackendType;

typedef struct BackendVTable BackendVTable;
typedef struct Backend Backend;

/**
 * @brief Virtual function table for backend operations
 */
struct BackendVTable {
    const char *name;
    
    // Initialization and cleanup
    int (*init)(Backend *backend, const char *output_path);
    void (*cleanup)(Backend *backend);
    
    // Module-level operations
    int (*begin_module)(Backend *backend, const char *module_name);
    int (*end_module)(Backend *backend);
    
    // Function operations
    int (*emit_function)(Backend *backend, Node *func_node);
    int (*emit_global)(Backend *backend, Node *global_node);
    int (*emit_type_decl)(Backend *backend, Node *type_node);
    
    // Expression and statement operations
    int (*emit_expression)(Backend *backend, Node *expr_node);
    int (*emit_statement)(Backend *backend, Node *stmt_node);
    
    // Linking and finalization
    int (*emit_main_wrapper)(Backend *backend);
    int (*finalize_output)(Backend *backend);
    
    // Multi-file support
    int (*emit_extern_decl)(Backend *backend, const char *name, const char *type);
    int (*emit_import)(Backend *backend, const char *module_name);
};

/**
 * @brief Backend instance
 */
struct Backend {
    const BackendVTable *vtable;
    BackendType type;
    FILE *output;
    char *output_path;
    void *context;  // Backend-specific context
};

/**
 * @brief Create a backend instance
 * @param type Backend type to create
 * @param output_path Path for output file
 * @return Pointer to backend instance, or NULL on failure
 */
Backend *backend_create(BackendType type, const char *output_path);

/**
 * @brief Destroy a backend instance
 * @param backend Backend to destroy
 */
void backend_destroy(Backend *backend);

/**
 * @brief Get the C backend implementation
 * @return Pointer to C backend vtable
 */
const BackendVTable *backend_get_c_vtable(void);

// Convenience macros for backend operations
#define BACKEND_CALL(backend, method, ...) \
    ((backend)->vtable->method ? (backend)->vtable->method((backend), ##__VA_ARGS__) : -1)

#define BACKEND_EMIT_FUNCTION(backend, node) BACKEND_CALL(backend, emit_function, node)
#define BACKEND_EMIT_GLOBAL(backend, node) BACKEND_CALL(backend, emit_global, node)
#define BACKEND_EMIT_TYPE(backend, node) BACKEND_CALL(backend, emit_type_decl, node)

#ifdef __cplusplus
}
#endif

#endif // BACKEND_H