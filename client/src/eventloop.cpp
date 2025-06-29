#include "../include/user.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/**
 * @brief 本文件包含enevtloop,getcmd两个方法的实现.因为这两个是一起组合使用的
 *
 *
 * @author youth (doxe20191817@gmail.com)
 * @date 2025-06-29 12:25:56
 */

/**
 * @brief 登陆之后的事件循环函数,所有的输入,输出,都将在这里面执行.配合getcmd使用.
 *
 *
 * @author youth (doxe20191817@gmail.com)
 * @date 2025-06-29 12:11:31
 */
void User::eventloop()
{
    while (true)
	{
	    std::cout << "user>>";
	    auto status = this->getcmd();
	}
    return;
}

/**
 * @brief 处理输入,返回对应int变量表示命令的类型.
 *
 * @return int ----> 返回值就是下列的枚举类型.
 *

enum class OPERATION
{
    ERROR = -1,
    NORMAL, //0
    LS,		//1
    PUSH,	//2
    PULL,	//3
    DELETE,	//4
    MKDIR,	//5
    PWD,	//6
    CD,		//7
};
 * @details 只有pwd和ls这两个命令直接发送到服务器.剩下的命令都含有参数,在eeventloop中跳转到别的位置执行具体逻辑.
 * @author youth (doxe20191817@gmail.com)
 * @date 2025-06-29 12:27:43
 */
OPERATION User::getcmd()
{
    std::vector<std::string> parameter;
    std::string cmdstirng;
    std::string singlecmd;

    std::getline(std::cin, cmdstirng);

    std::istringstream iss(cmdstirng); // 获取命令然后分割
    while (std::getline(iss, singlecmd, ' '))
	{
	    parameter.push_back(singlecmd);
	}

    switch (this->m_cmdmaps[parameter[0]])
	{
	case OPERATION::LS:
	    this->m_netlinker.send_encode_string("ls", std::string(this->obj.at("key").as_string()));
	    break;
	case OPERATION::PUSH:
	    // 上传一个文件,或者一群文件
	    break;
	case OPERATION::PULL:
	    // 下载一个文件,或者一群文件
	    break;
	case OPERATION::DELETE:
	    // 删除一个文件,或者一群文件
	    break;
	case OPERATION::MKDIR:
	    // 创建一个新的虚拟文件夹映射在服务器的数据库中
	    break;
	case OPERATION::PWD:
	    // 获取当前虚拟文件路径,一个数据库的列的值
	    this->m_netlinker.send_encode_string("pwd", std::string(this->obj.at("key").as_string()));
	    break;
	case OPERATION::CD:
	    // 切换当前虚拟文件路径到合法的虚拟路径位置.
	    break;
	default:
	    // 命令不存在,直接结束这次操作.
	    return OPERATION::ERROR;
	}
}