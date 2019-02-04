#include <gtest/gtest.h>
#include <chrono>
#include <iostream>

#include "breeze/thread_pool.hpp"

TEST(BREEZE, THREAD_POOL) {
  const int k_bound = 16;
  Breeze::ThreadPool thread_pool(4);

  std::vector<Breeze::Future<int>> futures;
  for (int i = 0; i < k_bound; ++i) {
    futures.emplace_back(thread_pool.enqueue([i] {
      std::this_thread::sleep_for(std::chrono::microseconds(100));
      return i;
    }));
  }

  int sum = 0;
  for (auto &future : futures) {
    sum += future.Get();
  }
  ASSERT_EQ(sum, k_bound * (k_bound - 1) / 2);
}
