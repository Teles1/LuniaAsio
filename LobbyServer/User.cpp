#include "User.h"
#include "fwPacketListener.h"
#include "UserRegistry.h"
#include "Network/NetStream.h"
namespace Lunia {
	namespace Lobby
	{
		void User::Send(Serializer::ISerializable& packet)
		{
			StaticBuffer<2 << 12> buffer;
			try
			{
				Net::StreamWriter writer(buffer);
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

			Net::StreamReader sReader(buffer);

			auto userPtr = Net::UserRegistry::GetInstance()->GetUserByUserId(this->GetUserId());

			fwPacketListener::GetInstance()->Invoke(userPtr, sReader.GetSerializedTypeHash(), sReader);

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

		void User::SetIsAuthenticated(const bool& toggle)
		{
			mtx.lock();
			{
				this->m_isAuthenticated = toggle;
			}
			mtx.unlock();
		}

		const bool User::IsAuthenticated()
		{
			return m_isAuthenticated;
		}

		const uint32 User::GetUserId() {
			return m_userId;
		}

		void User::SetUserId(const uint32& userId)
		{
			m_userId = userId;
		}

		void User::SetUserLocale(const String& inLocale)
		{
			mtx.lock();
			{
				mtx.lock();
				{
					this->m_Locale = inLocale;
				}
				mtx.unlock();
			}
		}

		void User::SetUserAccountName(const String& inAccountName)
		{
			mtx.lock();
			{
				this->m_AccountName = inAccountName;
			}
			mtx.unlock();
		}
	}
}