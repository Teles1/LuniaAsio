#include <Core/GameConstants.h>
#include <Core/Utils/ConfigReader.h>
#include <Core/fwEvent.h>
#include <Network/Tcp/Shared.h>
#include <Network/Api/Api.h>
#include <Info/Info/Event/EventInfo.h>
namespace Lunia {
	namespace XRated{
		namespace StageServer
		{	
			class User;
			typedef std::shared_ptr<User> UserSharedPtr;
			typedef std::weak_ptr<User> UserWeakPtr;

			class UserManager {
#pragma region Singleton
			private:
				~UserManager();
				UserManager();
			public:
				UserManager(const UserManager&) = delete; //anti creation  of a copy
				UserManager& operator= (const UserManager&) = delete; // anti copy
				UserManager(UserManager&&) = delete;
				UserManager& operator=(UserManager&&) = delete;
				static UserManager& GetInstance();
#pragma endregion Singleton
			public: 
				bool IsEventQuestPeriod(const uint8& eventType);
				void CheckAllEventQuestPeriod(UserSharedPtr user);
				void CheckAllNpcDropEventPeriod(UserSharedPtr user);
				std::wstring GetLastTextBoardMsg() const;
				bool IsScriptEventNow(const uint32& eventId) const;
				bool DoesExist(const UserSharedPtr& user) const;
				void SendCurrentStageEvents(const UserSharedPtr& user) const;
			public:
				bool AuthenticateUser(const uint32& userId, const json& result);
				void AuthedConnection(const UserSharedPtr& user, Net::Answer& answer);
				void Authed(const UserSharedPtr& user, Net::Answer& answer);
				void RoomAuth(const UserSharedPtr& user);
				void RemoveUser(const uint64& userSerial);
				UserSharedPtr MakeUser(asio::ip::tcp::socket& socket);
				UserSharedPtr GetUserByConnectionId(const uint32& userId);
				UserSharedPtr GetUserBySerial(const uint64& userSerial);
				UserSharedPtr GetUserByName(const std::wstring& name);

				fwEvent<const UserSharedPtr&>						OnUserConnected;
				fwEvent<const uint64&>								OnUserDisconnected;
				fwEvent<const UserSharedPtr&>						OnUserAuthenticated;
			private:
				void RemoveUser(const uint64& userSerial, AutoLock& _l);

				uint32												m_curUserId = 0;
				std::unordered_map<uint32, UserSharedPtr>			m_tempUsers; //temp connection id.
				mutable std::mutex									m_usersMutex;
				std::mutex											m_tempUsersMutex;
				std::unordered_map<uint64, UserSharedPtr>			m_users; //SerialId from the db after being authenticated.
				std::wstring										m_LastTextBoardMsg = L"";
				std::map<uint32 /*eventID*/, bool /*IsEventingNow*/>m_ScriptEventMap;

				std::map<uint8, bool>								m_QuestEventStates;
				std::map<uint8, std::wstring>						m_QuestEventExplains;

				std::map<uint8, bool>								m_NpcDropEventStates;
				std::map<uint8, std::wstring>						m_NpcDropEventExplains;
				class CurrentEvent
				{
				public:
					void SendCurrentStageEvents(UserSharedPtr& user);

					void AddCurrentEvent(const uint32& key, const Database::Info::EventUnit::ParamContainer& eventInfo, const DateTime& endTime);

					void EraseEvent(const uint32& key);

				private:
					std::mutex cs;

					typedef std::map< uint32, std::pair< Database::Info::EventUnit::ParamContainer, DateTime > > CurrentEventInfos;

					CurrentEventInfos infos;
				};

				CurrentEvent currentEvent;
			};

			UserManager& UserManagerInstance();
		}
	}
}
