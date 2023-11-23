//
// Created by LAD on 2023/7/25.
//

#include "Jsonumashal.h"
std::vector<std::string> HttpProtobuf::StrSplit(std::string buffer,std::string divstr){
    std::vector<std::string> result;
    size_t startpos=0,endpos=buffer.find_first_of(divstr,startpos);
    while(endpos!=std::string::npos){
        result.push_back(buffer.substr(startpos,endpos-startpos));
        startpos=endpos+1;
        endpos=buffer.find_first_of(divstr,startpos);
    }
    result.push_back(buffer.substr(startpos,buffer.length()));
    return result;
}
std::vector<std::string> HttpProtobuf::HttpUmashal(std::string& buffer,std::string divestr) {
    std::vector<std::string> result;
    size_t startpos=0;
    size_t endpos=buffer.find_first_of(divestr,startpos);
    int isbodystart=0;
    std::string body="";
    while (endpos!=std::string::npos){
        result.push_back(buffer.substr(startpos,endpos-startpos));
        if(result[result.size()-1]=="{\r"){
            isbodystart=1;
            body=buffer.substr(startpos,buffer.size()-1);
        }
        if(result[result.size()-1]=="}"){
            isbodystart=2;
        }
        startpos=endpos+1;
        endpos=buffer.find_first_of(divestr,startpos);
    }
    result.push_back(&buffer[buffer.size()-1]);
    result.push_back(body);
    return result;
}