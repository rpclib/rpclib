#include "rpc/detail/response.h"
#include "rpc/detail/log.h"
#include "rpc/detail/util.h"

#include <assert.h>

namespace rpc {
namespace detail {

response::response() : id_(0), error_(), result_(), empty_(false) {}

response::response(msgpack::object_handle o) : response() {
    response_type r;
    o.get().convert(&r);
    // TODO: check protocol [t.szelei 2015-12-30]
    id_ = std::get<1>(r);
    auto &&error_obj = std::get<2>(r);
    if (error_obj != msgpack::type::nil()) {
        error_ = std::make_shared<msgpack::object_handle>();
        error_->assign(msgpack::clone(error_obj));
    }
    result_ = std::make_shared<msgpack::object_handle>(
        std::get<3>(r), std::move(o.zone()));
}

msgpack::sbuffer response::get_data() const {
    msgpack::sbuffer data;
    response_type r(1, id_, error_ ? error_->get() : msgpack::object(),
                    result_ ? result_->get() : msgpack::object());
    msgpack::pack(&data, r);
    return data;
}

uint32_t response::get_id() const { return id_; }

std::shared_ptr<msgpack::object_handle> response::get_error() const { return error_; }

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
        error_ = std::shared_ptr<msgpack::object_handle>();
    }
    error_->assign(std::move(e));
}

} /* detail */
} /* rpc */
