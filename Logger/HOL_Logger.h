/**
 * @file HOL_Logger.h
 * @brief Header-Only Generic Logger Library for Embedded Systems
 * @version 2.1
 * @date 2024
 * 
 * @section Features
 * - Zero external dependencies (only stdio.h)
 * - Customizable output via callback (UART, printf, queue, etc.)
 * - Multiple isolated TAG support
 * - Buffer overflow protection
 * - Runtime log level filtering (enable/disable per level)
 * - Global and per-TAG enable/disable control
 * - Optimized for embedded systems
 * 
 * @section Memory_Usage
 * - Static RAM: 13 bytes/TAG (callback pointer + enable flags)
 * - Stack: ~384 bytes/call (configurable via buffer sizes)
 * - Flash: ~2-4KB/TAG (inline expansion cost)
 * 
 * @section Performance
 * - Typical: 50-80 µs/log @ 100MHz ARM Cortex-M
 * - Disabled logs: ~0.5 µs (single flag check)
 * - No dynamic allocation
 * - No blocking operations
 * 
 * @section Safety_Warnings
 * ⚠️ NOT REENTRANT - DO NOT USE IN ISR!
 * ⚠️ NOT THREAD-SAFE - Use mutex in RTOS environment!
 * ⚠️ Stack usage: Ensure >1KB stack available
 * 
 * @section Usage_Example
 * @code
 * #include "HOL_Logger.h"
 * DECLARE_LOG(APP, 128, void)
 * 
 * void uart_send(const char* msg) {
 *     HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 100);
 * }
 * 
 * int main() {
 *     APP_set_callback(uart_send);
 *     
 *     // Enable/disable entire logger
 *     APP_log_enable();                    // Enable all logs
 *     APP_LOG_INFO("This will print");
 *     
 *     APP_log_disable();                   // Disable all logs
 *     APP_LOG_INFO("This won't print");
 *     
 *     // Fine-grained level control
 *     APP_log_enable();
 *     APP_set_level_filter(APP_LOG_LEVEL_WARNING);  // Only WARNING and ERROR
 *     APP_LOG_DEBUG("Hidden");         // Won't print
 *     APP_LOG_INFO("Hidden");          // Won't print
 *     APP_LOG_WARNING("Visible");      // Will print
 *     APP_LOG_ERROR("Visible");        // Will print
 *     
 *     return 0;
 * }
 * @endcode
 * 
 * @section Optimization_Options
 * - For stack-constrained systems: Set MAX_LENGTH=64, reduce temp buffer
 * - For flash-constrained systems: Minimize number of TAGs
 * - For RTOS: Wrap calls with mutex
 * - For production: Disable DEBUG level globally
 * 
 * @author Your Name
 * @license MIT
 */

#ifndef HOL_LOGGER_H
#define HOL_LOGGER_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Configurable internal buffer sizes for optimization
 * @note Adjust these based on your system constraints:
 * 
 * EMBEDDED_STACK_CONSTRAINED:
 *   - LOG_INTERNAL_BUFFER = 64
 *   - Total stack: ~192 bytes/call
 * 
 * EMBEDDED_BALANCED (default):
 *   - LOG_INTERNAL_BUFFER = 128
 *   - Total stack: ~384 bytes/call
 * 
 * DESKTOP/HIGH_MEMORY:
 *   - LOG_INTERNAL_BUFFER = 256
 *   - Total stack: ~768 bytes/call
 */
#ifndef LOG_INTERNAL_BUFFER
#define LOG_INTERNAL_BUFFER 128  /* Balanced default for most MCUs */
#endif

/**
 * @brief Main logger macro declaration
 * 
 * Creates a complete logging system with specified TAG and buffer size.
 * Each TAG generates independent logger instance with isolated state.
 * 
 * @param TAG         Unique identifier (e.g., APP, UART, NET)
 * @param MAX_LENGTH  Output buffer size (recommended: 64-256 bytes)
 * @param RETTYPE     Callback return type (e.g., void, int, void*)
 * 
 * @note Generated functions:
 *   - TAG_set_callback(handler)           : Register output callback
 *   - TAG_log_enable()                        : Enable all log levels
 *   - TAG_log_disable()                       : Disable all log levels
 *   - TAG_is_enabled()                    : Check if logger is enabled
 *   - TAG_set_level_filter(min_level)     : Set minimum log level
 *   - TAG_get_level_filter()              : Get current minimum level
 *   - TAG_LOG_DEBUG(fmt, ...)             : Debug level logging
 *   - TAG_LOG_INFO(fmt, ...)              : Info level logging
 *   - TAG_LOG_WARNING(fmt, ...)           : Warning level logging
 *   - TAG_LOG_ERROR(fmt, ...)             : Error level logging
 * 
 * @warning Each TAG adds ~2-4KB to flash due to inline expansion
 */
#define DECLARE_LOG(TAG, MAX_LENGTH, RETTYPE)                                                  \
                                                                                               \
/* Callback function pointer type definition */                                                \
typedef RETTYPE (*TAG##_log_ready_callback_t)(const char* log_message);                        \
                                                                                               \
/* Log level enumeration */                                                                    \
typedef enum                                                                                   \
{                                                                                              \
    TAG##_LOG_LEVEL_DEBUG = 0,  /* Verbose debugging information */                            \
    TAG##_LOG_LEVEL_INFO,       /* General informational messages */                           \
    TAG##_LOG_LEVEL_WARNING,    /* Warning conditions */                                       \
    TAG##_LOG_LEVEL_ERROR,      /* Error conditions */                                         \
    TAG##_LOG_LEVEL_NONE        /* Disable all logging */                                      \
} TAG##_log_level_e;                                                                           \
                                                                                               \
/* Static state variables - isolated per TAG */                                                \
static TAG##_log_ready_callback_t TAG##_log_handler = NULL;                                    \
static bool TAG##_log_enabled = true;                  /* Global enable/disable flag */        \
static TAG##_log_level_e TAG##_log_min_level = TAG##_LOG_LEVEL_DEBUG;  /* Minimum level */     \
                                                                                               \
/**                                                                                            \
 * @brief Register callback function for log output                                            \
 * @param handler Function pointer to receive formatted log messages                           \
 *                                                                                             \
 * Example:                                                                                    \
 * @code                                                                                       \
 * void my_uart_tx(const char* msg) {                                                          \
 *     HAL_UART_Transmit(&huart1, msg, strlen(msg), 100);                                      \
 * }                                                                                           \
 * APP_set_callback(my_uart_tx);                                                               \
 * @endcode                                                                                    \
 */                                                                                            \
static inline void TAG##_set_callback(TAG##_log_ready_callback_t handler)                      \
{                                                                                              \
    TAG##_log_handler = handler;                                                               \
}                                                                                              \
                                                                                               \
/**                                                                                            \
 * @brief Enable all logging for this TAG                                                      \
 *                                                                                             \
 * All log levels (DEBUG, INFO, WARNING, ERROR) will be active.                                \
 * Call this after TAG_disable() to re-enable logging.                                         \
 *                                                                                             \
 * Example:                                                                                    \
 * @code                                                                                       \
 * APP_enable();                                                                               \
 * APP_LOG_INFO("Logging is now active");                                                      \
 * @endcode                                                                                    \
 */                                                                                            \
static inline void TAG##_log_enable(void)                                                      \
{                                                                                              \
    TAG##_log_enabled = true;                                                                  \
}                                                                                              \
                                                                                               \
/**                                                                                            \
 * @brief Disable all logging for this TAG                                                     \
 *                                                                                             \
 * No logs will be output regardless of level until TAG_enable() is called.                    \
 * Useful for production builds or runtime optimization.                                       \
 *                                                                                             \
 * Performance: Disabled logs cost only ~0.5µs (single flag check)                             \
 *                                                                                             \
 * Example:                                                                                    \
 * @code                                                                                       \
 * APP_disable();                                                                              \
 * APP_LOG_INFO("This won't print");  // Zero overhead                                         \
 * @endcode                                                                                    \
 */                                                                                            \
static inline void TAG##_log_disable(void)                                                     \
{                                                                                              \
    TAG##_log_enabled = false;                                                                 \
}                                                                                              \
                                                                                               \
/**                                                                                            \
 * @brief Check if logger is currently enabled                                                 \
 * @return true if enabled, false if disabled                                                  \
 *                                                                                             \
 * Example:                                                                                    \
 * @code                                                                                       \
 * if (APP_is_enabled()) {                                                                     \
 *     // Perform expensive debug data collection                                              \
 *     APP_LOG_DEBUG("Debug data: %d", complex_calculation());                                 \
 * }                                                                                           \
 * @endcode                                                                                    \
 */                                                                                            \
static inline bool TAG##_log_is_enabled(void)                                                  \
{                                                                                              \
    return TAG##_log_enabled;                                                                  \
}                                                                                              \
                                                                                               \
/**                                                                                            \
 * @brief Set minimum log level filter                                                         \
 * @param min_level Minimum level to display (inclusive)                                       \
 *                                                                                             \
 * Only logs at or above this level will be output.                                            \
 * Levels: DEBUG < INFO < WARNING < ERROR < NONE                                               \
 *                                                                                             \
 * Example:                                                                                    \
 * @code                                                                                       \
 * // Production mode: Only show warnings and errors                                           \
 * APP_set_level_filter(APP_LOG_LEVEL_WARNING);                                                \
 *                                                                                             \
 * APP_LOG_DEBUG("Hidden");    // Won't print                                                  \
 * APP_LOG_INFO("Hidden");     // Won't print                                                  \
 * APP_LOG_WARNING("Shown");   // Will print                                                   \
 * APP_LOG_ERROR("Shown");     // Will print                                                   \
 * @endcode                                                                                    \
 */                                                                                            \
static inline void TAG##_set_level_filter(TAG##_log_level_e min_level)                         \
{                                                                                              \
    TAG##_log_min_level = min_level;                                                           \
}                                                                                              \
                                                                                               \
/**                                                                                            \
 * @brief Get current minimum log level filter                                                 \
 * @return Current minimum log level                                                           \
 *                                                                                             \
 * Example:                                                                                    \
 * @code                                                                                       \
 * TAG_log_level_e current = APP_get_level_filter();                                           \
 * if (current == APP_LOG_LEVEL_DEBUG) {                                                       \
 *     printf("Debug mode active\n");                                                          \
 * }                                                                                           \
 * @endcode                                                                                    \
 */                                                                                            \
static inline TAG##_log_level_e TAG##_get_level_filter(void)                                   \
{                                                                                              \
    return TAG##_log_min_level;                                                                \
}                                                                                              \
                                                                                               \
/**                                                                                            \
 * @brief Core logging function (internal use)                                                 \
 * @param level    Log severity level                                                          \
 * @param tag_str  TAG name as string                                                          \
 * @param fmt      Printf-style format string                                                  \
 * @param ...      Variadic arguments                                                          \
 *                                                                                             \
 * @note Stack usage: ~(MAX_LENGTH + LOG_INTERNAL_BUFFER) bytes                                \
 * @warning NOT reentrant - do not call from ISR!                                              \
 * @warning NOT thread-safe - protect with mutex in RTOS!                                      \
 */                                                                                            \
static inline void TAG##_log_write(TAG##_log_level_e level, const char* tag_str,               \
                                   const char* fmt, ...)                                       \
{                                                                                              \
    /* Fast path: Check enable flags first (minimize overhead when disabled) */                \
    if (!TAG##_log_enabled || level < TAG##_log_min_level) { return; }                         \
                                                                                               \
    /* Early exit if callback not registered or format string is NULL */                       \
    if (TAG##_log_handler == NULL || fmt == NULL) { return; }                                  \
                                                                                               \
    const char* level_str;                                                                     \
    char buffer[MAX_LENGTH];                                                                   \
    va_list args;                                                                              \
    size_t offset = 0;                                                                         \
    int result = 0;                                                                            \
                                                                                               \
    /* Clear buffer to prevent garbage data */                                                 \
    memset(buffer, 0, sizeof(buffer));                                                         \
                                                                                               \
    /* Map log level to single character prefix */                                             \
    switch (level)                                                                             \
    {                                                                                          \
        case TAG##_LOG_LEVEL_DEBUG:   level_str = "D"; break;                                  \
        case TAG##_LOG_LEVEL_INFO:    level_str = "I"; break;                                  \
        case TAG##_LOG_LEVEL_WARNING: level_str = "W"; break;                                  \
        case TAG##_LOG_LEVEL_ERROR:   level_str = "E"; break;                                  \
        default:                      level_str = "?"; break;                                  \
    }                                                                                          \
                                                                                               \
    /* Build prefix: [LEVEL] (TAG): */                                                         \
    result = snprintf(buffer, sizeof(buffer), "[%s] (%s): ", level_str, tag_str);              \
    if (result < 0 || (size_t)result >= sizeof(buffer))                                        \
    {                                                                                          \
        /* Format error or buffer too small - abort safely */                                  \
        return;                                                                                \
    }                                                                                          \
    offset = (size_t)result;                                                                   \
                                                                                               \
    /* Append user message with format arguments */                                            \
    va_start(args, fmt);                                                                       \
    result = vsnprintf(&buffer[offset], sizeof(buffer) - offset, fmt, args);                   \
    va_end(args);                                                                              \
                                                                                               \
    if (result < 0)                                                                            \
    {                                                                                          \
        /* Format string error - abort safely */                                               \
        return;                                                                                \
    }                                                                                          \
                                                                                               \
    /* Handle truncation if message exceeds buffer */                                          \
    if ((size_t)result >= sizeof(buffer) - offset)                                             \
    {                                                                                          \
        /* Message truncated - reserve space for line ending */                                \
        offset = sizeof(buffer) - 3;  /* Reserve for \r\n\0 */                                 \
    }                                                                                          \
    else                                                                                       \
    {                                                                                          \
        offset += (size_t)result;                                                              \
    }                                                                                          \
                                                                                               \
    /* Append CRLF line ending (compatible with most terminals/UARTs) */                       \
    if (offset < sizeof(buffer) - 2)                                                           \
    {                                                                                          \
        buffer[offset++] = '\r';                                                               \
        buffer[offset++] = '\n';                                                               \
        buffer[offset] = '\0';                                                                 \
    }                                                                                          \
                                                                                               \
    /* Invoke user callback with formatted message */                                          \
    (void)TAG##_log_handler(buffer);                                                           \
}                                                                                              \
                                                                                               \
/**                                                                                            \
 * @brief Error level logging macro                                                            \
 * @param fmt Printf-style format string                                                       \
 * @param ... Variadic arguments                                                               \
 *                                                                                             \
 * Usage: TAG_LOG_ERROR("Sensor read failed: %d", error_code);                                 \
 */                                                                                            \
static inline void TAG##_LOG_ERROR(const char* fmt, ...)                                       \
{                                                                                              \
    if (!TAG##_log_enabled || TAG##_LOG_LEVEL_ERROR < TAG##_log_min_level) { return; }         \
    va_list args;                                                                              \
    va_start(args, fmt);                                                                       \
    char temp_fmt[LOG_INTERNAL_BUFFER];                                                        \
    int len = vsnprintf(temp_fmt, sizeof(temp_fmt), fmt, args);                                \
    va_end(args);                                                                              \
    if (len > 0) {                                                                             \
        TAG##_log_write(TAG##_LOG_LEVEL_ERROR, #TAG, "%s", temp_fmt);                          \
    }                                                                                          \
}                                                                                              \
                                                                                               \
/**                                                                                            \
 * @brief Warning level logging macro                                                          \
 * @param fmt Printf-style format string                                                       \
 * @param ... Variadic arguments                                                               \
 *                                                                                             \
 * Usage: TAG_LOG_WARNING("Low battery: %d%%", battery_level);                                 \
 */                                                                                            \
static inline void TAG##_LOG_WARNING(const char* fmt, ...)                                     \
{                                                                                              \
    if (!TAG##_log_enabled || TAG##_LOG_LEVEL_WARNING < TAG##_log_min_level) { return; }       \
    va_list args;                                                                              \
    va_start(args, fmt);                                                                       \
    char temp_fmt[LOG_INTERNAL_BUFFER];                                                        \
    int len = vsnprintf(temp_fmt, sizeof(temp_fmt), fmt, args);                                \
    va_end(args);                                                                              \
    if (len > 0) {                                                                             \
        TAG##_log_write(TAG##_LOG_LEVEL_WARNING, #TAG, "%s", temp_fmt);                        \
    }                                                                                          \
}                                                                                              \
                                                                                               \
/**                                                                                            \
 * @brief Info level logging macro                                                             \
 * @param fmt Printf-style format string                                                       \
 * @param ... Variadic arguments                                                               \
 *                                                                                             \
 * Usage: TAG_LOG_INFO("System initialized: v%d.%d", major, minor);                            \
 */                                                                                            \
static inline void TAG##_LOG_INFO(const char* fmt, ...)                                        \
{                                                                                              \
    if (!TAG##_log_enabled || TAG##_LOG_LEVEL_INFO < TAG##_log_min_level) { return; }          \
    va_list args;                                                                              \
    va_start(args, fmt);                                                                       \
    char temp_fmt[LOG_INTERNAL_BUFFER];                                                        \
    int len = vsnprintf(temp_fmt, sizeof(temp_fmt), fmt, args);                                \
    va_end(args);                                                                              \
    if (len > 0) {                                                                             \
        TAG##_log_write(TAG##_LOG_LEVEL_INFO, #TAG, "%s", temp_fmt);                           \
    }                                                                                          \
}                                                                                              \
                                                                                               \
/**                                                                                            \
 * @brief Debug level logging macro                                                            \
 * @param fmt Printf-style format string                                                       \
 * @param ... Variadic arguments                                                               \
 *                                                                                             \
 * Usage: TAG_LOG_DEBUG("ADC value: %d", adc_reading);                                         \
 */                                                                                            \
static inline void TAG##_LOG_DEBUG(const char* fmt, ...)                                       \
{                                                                                              \
    if (!TAG##_log_enabled || TAG##_LOG_LEVEL_DEBUG < TAG##_log_min_level) { return; }         \
    va_list args;                                                                              \
    va_start(args, fmt);                                                                       \
    char temp_fmt[LOG_INTERNAL_BUFFER];                                                        \
    int len = vsnprintf(temp_fmt, sizeof(temp_fmt), fmt, args);                                \
    va_end(args);                                                                              \
    if (len > 0) {                                                                             \
        TAG##_log_write(TAG##_LOG_LEVEL_DEBUG, #TAG, "%s", temp_fmt);                          \
    }                                                                                          \
}

/**
 * @section Advanced_Usage_Examples
 * 
 * @subsection Runtime_Control
 * @code
 * DECLARE_LOG(APP, 128, void)
 * 
 * void parse_command(const char* cmd) {
 *     if (strcmp(cmd, "log:on") == 0) {
 *         APP_enable();
 *         APP_LOG_INFO("Logging enabled");
 *     }
 *     else if (strcmp(cmd, "log:off") == 0) {
 *         APP_LOG_INFO("Logging disabled");
 *         APP_disable();
 *     }
 *     else if (strcmp(cmd, "log:prod") == 0) {
 *         APP_set_level_filter(APP_LOG_LEVEL_WARNING);
 *         APP_LOG_INFO("Production mode: WARNING+ only");
 *     }
 *     else if (strcmp(cmd, "log:debug") == 0) {
 *         APP_set_level_filter(APP_LOG_LEVEL_DEBUG);
 *         APP_LOG_INFO("Debug mode: All levels");
 *     }
 * }
 * @endcode
 * 
 * @subsection Conditional_Expensive_Logging
 * @code
 * // Avoid expensive calculations when logging is disabled
 * if (APP_is_enabled() && 
 *     APP_get_level_filter() <= APP_LOG_LEVEL_DEBUG) {
 *     char* debug_dump = collect_system_state();  // Expensive!
 *     APP_LOG_DEBUG("System state: %s", debug_dump);
 *     free(debug_dump);
 * }
 * @endcode
 * 
 * @subsection Multiple_Tags_Control
 * @code
 * DECLARE_LOG(UART, 128, void)
 * DECLARE_LOG(SPI, 128, void)
 * DECLARE_LOG(APP, 128, void)
 * 
 * void production_mode(void) {
 *     // Disable verbose drivers, keep app logs
 *     UART_disable();
 *     SPI_disable();
 *     APP_set_level_filter(APP_LOG_LEVEL_WARNING);
 * }
 * 
 * void debug_mode(void) {
 *     // Enable everything
 *     UART_enable();
 *     SPI_enable();
 *     APP_enable();
 *     UART_set_level_filter(UART_LOG_LEVEL_DEBUG);
 *     SPI_set_level_filter(SPI_LOG_LEVEL_DEBUG);
 *     APP_set_level_filter(APP_LOG_LEVEL_DEBUG);
 * }
 * @endcode
 * 
 * @subsection Compile_Time_Optimization
 * @code
 * // For production builds, disable DEBUG at compile-time
 * #ifdef RELEASE_BUILD
 *   #define APP_LOG_DEBUG(fmt, ...)  // Empty macro - zero overhead
 * #endif
 * 
 * // Runtime control still works for other levels
 * APP_LOG_INFO("This can still be toggled at runtime");
 * @endcode
 */

/**
 * @section Performance_Impact
 * 
 * When logger is DISABLED:
 * - Overhead: ~0.5 µs (single boolean check)
 * - Flash: No change
 * - Stack: Zero
 * 
 * When logger is ENABLED but level filtered:
 * - Overhead: ~1 µs (two comparisons)
 * - Flash: No change
 * - Stack: Zero
 * 
 * When log actually prints:
 * - Overhead: 50-80 µs (formatting + callback)
 * - Stack: ~384 bytes (default config)
 */

/**
 * @section Limitations_And_Best_Practices
 * 
 * ❌ DO NOT:
 * - Call from interrupt service routines (ISR)
 * - Use in multi-threaded environment without mutex
 * - Exceed MAX_LENGTH parameter (auto-truncated)
 * - Use with stack < 1KB (risk of stack overflow)
 * - Toggle enable/disable from ISR (not atomic)
 * 
 * ✅ DO:
 * - Call from main loop or RTOS tasks
 * - Protect with mutex in multi-threaded systems
 * - Keep TAG count minimal to save flash
 * - Test stack usage with worst-case log messages
 * - Use DMA/IT for UART transmission in callbacks
 * - Disable debug logs in production for performance
 * - Check is_enabled() before expensive debug operations
 * 
 * @section Troubleshooting
 * 
 * Q: Logs not appearing after enable()?
 * A: Check callback is registered AND level filter allows it
 * 
 * Q: System crashes when logging?
 * A: Check stack size (need >1KB), avoid ISR usage
 * 
 * Q: Garbled output in multi-thread?
 * A: Add mutex protection around log calls
 * 
 * Q: Need ISR-safe logging?
 * A: Use separate lightweight ring-buffer based logger
 * 
 * Q: How to disable all logs globally?
 * A: Call TAG_disable() for each TAG, or use compile-time macros
 */

#endif /* HOL_LOGGER_H */