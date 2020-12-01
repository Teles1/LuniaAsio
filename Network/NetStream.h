#pragma once
#include "../Core/Common.h"
#include "../Core/Serializer/Serializer.h"
namespace Network {
	class NetStream : public Serializer::StreamReader {
	protected:
		uint16 NetStreamHash = StringUtil::Hash(L"NetStream");
	public:
		NetStream(std::vector<uint8> inData) : StreamReader(inData){}
		int GetNetStreamSize();
		bool IsNetStream();
		Serializer::StreamReader GetStream();
		~NetStream(){
			
		}
	};
}