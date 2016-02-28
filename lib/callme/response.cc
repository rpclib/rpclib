#include "callme/response.h"
#include "callme/detail/log.h"

#include <assert.h>

namespace callme {

response::response()
    : id_(0),
      error_(""),
      result_(std::shared_ptr<msgpack::object_handle>()),
      empty_(false) {}

response::response(uint32_t id, std::string const &error,
                   std::unique_ptr<msgpack::object_handle> result)
    : id_(id), error_(error), result_(result.release()), empty_(false) {}

response::response(msgpack::object_handle o) : response() {
    LOG_DEBUG("Response {}", o.get());
    response_type r;
    o.get().convert(&r);
    // TODO: check protocol [t.szelei 2015-12-30]
    id_ = std::get<1>(r);
    auto error_obj = std::get<2>(r);
    if (error_obj != msgpack::type::nil()) {
        error_obj.convert(&error_);
    }
    result_ = std::make_shared<msgpack::object_handle>(std::get<3>(r),
                                                       std::move(o.zone()));
}

msgpack::sbuffer response::get_data() const {
    msgpack::sbuffer data;
    response_type r(1, id_,
                    error_.size() > 0 ? msgpack::object(error_)
                                      : msgpack::object(msgpack::type::nil()),
                    result_ ? result_->get() : msgpack::object());
    msgpack::pack(&data, r);
    return data;
}

uint32_t response::get_id() const { return id_; }

std::string const &response::get_error() const { return error_; }

msgpack::object_handle response::get_result() const {
    return std::move(*result_);
}

response response::empty() {
    response r;
    r.empty_ = true;
    return r;
}

bool response::is_empty() const { return empty_; }

} /* callme */
