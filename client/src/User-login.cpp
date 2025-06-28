#include "../include/tools.h"
#include "../include/user.h"
#include <boost/json/object.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/value.hpp>
#include <cstddef>

/**
 * @brief 登陆函数,逻辑:获取obj中的数据,包含user/jwt等,和log命令拼接起来之后,再加密,附加\n后传输到服务器.
 *
 * @details
 *
 * @return int 	----> 0,成功,   ----> 1,没有用户,回到登陆界面   ----> 2,jwt过期,密码登陆,然后等待服务器签发新的jwt
 *
 * @author youth (doxe20191817@gmail.com)
 * @date 2025-06-28 11:46:31
 */
int User::Login()
{
    std::string key = "0123456789abcdef";
    auto username = this->obj.at("user").as_string();
    auto jwt_token = obj.at("jwt").as_string();
    // 已经获取到了user和jwt两个属性,可以交给服务器检测是否合法.
    /*
	检测来自server的回信,然后根据回信的内容决定Login函数的返回值:
	1) user存在数据库但是kwt不对,要求重新输入密码.
	2) user不存在,要求注册.
	*/
    boost::json::object login_msg;
    login_msg["user"] = username;
    login_msg["jwt"] = jwt_token;
    login_msg["cmd"] = "login";

    // 先对消息加密,然后再增加\n发送到服务器.
    std::string msg_2_server = boost::json::serialize(login_msg);
    size_t sendbytes = this->m_netlinker.send_encode_string(msg_2_server, key);
    if (sendbytes == -1)
	{
	    // 发送失败.打印一个日志.
	}

    auto recvmsg = m_netlinker.recv_decode_string(key);
    boost::json::value jv = boost::json::parse(recvmsg);
    boost::json::object server_obj = jv.as_object();

    auto ans = server_obj.at("ans-login").as_string();
    if (ans == "time-out")
	{
	    return 2;
	}
    else if (ans == "no-user")
	{
	    return 1;
	}
    else if (ans == "success")
	{
	    return 0;
	}

    // 加密完成,发送,开始调用netlinker的方法.
    // this->m_netlinker.receive_line()

    /*
	把这里封装为使用NetLinker	======> this->conn.send(boost::asio::buffer(json_string));
    boost::asio::write(sock, boost::asio::buffer(json_string), ec);
    if (ec)
	{
	    // 处理发送错误
		}
		*/
    return 0;
}
/*TODO
			boost库传输boost/json的方法,

#include <boost/json.hpp>
#include <boost/asio.hpp>
// ... 其他头文件和 NetLinker 类的定义

// 假设已经建立了 socket 连接
boost::asio::ip::tcp::socket sock;

// 1. 构建 JSON 数据
boost::json::object my_json_object;
my_json_object["username"] = "Alice";
my_json_object["message"] = "Hello from Boost.Asio!";

// 2. 将 JSON 对象序列化为字符串
std::string json_string = boost::json::serialize(my_json_object);

// 3. (可选但推荐) 添加消息结束符，以便接收方知道消息的边界
json_string += "\n"; // 例如，用换行符作为消息分隔符

// 4. 通过 Boost.Asio 发送字符串
boost::system::error_code ec;
boost::asio::write(sock, boost::asio::buffer(json_string), ec);
if (ec) {
    // 处理发送错误
}


接收端处理json数据:
#include <boost/json.hpp>
#include <boost/asio.hpp>
#include <iostream>
// ... 其他头文件和 NetLinker 类的定义

// 假设已经建立了 socket 连接
boost::asio::ip::tcp::socket sock;

// ...

// 1. 通过 Boost.Asio 接收数据，直到遇到分隔符
boost::asio::streambuf buffer;
boost::system::error_code ec;
// 假设约定每条 JSON 消息以换行符结束
size_t bytes_transferred = boost::asio::read_until(sock, buffer, '\n', ec);

if (!ec) {
    // 2. 将接收到的数据从 streambuf 提取为字符串
    std::istream is(&buffer);
    std::string received_json_string;
    std::getline(is, received_json_string); // 读取一行（直到换行符）

    try {
	// 3. 使用 Boost.JSON 解析字符串
	boost::json::value parsed_json = boost::json::parse(received_json_string);

	// 4. 从解析后的 JSON 中提取数据
	std::string username = parsed_json.at("username").as_string();
	std::string message = parsed_json.at("message").as_string();

	std::cout << "Received from " << username << ": " << message << std::endl;

    } catch (const boost::json::system_error& e) {
	// 处理 JSON 解析错误
	std::cerr << "JSON 解析失败: " << e.what() << std::endl;
    } catch (const std::exception& e) {
	// 其他错误，例如键不存在
	std::cerr << "数据提取错误: " << e.what() << std::endl;
    }
} else if (ec == boost::asio::error::eof) {
    // 连接关闭
} else {
    // 处理网络接收错误
    std::cerr << "网络接收错误: " << ec.message() << std::endl;
}
*/