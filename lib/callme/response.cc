#include "callme/response.h"

namespace callme {

response::response(uint32_t id, boost::string_ref error,
                   std::unique_ptr<msgpack::object> result)
    : id_(id), error_(error), result_(std::move(result)) {}

void response::write(uv_stream_t *stream) {
    msgpack::sbuffer buf;
    response_type r(1, id_,
                    error_.size() > 0 ? msgpack::object(error_.to_string())
                                      : msgpack::object(msgpack::type::nil()),
                    *result_); // TODO: avoid copy [sztomi, 2015-11-23]
    msgpack::pack(buf, r);
    uv_buf_t out_buf = uv_buf_init(buf.data(), buf.size());

    uv_write_t *req =
        reinterpret_cast<uv_write_t *>(malloc(sizeof(uv_write_t)));
    uv_write(req, stream, &out_buf, 1, NULL);
}

} /* callme */
