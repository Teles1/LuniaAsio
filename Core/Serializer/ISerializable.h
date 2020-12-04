#pragma once
#ifndef ISerializable_HEADER_GUARD
#define ISerializable_HEADER_GUARD
namespace Serializer {
    class StreamWriter;
    class StreamReader;
    class ISerializable {
    private:
    public:
        virtual void Serialize(Serializer::StreamWriter& out) = 0;
        virtual void Deserialize(Serializer::StreamReader& in) = 0;
    };
}
#endif