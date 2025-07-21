#include "exception.h"
#include <stdio.h>

/**
 * @brief Global exception state
 */
DreamExceptionState dream_exception_state = {0};

void dream_exception_init(void) {
    dream_exception_state.top = -1;
    memset(&dream_exception_state.current, 0, sizeof(DreamException));
}

jmp_buf* dream_exception_push(int has_finally) {
    if (dream_exception_state.top >= DREAM_EXCEPTION_STACK_SIZE - 1) {
        fprintf(stderr, "Dream: Exception stack overflow\n");
        return NULL;
    }
    
    dream_exception_state.top++;
    DreamExceptionContext *ctx = &dream_exception_state.stack[dream_exception_state.top];
    
    memset(ctx, 0, sizeof(DreamExceptionContext));
    ctx->has_finally = has_finally;
    ctx->finally_executed = 0;
    ctx->in_catch = 0;
    
    return &ctx->jmp_buffer;
}

void dream_exception_pop(void) {
    if (dream_exception_state.top >= 0) {
        DreamExceptionContext *ctx = &dream_exception_state.stack[dream_exception_state.top];
        
        // Free exception message if allocated
        if (ctx->exc.message) {
            free(ctx->exc.message);
            ctx->exc.message = NULL;
        }
        
        dream_exception_state.top--;
    }
}

void dream_exception_throw(DreamExceptionType type, const char *message, 
                          const char *file, int line) {
    if (dream_exception_state.top < 0) {
        // No exception handler - terminate program
        fprintf(stderr, "Unhandled Dream exception: %s at %s:%d\n", 
                message ? message : "Unknown error", file ? file : "unknown", line);
        exit(1);
    }
    
    DreamExceptionContext *ctx = &dream_exception_state.stack[dream_exception_state.top];
    
    // If we're throwing from within a catch block, we need to pop this context
    // and throw to the outer context
    if (ctx->in_catch && dream_exception_state.top > 0) {
        // Save exception info before popping
        DreamExceptionType saved_type = type;
        char *saved_message = message ? strdup(message) : NULL;
        const char *saved_file = file;
        int saved_line = line;
        
        // Pop current context
        dream_exception_pop();
        
        // Throw to outer context
        if (dream_exception_state.top >= 0) {
            DreamExceptionContext *outer_ctx = &dream_exception_state.stack[dream_exception_state.top];
            
            // Free previous message if exists
            if (outer_ctx->exc.message) {
                free(outer_ctx->exc.message);
            }
            
            // Set exception information in outer context
            outer_ctx->exc.type = saved_type;
            outer_ctx->exc.message = saved_message;
            outer_ctx->exc.file = saved_file;
            outer_ctx->exc.line = saved_line;
            
            // Copy to current exception for easy access
            dream_exception_state.current = outer_ctx->exc;
            if (saved_message) {
                dream_exception_state.current.message = strdup(saved_message);
            }
            
            longjmp(outer_ctx->jmp_buffer, 1);
        }
        
        // If no outer context, cleanup and exit
        if (saved_message) free(saved_message);
        fprintf(stderr, "Unhandled Dream exception: %s at %s:%d\n", 
                message ? message : "Unknown error", file ? file : "unknown", line);
        exit(1);
    }
    
    // Free previous message if exists
    if (ctx->exc.message) {
        free(ctx->exc.message);
    }
    
    // Set exception information
    ctx->exc.type = type;
    ctx->exc.message = message ? strdup(message) : NULL;
    ctx->exc.file = file;
    ctx->exc.line = line;
    
    // Copy to current exception for easy access
    dream_exception_state.current = ctx->exc;
    if (message) {
        dream_exception_state.current.message = strdup(message);
    }
    
    // Jump to exception handler
    longjmp(ctx->jmp_buffer, 1);
}

DreamException* dream_exception_current(void) {
    if (dream_exception_state.top >= 0) {
        return &dream_exception_state.stack[dream_exception_state.top].exc;
    }
    return &dream_exception_state.current;
}

void dream_exception_finally_executed(void) {
    if (dream_exception_state.top >= 0) {
        dream_exception_state.stack[dream_exception_state.top].finally_executed = 1;
    }
}

int dream_exception_should_execute_finally(void) {
    if (dream_exception_state.top >= 0) {
        DreamExceptionContext *ctx = &dream_exception_state.stack[dream_exception_state.top];
        return ctx->has_finally && !ctx->finally_executed;
    }
    return 0;
}

void dream_throw_string(const char *message) {
    dream_exception_throw(DREAM_EXC_GENERIC, message, __FILE__, __LINE__);
}

void dream_throw_generic(void) {
    dream_exception_throw(DREAM_EXC_GENERIC, "An exception occurred", __FILE__, __LINE__);
}

void dream_exception_enter_catch(void) {
    if (dream_exception_state.top >= 0) {
        dream_exception_state.stack[dream_exception_state.top].in_catch = 1;
    }
}

void dream_exception_exit_catch(void) {
    if (dream_exception_state.top >= 0) {
        dream_exception_state.stack[dream_exception_state.top].in_catch = 0;
    }
}