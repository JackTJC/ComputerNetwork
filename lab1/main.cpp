#include "Server.h"
int main()
{
    //bind self-defined ip
    in_addr bindAddr={127,0,0,1};
    //bind all network card
//    in_addr bindAddr;
//    bindAddr.S_un.S_addr=INADDR_ANY;
    Server s=Server(80,bindAddr,"lab1/resources2/");
    s.startService();
}