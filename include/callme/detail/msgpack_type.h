#pragma once

#ifndef MSGPACK_TYPE_H_FUA6305B
#define MSGPACK_TYPE_H_FUA6305B


namespace callme {
namespace detail {

template<typename T>
struct msgpack_type; // non-specialized instantiation is prevented by this.

template<>
struct msgpack_type<char> {
    typedef int type;
};

}
}

#endif /* end of include guard: MSGPACK_TYPE_H_FUA6305B */
