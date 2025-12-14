#include "common.h"
#include <fmt/core.h>

int main() {
  int fd = shm_open(SHM_NAME, O_RDWR, 0666);
  if (fd == -1) {
    fmt::print(stderr, "Run producer first!\n");
    return 1;
  }

  auto *shm = static_cast<SharedMemory *>(mmap(nullptr, sizeof(SharedMemory),
                                               PROT_READ | PROT_WRITE,
                                               MAP_SHARED, fd, 0));

  pin_to_core(2); // Pin consumer to core 2 (different physical P-core)
  fmt::print("[Consumer] Started (pinned to core 2)\n");

  while (true) {
    uint64_t tail = shm->tail.load(std::memory_order_relaxed);
    uint64_t head = shm->head.load(std::memory_order_acquire);

    if (tail >= head) {
      cpu_pause();
      continue;
    }

    const Tick &tick = shm->buffer[tail % RING_BUFFER_SIZE];
    uint64_t now = rdtsc();
    uint64_t cycles = now - tick.timestamp;

    shm->tail.store(tail + 1, std::memory_order_release);

    // Track min latency (the real IPC latency, not queue wait time)
    static uint64_t min_cycles = UINT64_MAX;
    static uint64_t sample_count = 0;

    if (cycles < min_cycles)
      min_cycles = cycles;
    sample_count++;

    if (tick.id % 100000 == 0) {
      static double freq_ghz = get_counter_freq_ghz();
      double ns = cycles / freq_ghz;
      double min_ns = min_cycles / freq_ghz;
      uint64_t queue_depth = head - tail;
      fmt::print("ID: {:>7} | {:>5} ticks ({:>5.0f} ns) | min: {:>3} ({:>3.0f} "
                 "ns) | q:{}\n",
                 tick.id, cycles, ns, min_cycles, min_ns, queue_depth);
      min_cycles = UINT64_MAX;
    }
  }
}
