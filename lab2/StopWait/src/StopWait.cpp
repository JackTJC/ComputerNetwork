// StopWait.cpp : 定义控制台应用程序的入口点。
//


#include "Global.h"
#include "StopWaitRdtSender.h"
#include "StopWaitRdtReceiver.h"
//#define PC
#ifdef PC
#define INPUT "/mnt/c/Users/Administrator/Desktop/ComputerNetwork/lab2/input.txt"
#define OUTPUT "/mnt/c/Users/Administrator/Desktop/ComputerNetwork/lab2/output.txt"
#else
#define INPUT "/mnt/c/Users/JackTian/Development/Program/CXXProject/ComputeNetwork/lab2/input.txt"
#define OUTPUT "/mnt/c/Users/JackTian/Development/Program/CXXProject/ComputeNetwork/lab2/output.txt"
#endif

int main(int argc, char* argv[])
{
	RdtSender *ps = new StopWaitRdtSender();
	RdtReceiver * pr = new StopWaitRdtReceiver();
	pns->setRunMode(0);  //VERBOS模式
//	pns->setRunMode(1);  //安静模式
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile(INPUT);
	pns->setOutputFile(OUTPUT);

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
	delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
	
	return 0;
}

