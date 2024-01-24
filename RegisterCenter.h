//
// Created by LAD on 2023/12/28.
//

#ifndef UNTITLED2_REGISTERCENTER_H
#define UNTITLED2_REGISTERCENTER_H

#include "baseheader.h"
#include "SRPCnetwork.h"
class RegisterCenter:public SrpcServer{
public:
    RegisterCenter():SrpcServer(){};
    ~RegisterCenter(){}
    bool StartRegisterService();
};


#endif //UNTITLED2_REGISTERCENTER_H
