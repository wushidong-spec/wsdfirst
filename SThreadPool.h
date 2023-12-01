#ifndef UNTITLED1_STHREADPOOL_H
#define UNTITLED1_STHREADPOOL_H
#include "TaskQueue.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include "STimer.h"
#include <unistd.h>
#include "server.cpp"
//#include "SRpcNetwork.h"
#define MIN_THREADS 2

struct TaskThread{
    std::thread workthread;
    bool isworking;
    TaskThread(std::function<void()> wthread,bool isactive){
        this->workthread=std::thread(wthread);
        this->isworking=isactive;
    }
};

class SThreadPool {
public:
    TaskQueue *taskQueue;
    std::mutex workmutex;
    std::condition_variable con_var;
    SThreadPool();
    ~SThreadPool();
    void WorkThread();
    Info CreateWorkThreads();
    Info InitThreadPool();
    void AddTasktest();
    inline bool CheckThreadId(std::thread::id threadid,bool isactive);
    template<typename Func,typename... Args>
    Info AddTaskToThread(Func&& func,Args&&... args);
    template<typename Func,typename Class,typename... Args>
    Info AddTaskToThread(Func&& func,Class&& c ,Args&&... args);
    std::vector<TaskThread> sworkthreads;
    std::thread manageThread;
    void test(int b);
    void ManageThread();
    void ManageTask();
    std::atomic<int> currentworkingthreads;
    STimer *timer;
    TcpServer *tcpServer=new TcpServer();
};
inline bool SThreadPool::CheckThreadId(std::thread::id threadid,bool isactive){
    for(int i=0;i<this->sworkthreads.size();i++) {
        if(this->sworkthreads[i].workthread.get_id()==threadid){
            this->sworkthreads[i].isworking= isactive;
            return true;
        }
    }
    return false;
}
void SThreadPool::AddTasktest() {
    int b=0;
    int a=0;
    this->AddTaskToThread(&TcpServer::mains,tcpServer);
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    while (a==0) {
        while (b < 16) {
            this->AddTaskToThread(&SThreadPool::test, this, 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(600));
            b++;
        }
        a++;
        b=0;
        std::this_thread::sleep_for(std::chrono::seconds(20));
    }
    return;
}
void SThreadPool::ManageTask() {
    std::unique_lock<std::mutex> lck(this->workmutex);
    if(this->currentworkingthreads.load()>0) {
        while (float(this->sworkthreads.size()) / float(this->currentworkingthreads.load()) < 1.3 && this->sworkthreads.size() < 10) {
            this->sworkthreads.push_back(TaskThread(std::bind(&SThreadPool::WorkThread, this), false));
            this->sworkthreads.back().workthread.detach();
            std::cout<<"create thread:"<<this->sworkthreads.size()<<std::endl;
        }
        for(auto workthread=this->sworkthreads.begin();workthread!=this->sworkthreads.end()&&
                                                       float(this->sworkthreads.size()) / float(this->currentworkingthreads.load()) >= 2 && this->sworkthreads.size() <= 10&&this->sworkthreads.size()>MIN_THREADS
                ;){
            if(!workthread->isworking) {
                std::cout << "pop thread1" << workthread->workthread.native_handle()<<std::endl;
                workthread=this->sworkthreads.erase(workthread);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }else{
                ++workthread;
            }
        }
    }
    if(this->currentworkingthreads.load()==0&&this->sworkthreads.size()>MIN_THREADS){
        for(auto workthread=this->sworkthreads.begin();workthread!=this->sworkthreads.end()
                                                       &&this->sworkthreads.size()>MIN_THREADS;){
            std::cout<<sworkthreads.size()<<std::endl;
            if(!workthread->isworking){
                std::cout << "pop thread2" << workthread->workthread.native_handle()<<std::endl;
                workthread=this->sworkthreads.erase(workthread);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }else{
                ++workthread;
            }
        }
    }
    std::cout<<"current threads num:"<<this->sworkthreads.size()<<"->working threads num:"<<this->currentworkingthreads.load()<<std::endl;
    lck.unlock();
    std::cout<<"lunxun"<<std::endl;
    return;
}
void SThreadPool::ManageThread() {
    timer->SetTimer(5000,&SThreadPool::ManageTask, this);
    timer->Start();
}
template<typename Func,typename... Args>
Info SThreadPool::AddTaskToThread(Func &&func, Args&&... args) {
    try {
        WTask *task=new WTask();
        static_assert(std::is_invocable_v<Func, Args...>,"Invalid function signature");
        task->SetWTask(std::forward<Func>(func),std::forward<Args>(args)...);
        this->taskQueue->taskQueue.push_back(task);
        this->con_var.notify_one();
    }catch(std::exception e){
        std::cerr<<e.what()<<std::endl;
        return FillInfo(false,std::string(e.what()));
    }
    return FillInfo(true);
}
template<typename Func,typename Class,typename... Args>
Info SThreadPool::AddTaskToThread(Func &&func,Class&& c ,Args&&... args) {
    try {
        static_assert(std::is_invocable_v<Func,Class, Args...>,"Invalid function signature");
        WTask *task=new WTask();
        task->SetWTask(std::forward<Func>(func),std::forward<Class>(c),std::forward<Args>(args)...);
        this->taskQueue->taskQueue.push_back(task);
        this->con_var.notify_one();
    }catch(std::exception e){
        std::cerr<<e.what()<<std::endl;
        return FillInfo(false,std::string(e.what()));
    }
    return FillInfo(true);
}
void SThreadPool::test(int b){
    int a=0;
    std::cout<<"b:"<<b<<std::endl;
    while (a<15) {
        std::cout << "Implementation progress:" <<a<<std::endl;
        a++;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return;
}
Info SThreadPool::CreateWorkThreads() {
    try {
        this->sworkthreads.reserve(MIN_THREADS+1);
        for(int i=0;i<MIN_THREADS;i++) {
            this->sworkthreads.push_back(TaskThread(std::bind(&SThreadPool::WorkThread,this), false));
            this->sworkthreads.back().workthread.detach();
            std::cout<<"create thread:"<<this->sworkthreads.size()<<"->thread id:"<<this->sworkthreads.back().workthread.get_id()<<std::endl;
            sleep(1);
        }
    }catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
        return FillInfo(false, std::string(e.what()));
    }
    return FillInfo(true);
}
void SThreadPool::WorkThread() {
    while(true) {
        std::unique_lock<std::mutex> uniqueLock(this->workmutex);
        if (this->taskQueue->taskQueue.empty()) {
            this->CheckThreadId(std::this_thread::get_id(), false);
            this->con_var.wait(uniqueLock);
            this->CheckThreadId(std::this_thread::get_id(), true);
            uniqueLock.unlock();
        } else {
            WTask *currenttask = this->taskQueue->taskQueue.back();
            this->taskQueue->taskQueue.pop_back();
            this->currentworkingthreads.store(this->currentworkingthreads.load()+1);
            uniqueLock.unlock();
            currenttask->task();
            if(this->currentworkingthreads.load()!=0)
                this->currentworkingthreads.store(this->currentworkingthreads.load()-1);
        }
    }
}
Info SThreadPool::InitThreadPool() {
    this->CreateWorkThreads();
    return FillInfo(true);
}
SThreadPool::SThreadPool(){
    this->currentworkingthreads.store(0);
    this->taskQueue=new TaskQueue();
    this->timer=new STimer();
    this->InitThreadPool();
    this->ManageThread();
}


#endif //UNTITLED1_STHREADPOOL_H
