#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <csignal>
#include <netinet/in.h>
void Sendsignal(int signum){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sigaddr;
    sigaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sigaddr.sin_family = AF_INET;
    sigaddr.sin_port = htons(8080);
    if (connect(sock, (const sockaddr *)&sigaddr, sizeof(sigaddr)) < 0) {
        std::cout<<"connect failed"<<std::endl;
        exit(-1);
        return;
    }
    send(sock,"exit",4,0);
}

