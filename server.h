#include <iostream>  
#include <winsock2.h>
#include <windows.h>
#include <unistd.h>  
#include <fcntl.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <csignal>
const int BUFFER_SIZE = 20000;  

struct FileHeader{
  long int file_size;
  char filename[50];
};
void mainserver() {
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        std::cerr << "Winsock 初始化失败！" << std::endl;
        return;
    }
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;  
    int addrlen = sizeof(address);  
    char buffer[BUFFER_SIZE];  
    int bytes_received;  
  
    // 创建socket文件描述符  
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {  
        std::cout<<"socket failed"<<std::endl;  
        return;
    }  
  
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons(8083);  
  
    // 绑定socket到指定的IP地址和端口号  
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cout<<"bind failed"<<std::endl;  
        return;
    }  
  
    // 监听连接请求  
    if (listen(server_fd, 1000) < 0) {  
        std::cout<<"listen failed"<<std::endl;  
        return; 
    }
    int file_fd;
    std::cout<<"server start!"<<std::endl;
  while(true){ 
    // 接受客户端连接请求  
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
        std::cout<<"accept failed"<<std::endl;  
        return;
    }  
    std::cout<<"你好"<<std::endl; 
    // 打开要传输的文件  
   int count=0;
   struct stat statbuf;
   file_fd=open("large_file.txt", O_RDONLY);
   if(fstat(file_fd,&statbuf)<0){
       std::cout<<"获取文件大小失败"<<std::endl;
       return;
   }
   std::cout<<statbuf.st_size<<std::endl;
   FileHeader fileheader;
   fileheader.file_size=statbuf.st_size;
   strcpy(fileheader.filename,"autoparrot.tar.gz");
   write(new_socket,(char*)&fileheader,sizeof(fileheader));
    // 读取文件内容并发送给客户端  
    while ((bytes_received = read(file_fd, buffer, BUFFER_SIZE)) > 0) {  
      //  std::cout<<bytes_received<<std::endl;
        write(new_socket, buffer, bytes_received);  
    } 
    close(file_fd);
  }
   std::cout<<"咋了"<<std::endl;
    // 关闭文件和socket描述符  
    close(new_socket);  
    close(server_fd);  
  
    return;  
}
