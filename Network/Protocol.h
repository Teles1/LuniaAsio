#pragma once
#include "../Core/Common.h"
#include "../Core/Serializer/StreamReader.h"
#include "../Core/Serializer/StreamWriter.h"

#ifndef NET_SERIALIZABLE
#define NET_SERIALIZABLE								    \
    static const String TypeName;									\
    static const uint16 TypeHash;							\
    void Serialize(Serializer::StreamWriter &out) const;	\
    void Deserialize(Serializer::StreamReader &in);			\
    const String GetTypeName() { return TypeName; }
#endif