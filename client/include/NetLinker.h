#pragma once
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

/**
 * @brief 处理网络连接,数据收发的类,不涉及加密,只负责传输密文
 *
 *
 * @author youth (doxe20191817@gmail.com)
 * @date 2025-06-28 13:33:02
 */
class NetLinker
{
  public:
    NetLinker();
    ~NetLinker();

    // 失败是抛出异常,但是这个函数会返回false.
    bool connect(const std::string& ip, uint16_t port);

    void close();

    // bool send_data(const std::string& msg);
    size_t send_encode_string(const std::string& plaintext, const std::string& key);
    std::string recv_decode_string(const std::string& key);
    bool send_encode_file(const std::string& file_path, const std::string& key, size_t chunk_size);
    bool receive_decode_file(const std::string& save_path, const std::string& key);

  private:
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::socket sock;
    boost::system::error_code ec;
};
