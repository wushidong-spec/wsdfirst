#include <iostream>  
#include <sys/socket.h>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <fcntl.h>
#include <chrono>  
#include <cstring>  
  
const int BUFFER_SIZE = 65535;  
struct FileHeader{
   long int file_size;
   char filename[50];
};
int mainc() {  
    int sock = socket(AF_INET, SOCK_STREAM, 0);  
    struct sockaddr_in server;  
    char buffer[BUFFER_SIZE];  
    int bytes_received;  
    int file_fd;  
  
    // 设置服务器IP地址和端口号  
    server.sin_addr.s_addr = inet_addr("127.0.0.1");  
    server.sin_family = AF_INET;  
    server.sin_port = htons(8083);  
  
    // 连接到服务器  
    if (connect(sock, (const sockaddr *)&server, sizeof(server)) < 0) {  
        std::cout<<"connect failed"<<std::endl;  
        return -1; 
    }  
  
    // 创建文件并写入接收到的数据  
    file_fd = open("received_file.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);  
    if (file_fd < 0) {  
        std::cout<<"open failed"<<std::endl;  
        return -1;
    }  
    int count=0;
    FileHeader fileheader;
    int headbytes=recv(sock,(char*)&fileheader,sizeof(FileHeader),0);
    long int sumbytes=0;
    auto start = std::chrono::system_clock::now();  
    // 接收数据并写入文件中  
    while ((bytes_received = recv(sock, buffer, BUFFER_SIZE+1,0)) > 0) {
       // std::cout<<bytes_received<<std::endl;
        write(file_fd, buffer, bytes_received);
        sumbytes+=bytes_received;
        if(sumbytes==fileheader.file_size){
           std::cout<<"接收完毕!"<<std::endl;
           break;
        }
       // memset(buffer,0,sizeof(buffer));  
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> time_difference=end-start;
    std::cout<<"耗时："<<time_difference.count()<<" s"<<std::endl;
    std::cout<<"传输速率:"<<double((fileheader.file_size/(1024*1024))/time_difference.count())<<" Mi/s"<<std::endl;
    std::cout<<fileheader.filename<<std::endl; 
    std::cout<<"总大小："<<(fileheader.file_size*1.0)/(1024*1024*1024*1.0)<<" Gi"<<std::endl; 
    // 关闭文件和socket描述符  
    close(file_fd);  
    close(sock);  
  
    return 0;  
}
