#include <iostream>
#include <winsock.h>
#include "Server.h"
#pragma comment(lib,"ws2_32.lib")
#define MAXCONN 1
using namespace std;
int main()
{
    Server s=Server(80,"127.0.0.1");
    s.startService();
}