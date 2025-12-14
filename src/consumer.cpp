#include "common.h"
#include <fmt/core.h>

int main() {
  // 1. Open Existing Shared Memory
  int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
  if (shm_fd == -1) {
    fmt::print(stderr, "Error: Run Producer first to create Shared Memory!\n");
    return 1;
  }

  void *ptr = mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED,
                   shm_fd, 0);
  SharedMemory *shm = (SharedMemory *)ptr;

  fmt::print("[Consumer] Attached to Feed. Calculating Latency...\n");

  while (true) {
    uint64_t current_tail = shm->tail.load(std::memory_order_relaxed);
    uint64_t current_head = shm->head.load(std::memory_order_acquire);

    // Check if buffer is empty
    if (current_tail == current_head) {
      cpu_pause(); // Buffer empty, spin wait
      continue;
    }

    // 2. Read Data
    const Tick &tick = shm->buffer[current_tail % RING_BUFFER_SIZE];

    // 3. Measure Latency IMMEDIATELY
    uint64_t now = rdtsc();
    uint64_t latency_cycles = now - tick.timestamp;

    // 4. Mark as Read (Move Tail forward)
    shm->tail.store(current_tail + 1, std::memory_order_release);

    // Logging: Only occasionally
    if (tick.id % 100000 == 0) {
      // Cycle-to-nanosecond conversion is CPU dependent.
      // On modern 3GHz CPUs, 1 cycle approx 0.33ns
      double approx_ns = latency_cycles / 3.0;

      fmt::print("Tick ID: {:<8} | Latency: {:<5} cycles (~{:.1f} ns)\n",
                 tick.id, latency_cycles, approx_ns);
    }
  }

  return 0;
}