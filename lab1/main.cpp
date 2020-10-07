#include "Server.h"
int main()
{
    in_addr bindAddr={127,0,0,1};
//    in_addr bindAddr;
//    bindAddr.S_un.S_addr=INADDR_ANY;
    Server s=Server(80,bindAddr,"lab1/resources/");
    s.startService();
}