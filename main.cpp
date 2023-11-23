#include <iostream>
#include "SThreadPool.h"
#include <tuple>
#include <utility>
#include "SRPCServer.h"

struct Test{
    int a;
    std::string str;
    SThreadPool* threadpool;
    Test(int a,std::string str){
        this->a=a;
        this->str=str;
        this->threadpool=new SThreadPool();
    }
};

template<typename... Args>
void Tupletest(std::tuple<Args...> &tuple){
    constexpr auto index=std::index_sequence_for<Args...>();
    std::initializer_list<int>{((std::get<Args>(tuple)), 0)...};
}
template<typename Tuple, std::size_t Id>
void getv(Tuple& t) {
    if(typeid(std::get<Id>(std::forward<Tuple>(t)))== typeid(int)){
        std::cout<<"success!!"<<std::endl;
    }
    if(typeid(std::get<Id>(std::forward<Tuple>(t)))==typeid(std::vector<SThreadPool*>)){
        std::cout<<"is string"<<std::endl;
    }
}

template<typename Tuple, std::size_t... I>
Tuple get_tuple(std::index_sequence<I...>) {
    Tuple t;
    std::initializer_list<int>{((getv<Tuple, I>(t),0))...};
    return t;
}
template<typename R,typename... Params>
void CallProxy(R(*func)(Params...)){
    using args_type=std::tuple<typename std::decay<Params>::type...>;
    constexpr auto N=std::tuple_size<typename std::decay<args_type>::type>::value;
    args_type args= get_tuple<args_type>(std::make_index_sequence<N>{});
}
template<typename T>
void test(int b,std::vector<T> c){
    Test teststruct(1,"abc");
    std::tuple<int,std::string,Test> testtuple(1,std::string("abc"),teststruct);
    std::tuple_element<0, decltype(testtuple)>::type val_1;
    std::tuple_element<2, decltype(testtuple)>::type val_3(1,"abc");
    val_1=1;
    int a=2;
    if(std::is_same<std::tuple_element<0, decltype(testtuple)>::type , int>::value)
        std::cout<<"type is same"<<std::endl;
    Tupletest(testtuple);
}
#include "DataStream.h"
struct TestSeri:protected Serializable{
    int a=1;
    float b=2.3;
    std::vector<int> c{1};
    SERIALIZE(c);
};

struct Mytestin: public Serializable{
    float f;
    std::string ceshi;
    Mytestin(){};
    Mytestin(float f,std::string ceshi){
        this->f=f;
        this->ceshi=ceshi;
    }

    SERIALIZE(f,ceshi);
};

struct Mytest1:public Serializable{
    int a;
    std::string b;
    Mytestin testin;
    Mytest1(){};
    Mytest1(int a,std::string b,Mytestin mytestin){
        this->a=a;
        this->b=b;
        this->testin=mytestin;
    }

    SERIALIZE(a,b,testin);
};
class TestClass:public Serializable{
public:
    int a;
    float b;
    std::string tmpstr;
    Mytest1 test;
    TestClass(){
        this->a=1;
        this->b=2.1;
        this->tmpstr="123";
        this->test=Mytest1(1,"123",Mytestin(1.23,"struct2"));
    }

    SERIALIZE(a,b,tmpstr,test);
};
int main() {
    TestSeri tt;
    std::unique_ptr<int> ptr= std::make_unique<int>();
    DataStream *ds=new DataStream();
    Mytest1 mytest1(1,"123",Mytestin(1.23,"struct2"));
    TestClass *testClass=new TestClass();
    TestClass *testClass1=new TestClass();
    testClass1->a=10;
    int at=8,at1;
    *ds<<testClass<<at;
    *ds>>testClass1>>at1;
    std::cout<<testClass1->a<<at1<<std::endl;
    int b=1;
    int *a=&b;
    int **p=&a;
    std::vector<std::string> testvec;
    testvec.push_back("123");
    testvec.push_back("456");
    std::map<int,std::vector<std::string>> maptest,maptest1;
    maptest[0]=testvec;
  //  maptest[1]=testvec;
    std::cout << "Hello, World!" << std::endl;
    SThreadPool *threadPool=new SThreadPool();
    std::thread testthread(&SThreadPool::AddTasktest,threadPool);
    testthread.detach();
    SrpcServer *srpcServer=new SrpcServer();
    srpcServer->ServerStart();
    threadPool->timer->m_timethread.join();
    return 0;
}