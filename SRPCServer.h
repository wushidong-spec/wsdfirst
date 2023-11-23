//
// Created by LAD on 2023/11/22.
//

#ifndef UNTITLED2_SRPCSERVER_H
#define UNTITLED2_SRPCSERVER_H

#endif //UNTITLED2_SRPCSERVER_H
#include "baseheader.h"
#include "FunctionHandler.h"
#include "SThreadPool.h"
struct protobuf{
    char *buffer;
    std::string funcname;
};
class SrpcServer{
public:
    SOCKET *ServSocket=new SOCKET ;
    std::vector<SOCKET> ClientSockets;
    std::atomic<int> currentclients;
    SOCKADDR_IN socketaddr{};
    BOOL InitProtocol();
    BOOL InitSocket();
    BOOL CreateHttpServer();
    void ServerStart();
    void ClientTask(SOCKET clientsock);
    WSADATA wsaData;
    SrpcServer();
    ~SrpcServer();
private:
    FunctionHandler *functionHandler;
    SThreadPool *sThreadPool;
};
SrpcServer::SrpcServer(){
    currentclients.store(0);
    functionHandler=new FunctionHandler();
    sThreadPool=new SThreadPool();
}
void SrpcServer::ClientTask(SOCKET clientsock){
    char *buffer=new char[65535];
    while (true) {
        int buffersize = 0;
        DataStream *ds=new DataStream();
        if ((buffersize = recv(clientsock, buffer, 65535, 0)) < 0) {
            std::cout <<"客户端关闭!"<<std::endl;
            return;
        }
        DataStream fds=this->functionHandler->FaCall(ds,buffer,buffersize);
        send(clientsock,fds.streambuf->m_buffer,fds.streambuf->m_currentsize,0);
        delete ds;
    }
}
void SrpcServer::ServerStart() {
    if(this->CreateHttpServer()){
        std::cout<<"rpcserver is start"<<std::endl;
        while(true){
            SOCKADDR_IN  clientAddress{};
            int addrlength=sizeof(clientAddress);
            SOCKET clientsock = accept(*this->ServSocket,(struct sockaddr*)&clientAddress,&addrlength);
            this->ClientSockets.push_back(clientsock);
            if(this->ClientSockets.back()==-1){
                std::cerr<<"客户端连接请求失败!"<<std::endl;
                continue;
            }
            this->sThreadPool->AddTaskToThread(&SrpcServer::ClientTask,this,this->ClientSockets.back());
            if(this->currentclients.load()<100)
                this->currentclients.store(this->currentclients.load()+1);
        }
    }else{
        std::cerr<<"建立服务器失败！"<<std::endl;
    }
}
BOOL SrpcServer::InitProtocol() {
    if (WSAStartup(MAKEWORD(2, 2), &this->wsaData) != 0) {
        std::cerr << "Winsock 初始化失败！" << std::endl;
        return false;
    }else{
        return true;
    }
}
BOOL SrpcServer::InitSocket() {
    *this->ServSocket= socket(AF_INET,SOCK_STREAM,0);
    return true;
}
BOOL SrpcServer::CreateHttpServer() {
    this->InitSocket();
    this->socketaddr.sin_addr.s_addr=INADDR_ANY;
    this->socketaddr.sin_family=AF_INET;
    this->socketaddr.sin_port= htons(8080);
    if(bind(*ServSocket,(struct sockaddr*)&socketaddr, sizeof(socketaddr))==-1){
        //     this->logFormat->LogPrintf("服务器端口绑定失败！");
        std::cerr<<"服务器端口绑定失败！\n";
        closesocket(*ServSocket);
        return false;
    }
    if(listen(*this->ServSocket,10000)==-1){
        //     this->logFormat->LogPrintf("监听端口失败！");
        std::cerr<<"监听端口失败！";
        closesocket(*ServSocket);
        return false;
    }
    return true;
}
