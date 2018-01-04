#pragma once

#ifndef UV_LOOP_H_B1NDOIYA
#define UV_LOOP_H_B1NDOIYA

#include <mutex>
#include <thread>
#include <vector>

#include <uv.h>

#include "rpc/detail/log.h"
#include "rpc/detail/uv_tools.h"

namespace rpc {
namespace detail {

//! \brief Wraps a libuv loop and provides safe initialization, running and
//! shutdown for it.
class uv_loop {
public:
  uv_loop() : is_running_(false) {
    LOG_INFO("Initializing event loop");
    check_uv("event loop init", uv_loop_init(&loop_));
  }

  // Shutdown code based on http://stackoverflow.com/a/25831688/140367
  ~uv_loop() {
    LOG_INFO("Shutting down event loop.");
    stop();
  }

  //! \brief Allocates a handle. The handle memory is freed by uv_loop upon
  //! exit.
  template <typename T>
  T *make_handle(void *data = nullptr) {
    std::lock_guard<std::mutex> lock(mut_handles_);
    T *handle = new T;
    handle->data = data;
    handles_.push_back(reinterpret_cast<uv_handle_t *>(handle));
    return handle;
  }

  void start() {
    std::lock_guard<std::mutex> lock(mut_is_running_);
    if (!is_running_) {
      std::thread t([this]() {
        uv_run(&loop_, UV_RUN_DEFAULT);
        is_running_ = false;
        LOG_INFO("Loop stopped");
      });
      is_running_ = true;
      t.detach();
    }
  }

  void stop() {
    LOG_INFO("Stopping event loop");
    // uv_stop(&loop_);
    uv_walk(&loop_,
            [](uv_handle_t *handle, void *arg) {
              if (!uv_is_closing(handle)) {
                // LOG_INFO("uv_walk closing");
                // uv_close(handle, nullptr);
              }
            },
            nullptr);
    uv_run(&loop_, UV_RUN_DEFAULT);
    while (loop_.active_handles)
      ;
    uv_stop(&loop_);
    // uv_loop_close(&loop_);
    // for (auto h : handles_) {
    //   delete h;
    // }
  }

  uv_loop_t *get() const { return &loop_; }

  mutable uv_loop_t loop_;
  std::mutex mut_is_running_;
  std::mutex mut_handles_;
  std::vector<uv_handle_t *> handles_;
  bool is_running_;

  RPCLIB_CREATE_LOG_CHANNEL(uv_loop);
};

}  // namespace detail
}  // namespace rpc

#endif /* end of include guard: UV_LOOP_H_B1NDOIYA */
