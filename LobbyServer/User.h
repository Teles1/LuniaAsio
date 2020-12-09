#pragma once
#include "Network/Tcp/Client.h"
#include "Core/Serializer/Serializer.h"
#include "LobbyProtocol/LobbyProtocol.h"

namespace Lobby {
	class User : public net::ClientTcp
	{
	public:
		User(uint32& userId, asio::ip::tcp::socket&& socket)
			: ClientTcp(std::move(socket))
			, m_userId(userId)
		{
			Logger::GetInstance()->Info("User :: Hey, I was created!", GetUserId());
		}

		~User()
		{
			Logger::GetInstance()->Warn("User :: Hey, I deleted myself, you should check that!", GetUserId());
		}

		const uint32 GetUserId();
		void SetUserLocale(String& inLocale);
		bool QueryAliveAuth();
	public://Network Related;
		void Send(Serializer::ISerializable& packet);
		void Parse(uint8* buffer, size_t size);
	private:
		uint32 m_userId;
		enum Marks { AliveReceived, AliveCleared };
		struct Alive {
			Marks	Mark;

			///	backup alive data
			struct AliveData
			{
				uint32	index;
				uint32	value1;
				uint32	value2;
				uint32	value3;

				bool	operator==(AliveData& aliveData)
				{
					if (index == aliveData.index && value1 == aliveData.value1
						&& value2 == aliveData.value2 && value3 == aliveData.value3)
						return	true;

					return	false;
				}
			}answer, temp;
		}m_Alive;
		uint32 m_AlivePacketCount;
		String m_Locale;
		


	private:
		std::mutex mtx;
	};
	typedef std::shared_ptr<User> UserSharedPtr;
}