#include "../include/NetLinker.h"
#include <iostream>
#include <openssl/crypto.h>

NetLinker::NetLinker() : ioc(), sock(ioc), ec() {}

NetLinker::~NetLinker() { close(); }

/**
 * @brief 尝试对ip:port发起连接
 *
 * @param ip
 * @param port
 * @return bool
 *
 * @author youth (doxe20191817@gmail.com)
 * @date 2025-06-28 11:21:44
 */
bool NetLinker::connect(const std::string& ip, uint16_t port)
{
    try
	{
	    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(ip), port);
	    sock.connect(endpoint);
	    // 可以打印一个日志,表示完成连接.
	    return true;
	}
    catch (...)
	{
	    return false;
	}
}

void NetLinker::close()
{
    // 1. 关闭 Socket
    // 首先尝试关闭发送方向，阻止进一步的数据发送
    if (sock.is_open())
	{
	    sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
	    if (ec && ec != boost::asio::error::not_connected) // 忽略"未连接"的错误
		{
		    // 记录或处理关闭发送方向时的错误，但通常不致命
		    std::cerr << "警告: 关闭 socket 写入方向时发生错误: " << ec.message() << std::endl;
		}

	    // 无论 shutdown 是否成功，都尝试关闭 socket
	    sock.close(ec);
	    if (ec)
		{
		    // 记录或处理关闭 socket 时的错误
		    std::cerr << "错误: 关闭 socket 时发生错误: " << ec.message() << std::endl;
		    // 此时不抛出异常，因为析构函数中不应抛出
		}
	}

    // 2. 停止 io_context
    // io_context 停止操作通常不会抛出异常，可以直接调用
    // 但为了完整性，仍然可以考虑异常安全，尽管 Boost.Asio 很少会在 stop() 中抛出
    try
	{
	    if (!ioc.stopped())
		{
		    ioc.stop();
		    // 在某些异步场景中，停止io_context后可能需要重置它才能再次使用
		    // 但对于你的当前同步用法，如果 NetLinker 对象生命周期结束，
		    // 通常不需要 reset。如果在一个连接断开后立即尝试新的连接，可能需要
		    // ioc.reset();
		}
	}
    catch (const std::exception& e)
	{
	    // 捕获任何潜在的（极少发生）io_context::stop() 异常
	    std::cerr << "错误: 停止 io_context 时发生异常: " << e.what() << std::endl;
	}
}
