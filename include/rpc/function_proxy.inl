
namespace rpc {

template <typename ClientT>
function_proxy<ClientT>::function_proxy(std::string const &func_name, ClientT *client_instance):
	client_instance_(client_instance),
	func_name_(func_name)
{

}

template <typename ClientT>
template <typename... Args>
RPCLIB_MSGPACK::object_handle function_proxy<ClientT>::operator()(call_type::Normal, Args&&... args) const
{
	return call(std::forward<Args>(args)...);
}

template <typename ClientT>
template <typename... Args>
std::future<RPCLIB_MSGPACK::object_handle> function_proxy<ClientT>::operator()(call_type::Async, Args&&... args) const
{
	return async_call(std::forward<Args>(args)...);
}

template <typename ClientT>
template <typename... Args>
RPCLIB_MSGPACK::object_handle function_proxy<ClientT>::call(Args&&... args) const
{
	return client_instance_->call(func_name_, std::forward<Args>(args)...);
}

template <typename ClientT>
template <typename... Args>
std::future<RPCLIB_MSGPACK::object_handle> function_proxy<ClientT>::async_call(Args&&... args) const
{
	return client_instance_->async_call(func_name_, std::forward<Args>(args)...);
}

}
