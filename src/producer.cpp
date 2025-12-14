#include "common.h"
#include <chrono>
#include <fmt/core.h>
#include <thread>

int main() {
  // Clean up and create shared memory
  shm_unlink(SHM_NAME);
  int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  if (fd == -1) {
    fmt::print(stderr, "shm_open failed\n");
    return 1;
  }
  ftruncate(fd, sizeof(SharedMemory));

  auto *shm = static_cast<SharedMemory *>(mmap(nullptr, sizeof(SharedMemory),
                                               PROT_READ | PROT_WRITE,
                                               MAP_SHARED, fd, 0));

  shm->head.store(0);
  shm->tail.store(0);

  pin_to_core(0); // Pin producer to core 0
  fmt::print("[Producer] Started (pinned to core 0)\n");

  uint32_t id = 0;
  while (true) {
    uint64_t head = shm->head.load(std::memory_order_relaxed);
    uint64_t tail = shm->tail.load(std::memory_order_acquire);

    if (head - tail >= RING_BUFFER_SIZE) {
      cpu_pause();
      continue;
    }

    Tick &tick = shm->buffer[head % RING_BUFFER_SIZE];
    tick.id = ++id;
    tick.price = 100.0 + (id % 100) * 0.01;
    tick.volume = 100 + (id % 10);
    tick.timestamp = rdtsc(); // Timestamp last, right before publish

    shm->head.store(head + 1, std::memory_order_release);

    if (id % 100000 == 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }
}
