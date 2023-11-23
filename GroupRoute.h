//
// Created by LAD on 2023/7/27.
//

#ifndef PROJECT1_GROUPROUTE_H
#define PROJECT1_GROUPROUTE_H
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "HttpContext.h"
struct RouteFuncMap{
    std::map<std::string,std::function<void(HttpContext*)>> routefuncmap;
};
class GroupRoute {
public:
    std::string MainRoute;
    RouteFuncMap *routeFuncMap;
    HttpContext *httpcontext;
    template<typename F,typename C>
    void AddRoute(std::string routepath,F func,C classname);
    template<typename F>
    void AddRoute(std::string routepath,F func);
    GroupRoute AddGroupRoute(std::string mainroute);
    bool Routecheck(std::string routepath);
    void Test(HttpContext *httpContext);
    GroupRoute();
    ~GroupRoute();
};


template<typename F,typename C>
void GroupRoute::AddRoute(std::string routepath, F func, C classname) {
    if(std::is_member_function_pointer<F>::value && this->Routecheck(routepath)) {
        auto iter = this->routeFuncMap->routefuncmap.find(this->MainRoute+routepath);
        if (iter != this->routeFuncMap->routefuncmap.end()) {
            std::cerr << "已存在该路由!!!" << std::endl;
            exit(0);
        }
        this->routeFuncMap->routefuncmap[this->MainRoute+routepath] = std::bind(func, classname,std::placeholders::_1);
        this->routeFuncMap->routefuncmap.find(this->MainRoute+routepath)->second(this->httpcontext);
        std::cout << this->MainRoute+routepath + ":添加成功!" << std::endl;
    }else{
        std::cout<<"参数非函数或路由不规范!"<<std::endl;
        exit(0);
    }
}
template<typename F>
void GroupRoute::AddRoute(std::string routepath,F func){
    if(std::is_function<typename std::remove_pointer<F>::type>::value && this->Routecheck(routepath)) {
        auto iter = this->routeFuncMap->routefuncmap.find(routepath);
        if (iter != this->routeFuncMap->routefuncmap.end()) {
            std::cerr << "已存在该路由!!!" << std::endl;
            exit(0);
        }
        this->routeFuncMap->routefuncmap[this->MainRoute+routepath] = std::bind(func, std::placeholders::_1);
        this->routeFuncMap->routefuncmap.find(this->MainRoute+routepath)->second(this->httpcontext);
        std::cout << this->MainRoute+routepath + ":添加成功!" << std::endl;
    }else{
        std::cout<<"参数非函数或路由不规范!"<<std::endl;
        exit(0);
    }
}

#endif //PROJECT1_GROUPROUTE_H
