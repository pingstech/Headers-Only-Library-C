/**
 * @file HOL_Queue.h
 * @brief Generic ring buffer (circular queue) implementation for embedded systems
 * @note Thread-safe with volatile keywords, ISR-compatible
 * 
 * Features:
 * - Zero dynamic memory allocation
 * - Compile-time type safety
 * - Overwrite policy for full buffer
 * - O(1) operations
 */

#ifndef HOL_QUEUE_H
#define HOL_QUEUE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/**
 * @brief Queue status enumeration
 */
#define DECLARE_QUEUE_STATUS(TYPE, SIZE)                                                                   \
typedef enum {                                                                                             \
    QUEUE_##TYPE##_##SIZE##_OK = 0,                                                                        \
    QUEUE_##TYPE##_##SIZE##_ERROR_NULL_POINTER,                                                            \
    QUEUE_##TYPE##_##SIZE##_ERROR_EMPTY,                                                                   \
    QUEUE_##TYPE##_##SIZE##_ERROR_FULL,                                                                    \
    QUEUE_##TYPE##_##SIZE##_ERROR_INVALID_LENGTH                                                           \
} queue_##TYPE##_##SIZE##_status_e;

/**
 * @brief Main queue declaration macro
 * @param TYPE Data type (u8, u16, u32, float, custom struct, etc.)
 * @param SIZE Queue capacity (must be > 0)
 *
 * Usage Example:
 * DECLARE_QUEUE(u8, 16)
 * queue_u8_16_t my_queue;
 * queue_initialize_u8_16(&my_queue);                      // Initialization
 * queue_push_u8_16(&my_queue, 0xAA);                      // Push (Overwrite)
 * queue_push_no_overwrite_u8_16(&my_queue, 0xBB);         // Push (No Overwrite)
 * bool empty = queue_is_empty_u8_16(&my_queue);           // Check empty
 * bool full = queue_is_full_u8_16(&my_queue);             // Check full
 * size_t count = queue_count_u8_16(&my_queue);            // Get count
 * size_t space = queue_available_space_u8_16(&my_queue);  // Get available space
 * u8 data;
 * queue_peek_u8_16(&my_queue, &data);                     // Peek data
 * const u8* ptr = queue_peek_ptr_u8_16(&my_queue);        // Peek pointer
 * queue_pull_u8_16(&my_queue, &data);                     // Pull (Single)
 * u8 arr[5]; size_t read;
 * queue_pull_multiple_u8_16(&my_queue, arr, 5, &read);    // Pull (Multiple)
 * queue_clear_u8_16(&my_queue);                           // Clear
 */
#define DECLARE_QUEUE(TYPE, SIZE)                                                                          \
                                                                                                           \
DECLARE_QUEUE_STATUS(TYPE, SIZE)                                                                           \
                                                                                                           \
typedef struct {                                                                                           \
    TYPE buffer[SIZE];                                                                                     \
    volatile size_t write_index;                                                                           \
    volatile size_t read_index;                                                                            \
    volatile size_t count;                                                                                 \
} queue_##TYPE##_##SIZE##_t;                                                                               \
                                                                                                           \
static inline queue_##TYPE##_##SIZE##_status_e queue_initialize_##TYPE##_##SIZE(                           \
    queue_##TYPE##_##SIZE##_t* self)                                                                       \
{                                                                                                          \
    if(!self) {                                                                                            \
        return QUEUE_##TYPE##_##SIZE##_ERROR_NULL_POINTER;                                                 \
    }                                                                                                      \
                                                                                                           \
    self->write_index = 0;                                                                                 \
    self->read_index = 0;                                                                                  \
    self->count = 0;                                                                                       \
                                                                                                           \
    return QUEUE_##TYPE##_##SIZE##_OK;                                                                     \
}                                                                                                          \
                                                                                                           \
static inline bool queue_is_empty_##TYPE##_##SIZE(                                                         \
    const queue_##TYPE##_##SIZE##_t* self)                                                                 \
{                                                                                                          \
    return (self == NULL || self->count == 0);                                                             \
}                                                                                                          \
                                                                                                           \
static inline bool queue_is_full_##TYPE##_##SIZE(                                                          \
    const queue_##TYPE##_##SIZE##_t* self)                                                                 \
{                                                                                                          \
    return (self != NULL && self->count >= SIZE);                                                          \
}                                                                                                          \
                                                                                                           \
static inline size_t queue_count_##TYPE##_##SIZE(                                                          \
    const queue_##TYPE##_##SIZE##_t* self)                                                                 \
{                                                                                                          \
    if(!self) {                                                                                            \
        return 0;                                                                                          \
    }                                                                                                      \
                                                                                                           \
    return self->count;                                                                                    \
}                                                                                                          \
                                                                                                           \
static inline size_t queue_available_space_##TYPE##_##SIZE(                                                \
    const queue_##TYPE##_##SIZE##_t* self)                                                                 \
{                                                                                                          \
    if(!self) {                                                                                            \
        return 0;                                                                                          \
    }                                                                                                      \
                                                                                                           \
    return SIZE - self->count;                                                                             \
}                                                                                                          \
                                                                                                           \
static inline queue_##TYPE##_##SIZE##_status_e queue_push_##TYPE##_##SIZE(                                 \
    queue_##TYPE##_##SIZE##_t* self, TYPE data)                                                            \
{                                                                                                          \
    if(!self) {                                                                                            \
        return QUEUE_##TYPE##_##SIZE##_ERROR_NULL_POINTER;                                                 \
    }                                                                                                      \
                                                                                                           \
    /* Handle full buffer (overwrite oldest data) */                                                       \
    if(self->count >= SIZE) {                                                                              \
        self->read_index = (self->read_index + 1) % SIZE;                                                  \
    } else {                                                                                               \
        self->count++;                                                                                     \
    }                                                                                                      \
                                                                                                           \
    /* Write data */                                                                                       \
    self->buffer[self->write_index] = data;                                                                \
    self->write_index = (self->write_index + 1) % SIZE;                                                    \
                                                                                                           \
    return QUEUE_##TYPE##_##SIZE##_OK;                                                                     \
}                                                                                                          \
                                                                                                           \
static inline queue_##TYPE##_##SIZE##_status_e queue_push_no_overwrite_##TYPE##_##SIZE(                    \
    queue_##TYPE##_##SIZE##_t* self, TYPE data)                                                            \
{                                                                                                          \
    if(!self) {                                                                                            \
        return QUEUE_##TYPE##_##SIZE##_ERROR_NULL_POINTER;                                                 \
    }                                                                                                      \
                                                                                                           \
    if(self->count >= SIZE) {                                                                              \
        return QUEUE_##TYPE##_##SIZE##_ERROR_FULL;                                                         \
    }                                                                                                      \
                                                                                                           \
    self->buffer[self->write_index] = data;                                                                \
    self->write_index = (self->write_index + 1) % SIZE;                                                    \
    self->count++;                                                                                         \
                                                                                                           \
    return QUEUE_##TYPE##_##SIZE##_OK;                                                                     \
}                                                                                                          \
                                                                                                           \
static inline queue_##TYPE##_##SIZE##_status_e queue_pull_##TYPE##_##SIZE(                                 \
    queue_##TYPE##_##SIZE##_t* self, TYPE* data)                                                           \
{                                                                                                          \
    if(!self || !data) {                                                                                   \
        return QUEUE_##TYPE##_##SIZE##_ERROR_NULL_POINTER;                                                 \
    }                                                                                                      \
                                                                                                           \
    if(self->count == 0) {                                                                                 \
        return QUEUE_##TYPE##_##SIZE##_ERROR_EMPTY;                                                        \
    }                                                                                                      \
                                                                                                           \
    *data = self->buffer[self->read_index];                                                                \
    self->read_index = (self->read_index + 1) % SIZE;                                                      \
    self->count--;                                                                                         \
                                                                                                           \
    return QUEUE_##TYPE##_##SIZE##_OK;                                                                     \
}                                                                                                          \
                                                                                                           \
static inline queue_##TYPE##_##SIZE##_status_e queue_pull_multiple_##TYPE##_##SIZE(                        \
    queue_##TYPE##_##SIZE##_t* self, TYPE* data_out, size_t length, size_t* read_count)                    \
{                                                                                                          \
    if(!self || !data_out || length == 0) {                                                                \
        return QUEUE_##TYPE##_##SIZE##_ERROR_NULL_POINTER;                                                 \
    }                                                                                                      \
                                                                                                           \
    if(self->count == 0) {                                                                                 \
        if(read_count) *read_count = 0;                                                                    \
        return QUEUE_##TYPE##_##SIZE##_ERROR_EMPTY;                                                        \
    }                                                                                                      \
                                                                                                           \
    /* Limit read length to available data */                                                              \
    size_t actual_length = (length > self->count) ? self->count : length;                                  \
                                                                                                           \
    for(size_t i = 0; i < actual_length; i++) {                                                            \
        data_out[i] = self->buffer[self->read_index];                                                      \
        self->read_index = (self->read_index + 1) % SIZE;                                                  \
        self->count--;                                                                                     \
    }                                                                                                      \
                                                                                                           \
    if(read_count) {                                                                                       \
        *read_count = actual_length;                                                                       \
    }                                                                                                      \
                                                                                                           \
    return QUEUE_##TYPE##_##SIZE##_OK;                                                                     \
}                                                                                                          \
                                                                                                           \
static inline queue_##TYPE##_##SIZE##_status_e queue_peek_##TYPE##_##SIZE(                                 \
    const queue_##TYPE##_##SIZE##_t* self, TYPE* data)                                                     \
{                                                                                                          \
    if(!self || !data) {                                                                                   \
        return QUEUE_##TYPE##_##SIZE##_ERROR_NULL_POINTER;                                                 \
    }                                                                                                      \
                                                                                                           \
    if(self->count == 0) {                                                                                 \
        return QUEUE_##TYPE##_##SIZE##_ERROR_EMPTY;                                                        \
    }                                                                                                      \
                                                                                                           \
    *data = self->buffer[self->read_index];                                                                \
                                                                                                           \
    return QUEUE_##TYPE##_##SIZE##_OK;                                                                     \
}                                                                                                          \
                                                                                                           \
static inline const TYPE* queue_peek_ptr_##TYPE##_##SIZE(                                                  \
    const queue_##TYPE##_##SIZE##_t* self)                                                                 \
{                                                                                                          \
    if(!self || self->count == 0) {                                                                        \
        return NULL;                                                                                       \
    }                                                                                                      \
                                                                                                           \
    return &self->buffer[self->read_index];                                                                \
}                                                                                                          \
                                                                                                           \
static inline void queue_clear_##TYPE##_##SIZE(                                                            \
    queue_##TYPE##_##SIZE##_t* self)                                                                       \
{                                                                                                          \
    if(!self) {                                                                                            \
        return;                                                                                            \
    }                                                                                                      \
                                                                                                           \
    self->write_index = 0;                                                                                 \
    self->read_index = 0;                                                                                  \
    self->count = 0;                                                                                       \
}

/* ==================== STRING TYPE AND QUEUE WITH HELPERS ==================== */

/**
 * @brief Declare string type and queue with automatic helper functions
 * @param STRING_SIZE Maximum string length (including null terminator)
 * @param QUEUE_SIZE Queue capacity
 *
 * Usage Example:
 * DECLARE_STRING_QUEUE(32, 8)
 * queue_str_32_8_t my_string_queue;
 * char buffer[32];
 * queue_initialize_str_32_8(&my_string_queue);                                                 // Initialize queue
 * queue_push_with_string_support_32_8(&my_string_queue, "Message 1");                          // Push string helper
 * int success = queue_pull_with_string_support_32_8(&my_string_queue, buffer, sizeof(buffer)); // Pull string helper
 * size_t count = queue_count_str_32_8(&my_string_queue);                                       // Get count
 * queue_clear_str_32_8(&my_string_queue);                                                      // Clear
 */
#define DECLARE_STRING_QUEUE(STRING_SIZE, QUEUE_SIZE)                                                      \
                                                                                                           \
typedef struct {                                                                                           \
    char data[STRING_SIZE];                                                                                \
} str_##STRING_SIZE;                                                                                       \
                                                                                                           \
                                                                                                           \
DECLARE_QUEUE(str_##STRING_SIZE, QUEUE_SIZE)                                                               \
                                                                                                           \
static inline void queue_push_with_string_support_##STRING_SIZE##_##QUEUE_SIZE(                            \
    queue_str_##STRING_SIZE##_##QUEUE_SIZE##_t* queue, const char* str)                                    \
{                                                                                                          \
    if(!queue || !str) return;                                                                             \
                                                                                                           \
    str_##STRING_SIZE item;                                                                                \
    strncpy(item.data, str, STRING_SIZE - 1);                                                              \
    item.data[STRING_SIZE - 1] = '\0';                                                                     \
                                                                                                           \
    queue_push_str_##STRING_SIZE##_##QUEUE_SIZE(queue, item);                                              \
}                                                                                                          \
                                                                                                           \
static inline int queue_pull_with_string_support_##STRING_SIZE##_##QUEUE_SIZE(                             \
    queue_str_##STRING_SIZE##_##QUEUE_SIZE##_t* queue, char* output, size_t max_len)                       \
{                                                                                                          \
    if(!queue || !output || max_len == 0) return 0;                                                        \
                                                                                                           \
    str_##STRING_SIZE item;                                                                                \
    if(queue_pull_str_##STRING_SIZE##_##QUEUE_SIZE(queue, &item) ==                                        \
       QUEUE_str_##STRING_SIZE##_##QUEUE_SIZE##_OK) {                                                      \
        strncpy(output, item.data, max_len - 1);                                                           \
        output[max_len - 1] = '\0';                                                                        \
        return 1;                                                                                          \
    }                                                                                                      \
    return 0;                                                                                              \
}

/* ==================== HELPER MACROS ==================== */

/**
 * @brief Calculate memory usage of a queue
 *
 * Usage:
 * size_t size_bytes = QUEUE_MEMORY_BYTES(u16, 64);
 */
#define QUEUE_MEMORY_BYTES(TYPE, SIZE) (sizeof(TYPE) * (SIZE) + sizeof(size_t) * 3)

/**
 * @brief Declare and initialize a queue in one line
 *
 * Usage:
 * QUEUE_DECLARE_AND_INIT(u16, 64, sensor_queue)
 * This declares 'sensor_queue' of type u16, size 64, and initializes it.
 */
#define QUEUE_DECLARE_AND_INIT(TYPE, SIZE, name)                                                           \
    queue_##TYPE##_##SIZE##_t name;                                                                        \
    queue_initialize_##TYPE##_##SIZE(&name)

/* ==================== TYPE ALIASES ==================== */

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

#endif /* HOL_QUEUE_H */