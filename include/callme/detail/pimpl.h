#pragma once

#ifndef PIMPL_H_TV7E3C9K
#define PIMPL_H_TV7E3C9K

#include "pimpl_ptr.hh"

#define CALLME_DECL_PIMPL(Size, Alignment)                                     \
    struct impl;                                                               \
    pimpl_ptr<impl, Size, Alignment> pimpl;

#endif /* end of include guard: PIMPL_H_TV7E3C9K */
