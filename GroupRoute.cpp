//
// Created by LAD on 2023/7/27.
//

#include "GroupRoute.h"

void GroupRoute::Test(HttpContext *httpContext) {
    std::cout<<"qqqqqqqq"<<std::endl;
}
bool GroupRoute::Routecheck(std::string routepath){
    if(routepath[0]!='/'||routepath.find_first_of("/",1)!=std::string::npos){
        return false;
    }
    return true;
}
GroupRoute GroupRoute::AddGroupRoute(std::string mainroute) {
    this->MainRoute=mainroute;
    return *this;
}
GroupRoute::GroupRoute() {
    this->MainRoute="";
    this->routeFuncMap=new RouteFuncMap;
    this->httpcontext=new HttpContext();
}
GroupRoute::~GroupRoute() {
}