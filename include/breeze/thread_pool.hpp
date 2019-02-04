// Copyright [2019] <sfzhou.scut@gamil.com>
#ifndef BREEZE_THREAD_POOL_HPP_
#define BREEZE_THREAD_POOL_HPP_

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

#include "future.hpp"

namespace Breeze {

// refer to https://github.com/progschj/ThreadPool
class ThreadPool {
 public:
  ThreadPool(size_t thread_count) : stop(false) {
    for (size_t i = 0; i < thread_count; ++i) {
      this->workers.emplace_back([this] {
        while (true) {
          std::function<void()> task;

          {
            std::unique_lock<std::mutex> lock(this->queue_mutex);
            this->condition.wait(
                lock, [this] { return this->stop || !this->tasks.empty(); });
            if (this->stop && this->tasks.empty()) {
              return;
            }
            task = std::move(this->tasks.front());
            this->tasks.pop();
          }

          task();
        }
      });
    }
  }

  template <class F, class... Args>
  auto enqueue(F &&f, Args &&... args)
      -> Future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;
    Future<return_type> future(true);

    auto task_ptr = std::make_shared<std::function<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    {
      std::unique_lock<std::mutex> lock(this->queue_mutex);
      if (this->stop) {
        throw std::runtime_error("enqueue on stopped ThreadPool");
      }

      this->tasks.emplace([task_ptr, future] { future.Finish((*task_ptr)()); });
    }
    this->condition.notify_one();
    return future;
  }

  ~ThreadPool() {
    {
      std::unique_lock<std::mutex> lock(this->queue_mutex);
      this->stop = true;
    }
    this->condition.notify_all();
    for (std::thread &worker : this->workers) {
      worker.join();
    }
  }

 private:
  std::queue<std::function<void()>> tasks;
  std::vector<std::thread> workers;

  bool stop;
  std::mutex queue_mutex;
  std::condition_variable condition;
};

}  // namespace Breeze

#endif  // BREEZE_THREAD_POOL_HPP_
