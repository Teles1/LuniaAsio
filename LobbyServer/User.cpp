#include "User.h"
#include "fwPacketListener.h"
#include "UserRegistry.h"

namespace Lobby 
{
	void User::Send(Serializer::ISerializable& packet)
	{
		StaticBuffer<2 << 12> buffer;
		try
		{
			Serializer::StreamWriter writer(buffer);
			writer.Write(packet);
		}
		catch (Exception&) // catch something like buffer overflow
		{
			Logger::GetInstance()->Error("User@{} Unable to Parse packet", this->GetUserId());
			return;
		}

		this->SendAsync(reinterpret_cast<uint8*>(buffer.GetData()), buffer.GetLength());
	}
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
	bool User::QueryAliveAuth()
	{
		Lobby::Protocol::Alive packet;

		m_Alive.Mark = AliveCleared;
		packet.Index = m_Alive.temp.index = ++m_AlivePacketCount;
		packet.Value1 = m_Alive.temp.value1 = GetTickCount();
		packet.Value2 = m_Alive.temp.value2 = 0;
		packet.Value3 = m_Alive.temp.value3 = 0;

		Send(packet);
		return true;
	}
	const uint32 User::GetUserId(){
		return m_userId;
	}
	void User::SetUserLocale(String& inLocale)
	{
		mtx.lock(); 
		{
			this->m_Locale = std::move(inLocale);
		}
		mtx.unlock();
	}
}
