#include "task.h"
#include "memory.h"
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
static DWORD WINAPI thread_wrapper(LPVOID param) {
    ThreadContext* ctx = (ThreadContext*)param;
    void* result = ctx->func(ctx->arg);
    
    // Update task state
    ctx->task->state = TASK_COMPLETED;
    if (result) {
        ctx->task->result.ptr_val = result;
        ctx->task->has_result = 1;
    }
    
    free(ctx);
    return 0;
}
#else
static void* thread_wrapper(void* param) {
    ThreadContext* ctx = (ThreadContext*)param;
    void* result = ctx->func(ctx->arg);
    
    // Update task state
    ctx->task->state = TASK_COMPLETED;
    if (result) {
        ctx->task->result.ptr_val = result;
        ctx->task->has_result = 1;
    }
    
    free(ctx);
    return NULL;
}
#endif

Task* dr_task_create(void* (*func)(void*), void* arg) {
    Task* task = (Task*)dr_alloc(sizeof(Task));
    if (!task) {
        return NULL;
    }
    
    task->state = TASK_PENDING;
    task->has_result = 0;
    task->error_msg = NULL;
    memset(&task->result, 0, sizeof(TaskResult));
    
    ThreadContext* ctx = (ThreadContext*)malloc(sizeof(ThreadContext));
    if (!ctx) {
        dr_release(task);
        return NULL;
    }
    
    ctx->func = func;
    ctx->arg = arg;
    ctx->task = task;
    
#ifdef _WIN32
    task->thread = CreateThread(NULL, 0, thread_wrapper, ctx, 0, NULL);
    if (task->thread == NULL) {
        free(ctx);
        dr_release(task);
        return NULL;
    }
#else
    int result = pthread_create(&task->thread, NULL, thread_wrapper, ctx);
    if (result != 0) {
        free(ctx);
        dr_release(task);
        return NULL;
    }
#endif

    return task;
}

TaskResult dr_task_await(Task* task) {
    if (!task) {
        TaskResult empty = {0};
        return empty;
    }
    
    if (task->state == TASK_PENDING) {
#ifdef _WIN32
        WaitForSingleObject(task->thread, INFINITE);
#else
        pthread_join(task->thread, NULL);
#endif
        task->state = TASK_COMPLETED;
    }
    
    return task->result;
}

int dr_task_is_complete(Task* task) {
    if (!task) {
        return 1;
    }
    
    if (task->state != TASK_PENDING) {
        return 1;
    }
    
#ifdef _WIN32
    DWORD result = WaitForSingleObject(task->thread, 0);
    if (result == WAIT_OBJECT_0) {
        task->state = TASK_COMPLETED;
        return 1;
    }
#else
    // On POSIX, we can't check without blocking, so we'll use a different approach
    // For now, just return the current state
#endif
    
    return 0;
}

void dr_task_cleanup(Task* task) {
    if (!task) {
        return;
    }
    
    // Make sure the task is complete before cleaning up
    if (task->state == TASK_PENDING) {
        dr_task_await(task);
    }
    
#ifdef _WIN32
    if (task->thread) {
        CloseHandle(task->thread);
    }
#else
    // pthread_t doesn't need explicit cleanup
#endif
    
    if (task->error_msg) {
        dr_release(task->error_msg);
    }
    
    dr_release(task);
}

void dr_task_set_int_result(Task* task, int value) {
    if (task) {
        task->result.int_val = value;
        task->has_result = 1;
        task->state = TASK_COMPLETED;
    }
}

void dr_task_set_float_result(Task* task, float value) {
    if (task) {
        task->result.float_val = value;
        task->has_result = 1;
        task->state = TASK_COMPLETED;
    }
}

void dr_task_set_string_result(Task* task, const char* value) {
    if (task && value) {
        size_t len = strlen(value);
        char* copy = (char*)dr_alloc(len + 1);
        if (copy) {
            strcpy(copy, value);
            task->result.string_val = copy;
            task->has_result = 1;
            task->state = TASK_COMPLETED;
        }
    }
}

void dr_task_set_ptr_result(Task* task, void* value) {
    if (task) {
        task->result.ptr_val = value;
        task->has_result = 1;
        task->state = TASK_COMPLETED;
    }
}

void dr_task_set_error(Task* task, const char* error_msg) {
    if (task && error_msg) {
        size_t len = strlen(error_msg);
        char* copy = (char*)dr_alloc(len + 1);
        if (copy) {
            strcpy(copy, error_msg);
            task->error_msg = copy;
            task->state = TASK_FAILED;
        }
    }
}