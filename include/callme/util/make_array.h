#pragma once

#ifndef MAKE_ARRAY_H_CN3SZQXP
#define MAKE_ARRAY_H_CN3SZQXP

#include <array>

namespace callme {

template <typename R, typename... T>
std::array<R, sizeof...(T)> make_array(T &&... refs) {
    return std::array<R, sizeof...(T)>{{std::forward<T>(refs)...}};
}

} /* callme */

#endif /* end of include guard: MAKE_ARRAY_H_CN3SZQXP */
