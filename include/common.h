#ifndef COMMON_H
#define COMMON_H

#include <atomic>
#include <cstdint>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

// Simple, fast TSC read - no serialization overhead
inline uint64_t rdtsc() {
#if defined(__x86_64__) || defined(_M_X64)
  unsigned int lo, hi;
  __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
  return ((uint64_t)hi << 32) | lo;
#elif defined(__aarch64__)
  uint64_t val;
  __asm__ __volatile__("mrs %0, cntvct_el0" : "=r"(val));
  return val;
#else
  return __builtin_ia32_rdtsc();
#endif
}

// CPU pause hint for spin loops
inline void cpu_pause() {
#if defined(__x86_64__) || defined(_M_X64)
  __asm__ __volatile__("pause");
#elif defined(__aarch64__)
  __asm__ __volatile__("yield");
#endif
}

// Shared memory name and buffer size
constexpr const char *SHM_NAME = "/hft_shm";
constexpr size_t RING_BUFFER_SIZE = 1024;

// Market tick data
struct Tick {
  uint64_t timestamp;
  double price;
  uint32_t volume;
  uint32_t id;
};

// Ring buffer in shared memory
struct alignas(64) SharedMemory {
  alignas(64) std::atomic<uint64_t> head{0};
  alignas(64) std::atomic<uint64_t> tail{0};
  Tick buffer[RING_BUFFER_SIZE];
};

#endif
