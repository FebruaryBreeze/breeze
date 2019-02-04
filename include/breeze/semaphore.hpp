// Copyright [2019] <sfzhou.scut@gamil.com>
#ifndef BREEZE_SEMAPHORE_HPP_
#define BREEZE_SEMAPHORE_HPP_

#include <condition_variable>
#include <mutex>

namespace Breeze {

// refer to https://stackoverflow.com/a/4793662
class Semaphore {
 public:
  Semaphore(int count_ = 0) : count(count_) {}

  inline void Notify() {
    std::unique_lock<std::mutex> lock(this->mutex);
    ++this->count;
    this->condition.notify_one();
  }

  inline void Wait() {
    std::unique_lock<std::mutex> lock(this->mutex);
    while (this->count == 0) {
      this->condition.wait(lock);
    }
    --this->count;
  }

 private:
  int count;
  std::mutex mutex;
  std::condition_variable condition;
};

}  // namespace Breeze

#endif  // BREEZE_SEMAPHORE_HPP_
