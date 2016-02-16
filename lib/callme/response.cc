#include "callme/response.h"
#include "callme/detail/log.h"

#include <assert.h>

namespace callme {

response::response(uint32_t id, std::string const &error,
                   std::unique_ptr<msgpack::object> result)
    : id_(id), error_(error), result_(std::move(result)), empty_(false) {}

response::response(msgpack::object const &o)
    : result_(std::make_unique<msgpack::object>()) {
    LOG_DEBUG("Response {}", o);
    response_type r;
    o.convert(&r);
    // TODO: check protocol [t.szelei 2015-12-30]
    id_ = std::get<1>(r);
    auto error_obj = std::get<2>(r);
    if (error_obj != msgpack::type::nil()) {
        error_obj.convert(&error_);
    }
    *result_ = std::get<3>(r);
}

msgpack::sbuffer response::get_data() const {
    msgpack::sbuffer data;
    response_type r(1, id_,
                    error_.size() > 0 ? msgpack::object(error_)
                                      : msgpack::object(msgpack::type::nil()),
                    *result_); // TODO: avoid copy [sztomi, 2015-11-23]
    msgpack::pack(&data, r);
    return data;
}

int response::get_id() const { return id_; }

std::string const &response::get_error() const { return error_; }

msgpack::object response::get_result() const { return *result_; }

response& response::empty() {
    static response r;
    r.empty_ = true;
    return r;
}

bool response::is_empty() const { return empty_; }

} /* callme */
