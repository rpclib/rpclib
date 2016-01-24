#pragma once

#ifndef LOG_H_SPSC31OG
#define LOG_H_SPSC31OG

#ifdef CALLME_LOGGING

#include "format.h"
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <mutex>
#include <sstream>

namespace callme {
namespace detail {
class logger {
public:
    static logger &instance() {
        static logger inst;
        return inst;
    }

    template <typename... Args>
    void trace(const char *channel, const char *file, const char *func,
               std::size_t line, const char *msg, Args... args) {
        std::lock_guard<std::mutex> lock(mut_print_);
        fmt::print("[TRACE] {:10}  {:12}  {}:{} ({})     {:40}\n",
                   fmt::format("[{}]", channel), now(), file, line, func,
                   fmt::format(msg, args...));
    }

    template <typename... Args>
    void debug(const char *channel, const char *file, const char *func,
               std::size_t line, const char *msg, Args... args) {
        basic_log(
            "DEBUG", channel,
            fmt::format("{} ({}:{})", fmt::format(msg, args...), file, line));
    }

    template <typename... Args>
    void warn(const char *channel, const char *msg, Args... args) {
        basic_log("WARN", channel, fmt::format(msg, args...));
    }

    template <typename... Args>
    void error(const char *channel, const char *msg, Args... args) {
        basic_log("ERROR", channel, fmt::format(msg, args...));
    }

    template <typename... Args>
    void info(const char *channel, const char *msg, Args... args) {
        basic_log("INFO", channel, fmt::format(msg, args...));
    }

private:
    logger() {}
    static std::string now() {
        std::stringstream ss;
        std::time_t now_t = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
        ss << std::put_time(std::localtime(&now_t), "%F %T");
        return ss.str();
    }

    void basic_log(const char *severity, const char *channel, std::string const& msg) {
        using fmt::arg;
        std::lock_guard<std::mutex> lock(mut_print_);
        fmt::print("{severity:8}  {channel:10}  {time:12}         {msg:40}\n",
                   arg("severity", fmt::format("[{}]", severity)),
                   arg("channel", fmt::format("[{}]", channel)),
                   arg("time", now()), arg("msg", msg));
    }

private:
    std::mutex mut_print_;
};
} /* detail */
} /* callme */

#define LOG_INFO(...)                                                          \
    callme::detail::logger::instance().info("callme", __VA_ARGS__)

#define LOG_WARN(...)                                                          \
    callme::detail::logger::instance().warn("callme", __VA_ARGS__)

#define LOG_ERROR(...)                                                         \
    callme::detail::logger::instance().error("callme", __VA_ARGS__)

#define LOG_DEBUG(...)                                                         \
    callme::detail::logger::instance().debug("callme", __FILE__, __func__,     \
                                             __LINE__, __VA_ARGS__)

#define LOG_TRACE(...)                                                         \
    callme::detail::logger::instance().trace("callme", __FILE__, __func__,     \
                                             __LINE__, __VA_ARGS__)

#define LOG_EXPR(Level, Expr) LOG_##Level("`" #Expr "` = {}", Expr)

#else

#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)
#define LOG_DEBUG(...)
#define LOG_TRACE(...)
#define LOG_EXPR(...)

#endif

#endif /* end of include guard: LOG_H_SPSC31OG */
