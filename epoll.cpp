#include <iostream>
#include <string>
#include <sys/epoll.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <stdio.h>  
#include <unistd.h>
int main(){
    int epollfd,serversocket;
    struct epoll_event event;
    serversocket=socket(AF_INET, SOCK_STREAM, 0);
    if(serversocket<0){
        std::cerr<<"初始化失败!"<<std::endl;
        return -1;
    }
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8081);
    if(bind(serversocket,reinterpret_cast<sockaddr*>(&serverAddr),sizeof(serverAddr))==-1){
        std::cerr<<"绑定失败!"<<std::endl;
        return -1;
    }
    if(listen(serversocket,1000)==-1){
        std::cerr<<"监听失败"<<std::endl;
        return -1;
    }
    event.events=EPOLLIN | EPOLLET;
    event.data.fd=serversocket;
    epollfd = epoll_create1(0);
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, serversocket, &event) == -1){
        std::cerr<<"epoll内部错误！"<<std::endl;
        close(epollfd);
        close(serversocket);
        return -1;
    }
    while(true){
        epoll_event events[1000];
        int numEvents = epoll_wait(epollfd, events, 1000, -1);
        if(numEvents<0)
	{
	    std::cerr<<"文件描述符监听错误!"<<std::endl;
	    return -1;
	}
        for(int i=0;i<numEvents;++i){
            if(events[i].data.fd==serversocket){
		sockaddr_in clientAddr{};
	        socklen_t clientAddrLen = sizeof(clientAddr);
                int clientsock=accept(serversocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
                if(clientsock<0){
                    std::cout<<"客户端连接失败！"<<std::endl;
		    return -1;
                }
                event.events=EPOLLIN;
                event.data.fd=clientsock;
		if(epoll_ctl(epollfd,EPOLL_CTL_ADD,clientsock,&event)==-1){
		    std::cout<<"添加客户端epoll失败！"<<std::endl;
                    close(clientsock);
		    return -1;
		}
            }
        }
    }
    return 0;
}
