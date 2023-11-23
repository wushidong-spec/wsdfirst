//
// Created by LAD on 2023/7/24.
//
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <windows.h>
#include <winsock2.h>
#ifndef PROJECT1_HTTPCONTEXT_H
#define PROJECT1_HTTPCONTEXT_H
struct Header{
    std::map<std::string,std::string> header;
};
struct Body{
    std::string body;
};
struct Httpcontext{
  Header *header;
  Body *body;
  std::string status;
};
class HttpContext {
public:
    Httpcontext *httpcontext;
    SOCKET clientsock;
    HttpContext();
    ~HttpContext();
    bool AddHeader(std::string headkey,std::string headvalue);
    void SetHeader(std::string headkey,std::string headvalue);
    std::string Responsestr();
    std::string FileStream(std::string filepath);
};

#endif //PROJECT1_HTTPCONTEXT_H
