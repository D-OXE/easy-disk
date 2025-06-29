#include "../include/user.h"
#include <fstream>
#include <iostream>
#include <string>

/**
 * @brief Construct a new User:: User object
 * 构造中解析json的配置文件,然后将相关生成的json object(类成员)构造出来.
 *
 * @author youth (doxe20191817@gmail.com)
 * @date 2025-06-28 11:24:43
 */
User::User() : m_netlinker()
{
    // 维护一个命令映射表
    m_cmdmaps["ls"] = OPERATION::LS;
    m_cmdmaps["push"] = OPERATION::PUSH;
    m_cmdmaps["pull"] = OPERATION::PULL;
    m_cmdmaps["delete"] = OPERATION::DELETE;
    m_cmdmaps["mkdir"] = OPERATION::MKDIR;
    m_cmdmaps["pwd"] = OPERATION::PWD;
    m_cmdmaps["cd"] = OPERATION::CD;

    // 打开配置文件
    std::ifstream ifs("./default_conf.json");
    if (!ifs.is_open())
	{
	    std::cerr << "无法打开配置文件\n";
	    exit(EXIT_FAILURE);
	}

    // 读取文件内容到字符串->然后 解析 JSON 字符串
    std::string json_str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    boost::system::error_code ec;
    auto config = boost::json::parse(json_str, ec);

    if (ec)
	{
	    std::cerr << "JSON 解析失败: " << ec.message() << "\n";
	    exit(EXIT_FAILURE);
	}

    // 确保 config 是对象类型
    if (!config.is_object())
	{
	    std::cerr << "JSON 配置文件不是一个对象\n";
	    exit(EXIT_FAILURE);
	}

    // // 安全获取字段,然后创建链接.
    this->obj = config.as_object();

    std::string ip(obj.at("server-ipaddr").as_string());
    auto port = obj.at("server-port").as_int64();

    this->m_netlinker.connect(ip, port);
}