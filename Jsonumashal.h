//
// Created by LAD on 2023/7/25.
//

#ifndef PROJECT1_JSONUMASHAL_H
#define PROJECT1_JSONUMASHAL_H
#include <iostream>
#include <string>
#include <vector>
#include<map>

struct Protobuf {
    std::vector<std::string> protobuf;
};
class HttpProtobuf {
public:
    Protobuf httpbuf;
    std::vector<std::string> HttpUmashal(std::string& buffer, std::string divdstr);
    std::vector<std::string> StrSplit(std::string buffer,std::string divstr);
};


#endif //PROJECT1_JSONUMASHAL_H
