#include "User.h"
#include "fwPacketListener.h"
#include "UserRegistry.h"

namespace Lobby 
{
	void User::Parse(uint8* buffer, size_t size)
	{
		/*
			We should technically see but i think that if the server doesn't recognize the packet I say we let the client connection dies. 
			That's why I want to keep this call in here
			Parse only known packets. At least the ones which follow NetStream Standarts.
		*/

		uint32 a = 0;
		this->SetCryptoKey(a);

		Serializer::StreamReader sReader(buffer, size);

		uint16 packetHeaderHash;
		sReader.Read(packetHeaderHash);

		auto userPtr = net::UserRegistry::GetInstance()->GetUserByUserId(this->GetUserId());

		fwPacketListener::GetInstance()->Invoke(userPtr, packetHeaderHash, sReader);

		HandleRead();
	}
	const uint32 User::GetUserId(){
		return m_userId;
	}
}
