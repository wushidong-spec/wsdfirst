//
// Created by LAD on 2023/11/19.
//

#ifndef UNTITLED3_FUNCTIONHANDLER_H
#define UNTITLED3_FUNCTIONHANDLER_H

#endif //UNTITLED3_FUNCTIONHANDLER_H
#include "DataStream.h"
#include<functional>
template<typename T>
struct result_type { typedef T type; };
template<>
struct result_type<void> { typedef int8_t type; };
template<typename T>
class RValue{
public:
    typename result_type<T>::type result;
    typename result_type<T>::type TMP;
    std::string msg;
    int errorcode;
    void Set(typename result_type<T>::type result,std::string msg,int errorcode){
        this->result=result;
        this->msg=msg;
        this->errorcode=errorcode;
    }
    void Clear(){
        this->msg="";
        result=TMP;
        this->errorcode=false;
    }
    friend DataStream& operator <<(DataStream& ds,RValue<T>& rvalue){
        ds<<rvalue.result<<rvalue.msg<<rvalue.errorcode;
        return ds;
    }
    friend DataStream& operator >>(DataStream& ds,RValue<T>& rvalue){
        ds>>rvalue.result>>rvalue.msg>>rvalue.errorcode;
        return ds;
    }
};

class FunctionHandler {
public:
    template<typename Func>
    void Bind(std::string funcname, Func func);
    template<typename Func, typename Class>
    void Bind(std::string funcname, Func func, Class* c);
    template<typename R,typename... Params>
    DataStream& Call(std::string funcname,Params... params);
    DataStream& FaCall(DataStream *ds, const char * data,int len);
    std::map < std::string, std::function<void(DataStream*, const char*, int)>> m_funcion;
    template<typename Func,typename Class>
    void CallProxy(Func func, Class* s,DataStream* ds, const char* data, int len);
    template<typename Func>
    void CallProxy(Func func, DataStream* ds, const char* data, int len);
    template<typename R,typename... Params>
    void CallProxy_(R(*func)(Params...), DataStream* ds, const char* data, int len);
    template<typename R,typename Class, typename S,typename... Params>
    void CallProxy_(R(Class::*func)(Params...), DataStream* ds, S * c,const char* data, int len);
    template<typename Func,typename Tuple,std::size_t... Index>
    decltype(auto) GetFuncResult(Func&& func, Tuple&& tuple, std::index_sequence<Index...>);
    template<typename R, typename Func,typename Tuple>
    typename std::enable_if<std::is_same<R, void>::value, typename result_type<R>::type>::type ExcuteFunc(Func& func, Tuple& tuple);
    template<typename R, typename Func, typename Tuple>
    typename std::enable_if<!std::is_same<R, void>::value, typename result_type<R>::type>::type ExcuteFunc(Func& func, Tuple& tuple);
};
DataStream& FunctionHandler::FaCall(DataStream *ds, const char * data,int len){
    ds->streambuf->LoadData(const_cast<char*>(data),len);
    std::string funcname;
    *ds>>funcname;
    auto func = this->m_funcion[funcname];
    func(ds,data,len);
    return *ds;
}
template<typename R,typename... Params>
DataStream& FunctionHandler::Call(std::string funcname,Params... params){
    DataStream* ds=new DataStream();
    ds->Write_args(params...);
    if(this->m_funcion.find(funcname)!=this->m_funcion.end())
        this->m_funcion.find(funcname)->second(ds, ds->streambuf->m_buffer, ds->streambuf->m_currentsize.load());
    return *ds;
}
template<typename Func, typename Tuple, std::size_t... Index>
decltype(auto) FunctionHandler::GetFuncResult(Func&& func, Tuple&& tuple, std::index_sequence<Index...>) {
    return func(std::get<Index>(std::forward<Tuple>(tuple))...);
}
template<typename R, typename Func, typename Tuple>
typename std::enable_if<std::is_same<R, void>::value, typename result_type<R>::type>::type FunctionHandler::ExcuteFunc(Func& func, Tuple& tuple) {
    constexpr auto argsSize = std::tuple_size<typename std::decay<Tuple>::type>::value;
    this->GetFuncResult(std::forward<Func>(func), std::forward<Tuple>(tuple), std::make_index_sequence<argsSize>{});
    return 0;
}
template<typename R,typename Func, typename Tuple>
typename std::enable_if<!std::is_same<R, void>::value, typename result_type<R>::type>::type FunctionHandler::ExcuteFunc(Func& func, Tuple& tuple) {
    constexpr auto argsSize = std::tuple_size<typename std::decay<Tuple>::type>::value;
    return this->GetFuncResult(std::forward<Func>(func), std::forward<Tuple>(tuple), std::make_index_sequence<argsSize>{});
}
template<typename R, typename Class, typename S, typename... Params>
void FunctionHandler::CallProxy_(R(Class::* func)(Params...), DataStream* ds, S* c, const char* data, int len) {
    using Arg_args = std::tuple<typename std::decay<Params>::type...>;
    DataStream *ds1=new DataStream();
    ds1->streambuf->LoadData(const_cast<char*>(data),len);
    auto fafunc = [=](Params... params)->R {
        return (c->*func)(params...);
    };
    constexpr auto N = std::tuple_size<typename std::decay<Arg_args>::type>::value;
    Arg_args arg_types = ds1->Get_tuple<Arg_args>(std::make_index_sequence<N>{});
    typename result_type<R>::type result = this->ExcuteFunc<R>(fafunc, arg_types);
    RValue<R> rvalue;
    rvalue.Set(result,"ok",true);
    ds->streambuf->Clear();
    *ds << rvalue;
    delete ds1;
}
template<typename R, typename... Params>
void FunctionHandler::CallProxy_(R(*func)(Params...), DataStream* ds, const char* data, int len) {
    using Arg_types = std::tuple<typename std::decay<Params>::type...>;
    DataStream *ds1=new DataStream();
    ds1->streambuf->LoadData(const_cast<char*>(data),len);

    constexpr auto N = std::tuple_size<typename std::decay<Arg_types>::type>::value;
    Arg_types arg_types = ds1->Get_tuple<Arg_types>(std::make_index_sequence<N>{});
    typename result_type<R>::type result = this->ExcuteFunc<R>(func, arg_types);
    RValue<R> rvalue;
    rvalue.Set(result,"ok",true);
    ds->streambuf->Clear();
    *ds << rvalue;
    delete ds1;
}
template<typename Func, typename Class>
void FunctionHandler::CallProxy(Func func, Class* s,DataStream* ds, const char* data, int len) {
    this->CallProxy_(func, ds, s, data, len);
}
template<typename Func>
void FunctionHandler::CallProxy(Func func, DataStream* ds, const char* data, int len) {
    this->CallProxy_(func, ds, data, len);
}
template<typename Func>
void FunctionHandler::Bind(std::string funcname, Func func) {
    this->m_funcion[funcname] = std::bind(&FunctionHandler::CallProxy<Func>, this, func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}
template<typename Func, typename Class>
void FunctionHandler::Bind(std::string funcname, Func func, Class *c) {
    this->m_funcion[funcname] = std::bind(&FunctionHandler::CallProxy<Func,Class>, this, func, c,std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}