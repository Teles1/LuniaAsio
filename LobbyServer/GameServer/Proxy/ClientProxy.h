#ifndef ClientProxy_H
#define ClientProxy_H
#pragma once
#include "../NetworkIO/ClientNetworkIO.h"
#include <Core/Serializer/Serializer.h>
#include <Core/GameConstants.h>
namespace Lunia {
	namespace XRated {
		enum class ErrorLevel
		{
			Curious,			///< not that critical but it should not happen by normal client.
			Critical,			///< critical, it's doubtful about a sort of hack.
			Unexpected,			///< internal wrong situation
			Pvp
		};

		struct ClientProxy : public ClientNetworkIO //, TODO? std::enable_shared_from_this<Client>
		{
			ClientProxy(asio::ip::tcp::socket&& socket);

			~ClientProxy() { };

			void MakeSocketAsyncWriteSerializable(Lunia::Serializer::ISerializable& packet);

			uint32_t GetId() const;

			void SetId(const uint32_t& id);

			void SetLocale(const std::wstring& locale);

			virtual bool Ping() = 0;

			bool IsWaitingOnPing() const;

			std::wstring GetAccountName() const;

			void SetAccountName(const std::wstring& accountName);

			/* Old name DoesHaveLicense */
			bool HasLicense(Lunia::XRated::Constants::ClassType classType);

			std::wstring GetCharacterName() const;

			void SetCharacterStateFlags(const Lunia::XRated::CharacterStateFlags& flag);

			Lunia::XRated::CharacterStateFlags GetCharacterStateFlags() const
			{
				return m_characterStateFlags;
			}

			/* Old name IsAValidCharacterName */
			bool DoesCharacterExist(const std::wstring& characterName);

			bool DeleteCharacter(std::wstring& characterName);

			bool SetSelectedCharacter(std::wstring& characterName);

			/* IsCharacterSelected */
			bool HasSelectedACharacter() const;

			void Error(ErrorLevel error, const std::wstring& message); // error handling with different outcome based on severity.

		public:
			fwEvent<> OnDropped;

		public:

			bool m_isWaitingOnPing = false;

			uint16_t m_numNonACKPings = 0;

			uint8_t m_numOfCharacterSlots;

			uint32_t m_accountLicense;

			std::vector<Lunia::XRated::LobbyPlayerInfo> m_characters;

			Lunia::XRated::LobbyPlayerInfo m_selectedCharacter;

			Lunia::XRated::CharacterStateFlags m_characterStateFlags;

		private:

			uint32_t m_id = 0;

			bool m_isAuthenticated = false;

			std::wstring m_locale;

			std::wstring m_accountName;

			mutable std::mutex mtx;
		};
	}
}
#endif // ! ClientProxy_H