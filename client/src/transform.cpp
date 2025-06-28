#include "../include/NetLinker.h"
#include "../include/tools.h"
#include <boost/asio.hpp>
#include <fstream>
#include <openssl/evp.h>
#include <openssl/rand.h>

const int AES_KEYLEN = 16; // AES-128

/**
 * //TODO
 * @brief 收发数据也似加解密,4个版本,消息收发中直接处理加密,并添加\n,而不需要在主程序逻辑中增加\n
 * @param plaintext
 * @param key
 * @return size_t
 *
 * @author youth (doxe20191817@gmail.com)
 * @date 2025-06-28 19:32:01
 */
size_t NetLinker::send_encode_string(const std::string& plaintext, const std::string& key)
{
    // 复用加密函数,这样数据只管交给netlinker即可.
    std::string ciphertext = encode_string(plaintext, key);
    ciphertext += '\n';
    boost::asio::write(this->sock, boost::asio::buffer(ciphertext), this->ec);
    if (ec)
	{
	    return -1;
	}
    return ciphertext.size();
}

/**
 * @brief 解密消息,传入密钥.用于短命令传输.
 *
 * @param key
 * @return std::string
 *
 * @author youth (doxe20191817@gmail.com)
 * @date 2025-06-28 20:30:38
 */
std::string NetLinker::recv_decode_string(const std::string& key)
{
    using namespace boost::asio;
    // 读消息时候,\n会留在streambuf中.
    streambuf buf;
    read_until(sock, buf, '\n', ec); // 读取到分隔符
    if (ec)
	{
	    throw boost::system::system_error(ec);
	}

    std::string ciphertext(buffers_begin(buf.data()), buffers_end(buf.data()) - 1);

    // decode_string已经处理了iv,因此返回的就是解密的消息.
    return decode_string(ciphertext, key);
}

// 关于文件传输的接口,后面再写.
bool NetLinker::send_encode_file(const std::string& file_path, const std::string& key, size_t chunk_size = 128 * 1024)
{
    // 可以考虑128KB一次传输,--> 128*1024 个字节
}

bool NetLinker::receive_decode_file(const std::string& save_path, const std::string& key) {}

/*   简要熟悉asio的收发操作.
boost中发送数据流的操作
boost::system::error_code ec;
std::string message = "Hello, server!";
size_t bytes_sent = sock.send(boost::asio::buffer(message), 0, ec); // 0 是 flags
if (ec) {
    std::cerr << "发送失败: " << ec.message() << std::endl;
} else {
    std::cout << "发送了 " << bytes_sent << " 字节。" << std::endl;
}

boost中接受数据的操作:
boost::system::error_code ec;
char data[1024]; // 使用字符数组作为缓冲区
size_t bytes_received = sock.receive(boost::asio::buffer(data), 0, ec);
if (ec == boost::asio::error::eof) { // 对端关闭连接
    std::cout << "连接已关闭。" << std::endl;
} else if (ec) {
    std::cerr << "接收失败: " << ec.message() << std::endl;
} else {
    std::string received_str(data, bytes_received); // 将接收到的数据转为字符串
    std::cout << "收到 " << bytes_received << " 字节: " << received_str << std::endl;
}
*/