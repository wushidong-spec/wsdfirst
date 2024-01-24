

#ifndef UNTITLED2_SRPCSERVER_H
#define UNTITLED2_SRPCSERVER_H

#endif //UNTITLED2_SRPCSERVER_H
#include "baseheader.h"
#include "FunctionHandler.h"
#include <sys/socket.h>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>  
#include <netinet/in.h>
#include "Signal.h"
struct protobuf{
    char *buffer;
    std::string funcname;
};
typedef int SOCKET;
typedef bool BOOL;
class SrpcServer:public FunctionHandler{
public:
    SOCKET *ServSocket=new SOCKET;
    SOCKET clientsocket;
    std::vector<SOCKET> ClientSockets;
    std::atomic<int> currentclients;
    struct sockaddr_in socketaddr;
    BOOL InitSocket();
    BOOL CreateHttpServer();
    void exitsignalHandler(int signum);
    void ServerStart();
    void StartClient();
    void ClientTask(SOCKET clientsock);
    std::mutex clientmutex;
    SrpcServer();
    ~SrpcServer();
    struct epoll_event serverevent;
    struct epoll_event events[1000];
    int epollfd;
//private:
    FunctionHandler *functionHandler;
    SThreadPool *sThreadPool;
};
int testfunction(int a){
   std::cout<<"这是："<<a<<std::endl;
   return a+3;
}
SrpcServer::SrpcServer(){
    currentclients.store(0);
    functionHandler=new FunctionHandler();
}
void SrpcServer::StartClient() {
    if((this->clientsocket=socket(AF_INET,SOCK_STREAM,0))<0){
        std::cerr<<"socket init failed!"<<std::endl;
        return;
    }
    this->functionHandler->clientsock=this->clientsocket;
    std::cout<<"客户端socket:"<<this->clientsocket<<std::endl;
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 服务器IP地址
    serverAddr.sin_port = htons(8080); // 服务器端口号
    if(connect(this->clientsocket, (const sockaddr *)&serverAddr, sizeof(serverAddr))<0){
        std::cerr<<"connection failed"<<std::endl;
        return;
    }
}
void SrpcServer::ClientTask(SOCKET clientsock){
    char *buffer=new char[65535];
    std::cout<<"client:"<<clientsock<<" has connected."<<std::endl;
    DataStream *ds=new DataStream();
    while (true) {
        int buffersize = 0;
        if ((buffersize = recv(clientsock, buffer, 65535, 0)) <= 0) {
            std::cout <<"客户端关闭!"<<std::endl;
            close(clientsock);
            this->clientmutex.lock();
            for(auto iter=this->ClientSockets.begin();iter!=this->ClientSockets.end();++iter){
            	if(*iter==clientsock){
		    this->ClientSockets.erase(iter);
		    break;
		}
	    }
            std::cout<<"当前客户端数:"<<this->ClientSockets.size()<<std::endl;
	    this->clientmutex.unlock();
            delete ds;
            delete[] buffer;
            return;
        }
        if(buffersize==4&&(strcmp(buffer,"exit")==0)){
            std::cout<<"epoll句柄:"<<epollfd<<std::endl;
	    close(clientsock);
            close(*this->ServSocket);
            exitsignalHandler(0);
        }
        std::cout<<"到这了吗:"<<buffersize<<std::endl;
        this->functionHandler->FaCall(ds,buffer,buffersize);
        send(clientsock,ds->streambuf->m_buffer,ds->streambuf->m_currentsize,0);
        ds->streambuf->Clear();
    }
}
void SrpcServer::exitsignalHandler(int signum) {  
    std::cout << "Received SIGINT, exiting..." << std::endl;
    for(auto iter=this->ClientSockets.begin();iter!=this->ClientSockets.end();++iter){
        close(*iter);
    }
    close(*this->ServSocket);
    close(this->epollfd);
    exit(signum);  
}  
void SrpcServer::ServerStart() {
    if(this->CreateHttpServer()){
        signal(SIGINT,Sendsignal);
        this->serverevent.events= EPOLLIN|EPOLLET;
        this->serverevent.data.fd=*this->ServSocket;
        epollfd = epoll_create1(0);
        if(epoll_ctl(epollfd,EPOLL_CTL_ADD,*this->ServSocket,&this->serverevent)==-1){
            close(epollfd);
            close(*this->ServSocket);
	    std::cerr<<"服务器epoll监听添加失败!"<<std::endl;
	    return;
	}
        std::cout<<"rpcserver is start"<<std::endl;
        while(true){
            int numEvents = epoll_wait(epollfd, events, 1000, -1);
            if(numEvents<0){
                std::cerr<<"epoll未知错误!"<<std::endl;
                continue;
            }
	    std::cout<<"并发请求数:"<<numEvents<<std::endl;
            for(int i=0;i<numEvents;i++){
                if(this->events[i].data.fd==*this->ServSocket){
                    sockaddr_in  clientAddress;
                    int addrlength=sizeof(clientAddress);
                    SOCKET clientsock = accept(*this->ServSocket,(struct sockaddr*)&clientAddress,(socklen_t*)&addrlength);
                    if(this->clientsock<0){
                        std::cerr<<"客户端连接请求失败!"<<std::endl;
                        continue;
                    }
                    this->serverevent.events= EPOLLIN|EPOLLET;
        	        this->serverevent.data.fd=clientsock;
                    if(epoll_ctl(epollfd,EPOLL_CTL_ADD,clientsock,&this->serverevent)==-1){
		    	std::cerr<<"添加客户端epoll监听事件失败!"<<std::endl;
                        close(clientsock);
                        continue;
    		    }
		    this->ClientSockets.push_back(clientsock);
                    std::cout<<"当前客户端数:"<<this->ClientSockets.size()<<std::endl;
                    this->sThreadPool->AddTaskToThread(&SrpcServer::ClientTask,this,this->ClientSockets.back());
                    if(this->currentclients.load()<100)
                        this->currentclients.store(this->currentclients.load()+1);
	        }
	    }
        }
    }else{
        std::cerr<<"建立服务器失败！"<<std::endl;
    }
}
BOOL SrpcServer::InitSocket() {
    sThreadPool=new SThreadPool();
    functionHandler->Bind("testfunction",testfunction);
    if((*this->ServSocket= socket(AF_INET,SOCK_STREAM,0))==0)
        return false;
    return true;
}
BOOL SrpcServer::CreateHttpServer() {
    if(!this->InitSocket()){
        std::cerr<<"socket初始化错误!"<<std::endl;
        return false;
    }
    this->socketaddr.sin_addr.s_addr=INADDR_ANY;
    this->socketaddr.sin_family=AF_INET;
    this->socketaddr.sin_port= htons(8080);
    if(bind(*ServSocket,(const sockaddr*)&socketaddr, sizeof(socketaddr))<0){
        std::cerr<<"服务器端口绑定失败！\n";
        close(*ServSocket);
        return false;
    }
    if(listen(*this->ServSocket,1000)<0){
        std::cerr<<"监听端口失败！";
        close(*ServSocket);
        return false;
    }
    return true;
}

