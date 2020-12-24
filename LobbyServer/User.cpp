#include "..\StageServer\User\User.h"
#include "User.h"
#include "fwPacketListener.h"
#include "Network/NetStream.h"
#include "./Network/Api/Api.h"
#include <LobbyServer/Common.h>
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
				Logger::GetInstance().Error("User@{0} Unable to Parse packet", this->GetId());
				return;
			}

			this->SendAsync(reinterpret_cast<uint8*>(buffer.GetData()), buffer.GetLength());
		}
		uint32 User::Parse(uint8* buffer, size_t size)
		{
			/*
				We should technically see but i think that if the server doesn't recognize the packet I say we let the client connection dies.
				That's why I want to keep this call in here
				Parse only known packets. At least the ones which follow NetStream Standarts.
			*/

			Net::StreamReader sReader(buffer);

			auto userPtr = UserRegistry().GetUserByUserId(this->GetId());

			HandleRead();
			return (uint32)size;
		}
		bool User::SatisfyAlivePingWait()
		{
			Lobby::Protocol::Alive packet;

			this->m_waitingOnAlivePing = false;

			/*
			// Doesnt seem to be needing values at all?!

			packet.Index  = m_lastTickAlivePing.temp.index  = ++m_AlivePacketCount;
			packet.Value1 = m_lastTickAlivePing.temp.value1 = (uint32)GetTickCount64();
			packet.Value2 = m_lastTickAlivePing.temp.value2 = 0;
			packet.Value3 = m_lastTickAlivePing.temp.value3 = 0;
			*/

			Send(packet);

			return true;
		}

		bool User::IsWaitingOnAlivePing() const
		{
			return this->m_waitingOnAlivePing;
		}

		void User::SetAliveAsLastTickAlivePing(const Alive::AliveData& answer)
		{
			//this->m_lastTickAlivePing = answer;

			this->m_waitingOnAlivePing = true;
		}

		void User::SetIsAuthenticated()
		{
			AutoLock _l(mtx);
			this->m_isAuthenticated = true;
		}

		bool User::DoesHaveLicense(XRated::Constants::ClassType classType)
		{
			AutoLock _l(mtx);
			if (static_cast<bool>(m_AccountLicenses & (1 << static_cast<int>(classType))))
				return true;
			return false;
		}

		bool User::IsAccountAuthorized() const
		{
			return !GetAccountName().empty();
		}

		bool User::IsAuthenticated() const
		{
			return m_isAuthenticated;
		}

		bool User::PassedSecondPassword(const bool& newBool)
		{
			AutoLock _l(mtx);
			this->m_isAuthenticated = newBool;
			return this->m_isAuthenticated;
		}

		bool User::IsAValidCharacterName(String& characterName)
		{
			AutoLock _l(mtx);
			for (auto& x : m_Characters)
				if (x.CharacterName == characterName)
					return true;
			return false;
		}

		bool User::DeleteCharacter(String& characterName)
		{
			AutoLock _l(mtx);
			for(CharactersIterator it = m_Characters.begin(); it != m_Characters.end(); it++)
				if (it->CharacterName == characterName) {
					m_Characters.erase(it);
					return true;
				}
			return false;
		}

		const String& User::GetCharacterName() const
		{
			return m_selectedCharacter.CharacterName;
		}

		bool User::SetSelectedCharacter(String& characterName)
		{
			AutoLock _l(mtx);
			for (auto& x : m_Characters)
				if (x.CharacterName == characterName) {
					m_selectedCharacter = x;
					return true;
				}
			return false;
		}

		void User::SetCharacterStateFlags(const XRated::CharacterStateFlags& flag)
		{
			if (IsCharacterSelected()) {
				AutoLock _l(mtx);
				m_CharacterStateFlags = m_selectedCharacter.StateFlags.EquipmentSet;
				m_CharacterStateFlags = flag;
				Net::Api api("UpdateCharacterStateFlags");
				api << GetAccountName() << GetCharacterName() << (int)m_CharacterStateFlags;
				if (api.RequestApi().errorCode != 0)
					Logger::GetInstance().Warn(L"Could not update StateFlags for user = {0}, character = {1}", GetAccountName(), GetCharacterName());
			}else
				Logger::GetInstance().Warn(L"Account ={0} doesn't have a selected character and there is a call to update flags.", GetAccountName());
		}

		bool User::IsCharacterSelected() const
		{
			return m_selectedCharacter.CharacterSerial != -1;
		}

		void User::Error(ErrorLevel error, const String& message)
		{
			switch (error)
			{
			case ErrorLevel::Pvp:
			case ErrorLevel::Curious:
				break;
			case ErrorLevel::Critical:
			case ErrorLevel::Unexpected:
			{
				std::string operation(error == ErrorLevel::Critical ? "critical" : "unexpected");
				std::string characterName;
				if (m_selectedCharacter.CharacterSerial != -1)
					characterName = StringUtil::ToASCII(m_selectedCharacter.CharacterName);

				/* LOGGING */
			}
			break;
			}
			//Some sort of disconnecting process going in here. 
		}

		uint32 User::GetId() const {
			return m_userId;
		}

		void User::SetId(const uint32& userId)
		{
			AutoLock _l(mtx);
			m_userId = userId;
		}

		void User::SetLocale(const String& inLocale)
		{
			AutoLock _l(mtx);
			this->m_Locale = inLocale;
		}

		void User::SetAccountName(const String& inAccountName)
		{
			AutoLock _l(mtx);
			this->m_AccountName = inAccountName;
		}

		String User::GetAccountName() const
		{
			return this->m_AccountName;
		}
		Alive::AliveData::AliveData(uint32 index, uint32 value1, uint32 value2, uint32 value3)
		{
			this->index = index;
			this->value1 = value1;
			this->value2 = value2;
			this->value3 = value3;
		}
}
}