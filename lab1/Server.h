/*
 * Copyright:   None
 * Author:      TianJincai,tianjincai@hotmail.com
 * Date:        2020.10.06
 * Description: this is a simple http server to response simple http request
 */
#ifndef LAB1_SERVER_H
#define LAB1_SERVER_H
#include <fstream>
#include <winsock.h>
#include <string>
#include <fstream>
#include <iostream>
#include <utility>
#pragma comment(lib,"ws2_32.lib")
#define MAXCONN 1
#define HTML_FILE_MAX_LEN 1024
#define IMAGE_FILE_MAX_LEN 40960
#define IN_BUFF_SIZE 1024
#define OUT_BUFF_SIZE IMAGE_FILE_MAX_LEN+IN_BUFF_SIZE
using namespace std;
/*
 * Description: the server class packet the http service using the socket
 * Member:
 *      nAddrLen:           the size of sockaddr
 *      wsaData:            the data structure initialized in WSAStartup()
 *      Socket:             the connected when browser connect to the background server
 *      connectedSocket:    the socket used in tcp connection
 *      addr:               the data structure used in bind function
 * Method:
 *      Server(int,const string&):          constructor when give the listen port and bind address
 *      void startService():                start the service in background and output the log of connection
 *      void responseRequest(sockaddr_in):  response the according to different request
 */
class Server{
public:
    Server()=default;
    Server(int bindPort,in_addr bindAddr,string const &mainDir);
    void startService();
private:
    int nAddrLen=sizeof(sockaddr);
    WSAData wsaData{};
    SOCKET Socket{},connectedSocket{};
    sockaddr_in addr{};
    void responseRequest(sockaddr_in) const;
    string mainDir;
};
/*
 * Description: this function create a http response header according to given parameter
 * Parameter:
 *      stateConde:     state code integer,200,403,404...
 *      state:          state description, OK, Not Found ...
 *      contentType:    response data type, image,text ...
 * Example:
 *      constructRequestHeader(404,"Not Found","text/html")
 *      =>
 *      "
 *      HTTP/1.1 404 Not Found
 *      Server: JackTian's Server
 *
 *
 *      (data..)
 *      "
 */
string constructRequestHeader(int stateCode,const string& state,const string& contentType)
{
    return "HTTP/1.1 "+to_string(stateCode)+" "+state+"\n"+ //
           +"Content-Type: "+contentType+"\nServer: JackTian's Server\n\n";
}
/*
 * Description: read a html file and return it as string object
 * Parameter:
 *      dir: the relative directory to .exe file
 */
string readHtmlFile(const string& dir)
{
    int len;
    char fileBuf[HTML_FILE_MAX_LEN];
    ifstream in(dir);
    in.read(fileBuf,HTML_FILE_MAX_LEN);
    len=(int)in.gcount();
    string fileContent(fileBuf,len);
    return fileContent;
}
/*
 * Description:return the length of a file(in byte)
 * Parameter:
 *      dir: the relative directory to .exe file
 */
int getFileLength(const string& dir)
{
    int len;
    ifstream in(dir,ios::in|ios::binary);
    in.seekg(0,ios::end);
    len = (int)in.tellg();
    in.close();
    return len;
}
/*
 * Description: return the file suffix of a file dir
 * Example:
 *      getFileLength("/hust.jpg")=>"jpg"
 */
string getSuffix(const string& dir)
{
    int pointPos=dir.find_first_of('.');
    return dir.substr(pointPos+1,dir.length());
}
/*
 * Description: the constructor of Server class
 * Parameter:
 *      bindPort: the http port to be listened
 *      bindAddr: the http address to be bind
 *      mainDir:  main directory of this web server
 * Example:
 *      Server s=Server(8080,"127.0.0.1");
 *      s.startService();
 *      =>
 *      you can visit web page in http://127.0.0.1:8080/index.html
 */
Server::Server(int bindPort,in_addr bindAddr,const string &mainDir) {
    this->mainDir=mainDir;
    if(WSAStartup(0x0101,&this->wsaData))
    {
        cout<<"Server initialize error!\n";
        exit(-1);
    }
    if(this->wsaData.wVersion!=0x0101)
    {
        cout<<"Server win sock version error\n";
        WSACleanup();
    }
    cout<<"Server initialize successfully\n";
    this->Socket=socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
    if(this->Socket==INVALID_SOCKET)
    {
        cout<<"Can't create a new socket\n";
        exit(-1);
    }
    cout<<"Socket create successfully\n";
    this->addr.sin_family=AF_INET;
    this->addr.sin_port=htons(bindPort);
    this->addr.sin_addr=bindAddr;
    if(bind(this->Socket,(LPSOCKADDR)&(this->addr),sizeof(this->addr))==SOCKET_ERROR)
    {
        cout<<"Server socket bind error\n";
        closesocket(this->Socket);
        WSACleanup();
        exit(-1);
    }
    cout<<"Server socket bind port successfully\n";
    if(listen(Socket,MAXCONN)==SOCKET_ERROR)
    {
        cout<<"Server socket lister error\n";
        closesocket(this->Socket);
        WSACleanup();
        exit(-1);
    }
    cout<<"Server socket listen successfully\n";
}
/*
 * Description: this method will run a service in the background after the initialization
 */
void Server::startService() {
    sockaddr_in clientAddr{};
    cout<<"Wait for client connect...\n";
    while (true)
    {
        this->connectedSocket=accept(this->Socket,(LPSOCKADDR)&clientAddr,&nAddrLen);
        if(this->connectedSocket==INVALID_SOCKET)
        {
            cout<<"Server accept connection request error\n";
            closesocket(Socket);
            WSACleanup();
            exit(-1);
        }
        char *ip = inet_ntoa(clientAddr.sin_addr);
        cout<<"connection from "<<ip<<":"<<clientAddr.sin_port<<endl;
        this->responseRequest(clientAddr);
    }
}
/*
 * Description: this method can receive the request from local browser and send response to it
 * Parameter:
 *      clientAddr: cause we use the sendto function, so the addr of client must be saved and used in sendto function
 */
void Server::responseRequest(sockaddr_in clientAddr) const {
    char inBuffer[IN_BUFF_SIZE];
    char outBuffer[OUT_BUFF_SIZE];
    string request;
    recv(this->connectedSocket,inBuffer,IN_BUFF_SIZE,0);
    request=inBuffer;
    int firstSpacePos=request.find_first_of(' ');
    int secondSpacePos=request.find_first_of(' ',firstSpacePos+1);
    string requestDir=request.substr(firstSpacePos+1,secondSpacePos-firstSpacePos-1);
    ifstream in("../"+this->mainDir+requestDir);
    if (!in)
    {
        cout<<"GET "+requestDir<<":";
        cout<<"404 Not Found"<<endl;
        string response=constructRequestHeader(404,"Not Found","text/html");
        response+=readHtmlFile("../lab1/resources/404.html");
        strcpy_s(outBuffer,OUT_BUFF_SIZE,response.c_str());
        sendto(this->connectedSocket,outBuffer,response.length(),0,(LPSOCKADDR)&clientAddr,nAddrLen);
        closesocket(this->connectedSocket);
    }
    else
    {
        if(getSuffix(requestDir)=="html")
        {
            cout<<"GET "+requestDir<<":";
            cout<<"200 OK"<<endl;
            string response=constructRequestHeader(200,"OK","text/html");
            response+=readHtmlFile("../"+this->mainDir+requestDir);
            strcpy_s(outBuffer,OUT_BUFF_SIZE,response.c_str());
            sendto(this->connectedSocket,outBuffer,response.length(),0,(LPSOCKADDR)&clientAddr,nAddrLen);
            closesocket(this->connectedSocket);
        }
        else
        {
            cout<<"GET "+requestDir<<":";
            cout<<"200 OK"<<endl;
            int imgSize=getFileLength("../"+this->mainDir+requestDir);
            char imgBuf[IMAGE_FILE_MAX_LEN];
            string response=constructRequestHeader(200,"OK","image/jpg");
            ifstream in("../"+this->mainDir+requestDir,ios::binary);
            in.read(imgBuf,imgSize);
            int len=imgSize+response.length();
            if(len>OUT_BUFF_SIZE)
            {
                cout<<"Buffer overflow!\n";
                closesocket(this->connectedSocket);
                exit(-1);
            }
            strcpy_s(outBuffer,OUT_BUFF_SIZE,response.c_str());
            memcpy_s(outBuffer+response.length(),imgSize,imgBuf,imgSize);
            sendto(this->connectedSocket,outBuffer,len,0,(LPSOCKADDR)&clientAddr,nAddrLen);
            closesocket(this->connectedSocket);
        }
    }
}
#endif //LAB1_SERVER_H
