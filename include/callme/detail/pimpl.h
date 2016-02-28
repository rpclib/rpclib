#pragma once

#ifndef PIMPL_H_TV7E3C9K
#define PIMPL_H_TV7E3C9K

#include "pimpl_ptr.hh"

#ifdef CALLME_ARCH_X86
#define CALLME_DEF_ALIGN 4
#elif CALLME_ARCH_X64
#define CALLME_DEF_ALIGN 8
#else
#error "Unkown architecture"
#endif

#define CALLME_DECL_PIMPL(Size)                                                \
    struct impl;                                                               \
    detail::pimpl_ptr<impl, Size, CALLME_DEF_ALIGN> pimpl;

#endif /* end of include guard: PIMPL_H_TV7E3C9K */
