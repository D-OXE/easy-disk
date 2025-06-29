#pragma once
#include "NetLinker.h"
#include <boost/json.hpp>
#include <boost/json/object.hpp>
#include <boost/json/value.hpp>
#include <fstream>
#include <string>
#include <unordered_map>
/*
会使用的数据结构,和常量
*/

const std::string config_file = "../default_conf.json";

// 这些枚举类型操作就和在linux下的命令一样.
enum class OPERATION
{
    ERROR = -1,
    NORMAL,
    LS,
    PUSH,
    PULL,
    DELETE,
    MKDIR,
    PWD,
    CD,
};

class User
{
  public:
    User();
    ~User() = default;
    // 等待输入命令
    OPERATION getcmd();
    int Login();
    int Register();
    void eventloop();

  private:
    // ================================成员方法================================

    // 处理输入命令
    int solve_cmd(const std::string& cmd);

    //    下载文件
    int pullfile();

    //    上传文件
    int pushfile();

    //  更新当前浏览位置
    void update_current_position();

    //    获取当前浏览位置.
    std::string get_current_position();

    int connect_to_server();

    // =================================成员变量===============================
    std::unordered_map<std::string, OPERATION> m_cmdmaps; // 命令映射表.
    std::string m_current_file_position;		  // 当前浏览文件路径
    NetLinker m_netlinker;
    boost::json::object obj; // 配置文件的json库内容

    /*
	boost::json库的简单使用
	{
		"name": "Alice",
		"age": 30,
		"isStudent": false,
		"hobbies": ["reading", "hiking"]
	}
	解析
	JSON ----->>>字符串<<<------ ，
	返回一个 boost::json::value 对象
	json::value jv = json::parse(file_name);
	auto obj = jv.as_object();
	现在可以使用 jv 来访问 JSON 数据了
	std::cout << "Name: " << jv.at("name").as_string() << std::endl;
	std::cout << "Age: " << jv.at("age").as_int64() << std::endl;
	*/
};