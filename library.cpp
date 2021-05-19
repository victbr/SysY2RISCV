#include "utils.hpp"

extern std::map<std::string, bool> FuncRet; //didn't clean because it change with IdentFunc. Can not use alone

void addLibFunc(){
	FuncRet["getint"] = true;
	FuncRet["getch"] = true;
	FuncRet["getarray"] = true;
	FuncRet["putint"] = false;
	FuncRet["putch"] = false;
	FuncRet["putarray"] = false;
	FuncRet["starttime"] = false;
	FuncRet["stoptime"] = false;
/*
计时函数
SysY 运行时库提供 starttime 和 stoptime “函数”, 用于测量 SysY 中某段代码的运行时间. 在一个 SysY 程序中, 可以插入多对 starttime, stoptime 调用, 以此来获得每对调用之间的代码的执行时长, 并在 SysY 程序执行结束后得到这些计时的累计执行时长.

你需要注意两件事:

在 sylib.h 中, starttime 和 stoptime 并未被定义为函数的形式, 而是宏定义的形式 (这种奇怪的定义方式也许出于编译系统设计赛委员会某种特殊考虑? 详情我们不得而知). SysY 中, 对 starttime 和 stoptime 的调用:

starttime();
stoptime();
会被展开为:

_sysy_starttime(line_no_1);
_sysy_stoptime(line_no_2);
其中, line_no_1 和 line_no_2 是调用处的代码行号. 所以, 你的编译器在遇到 starttime 或 stoptime 时, 需要将其处理为对函数 _sysy_starttime 或 _sysy_stoptime 的调用, 并传入正确的参数.

starttime 和 stoptime 只会出现在课程提供的性能测试用例中.

starttime, stoptime 不支持嵌套调用的形式, 即不支持:

starttime();
...
starttime();
...
stoptime();
...
stoptime();
这样的调用执行序列.

下面分别介绍所提供的计时函数的访问接口.

starttime
函数声明: 该符号为宏定义, 展开后的函数为 _sysy_starttime, 声明为 void _sysy_starttime(int).

描述: 开启计时器. 此函数应和 stoptime() 联用.

stoptime
函数声明: 该符号为宏定义, 展开后的函数为 _sysy_stoptime, 声明为 void _sysy_stoptime(int).

描述: 停止计时器. 此函数应和 starttime() 联用.

程序会在最后结束的时候, 整体输出每个计时器所花费的时间, 并统计所有计时器的累计值. 格式为 Timer#编号@开启行号-停止行号: 时-分-秒-微秒.
*/
}
