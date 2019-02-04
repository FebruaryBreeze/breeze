// Copyright [2019] <sfzhou.scut@gamil.com>
#ifndef BREEZE_FUTURE_HPP_
#define BREEZE_FUTURE_HPP_

#include <memory>

#include "semaphore.hpp"

namespace Breeze {

template <class T>
class Future {
 public:
  Future(bool valid = false)
      : result(valid ? std::make_shared<Result>() : nullptr) {}

  T Get() const {
    this->result->semaphore.Wait();
    T ret_value = std::move(this->result->value);
    return ret_value;
  }

  void Finish(T &&value) const {
    this->result->value = std::move(value);
    this->result->semaphore.Notify();
  }

  operator bool() { return result != nullptr; }

 private:
  struct Result {
    Semaphore semaphore;
    T value;
  };

  std::shared_ptr<Result> result;
};

}  // namespace Breeze

#endif  // BREEZE_FUTURE_HPP_
