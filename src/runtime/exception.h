#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <setjmp.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief Maximum nesting depth for try-catch blocks
 */
#define DREAM_EXCEPTION_STACK_SIZE 32

/**
 * @brief Exception types supported by Dream
 */
typedef enum {
    DREAM_EXC_GENERIC,     /**< Generic exception */
    DREAM_EXC_RUNTIME,     /**< Runtime error */
    DREAM_EXC_NULL_PTR,    /**< Null pointer exception */
    DREAM_EXC_OUT_OF_BOUNDS, /**< Array out of bounds */
    DREAM_EXC_DIVIDE_BY_ZERO /**< Division by zero */
} DreamExceptionType;

/**
 * @brief Exception information structure
 */
typedef struct {
    DreamExceptionType type;  /**< Type of exception */
    char *message;           /**< Exception message */
    const char *file;        /**< Source file where exception occurred */
    int line;                /**< Line number where exception occurred */
} DreamException;

/**
 * @brief Exception context for one try-catch block
 */
typedef struct {
    jmp_buf jmp_buffer;     /**< setjmp buffer for exception handling */
    DreamException exc;     /**< Current exception information */
    int has_finally;        /**< Whether this context has a finally block */
    int finally_executed;   /**< Whether finally block has been executed */
    int in_catch;           /**< Whether currently executing catch block */
} DreamExceptionContext;

/**
 * @brief Global exception management state
 */
typedef struct {
    DreamExceptionContext stack[DREAM_EXCEPTION_STACK_SIZE];
    int top;                /**< Top of exception stack (-1 if empty) */
    DreamException current; /**< Current exception being handled */
} DreamExceptionState;

/**
 * @brief Global exception state
 */
extern DreamExceptionState dream_exception_state;

/**
 * @brief Initialize the exception handling system
 */
void dream_exception_init(void);

/**
 * @brief Push a new exception context onto the stack
 * @param has_finally Whether this try block has a finally clause
 * @return Pointer to the jmp_buf for setjmp, or NULL if stack overflow
 */
jmp_buf* dream_exception_push(int has_finally);

/**
 * @brief Pop the top exception context from the stack
 */
void dream_exception_pop(void);

/**
 * @brief Throw an exception with the given type and message
 * @param type Exception type
 * @param message Exception message (will be copied)
 * @param file Source file name
 * @param line Line number
 */
void dream_exception_throw(DreamExceptionType type, const char *message, 
                          const char *file, int line);

/**
 * @brief Get the current exception information
 * @return Pointer to current exception, or NULL if none
 */
DreamException* dream_exception_current(void);

/**
 * @brief Mark finally block as executed for current context
 */
void dream_exception_finally_executed(void);

/**
 * @brief Check if finally block should be executed for current context
 * @return 1 if finally should execute, 0 otherwise
 */
int dream_exception_should_execute_finally(void);

/**
 * @brief Create a string exception (convenience function)
 * @param message Exception message
 */
void dream_throw_string(const char *message);

/**
 * @brief Create a generic exception (convenience function)
 */
void dream_throw_generic(void);

/**
 * @brief Mark that we're currently in a catch block
 */
void dream_exception_enter_catch(void);

/**
 * @brief Mark that we're no longer in a catch block
 */
void dream_exception_exit_catch(void);

#endif /* EXCEPTION_H */