#ifndef COMMON_H
#define COMMON_H

#include <atomic>
#include <chrono>
#include <cstdint>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

// Cross-platform CPU cycle counter
inline uint64_t rdtsc() {
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) ||             \
    defined(_M_IX86)
  // x86/x64: Read Time-Stamp Counter
  unsigned int lo, hi;
  __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
  return ((uint64_t)hi << 32) | lo;
#elif defined(__aarch64__) || defined(_M_ARM64)
  // ARM64: Read cycle counter register
  uint64_t val;
  __asm__ __volatile__("mrs %0, cntvct_el0" : "=r"(val));
  return val;
#elif defined(__arm__)
  // ARM32: Read cycle counter (if available)
  uint32_t val;
  __asm__ __volatile__("mrc p15, 0, %0, c9, c13, 0" : "=r"(val));
  return val;
#else
  // Fallback: Use high-resolution clock (less accurate but works everywhere)
  return std::chrono::steady_clock::now().time_since_epoch().count();
#endif
}

// Cross-platform CPU pause/yield hint
inline void cpu_pause() {
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) ||             \
    defined(_M_IX86)
  // x86: PAUSE instruction (saves power in spin loops)
  __asm__ __volatile__("pause");
#elif defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__)
  // ARM: YIELD hint
  __asm__ __volatile__("yield");
#else
  // Fallback: compiler barrier
  std::atomic_signal_fence(std::memory_order_relaxed);
#endif
}

const char *SHM_NAME = "/hft_demo_shm";
const size_t RING_BUFFER_SIZE = 1024; // 1MB

// Single market update
struct Tick {
  uint64_t timestamp; // CPU cycle count on creation
  double price;
  uint32_t volume;
  uint32_t id;
};

// Ring buffer structure
// alignas(64) prevents false sharing - CPU cores won't fight over same cache
// lines
struct SharedMemory {
  alignas(64) std::atomic<uint64_t> head;    // Where producer writes next
  alignas(64) std::atomic<uint64_t> tail;    // Where consumer reads next
  alignas(64) Tick buffer[RING_BUFFER_SIZE]; // Actual data
};

#endif