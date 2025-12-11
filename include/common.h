#ifndef COMMON_H
#define COMMON_H

#include <atomic>
#include <cstdint>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

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