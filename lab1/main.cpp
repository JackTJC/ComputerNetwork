#include "Server.h"
int main()
{
    //bind self-defined ip
    //the following bind the local host (127.0.0.1)
    //192.168.137.1
    in_addr bindAddr={127,0,0,4};
    //bind all network card
//    in_addr bindAddr;
//    bindAddr.S_un.S_addr=INADDR_ANY;
    Server s=Server(8080,bindAddr,"resources1");
    s.startService();
}