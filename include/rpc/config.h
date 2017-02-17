#pragma once

#ifndef CONFIG_H_L7IVDSPZ
#define CONFIG_H_L7IVDSPZ

#include <cstddef>

namespace rpc
{

static constexpr std::size_t DEFAULT_BUFFER_SIZE = 1024 << 10;
static constexpr double BUFFER_GROW_FACTOR = 2.0;

} /* rpc */

#ifndef RPCLIB_MSGPACK
#define RPCLIB_MSGPACK clmdep_msgpack
#endif /* ifndef RPCLIB_MSGPACK */

#endif /* end of include guard: CONFIG_H_L7IVDSPZ */
