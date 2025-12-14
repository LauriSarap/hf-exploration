#include "common.h"
#include <chrono>     // For sleep duration
#include <fmt/core.h> // Fast formatting
#include <thread>     // For sleep

int main() {
  // 1. Create Shared Memory
  // O_CREAT = Create if missing, O_RDWR = Read/Write
  int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    fmt::print(stderr, "Failed to create shared memory.\n");
    return 1;
  }

  // Set size of shared memory object
  ftruncate(shm_fd, sizeof(SharedMemory));

  // Map into this process's memory space
  void *ptr = mmap(0, sizeof(SharedMemory), PROT_WRITE, MAP_SHARED, shm_fd, 0);
  SharedMemory *shm = (SharedMemory *)ptr;

  // Reset indices
  shm->head.store(0);
  shm->tail.store(0);

  fmt::print("[Producer] Starting High-Frequency Feed (SHM: {})...\n",
             SHM_NAME);

  uint32_t msg_id = 0;

  // 2. The Hot Path (Infinite Loop)
  while (true) {
    // Load indices
    // memory_order_relaxed is fastest; strict ordering needed only at the end
    uint64_t current_head = shm->head.load(std::memory_order_relaxed);
    uint64_t current_tail = shm->tail.load(std::memory_order_acquire);

    // Check if buffer is full (Head cannot lap Tail)
    if (current_head - current_tail >= RING_BUFFER_SIZE) {
      // BUSY SPIN: Don't sleep, just burn CPU to be ready instantly
      cpu_pause(); // Hint to CPU to save power
      continue;
    }

    // 3. Write Data (Zero Copy)
    // We write directly into the shared memory slot
    Tick &tick = shm->buffer[current_head % RING_BUFFER_SIZE];

    tick.id = ++msg_id;
    tick.price = 100.0 + (msg_id % 100) * 0.01; // Fake price pattern
    tick.volume = 100 + (msg_id % 10);

    // Critical: Timestamp right before "publishing"
    tick.timestamp = rdtsc();

    // 4. Publish (Move Head forward)
    // memory_order_release guarantees Consumer sees the data written above
    shm->head.store(current_head + 1, std::memory_order_release);

    // Logging: Only occasionally to simulate real load (printing is slow!)
    if (msg_id % 100000 == 0) {
      fmt::print("[Producer] Sent 100k ticks. Last ID: {}\n", msg_id);
      // Artificial slow down just so we can read the console
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }

  return 0;
}