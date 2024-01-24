#include <iostream>
#include "SThreadPool.h"
#include <tuple>
#include <utility>
#include "SRpcNetwork.h"
#include <unistd.h>  

int main(){
    SrpcServer *srpcClient=new SrpcServer();
    srpcClient->StartClient();
    sleep(5);
    int a=3;
   while(true){
       RValue<int> result = srpcClient->functionHandler->ClientCall<int>("testfunction",a);
       a++;
       std::cout<<"返回值："<<result.result<<std::endl;
       sleep(1);
    }
    sleep(10);
    close(srpcClient->clientsocket);
    return 0;
}
