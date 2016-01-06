#include "callme/response.h"
#include "callme/detail/log.h"

namespace callme {

response::response(uint32_t id, std::string const& error,
                   std::unique_ptr<msgpack::object> result)
    : id_(id), error_(error), result_(std::move(result)) {}

response::response(msgpack::object const &o)
    : result_(std::make_unique<msgpack::object>()) {
    LOG_DEBUG("Response %v", o);
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

void response::write(uv_stream_t *stream) {
    LOG_DEBUG("Writing response");
    msgpack::sbuffer buf;

    response_type r(1, id_,
                    error_.size() > 0 ? msgpack::object(error_)
                                      : msgpack::object(msgpack::type::nil()),
                    *result_); // TODO: avoid copy [sztomi, 2015-11-23]
    msgpack::pack(buf, r);
    uv_buf_t out_buf = uv_buf_init(buf.data(), buf.size());

    uv_write_t *req =
        reinterpret_cast<uv_write_t *>(malloc(sizeof(uv_write_t)));
    req->data = this;
    uv_write(req, stream, &out_buf, 1, &response::fw_on_write);
}

int response::get_id() const { return id_; }

std::string const& response::get_error() const { return error_; }

msgpack::object response::get_result() const { return *result_; }

void response::on_write(uv_write_t *req, int status) {
    LOG_DEBUG("Write finished with status %v", status);
}

} /* callme */
