
# 🧰 Queue Header-Only Library

A lightweight, dependency-free, **header-only embedded utility library** providing two core modules:

- **HOL_Queue.h** — Generic Circular Queue (Ring Buffer)
- **HOL_Logger.h** — Generic Logging System with Runtime Control

Both modules are designed for **embedded C environments**, optimized for **zero dynamic memory**, **O(1)** performance, and **cross-platform compatibility**.

---

## 📦 Project Structure

```

queue_header_only_library/
├── Queue/
│   └── HOL_Queue.h
│   └── README.md
├── Logger/
│   └── HOL_Logger.h
│   └── README.md
└── README.md   ← (this file)

```

---

## ⚙️ Overview

| Module | Description | Key Features |
| :------ | :----------- | :------------ |
| **HOL_Queue** | Generic circular buffer for embedded systems | O(1) ops, ISR safe, static memory, macro-generated API |
| **HOL_Logger** | Lightweight modular logger | Callback-based output, multi-tag, runtime filtering, stack-safe |

Both modules share the same philosophy: **header-only**, **no dynamic memory**, and **high efficiency** for embedded systems.

---

## 🚀 Quick Start

### Queue Example

```c
#include "HOL_Queue.h"

DECLARE_QUEUE(u8, 16)

queue_u8_16_t my_queue;

int main(void) {
    queue_initialize_u8_16(&my_queue);

    queue_push_u8_16(&my_queue, 0xAA);
    queue_push_no_overwrite_u8_16(&my_queue, 0xBB);

    u8 data;
    queue_pull_u8_16(&my_queue, &data);
}
````

---

### Logger Example

```c
#include "HOL_Logger.h"

DECLARE_LOG(APP, 128, void)

void uart_send(const char* msg) {
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 100);
}

int main(void) {
    APP_set_callback(uart_send);
    APP_log_enable();

    APP_LOG_INFO("System initialized");
    APP_LOG_WARNING("Low battery: %d%%", 15);
    APP_LOG_ERROR("Critical fault!");
}
```

---

## 🧩 HOL_Queue — Circular Queue

### Features

* **O(1)** performance for all operations
* **Static memory allocation** — no `malloc()` or `free()`
* **Compile-time type safety** via macros
* **Automatic overwrite policy** (oldest data discarded when full)
* **ISR-compatible** — safe with `volatile`, external sync required

### API Summary

| Function                              | Description                                 |
| :------------------------------------ | :------------------------------------------ |
| `queue_initialize_TYPE_SIZE()`        | Initialize queue                            |
| `queue_push_TYPE_SIZE()`              | Push element (overwrite if full)            |
| `queue_push_no_overwrite_TYPE_SIZE()` | Push element safely (returns error if full) |
| `queue_pull_TYPE_SIZE()`              | Pop oldest element                          |
| `queue_is_empty_TYPE_SIZE()`          | Check if empty                              |
| `queue_is_full_TYPE_SIZE()`           | Check if full                               |
| `queue_count_TYPE_SIZE()`             | Element count                               |
| `queue_clear_TYPE_SIZE()`             | Clear queue state                           |

---

## 🧠 HOL_Logger — Modular Logging System

### Features

* **Header-only, dependency-free**
* **Callback-based output** (UART, printf, DMA, etc.)
* **Multiple isolated TAGs**
* **Runtime level filtering**
* **Zero dynamic memory**

### Example Output Control

```c
APP_set_level_filter(APP_LOG_LEVEL_WARNING);
APP_LOG_DEBUG("Hidden");
APP_LOG_WARNING("Battery low!");
APP_LOG_ERROR("System halted!");
```

### Safety Notes

* ❌ Not reentrant — avoid use in ISR
* ❌ Not thread-safe — wrap with mutex for RTOS
* ⚙️ Stack ≥ 1 KB recommended

---

## 📊 Performance Summary

| Operation     | HOL_Queue       | HOL_Logger             |
| :------------ | :-------------- | :--------------------- |
| Insertion     | O(1)            | ~50–80 µs (active log) |
| Retrieval     | O(1)            | N/A                    |
| Memory Use    | Static          | Stack only             |
| ISR Safety    | Yes (with care) | No                     |
| Thread Safety | Requires lock   | Requires lock          |

---

## 🧪 Integration Tips

| Goal               | Suggestion                       |
| :----------------- | :------------------------------- |
| Reduce Flash       | Minimize number of TAGs / queues |
| RTOS Compatibility | Wrap logger with mutex           |
| Low Memory Targets | Reduce buffer sizes              |
| Production Builds  | Disable debug logs               |
| Performance        | Use callback-based async output  |

---

## 🪲 Troubleshooting

| Problem         | Cause              | Solution                  |
| :-------------- | :----------------- | :------------------------ |
| No log output   | Missing callback   | Call `TAG_set_callback()` |
| Stack overflow  | Buffer too large   | Reduce buffer length      |
| Garbled logs    | Race conditions    | Add mutex                 |
| Queue overwrite | Capacity full      | Use `push_no_overwrite()` |
| ISR crash       | Logger used in ISR | Move to main context      |

---


## 📚 References

* [HOL_Logger module documentation](Logger/README.md)
* [HOL_Queue module documentation](Queue/README.md)