#include "rpc/nonstd/optional.hpp"

// This is no-op; the reason it exists is to avoid
// the weak vtables problem. For more info, see
// https://stackoverflow.com/a/23749273/140367
nonstd::bad_optional_access::~bad_optional_access() {}
