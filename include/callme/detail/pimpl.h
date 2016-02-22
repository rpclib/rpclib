#pragma once

#ifndef PIMPL_H_TV7E3C9K
#define PIMPL_H_TV7E3C9K

#include "pimpl_ptr.hh"

#ifdef _MSC_VER
#define CALLME_DEF_ALIGN 4
#else
#define CALLME_DEF_ALIGN 8
#endif

#define CALLME_DECL_PIMPL(Size)                                                \
    struct impl;                                                               \
    pimpl_ptr<impl, Size, CALLME_DEF_ALIGN> pimpl;

#endif /* end of include guard: PIMPL_H_TV7E3C9K */
