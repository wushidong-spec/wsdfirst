//
// Created by LAD on 2023/11/19.
//

#ifndef UNTITLED3_SERIALIZABLE_H
#define UNTITLED3_SERIALIZABLE_H

#endif //UNTITLED3_SERIALIZABLE_H
class DataStream;

class Serializable {
public:
    virtual void Serialize(DataStream& dstream) const = 0;
    virtual void DeSerialize(DataStream& dstream) = 0;
};
#define SERIALIZE(...) \
  void Serialize(DataStream& dstream) const        \
{                \
        dstream.Write_args(__VA_ARGS__);                \
}                        \
 void DeSerialize(DataStream& dstream)\
{ \
    dstream.Read_args(__VA_ARGS__);\
}
