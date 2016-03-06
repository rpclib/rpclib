
namespace callme {

template <typename T> void this_handler_t::set_error(T &&err_obj) {
    error_ = detail::pack(std::forward<T>(err_obj));
}

template <typename T> void this_handler_t::set_special_response(T &&resp_obj) {
    resp_ = detail::pack(std::forward<T>(err_obj));
}

} /* callme */
