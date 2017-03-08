#pragma once

#ifndef RPCLIB_COMPAT_HPP
#define RPCLIB_COMPAT_HPP

namespace std {

#if RPCLIB_CXX_STANDARD < 14
template<typename T, typename... Ts>
unique_ptr<T> make_unique(Ts&&... params)
{
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}
#endif

}

#endif /* end of include guard: RPCLIB_COMPAT_HPP */
