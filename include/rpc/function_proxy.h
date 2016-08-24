#pragma once

#include <string>
#include <future>

#include "msgpack.hpp"

namespace rpc {

class client;

namespace call_type {

//! \brief Tag type indicating normal (blocking) function call
struct Normal{};

//! \brief Tag type indicating asynchronous function call
struct Async{};

}

//! \brief This must be a template to overcome the problems
//!	caused by the circular dependency between this and 'client'
template <typename ClientT>
class function_proxy
{
public:
	function_proxy() = delete;

	~function_proxy() = default;
	function_proxy(function_proxy const &) = default;
	function_proxy(function_proxy &&) = default;
	function_proxy& operator=(function_proxy const &) = default;
	function_proxy& operator=(function_proxy &&) = default;

	function_proxy(std::string const &func_name, ClientT *client_instance);

	template <typename... Args>
	RPCLIB_MSGPACK::object_handle operator()(call_type::Normal, Args&&... args) const;

	template <typename... Args>
	std::future<RPCLIB_MSGPACK::object_handle> operator()(call_type::Async, Args&&... args) const;

	template <typename... Args>
	RPCLIB_MSGPACK::object_handle call(Args&&... args) const;

	template <typename... Args>
	std::future<RPCLIB_MSGPACK::object_handle> async_call(Args&&... args) const;

private:
	ClientT *client_instance_ = nullptr;
	std::string func_name_;
};

}

#include "function_proxy.inl"
