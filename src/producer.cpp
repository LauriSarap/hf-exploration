#include "common.h"
#include <fmt/core.h>
#include <x86intrin.h>

int main() {
  // 1. Shared memory creation
  // O_CREAT = Create if missing, O_RDWR = Read/Write
  int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    fmt::print(stderr, "Failed to create shared memory.\n");
    return 1;
  }

  return 0;
}