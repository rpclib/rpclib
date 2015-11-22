#pragma once

#ifndef STRINGREF_HASH_H_CEVY7HV0
#define STRINGREF_HASH_H_CEVY7HV0

#include <boost/functional/hash.hpp>

// copied from http://stackoverflow.com/a/23444316/140367
namespace std {
template <> struct hash<boost::string_ref> {
    size_t operator()(boost::string_ref sr) const {
        return boost::hash_range(sr.begin(), sr.end());
    }
};
}

#endif /* end of include guard: STRINGREF_HASH_H_CEVY7HV0 */
