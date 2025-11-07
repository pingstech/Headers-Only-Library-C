## 📘 README.md — HOL Logger (Header-Only Logger for Embedded Systems)

### 🌐 Language / Dil Seçimi

[🇺🇸 English](#-english-us) | [🇹🇷 Türkçe](#-türkçe)

---

## 🇺🇸 English (US)

# ⚙️ HOL Logger — Header-Only Generic Logger for Embedded Systems

A **header-only**, dependency-free, and ultra-lightweight logger designed for embedded systems.
Provides multi-tag isolated loggers with runtime level filtering and callback-based output.

---

## ✨ Features

* **Zero external dependencies:** Only uses `<stdio.h>`
* **Header-only design:** No `.c` file needed
* **Callback-based output:** UART, printf, DMA, or queue supported
* **Multiple isolated TAGs:** Each logger has its own configuration
* **Runtime control:** Enable/disable logs or filter by severity
* **Stack and flash optimized:** Configurable buffer sizes
* **Safe truncation:** Prevents buffer overflow
* **Zero dynamic memory:** No `malloc`, no blocking operations

---

## 🚀 Quick Start Example

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

    APP_set_level_filter(APP_LOG_LEVEL_WARNING);
    APP_LOG_DEBUG("Hidden");
    APP_LOG_WARNING("Low battery: %d%%", 15);
    APP_LOG_ERROR("Critical fault!");
}
```

---

## 🧩 API Overview

| Function / Macro                         | Description                                 |
| ---------------------------------------- | ------------------------------------------- |
| `DECLARE_LOG(TAG, SIZE, RETTYPE)`        | Creates independent logger instance         |
| `TAG_set_callback(func)`                 | Set output handler                          |
| `TAG_log_enable()` / `TAG_log_disable()` | Global on/off control                       |
| `TAG_set_level_filter(level)`            | Minimum level control                       |
| `TAG_LOG_DEBUG/INFO/WARNING/ERROR()`     | Logging macros with printf-style formatting |
| `TAG_is_enabled()`                       | Returns current enable state                |

---

## ⚙️ Configuration

| Mode                            | Buffer | Stack Usage | Description            |
| ------------------------------- | ------ | ----------- | ---------------------- |
| `EMBEDDED_STACK_CONSTRAINED`    | 64     | ~192 B      | Low-memory targets     |
| `EMBEDDED_BALANCED` *(default)* | 128    | ~384 B      | Recommended for MCU    |
| `DESKTOP/HIGH_MEMORY`           | 256    | ~768 B      | For simulation/testing |

Override buffer via:

```c
#define LOG_INTERNAL_BUFFER 64
```

---

## ⚠️ Safety Notes

* ❌ **Not reentrant** — do not call from ISR
* ❌ **Not thread-safe** — wrap with mutex in RTOS
* ⚠️ **Stack usage:** >1 KB recommended
* ✅ Safe for main loop or task context

---

## 🧠 Optimization Tips

| Goal             | Suggestion                |
| ---------------- | ------------------------- |
| Reduce Flash     | Minimize number of TAGs   |
| Reduce Stack     | Use smaller MAX_LENGTH    |
| RTOS Use         | Protect with mutex        |
| Production Build | Disable DEBUG via macro   |
| Performance      | Disable logger at runtime |

---

## 📊 Performance

| Condition      | Time (100 MHz Cortex-M) | Stack  |
| -------------- | ----------------------- | ------ |
| Disabled       | ~0.5 µs                 | 0      |
| Level filtered | ~1 µs                   | 0      |
| Active log     | 50–80 µs                | ~384 B |

---

## 🧪 Example: Multiple TAGs

```c
DECLARE_LOG(UART, 128, void)
DECLARE_LOG(SPI, 128, void)
DECLARE_LOG(APP, 128, void)

void debug_mode(void) {
    UART_log_enable();
    SPI_log_enable();
    APP_log_enable();
    APP_set_level_filter(APP_LOG_LEVEL_DEBUG);
}
```

---

## 🪲 Troubleshooting

| Problem          | Cause               | Fix                          |
| ---------------- | ------------------- | ---------------------------- |
| Logs not printed | Callback missing    | Set via `TAG_set_callback()` |
| Stack overflow   | Buffer too large    | Reduce `MAX_LENGTH`          |
| Garbled output   | Multithread access  | Add mutex                    |
| Crash in ISR     | Logger not ISR-safe | Avoid ISR use                |

---

## 🇹🇷 Türkçe

# ⚙️ HOL Logger — Gömülü Sistemler için Header-Only Log Kütüphanesi

**Dış bağımlılığı olmayan**, tamamen başlık dosyasında tanımlı, çok hafif bir log kütüphanesidir.
Her modül için bağımsız log sistemi, seviye filtreleme ve geri çağrı (callback) tabanlı çıktı sağlar.

---

## ✨ Özellikler

* **Sıfır dış bağımlılık** – yalnızca `<stdio.h>` kullanır
* **Header-only yapı** – `.c` dosyası gerekmez
* **Callback tabanlı çıktı** – UART, DMA veya printf uyumlu
* **Çoklu TAG desteği** – her modül bağımsızdır
* **Çalışma zamanı kontrolü** – log seviyesini filtrele veya tümünü kapat
* **Yığın (stack) ve flash optimizasyonu**
* **Taşma koruması** – mesaj uzunluğu otomatik sınırlandırılır
* **Dinamik bellek kullanılmaz** – `malloc` yok, bloklama yok

---

## 🚀 Hızlı Başlangıç

```c
#include "HOL_Logger.h"
DECLARE_LOG(APP, 128, void)

void uart_send(const char* msg) {
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 100);
}

int main(void) {
    APP_set_callback(uart_send);

    APP_log_enable();
    APP_LOG_INFO("Sistem başlatıldı");

    APP_set_level_filter(APP_LOG_LEVEL_WARNING);
    APP_LOG_WARNING("Pil seviyesi düşük: %d%%", 15);
    APP_LOG_ERROR("Kritik hata!");
}
```

---

## ⚠️ Güvenlik Uyarıları

* ISR içinde kullanmayın (reentrant değildir)
* RTOS ortamında mutex ile koruyun
* Stack boyutu en az **1 KB** olmalıdır
* Üretim (release) modunda `DEBUG` seviyesini kapatın

---

## ⚙️ Performans

| Durum           | Süre (100 MHz MCU) | Stack  |
| --------------- | ------------------ | ------ |
| Logger kapalı   | ~0.5 µs            | 0      |
| Seviye filtreli | ~1 µs              | 0      |
| Aktif log       | 50–80 µs           | ~384 B |

---

## 🧠 Optimizasyon İpuçları

| Amaç            | Öneri                              |
| --------------- | ---------------------------------- |
| Flash tasarrufu | TAG sayısını azaltın               |
| Stack tasarrufu | MAX_LENGTH küçültün                |
| RTOS uyumu      | Mutex ekleyin                      |
| Üretim modu     | DEBUG loglarını devre dışı bırakın |

---

## 🧩 Sorun Giderme

| Sorun         | Sebep                  | Çözüm                        |
| ------------- | ---------------------- | ---------------------------- |
| Log yok       | Callback tanımlı değil | `TAG_set_callback()` çağırın |
| Stack taşması | Buffer büyük           | MAX_LENGTH küçültün          |
| Karışık çıktı | Paralel erişim         | Mutex kullanın               |

---
