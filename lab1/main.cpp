#include "Server.h"
int main()
{
    //bind self-defined ip
    //the following bind the local host (127.0.0.1)
    in_addr bindAddr={127,0,0,1};
    //bind all network card
//    in_addr bindAddr;
//    bindAddr.S_un.S_addr=INADDR_ANY;
    Server s=Server(8080,bindAddr,"lab1/resources2/");
    s.startService();
}