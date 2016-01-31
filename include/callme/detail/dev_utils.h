#pragma once

#ifndef DEV_UTILS_H_JQSWE2OS
#define DEV_UTILS_H_JQSWE2OS

#ifdef CALLME_LINUX
#include "pthread.h"
#endif

namespace callme {
namespace detail {
inline void name_thread(std::string const &name) {
#ifdef CALLME_LINUX
    pthread_setname_np(pthread_self(), name.c_str());
#endif
}
} /* detail */
} /* callme */

#endif /* end of include guard: DEV_UTILS_H_JQSWE2OS */
