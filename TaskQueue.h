//
// Created by LAD on 2023/11/1.
//
#include <functional>
#include <iostream>
#include <vector>
#include <map>
#include <future>
#include <tuple>
#include <queue>
#include <list>
#include <atomic>
#ifndef UNTITLED1_TASKQUEUE_H
#define UNTITLED1_TASKQUEUE_H
struct Info{
    bool success;
    std::string msg;
};
class WTask{
public:
    inline WTask();
    using TaskFunc=std::function<void()>;
    TaskFunc task;
    template<typename Func,typename... Args>
    Info SetWTask(Func&& taskFunc,Args&&... args);

    template<typename Func,typename Class,typename... Args>
    Info SetWTask(Func&& taskFunc,Class&& c,Args&&... args);
};
inline WTask::WTask() {}
inline Info FillInfo(bool success,std::string msg=""){
    Info info;
    info.success=success;
    info.msg=msg;
    return info;
}
template<typename Func,typename Class,typename... Args>
Info WTask::SetWTask(Func &&taskFunc,Class&& c,Args &&...args) {
    try {
        static_assert(std::is_invocable_v<Func,Class, Args...>,"Invalid function signature");
        this->task = std::bind(std::forward<Func>(taskFunc),std::forward<Class>(c),std::forward<Args>(args)...);
    }catch(std::exception e){
        std::cerr<<e.what()<<std::endl;
        return FillInfo(false,std::string(e.what()));
    }
    return FillInfo(true);
}
template<typename Func,typename... Args>
Info WTask::SetWTask(Func &&taskFunc, Args &&...args) {
    try {
        static_assert(std::is_invocable_v<Func, Args...>,"Invalid function signature");
        this->task = std::bind(std::forward<Func>(taskFunc),std::forward<Args>(args)...);
    }catch(std::exception e){
        std::cerr<<e.what()<<std::endl;
        return FillInfo(false,std::string(e.what()));
    }
    return FillInfo(true);
}
class TaskQueue {
public:
    std::list<WTask*> taskQueue;
    template<typename Func,typename... Args>
    Info AddTask(Func&& func,Args&&... args);
    inline Info ClearTaskQ();
    inline Info RemoveTask();
};
inline Info TaskQueue::RemoveTask() {
    try {
        this->taskQueue.pop_back();
    }catch(std::exception e){
        std::cerr<<e.what()<<std::endl;
        return FillInfo(false,std::string(e.what()));
    }
    return FillInfo(true);
}
template<typename Func,typename... Args>
Info TaskQueue::AddTask(Func &&func, Args &&...args) {
    try {
        static_assert(std::is_invocable_v<Func, Args...>,"Invalid function signature");
        WTask *task=new WTask();
        task->SetWTask(std::forward<Func>(func),std::forward<Args>(args)...);
        this->taskQueue.push_back(task);
    }catch(std::exception e){
        std::cerr<<e.what()<<std::endl;
        return FillInfo(false,std::string(e.what()));
    }
    return FillInfo(true);
}
Info TaskQueue::ClearTaskQ() {
    try {
        this->taskQueue = std::list<WTask*>();
    }catch(std::exception e){
        std::cerr<<e.what()<<std::endl;
        return FillInfo(false,std::string(e.what()));
    }
    return FillInfo(true);
}

#endif //UNTITLED1_TASKQUEUE_H
