/*
	Header containing all includes needed for Serializer's
*/

#pragma once
#include "../Common.h"
#include "../Serializer/StreamReader.h"
#include "../Serializer/StreamWriter.h"

#ifndef NET_SERIALIZABLE
#define NET_SERIALIZABLE								    \
    static const String TypeName;									\
    static const uint16 TypeHash;							\
    void Serialize(Serializer::StreamWriter &out) const;	\
    void Deserialize(Serializer::StreamReader &in);			\
    const String GetTypeName() { return TypeName; }
#endif