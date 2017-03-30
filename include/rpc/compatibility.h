#pragma once

#ifndef COMPATIBILITY_H_PODKJ3
#define COMPATIBILITY_H_PODKJ3

#ifndef _MSC_VER

#define RPCLIB_NORETURN [[noreturn]]

#else

#define RPCLIB_NORETURN __declspec(noreturn) 

#endif // !_MSC_VER

#endif // COMPATIBILITY_H_PODKJ3
