## 📘 README.md — HOL Queue (Header-Only Queue for Embedded Systems)

# 🌐 Language / Dil Seçimi
[🇺🇸 English](#-english-us) | [🇹🇷 Türkçe](#-türkçe)

---

## 🇺🇸 English (US)

# ⚙️ HOL Queue — Generic Ring Buffer (Circular Queue) for Embedded Systems

A generic, zero-dynamic-memory ring buffer implementation for embedded systems.
Provides **O(1)** performance for all core operations and full compile-time type safety.

---

## ✨ Features

* **Zero Dynamic Memory Allocation:**
  All buffers and indices are statically defined. No `malloc` or `free` calls.

* **Compile-time Type Safety:**
  The `DECLARE_QUEUE(TYPE, SIZE)` macro generates unique structures and functions per data type and size.

* **O(1) Operations:**
  All operations execute in constant time regardless of queue size.

* **Automatic Overwrite Policy:**
  When the queue is full, the oldest data is automatically discarded.

* **ISR Compatible:**
  Volatile qualifiers allow safe access from ISRs.
  *(For multi-threaded use, external synchronization is still required.)*

---

## 🚀 Quick Start Example

```c
#include "HOL_Queue.h"

// 1. Declare a queue for uint8_t (u8) with capacity 16
DECLARE_QUEUE(u8, 16)

queue_u8_16_t my_queue;
queue_initialize_u8_16(&my_queue);

// 2. Push data (with overwrite policy)
queue_push_u8_16(&my_queue, 0xAA);

// 3. Push data (no-overwrite policy)
queue_push_no_overwrite_u8_16(&my_queue, 0xBB);

// 4. Check status
bool empty = queue_is_empty_u8_16(&my_queue);
bool full = queue_is_full_u8_16(&my_queue);
size_t count = queue_count_u8_16(&my_queue);
size_t free_space = queue_available_space_u8_16(&my_queue);

// 5. Pull data (FIFO)
u8 data;
queue_pull_u8_16(&my_queue, &data);

// 6. Pull multiple elements
u8 arr[5];
size_t read;
queue_pull_multiple_u8_16(&my_queue, arr, 5, &read);

// 7. Clear the queue
queue_clear_u8_16(&my_queue);
```

---

## 🧱 String Queue Example

```c
#include "HOL_Queue.h"

// Declare a string queue: 32-char strings, 8 elements capacity
DECLARE_STRING_QUEUE(32, 8)

queue_str_32_8_t log_queue;
char buffer[32];

// Initialize
queue_initialize_str_32_8(&log_queue);

// Push strings
queue_push_with_string_support_32_8(&log_queue, "System Start");
queue_push_with_string_support_32_8(&log_queue, "Error 42");

// Pull a string
queue_pull_with_string_support_32_8(&log_queue, buffer, sizeof(buffer));
// buffer -> "System Start"
```

---

## 🛠️ Macro Reference

| Macro                                      | Description                                | Generated Components                                        |
| :----------------------------------------- | :----------------------------------------- | :---------------------------------------------------------- |
| `DECLARE_QUEUE(TYPE, SIZE)`                | Declares queue struct and inline functions | `queue_TYPE_SIZE_t`, `queue_initialize_TYPE_SIZE`, etc.     |
| `DECLARE_QUEUE_STATUS(TYPE, SIZE)`         | Defines status enum                        | `QUEUE_TYPE_SIZE_OK`, `_ERROR_FULL`, `_ERROR_EMPTY`, etc.   |
| `DECLARE_STRING_QUEUE(STR_SIZE, Q_SIZE)`   | Declares string struct and queue helpers   | `str_STR_SIZE`, `queue_push_with_string_support_...`        |
| `QUEUE_MEMORY_BYTES(TYPE, SIZE)`           | Calculates memory usage                    | `sizeof(TYPE)*SIZE + sizeof(size_t)*3`                      |
| `QUEUE_DECLARE_AND_INIT(TYPE, SIZE, name)` | Declares and initializes a queue           | `queue_TYPE_SIZE_t name; queue_initialize_TYPE_SIZE(&name)` |

---

## ⚙️ Generated Function List

| Function                            | Description                          |
| :---------------------------------- | :----------------------------------- |
| `queue_initialize_TYPE_SIZE`        | Initializes the queue                |
| `queue_push_TYPE_SIZE`              | Push element (overwrite if full)     |
| `queue_push_no_overwrite_TYPE_SIZE` | Push element (returns error if full) |
| `queue_pull_TYPE_SIZE`              | Pop oldest element                   |
| `queue_pull_multiple_TYPE_SIZE`     | Pop multiple elements                |
| `queue_peek_TYPE_SIZE`              | Read oldest element without removing |
| `queue_peek_ptr_TYPE_SIZE`          | Get pointer to oldest element        |
| `queue_is_empty_TYPE_SIZE`          | Check if queue is empty              |
| `queue_is_full_TYPE_SIZE`           | Check if queue is full               |
| `queue_count_TYPE_SIZE`             | Return current count                 |
| `queue_available_space_TYPE_SIZE`   | Return available slots               |
| `queue_clear_TYPE_SIZE`             | Clear queue state                    |

---

## ⚠️ Concurrency Warning

While internal variables are marked as `volatile`, operations are **not atomic**.
For ISR or multi-thread safe usage, wrap operations with synchronization.

```c
void safe_push(queue_u8_16_t* q, u8 data) {
    uint32_t flags = disable_interrupts();
    queue_push_u8_16(q, data);
    restore_interrupts(flags);
}
```

---

## 📏 Memory Calculation

```c
size_t mem = QUEUE_MEMORY_BYTES(u16, 64);
// Result = sizeof(u16)*64 + sizeof(size_t)*3
```

---
## 🇹🇷 Türkçe


# ⚙️ HOL Queue — Generic Ring Buffer (Circular Queue) for Embedded Systems

Bu kütüphane, gömülü sistemlerde kullanılmak üzere tasarlanmış, dinamik bellek kullanmayan, **O(1)** karmaşıklıkta çalışan genel amaçlı bir **dairesel kuyruk (ring buffer)** implementasyonudur. Tüm işlemler makro temelli olarak derleme zamanında üretilir.

---

## ✨ Özellikler

* **Sıfır Dinamik Bellek Kullanımı:** Kuyruk yapısı ve tampon belleği tamamen statik olarak tanımlanır.
* **Derleme Zamanı Tip Güvenliği:** `DECLARE_QUEUE(TYPE, SIZE)` makrosu, tür ve kapasiteye göre benzersiz tip ve fonksiyon isimleri üretir.
* **O(1) İşlemler:** `push`, `pull`, `peek`, `count`, `is_empty`, `is_full` işlemleri sabit zamanda tamamlanır.
* **Otomatik Overwrite Politikası:** Tampon dolduğunda en eski veri otomatik olarak silinir.
* **ISR Uyumlu (Interrupt Safe):** `volatile` kullanımı ile ISR ortamlarında güvenli temel erişim.

  * Çok çekirdekli veya çok iş parçacıklı sistemlerde, kullanıcı dış kilitleme (mutex, interrupt disable vb.) eklemelidir.

---

## 🚀 Hızlı Başlangıç

```c
#include "HOL_Queue.h"

// 1. uint8_t tipi ve 16 elemanlık kuyruk oluştur
DECLARE_QUEUE(u8, 16)

queue_u8_16_t my_queue;
queue_initialize_u8_16(&my_queue);

// 2. Veri ekleme (overwrite)
queue_push_u8_16(&my_queue, 0xAA);

// 3. Veri ekleme (no-overwrite)
queue_push_no_overwrite_u8_16(&my_queue, 0xBB);

// 4. Kuyruk durumu kontrolü
bool empty = queue_is_empty_u8_16(&my_queue);
bool full = queue_is_full_u8_16(&my_queue);
size_t count = queue_count_u8_16(&my_queue);
size_t space = queue_available_space_u8_16(&my_queue);

// 5. Veri okuma
u8 data;
queue_pull_u8_16(&my_queue, &data);

// 6. Birden fazla veri çekme
u8 buffer[5];
size_t read;
queue_pull_multiple_u8_16(&my_queue, buffer, 5, &read);

// 7. Kuyruğu temizleme
queue_clear_u8_16(&my_queue);
```

---

## 🧱 String Kuyruk Örneği

```c
#include "HOL_Queue.h"

// 32 karakterlik string, 8 eleman kapasiteli kuyruk
DECLARE_STRING_QUEUE(32, 8)

queue_str_32_8_t log_queue;
char buffer[32];

queue_initialize_str_32_8(&log_queue);

// String ekleme (helper makro)
queue_push_with_string_support_32_8(&log_queue, "System Start");
queue_push_with_string_support_32_8(&log_queue, "Error 42");

// String çekme
queue_pull_with_string_support_32_8(&log_queue, buffer, sizeof(buffer));
// buffer -> "System Start"
```

---

## 🛠️ Makro Referans Tablosu

| Makro                                      | Açıklama                                     | Üretilen Yapılar/Fonksiyonlar                                                                          |
| :----------------------------------------- | :------------------------------------------- | :----------------------------------------------------------------------------------------------------- |
| `DECLARE_QUEUE(TYPE, SIZE)`                | Ana kuyruk tanımı                            | `queue_TYPE_SIZE_t`, `queue_initialize_TYPE_SIZE`, `queue_push_TYPE_SIZE`, `queue_pull_TYPE_SIZE`, ... |
| `DECLARE_QUEUE_STATUS(TYPE, SIZE)`         | Durum enum'u üretir                          | `QUEUE_TYPE_SIZE_OK`, `_ERROR_EMPTY`, `_ERROR_FULL`, ...                                               |
| `DECLARE_STRING_QUEUE(STR_SIZE, Q_SIZE)`   | Sabit uzunluklu string tipi ve kuyruk tanımı | `str_STR_SIZE`, `queue_push_with_string_support_...`, ...                                              |
| `QUEUE_MEMORY_BYTES(TYPE, SIZE)`           | Kuyruğun bellek boyutunu hesaplar            | `sizeof(TYPE) * SIZE + sizeof(size_t) * 3`                                                             |
| `QUEUE_DECLARE_AND_INIT(TYPE, SIZE, name)` | Tek satırda kuyruk tanımlama ve başlatma     | `queue_TYPE_SIZE_t name; queue_initialize_TYPE_SIZE(&name)`                                            |

---

## ⚙️ Üretilen Fonksiyonlar

| Fonksiyon                           | Açıklama                        |
| :---------------------------------- | :------------------------------ |
| `queue_initialize_TYPE_SIZE`        | Kuyruğu sıfırlar                |
| `queue_push_TYPE_SIZE`              | FIFO ekleme (overwrite)         |
| `queue_push_no_overwrite_TYPE_SIZE` | Kuyruk doluysa hata döner       |
| `queue_pull_TYPE_SIZE`              | En eski öğeyi çeker             |
| `queue_pull_multiple_TYPE_SIZE`     | Birden fazla öğeyi çeker        |
| `queue_peek_TYPE_SIZE`              | En eski öğeyi okur (çekmeden)   |
| `queue_peek_ptr_TYPE_SIZE`          | En eski öğeye işaretçi döndürür |
| `queue_is_empty_TYPE_SIZE`          | Boş mu kontrol eder             |
| `queue_is_full_TYPE_SIZE`           | Dolu mu kontrol eder            |
| `queue_count_TYPE_SIZE`             | Eleman sayısını döndürür        |
| `queue_available_space_TYPE_SIZE`   | Boş kapasiteyi döndürür         |
| `queue_clear_TYPE_SIZE`             | Kuyruğu temizler                |

---

## ⚠️ Eşzamanlılık Uyarısı

`volatile` değişkenler kullanılsa da işlemler atomik değildir. ISR veya çok iş parçacıklı ortamlarda güvenli kullanım için kilitleme yapılmalıdır.

```c
void safe_push(queue_u8_16_t* q, u8 data) {
    uint32_t flags = disable_interrupts();
    queue_push_u8_16(q, data);
    restore_interrupts(flags);
}
```

---

## 📄 Bellek Hesaplama

```c
size_t mem = QUEUE_MEMORY_BYTES(u16, 64); 
// Yaklaşık: sizeof(u16)*64 + sizeof(size_t)*3
```