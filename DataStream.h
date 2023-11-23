//
// Created by LAD on 2023/11/19.
//

#ifndef UNTITLED3_DATASTREAM_H
#define UNTITLED3_DATASTREAM_H

#endif //UNTITLED3_DATASTREAM_H
#pragma once
#include "baseheader.h"
#include "Serializable.h"
#include <typeinfo>

class StreamBuffer {
public:
    StreamBuffer() { savebuffer = ""; };
    StreamBuffer(char* buffer,int len);
    ~StreamBuffer() {};
    long int Get_current_Pos();
    auto Add_stream_buf(char* buf, int len);
    void Get_stream_buf(char* buf, int len, bool special);
    void LoadData(char* data,int len){
        memcpy(this->m_buffer, data, len);
    }
    void Clear(){
        this->m_currentsize=0;
        this->m_position=0;
        memset(this->m_buffer,0,sizeof(this->m_buffer));
    }
    std::string savebuffer;
    char* m_buffer=new char[65535];
    std::atomic<int> m_currentsize=0;
private:
    std::atomic<int> m_position=0;
};
auto StreamBuffer::Add_stream_buf(char *buf, int len) {
    memcpy(this->m_buffer + this->m_currentsize, buf, len);
    this->m_currentsize.store(this->m_currentsize.load() + len);
}
long int StreamBuffer::Get_current_Pos() {
    return this->m_currentsize.load();
}
void StreamBuffer::Get_stream_buf(char* buf, int len,bool special) {
    if (!special) {
        memcpy(buf, this->m_buffer + this->m_position + sizeof(char), len);
        this->m_position.store(this->m_position.load() + sizeof(char) + len);
    }
    else {
        memcpy(buf, this->m_buffer + this->m_position, len);
        this->m_position.store(this->m_position.load() + len);
    }
}
StreamBuffer::StreamBuffer(char* buffer,int len) {
   this->LoadData(buffer,len);
}
class DataStream
{
public:
    DataStream();
    ~DataStream();
    template<typename T>
    DataStream &operator << (T& value);
    template<typename T>
    DataStream& operator>>(T& value);
    enum Valuetype
    {
        S_INI=1,
        S_CHAR=2,
        S_FLOAT=3,
        S_BOOL=4,
        S_DOUBLE=5,
        S_STRING=6,
        S_VECTOR=7,
        S_LIST=8,
        S_SET=9,
        S_MAP=10,
    };
    StreamBuffer *streambuf;
    void write_data(int& value);
    void write_data(const int& value);
    void write_data(char & value);
    void write_data(const char& value);
    void write_data(bool& value);
    void write_data(const bool& value);
    void write_data(float& value);
    void write_data(const float& value);
    void write_data(double& value);
    void write_data(const double& value);
    void write_data(std::string & value);
    void write_data(Serializable& value);
    void write_data(const std::string& value);
    template<typename T,typename... Args>
    void Write_args(T first, Args... args);
    template<typename T>
    void Write_args(T last);
    template<typename T>
    void write_data(std::vector<T> & value);
    template<typename T>
    void write_data(const std::vector<T>& value);
    template<typename T>
    void write_data(std::list<T>& value);
    template<typename T>
    void write_data(const std::list<T>& value);
    template<typename K,typename V>
    void write_data(std::map<K,V>& value);
    template<typename K,typename V>
    void write_data(const std::list<K,V>& value);
    template<typename K, typename V>
    void write_data(const std::map<K, V>& value);
    template<typename T>
    typename std::enable_if<std::is_pointer<T>::value, void>::type write_data(T& value);
    template<typename T>
    typename std::enable_if<std::is_pointer<T>::value, void>::type read_data(T& value);
    void read_data(int& value);
    void read_data(const int& value);
    void read_data(char& value);
    void read_data(const char& value);
    void read_data(bool& value);
    void read_data(const bool& value);
    void read_data(float& value);
    void read_data(const float& value);
    void read_data(double& value);
    void read_data(const double& value);
    void read_data(std::string& value);
    void read_data(const std::string& value);
    void read_data(Serializable& value);
    template<typename T, typename... Args>
    void Read_args(T& first, Args&... args);
    template<typename T>
    void Read_args(T& last);
    template<typename T>
    void read_data(std::vector<T>& value);
    template<typename T>
    void read_data(const std::vector<T>& value);
    template<typename T>
    void read_data(std::list<T>& value);
    template<typename T>
    void read_data(const std::list<T>& value);
    template<typename K, typename V>
    void read_data(std::map<K, V>& value);
    template<typename K, typename V>
    void read_data(const std::list<K, V>& value);
    template<typename K, typename V>
    void read_data(const std::map<K, V>& value);
    template<typename Tuple,std::size_t... I>
    Tuple Get_tuple(std::index_sequence<I...>);
};
template<typename Tuple, std::size_t... I>
Tuple DataStream::Get_tuple(std::index_sequence<I...>) {
    Tuple t;
    std::initializer_list<int>{((*this >> std::get<I>(t)), 0)...};
    return t;
}
void DataStream::write_data(Serializable & value) {
    value.Serialize(*this);
}
void DataStream::read_data(Serializable& value) {
    value.DeSerialize(*this);
}
template<typename T, typename... Args>
void DataStream::Write_args(T  first, Args... args) {
    this->write_data(first);
    this->Write_args(args...);
}
template<typename T>
void DataStream::Write_args(T last) {
    this->write_data(last);
}
template<typename T, typename... Args>
void DataStream::Read_args(T& first, Args&... args) {
    this->read_data(first);
    this->Read_args(args...);
}
template<typename T>
void DataStream::Read_args(T& last) {
    this->read_data(last);
}
template<typename T>
void DataStream::read_data(std::vector<T>& value) {
    char* valuesize = new char[sizeof(int)];
    this->streambuf->Get_stream_buf(valuesize, sizeof(int), false);
    for (int i = 0; i < *reinterpret_cast<int*>(&valuesize[0]); i++) {
        T t;
        value.push_back(t);
        this->read_data(value[i]);
    }
    delete[] valuesize;
}
template<typename T>
void DataStream::read_data(std::list<T>& value) {
    char* valuesize = new char[sizeof(int)];
    this->streambuf->Get_stream_buf(valuesize, sizeof(int), false);
    for (int i = 0; i < *reinterpret_cast<int*>(&valuesize[0]); i++) {
        T t;
        value.push_back(t);
        this->read_data(value[i]);
    }
    delete[] valuesize;
}
template<typename K,typename V>
void DataStream::read_data(std::map<K,V>& value) {
    char* valuesize = new char[sizeof(int)];
    this->streambuf->Get_stream_buf(valuesize, sizeof(int), false);
    for (int i = 0; i < *reinterpret_cast<int*>(&valuesize[0]); i++) {
        K k; V v;
        this->read_data(k);
        this->read_data(v);
        value.insert(std::pair<K,V>(k, v));
    }
    delete[] valuesize;
}
void DataStream::read_data(int& value) {
    char* buffer = new char[sizeof(int)];
    this->streambuf->Get_stream_buf(buffer, sizeof(int), false);
    value = *reinterpret_cast<int*>(&buffer[0]);
    delete[] buffer;
}
void DataStream::read_data(char& value) {
    char* buffer = new char[sizeof(char)];
    this->streambuf->Get_stream_buf(buffer, sizeof(char), false);
    value = *reinterpret_cast<char*>(&buffer[0]);
    delete[] buffer;
}
void DataStream::read_data(float& value) {
    char* buffer = new char[sizeof(float)];
    this->streambuf->Get_stream_buf(buffer, sizeof(float), false);
    value = *reinterpret_cast<float*>(&buffer[0]);
    delete[] buffer;
}
void DataStream::read_data(double& value) {
    char* buffer = new char[sizeof(double)];
    this->streambuf->Get_stream_buf(buffer, sizeof(double), false);
    value = *reinterpret_cast<double*>(&buffer[0]);
    delete[] buffer;
}
void DataStream::read_data(bool& value) {
    char* buffer = new char[sizeof(bool)];
    this->streambuf->Get_stream_buf(buffer, sizeof(bool), false);
    value = *reinterpret_cast<bool*>(&buffer[0]);
    delete[] buffer;
}
void DataStream::read_data(std::string& value) {
    char* valuesize = new char[sizeof(int)];
    this->streambuf->Get_stream_buf(valuesize, sizeof(int), false);
    int tmpsize = *reinterpret_cast<int*>(&valuesize[0]);
    char* buffer = new char[tmpsize];
    this->streambuf->Get_stream_buf(buffer, tmpsize, true);
    value.assign(buffer, tmpsize);
    delete[] valuesize;
    delete[] buffer;
}
template<typename T>
typename std::enable_if<std::is_pointer<T>::value, void>::type DataStream::write_data(T& value) {
    *this << *value;
}
template<typename T>
typename std::enable_if<std::is_pointer<T>::value, void>::type DataStream::read_data(T& value) {
    *this >> *value;
}
template<typename T>
DataStream& DataStream::operator<<(T& value) {
    write_data(value);
    return *this;
}
template<typename T>
DataStream& DataStream::operator>>(T& value) {
    read_data(value);
    return *this;
}
template<typename K, typename V>
void DataStream::write_data(std::map<K, V>& value) {
    char type = (char)Valuetype::S_MAP;
    char* data = new char[sizeof(char) + sizeof(int)];
    memcpy(data, (char*)&type, sizeof(char));
    int valuelen = value.size();
    memcpy(data + sizeof(char), (char*)&valuelen, sizeof(int));
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(int));
    for (auto iter = value.begin(); iter != value.end(); ++iter) {
        K k = iter->first;
        V v = iter->second;
        this->write_data(k);
        this->write_data(v);
    }
    delete[] data;
}
template<typename K, typename V>
void DataStream::write_data(const std::map<K, V>& value) {
    char type = (char)Valuetype::S_MAP;
    char* data = new char[sizeof(char) + sizeof(int)];
    memcpy(data, (char*)&type, sizeof(char));
    int valuelen = value.size();
    memcpy(data + sizeof(char), (char*)&valuelen, sizeof(int));
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(int));
    for (auto iter = value.begin(); iter != value.end(); ++iter) {
        K k = iter->first;
        V v = iter->second;
        this->write_data(k);
        this->write_data(v);
    }
    delete[] data;
}
template<typename T>
void DataStream::write_data(std::list<T>& value) {
    char type = (char)Valuetype::S_LIST;
    char* data = new char[sizeof(char) + sizeof(int)];
    memcpy(data, (char*)&type, sizeof(char));
    int valuelen = value.size();
    memcpy(data + sizeof(char), (char*)&valuelen, sizeof(int));
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(int));
    for (auto iter = value.begin(); iter != value.end(); ++iter) {
        this->write_data(*iter);
    }
    delete[] data;
}

template<typename T>
void DataStream::write_data(const std::list<T>& value) {
    char type = (char)Valuetype::S_LIST;
    char* data = new char[sizeof(char) + sizeof(int)];
    memcpy(data, (char*)&type, sizeof(char));
    int valuelen = value.size();
    memcpy(data + sizeof(char), (char*)&valuelen, sizeof(int));
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(int));
    for (auto iter = value.begin(); iter != value.end(); ++iter) {
        this->write_data(*iter);
    }
    delete[] data;
}
template<typename T>
void DataStream::write_data(std::vector<T>& value) {
    char type = (char)Valuetype::S_VECTOR;
    char* data = new char[sizeof(char) + sizeof(int)];
    memcpy(data, (char*)&type, sizeof(char));
    int valuelen = value.size();
    memcpy(data + sizeof(char), (char*)&valuelen, sizeof(int));
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(int));
    for (auto iter = value.begin(); iter != value.end(); ++iter) {
        this->write_data(*iter);
    }
    delete[] data;
}
template<typename T>
void DataStream::write_data(const std::vector<T>& value) {
    char type = (char)Valuetype::S_VECTOR;
    char* data = new char[sizeof(char) + sizeof(int)];
    memcpy(data, (char*)&type, sizeof(char));
    int valuelen = value.size();
    memcpy(data + sizeof(char), (char*)&valuelen, sizeof(int));
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(int));
    for (auto iter = value.begin(); iter != value.end(); ++iter) {
        this->write_data(*iter);
    }
    delete[] data;
}
void DataStream::write_data(std::string& value) {
    char type = (char)Valuetype::S_STRING;
    char* data = new char[sizeof(char) + sizeof(int) + value.length()];
    memcpy(data, (char*)&type, sizeof(char));
    int valuelen = value.length();
    memcpy(data + sizeof(char), (char*)&valuelen, sizeof(int));
    memcpy(data + sizeof(char) + sizeof(int), value.c_str(), valuelen);
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(int) + valuelen);
    delete[] data;
}
void DataStream::write_data(const std::string& value) {
    char type = (char)Valuetype::S_STRING;
    char* data = new char[sizeof(char) + sizeof(int) + value.length()];
    memcpy(data, (char*)&type, sizeof(char));
    int valuelen = value.length();
    memcpy(data + sizeof(char), (char*)&valuelen, sizeof(int));
    memcpy(data + sizeof(char) + sizeof(int), value.c_str(), valuelen);
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(int) + valuelen);
    delete[] data;
}
void DataStream::write_data(int& value) {
    char type = (char)Valuetype::S_INI;
    char* data = new char[sizeof(char)+sizeof(int)+1];
    memcpy(data, (char*)&type, sizeof(char));
    memcpy(data + sizeof(char), (char*)&value, sizeof(int));
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(int));
    delete[] data;
}

void DataStream::write_data(const int & value) {
    char type = (char)Valuetype::S_INI;
    char* data = new char[sizeof(char) + sizeof(int) + 1];
    memcpy(data, (char*)&type, sizeof(char));
    memcpy(data + sizeof(char), (char*)&value, sizeof(int));
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(int));
    delete[] data;
}

void DataStream::write_data(char & value) {
    char type = (char)Valuetype::S_CHAR;
    char* data = new char[sizeof(char) + sizeof(char) + 1];
    memcpy(data, (char*)&type, sizeof(char));
    memcpy(data + sizeof(char), (char*)&value, sizeof(char));
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(char));
    delete[] data;
}
void DataStream::write_data(const char& value) {
    char type = (char)Valuetype::S_CHAR;
    char* data = new char[sizeof(char) + sizeof(char) + 1];
    memcpy(data, (char*)&type, sizeof(char));
    memcpy(data + sizeof(char), (char*)&value, sizeof(char));
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(char));
    delete[] data;
}
void DataStream::write_data(bool & value) {
    char type = (char)Valuetype::S_BOOL;
    char* data = new char[sizeof(char) + sizeof(bool) + 1];
    memcpy(data, (char*)&type, sizeof(char));
    memcpy(data + sizeof(char), (char*)&value, sizeof(bool));
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(bool));
    delete[] data;
}
void DataStream::write_data(const bool& value) {
    char type = (char)Valuetype::S_BOOL;
    char* data = new char[sizeof(char) + sizeof(bool) + 1];
    memcpy(data, (char*)&type, sizeof(char));
    memcpy(data + sizeof(char), (char*)&value, sizeof(bool));
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(bool));
    delete[] data;
}
void DataStream::write_data(float & value) {
    char type = (char)Valuetype::S_FLOAT;
    char* data = new char[sizeof(char) + sizeof(float) + 1];
    memcpy(data, (char*)&type, sizeof(char));
    memcpy(data + sizeof(char), (char*)&value, sizeof(float));
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(float));
    delete[] data;
}
void DataStream::write_data(const float& value) {
    char type = (char)Valuetype::S_FLOAT;
    char* data = new char[sizeof(char) + sizeof(float) + 1];
    memcpy(data, (char*)&type, sizeof(char));
    memcpy(data + sizeof(char), (char*)&value, sizeof(float));
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(float));
    delete[] data;
}
void DataStream::write_data(double& value) {
    char type = (char)Valuetype::S_DOUBLE;
    char* data = new char[sizeof(char) + sizeof(double) + 1];
    memcpy(data, (char*)&type, sizeof(char));
    memcpy(data + sizeof(char), (char*)&value, sizeof(double));
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(double));
    delete[] data;
}
void DataStream::write_data(const double& value) {
    char type = (char)Valuetype::S_DOUBLE;
    char* data = new char[sizeof(char) + sizeof(double) + 1];
    memcpy(data, (char*)&type, sizeof(char));
    memcpy(data + sizeof(char), (char*)&value, sizeof(double));
    this->streambuf->Add_stream_buf(data, sizeof(char) + sizeof(double));
    delete[] data;
}

DataStream::DataStream()
{
    this->streambuf = new StreamBuffer();
}

DataStream::~DataStream()
{
    delete streambuf;
}