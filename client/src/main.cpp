#include "../include/user.h"
#include <iostream>
#include <limits>

int main()
{
    User user;
    std::cout << "欢迎来到我的数据库.请选择你的身份:\n";
    std::cout << "1) 登陆(l/L)\n2) 注册(r/R)\n3) 退出(q/Q)";
    char select;
    while (std::cin >> select)
	{
	    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	    if (select == 'r' || select == 'R')
		{
		    // rigester();
		}
	    else if (select == 'l' || select == 'L')
		{
		    // 登陆
		    int ans = user.Login();
		    if (ans == 0)
			{
			    user.eventloop();
			    // success
			}
		    else if (ans == 1)
			{
			    // nouser
			}
		    else
			{
			    // timeout
			}
		}
	    else if (select == 'q' || select == 'Q')
		{
		    std::cout << "江湖再见\n";
		    break;
		}
	    else
		{
		    std::cout << "无效的输入指令,请重新输入\n";
		    std::cout << "1) 登陆(l/L)\n2) 注册(r/R)\n3) 退出(q/Q)";
		    continue;
		}
	}
    return 0;
}