//
// Created by LAD on 2023/7/24.
//

#include "HttpContext.h"
#include <fstream>

HttpContext::HttpContext() {
    this->httpcontext=new Httpcontext;
    this->httpcontext->body=new Body;
    this->httpcontext->status="200";
    this->httpcontext->header=new Header;
    this->httpcontext->body->body="";
    this->httpcontext->header->header["Content-Type"]=" text/html;charset=utf-8\r\n";
}
HttpContext::~HttpContext() {}

bool HttpContext::AddHeader(std::string headkey, std::string headvalue) {
    if(this->httpcontext->header->header.find(headkey)!=this->httpcontext->header->header.end()){
        std::cout<<headkey+":已存在"<<std::endl;
        return false;
    }
    this->httpcontext->header->header[headkey]=headvalue;
    return true;
}
void HttpContext::SetHeader(std::string headkey, std::string headvalue) {
    this->httpcontext->header->header[headkey]=headvalue;
}
std::string HttpContext::Responsestr() {
    std::string response= "HTTP/1.1 "+this->httpcontext->status+" OK\r\n";
    for(auto iter=this->httpcontext->header->header.begin();iter!=this->httpcontext->header->header.end();++iter)
        response+=iter->first+":"+iter->second+"\r\n";
    response+="\r\n"+this->httpcontext->body->body+"\n";
    return response;
}
std::string HttpContext::FileStream(std::string filepath){
    std::ifstream file;
    file.open(filepath,std::ios::in|std::ios::binary);
    if(file.is_open()){
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        return content;
    }
    file.close();
    return "";
}