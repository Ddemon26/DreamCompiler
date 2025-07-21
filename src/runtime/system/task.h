#ifndef TASK_H
#define TASK_H

#include "../../util/platform.h"
#include <stddef.h>

#ifdef _WIN32
#include <windows.h>
typedef HANDLE dr_thread_t;
typedef DWORD (WINAPI *dr_thread_func_t)(LPVOID);
#else
#include <pthread.h>
typedef pthread_t dr_thread_t;
typedef void* (*dr_thread_func_t)(void*);
#endif

/**
 * @brief Represents the state of an async task.
 */
typedef enum {
    TASK_PENDING,   /**< Task is running or waiting to run. */
    TASK_COMPLETED, /**< Task has completed successfully. */
    TASK_FAILED     /**< Task has failed with an error. */
} TaskState;

/**
 * @brief Represents a generic task result value.
 */
typedef union {
    int int_val;
    float float_val;
    char* string_val;
    void* ptr_val;
} TaskResult;

/**
 * @brief Represents an asynchronous task.
 */
typedef struct {
    dr_thread_t thread;     /**< Platform-specific thread handle. */
    TaskState state;        /**< Current state of the task. */
    TaskResult result;      /**< Result value of the task. */
    int has_result;         /**< Whether the task has a return value. */
    char* error_msg;        /**< Error message if task failed. */
} Task;

/**
 * @brief Thread function wrapper structure.
 */
typedef struct {
    void* (*func)(void*);   /**< Function to execute. */
    void* arg;              /**< Argument to pass to the function. */
    Task* task;             /**< Task structure to update. */
} ThreadContext;

/**
 * @brief Creates a new task and starts it running on a separate thread.
 * 
 * @param func Function to execute asynchronously.
 * @param arg Argument to pass to the function.
 * @return Pointer to the created Task structure.
 */
Task* dr_task_create(void* (*func)(void*), void* arg);

/**
 * @brief Waits for a task to complete and returns its result.
 * 
 * @param task Pointer to the task to wait for.
 * @return The result of the task.
 */
TaskResult dr_task_await(Task* task);

/**
 * @brief Checks if a task has completed without blocking.
 * 
 * @param task Pointer to the task to check.
 * @return 1 if the task is complete, 0 otherwise.
 */
int dr_task_is_complete(Task* task);

/**
 * @brief Cleans up resources associated with a task.
 * 
 * @param task Pointer to the task to clean up.
 */
void dr_task_cleanup(Task* task);

/**
 * @brief Sets an integer result for a task.
 * 
 * @param task Pointer to the task.
 * @param value Integer value to set as the result.
 */
void dr_task_set_int_result(Task* task, int value);

/**
 * @brief Sets a float result for a task.
 * 
 * @param task Pointer to the task.
 * @param value Float value to set as the result.
 */
void dr_task_set_float_result(Task* task, float value);

/**
 * @brief Sets a string result for a task.
 * 
 * @param task Pointer to the task.
 * @param value String value to set as the result.
 */
void dr_task_set_string_result(Task* task, const char* value);

/**
 * @brief Sets a pointer result for a task.
 * 
 * @param task Pointer to the task.
 * @param value Pointer value to set as the result.
 */
void dr_task_set_ptr_result(Task* task, void* value);

/**
 * @brief Sets a task as failed with an error message.
 * 
 * @param task Pointer to the task.
 * @param error_msg Error message describing the failure.
 */
void dr_task_set_error(Task* task, const char* error_msg);

#endif /* TASK_H */