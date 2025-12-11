#include "common.h"
#include <fmt/core.h>
#include <x86intrin.h>

int main() {
  // 1. Open Existing Shared Memory
  int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
  if (shm_fd == -1) {
    fmt::print(stderr, "Error: Run Producer first to create Shared Memory!\n");
    return 1;
  }

  return 0;
}