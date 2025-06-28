#include <chrono>
#include <iostream>
#include <jwt-cpp/jwt.h>
// #include <mysql/mysql.h>

int main()
{

    // std::cout << "欢迎来到我的文件传输系统......\n";
    //  MYSQL* conn = mysql_init(NULL);
    //  if (!conn)
    //  {
    //      std::cerr << "mysql_init failed\n";
    //      return 1;
    //  }

    // if (!mysql_real_connect(conn, "localhost", "youth", "111111", "file_sys", 0, NULL, 0))
    // {
    //     std::cerr << "Connection failed: " << mysql_error(conn) << "\n";
    //     return 1;
    // }

    // std::cout << "Connected!\n";
    // mysql_close(conn);
    // 模拟登录后：签发 token
    std::string token = jwt::create()
			    .set_issuer("server")
			    .set_id("doxe")
			    .set_type("JWT")
			    .set_subject("user123")
			    .set_payload_claim("role", jwt::claim(std::string("admin")))
			    .set_expires_at(std::chrono::system_clock::now() + std::chrono::minutes(1))
			    .sign(jwt::algorithm::hs256{"youth-doxe"});

    std::cout << "签发的 JWT token:\n" << token << "\n\n";
    // std::chrono::minutes();
    //   模拟客户端请求带 token，服务器验证
    try
	{
	    // 解码
	    auto decoded = jwt::decode(token);

	    // 验证,创建一个验证器verifier,使用HMAC-SHA256算法验证,"secret_key"就是上面token签发的密钥.
	    auto verifier = jwt::verify().allow_algorithm(jwt::algorithm::hs256{"youth-doxe"}).with_issuer("server").with_id("doxe");

	    verifier.verify(decoded); // 如果验证失败会抛异常

	    std::cout << "Token 验证通过!\n";

	    // 读取信息
	    std::cout << "用户ID: " << decoded.get_subject() << "\n";
	    std::cout << "角色: " << decoded.get_payload_claim("role").as_string() << "\n";
	}
    catch (const std::exception& e)
	{
	    std::cerr << "Token 验证失败: " << e.what() << "\n";
	}

    return 0;
}