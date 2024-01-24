#include <iostream>
#include "SThreadPool.h"
#include <tuple>
#include <utility>
#include "SRpcNetwork.h"
#include <unistd.h>
#include <pthread.h>
int main(){
    SThreadPool *threadPool=new SThreadPool();
    SrpcServer *srpcServer=new SrpcServer();
    threadPool->AddTaskToThread(&SThreadPool::AddTasktest,threadPool);
    threadPool->AddTaskToThread(&SrpcServer::ServerStart,srpcServer);
    threadPool->timer->m_timethread.join();
    return 0;
}

