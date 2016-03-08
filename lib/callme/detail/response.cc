#include "callme/detail/response.h"
#include "callme/detail/log.h"
#include "callme/detail/util.h"

#include <assert.h>

namespace callme {
namespace detail {

response::response() : id_(0), error_(), result_(), empty_(false) {}

response::response(msgpack::object_handle o) : response() {
    LOG_DEBUG("Response {}", o.get());
    response_type r;
    o.get().convert(&r);
    // TODO: check protocol [t.szelei 2015-12-30]
    id_ = std::get<1>(r);
    auto &&error_obj = std::get<2>(r);
    if (error_obj != msgpack::type::nil()) {
        auto z = std::make_unique<msgpack::zone>();
        error_ =
            std::make_shared<msgpack::object_handle>(error_obj, std::move(z));
    }
    result_ = std::make_shared<msgpack::object_handle>(std::get<3>(r),
                                                       std::move(o.zone()));
}

msgpack::sbuffer response::get_data() const {
    msgpack::sbuffer data;
    response_type r(1, id_, error_ > 0 ? msgpack::object(error_->get())
                                       : msgpack::object(msgpack::type::nil()),
                    result_ ? result_->get() : msgpack::object());
    msgpack::pack(&data, r);
    return data;
}

uint32_t response::get_id() const { return id_; }

std::shared_ptr<msgpack::object_handle> response::get_error() const {
    return error_;
}

std::shared_ptr<msgpack::object_handle> response::get_result() const {
    return result_;
}

response response::empty() {
    response r;
    r.empty_ = true;
    return r;
}

bool response::is_empty() const { return empty_; }

void response::capture_result(msgpack::object_handle &r) {
    if (!result_) {
        result_ = std::make_shared<msgpack::object_handle>();
    }
    result_->assign(std::move(r));
}

void response::capture_error(msgpack::object_handle &e) {
    if (!error_) {
        error_ = std::make_shared<msgpack::object_handle>();
    }
    error_->assign(std::move(e));
}

} /* detail */
} /* callme */
