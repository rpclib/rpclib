#pragma once

#ifndef UV_LOOP_H_B1NDOIYA
#define UV_LOOP_H_B1NDOIYA

#include "uv.h"
#include <mutex>
#include <thread>
#include "callme/detail/log.h"

namespace callme {
namespace detail {

//! \brief Wraps a libuv loop and provides safe initialization, running and
//! shutdown for it.
//! \note This class is a singleton.
//! \note This class is a singleton on purpose.
//! \note No, seriously.
class uv_loop {
public:
    static uv_loop &instance() {
        static uv_loop inst;
        return inst;
    }

    //! \brief Allocates a handle. The handle memory is freed by uv_loop upon
    //! exit.
    //! \note The handles are reachable because the libuv loop stores them and
    //! allow uv_walk-ing through them.
    template <typename T> static T *make_handle(void *owner = nullptr) {
        T *handle = new T;
        handle->data = owner;
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
        uv_stop(&loop_);
    }

    uv_loop_t *get_loop() const { return &loop_; }

    // Shutdown code based on http://stackoverflow.com/a/25831688/140367
    ~uv_loop() {
        LOG_INFO("Shutting down event loop.");
        stop();
        uv_walk(&loop_, [](uv_handle_t *handle, void *arg) {
            if (!uv_is_closing(handle)) {
                uv_close(handle, nullptr);
                delete handle;
            }
        }, nullptr);
        uv_run(&loop_, UV_RUN_DEFAULT);
        while (loop_.active_handles)
            ;
        uv_loop_close(&loop_);
    }

private:
    uv_loop() : is_running_(false) {
        LOG_INFO("Initializing event loop");
        uv_loop_init(&loop_);
    }

    mutable uv_loop_t loop_;
    std::mutex mut_is_running_;
    bool is_running_;
};
} /* detail */
} /* callme  */

#endif /* end of include guard: UV_LOOP_H_B1NDOIYA */
