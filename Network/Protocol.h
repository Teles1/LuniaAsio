#include "../Core/Common.h"
#include "../Core/Serializer/Serializer.h"

#ifndef NET_SERIALIZABLE
#define NET_SERIALIZABLE								    \
    static const String TypeName;									\
    static const uint16 TypeHash;							\
    void Serialize(Serializer::StreamWriter &out) const;	\
    void Deserialize(Serializer::StreamReader &in);			\
    const String GetTypeName() { return TypeName; }
#endif