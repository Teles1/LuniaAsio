#include <LobbyServer/LobbyServer.h>
#include "fwPacketListener.h"
#include "Network/NetStream.h"
#include "./Network/Api/Api.h"
#include <LobbyServer/Common.h>
#include <LobbyServer/User.h>
namespace Lunia {
	namespace XRated {
		namespace Lobby
		{
			template <typename T> inline std::shared_ptr<T> shared_from_enabled(std::enable_shared_from_this<T>* base) {
				return base->shared_from_this();
			}

			template <typename T> inline std::shared_ptr<T> shared_from(T* derived) {
				return std::static_pointer_cast<T>(shared_from_enabled(derived));
			}

			template<> std::shared_ptr<User> shared_from_enabled(std::enable_shared_from_this<User>* base);

			

			std::shared_ptr<User> User::shared_from_this() {
				return shared_from(this);
			}

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


				fwPacketListener::GetInstance().Invoke(shared_from_this(), sReader.GetSerializedTypeHash(), sReader);

				HandleRead();
				return (uint32)size;
			}

			void User::Terminate(bool withRemoval) {
				Http::TextPacket packet("Terminate");
				packet << m_AccountName << L"";
				DB().Request(packet);
				if(withRemoval)
					UserRegistry().RemoveUser(shared_from_this());
			}

			bool User::SatisfyAlivePingWait()
			{
				Lobby::Protocol::FromServer::Alive packet;

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
				this->m_isAuthenticated = true;
			}

			bool User::DoesHaveLicense(XRated::Constants::ClassType classType)
			{
				AutoLock _l(mtx);
				for(auto x : m_AccountLicenses)
					if(x == (int)classType)
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
				for (CharactersIterator it = m_Characters.begin(); it != m_Characters.end(); it++)
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
				}
				else
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

			void User::Authorize(std::shared_ptr<User> user, int16 errorNumber, Http::TextPacket* packet, unsigned int senderUniqueSerial)
			{
				AutoLock lock(mtx);
				Lobby::Protocol::FromServer::Auth sendPacket;
				sendPacket.Result = static_cast<Lobby::Protocol::FromServer::Auth::Results>(errorNumber);
				if (!packet) {
					LoggerInstance().Error("Not a valid packet received from DB");
					return; // Gotta close the user somehow from within the user ? lol
				}
				//Set account name

				switch (sendPacket.Result) {
				case Lobby::Protocol::FromServer::Auth::Results::Ok:
				{
					String dbAccountName = StringUtil::ToUnicode((*packet)[0]);
					// Checking if accountName is valid and the serials are matching.
					if (dbAccountName.empty() || senderUniqueSerial != GetUniqueSerial()) {
						Terminate();
						LoggerInstance().Error(L"User({}) is being closed with an authorizing request", dbAccountName);
						return;
					}

					Http::TextPacket request("ListCharacter");
					request << m_AccountName;
					DB().Request(user, request, *this, &User::CharacterListed, true);
					break;
				}
				default: 
				{
					Terminate();
					break;
				}
				Send(sendPacket);
				}
			}

			void User::CharacterListed(std::shared_ptr<User> user, int16 errorNumber, Http::TextPacket* packet, unsigned int senderUniqueSerial)
			{
				AutoLock lock(mtx);
				/*if (!IsAccountAuthorized()){ // This has to go to SelectCharacter making sure he has second password checked.
					LoggerInstance().Error("List character called for a non authorized account");
					return;
				}*/
				assert(packet && errorNumber == 0);

				if (errorNumber != 0 || packet == NULL) {
					LoggerInstance().Error("Unexpected http error on CharacterList");
					return;
				}
				if (packet->IsEmpty() == false) {
					uint16 i(0), j(0);

					m_NumberOfSlots = StringUtil::ToInt((*packet)[i++]);

					String accountName(StringUtil::ToUnicode((*packet)[i++]));

					if (accountName != GetAccountName())
					{
						LoggerInstance().Error(L"account name mismatched on listing characters ({}, {})", accountName.c_str(), GetAccountName().c_str());
						Terminate();
						return;
					}

					Http::TextPacket::Subpacket subpackets((*packet)[i++]);
					j = 0;
					while (j < subpackets.GetCount())
					{
						m_AccountLicenses.push_back(StringUtil::ToInt(subpackets[j++]));
					}

					if (packet->GetCount() > 4)
					{
						while (i < packet->GetCount())
						{
							m_Characters.push_back(LobbyPlayerInfo());
							LobbyPlayerInfo& info = m_Characters.back();

							info.CharacterName = StringUtil::ToUnicode((*packet)[i++]);
							info.CharacterSerial = StringUtil::ToInt64((*packet)[i++]); // added on 3.1 packet :C
							info.VirtualIdCode = static_cast<uint32>(StringUtil::ToInt((*packet)[i++]));
							info.ClassType = static_cast<Constants::ClassType>(StringUtil::ToInt((*packet)[i++]));
							info.Level = static_cast<uint16>(StringUtil::ToInt((*packet)[i++]));
							info.Exp = static_cast<uint32>(StringUtil::ToInt((*packet)[i++]));
							info.PvpLevel = static_cast<uint16>(StringUtil::ToInt((*packet)[i++]));
							info.PvpExp = static_cast<uint32>(StringUtil::ToInt((*packet)[i++]));
							info.WarLevel = static_cast<uint16>(StringUtil::ToInt((*packet)[i++]));
							info.WarExp = static_cast<uint32>(StringUtil::ToInt((*packet)[i++]));
							info.LastLoggedDate.Parse(StringUtil::ToUnicode((*packet)[i++]).c_str());
							info.StateFlags = static_cast<XRated::CharacterStateFlags>(StringUtil::ToInt((*packet)[i++]));
							info.RebirthCount = static_cast<uint32>(StringUtil::ToInt((*packet)[i++]));
							info.StoredLevel = static_cast<uint32>(StringUtil::ToInt((*packet)[i++]));

							// subseparated licenses
							Http::TextPacket::Subpacket licenses((*packet)[i++]);
							j = 0;
							while (j < licenses.GetCount())
							{
								info.Licenses.push_back(XRated::StageLicense());
								info.Licenses.back().StageGroupHash = static_cast<uint32>(StringUtil::ToInt(licenses[j++]));
								info.Licenses.back().Level = static_cast<uint16>(StringUtil::ToInt(licenses[j++]));
							}

							// subseparated equipment
							Http::TextPacket::Subpacket equipments((*packet)[i++]);
							j = 0;
							while (j < equipments.GetCount())
							{
								XRated::ItemSlot slot;
								slot.Position.Bag = static_cast<uint8>(StringUtil::ToInt(equipments[j++])); // equipment slots at
								slot.Position.Position = static_cast<uint8>(StringUtil::ToInt(equipments[j++]));
								slot.Stacked = 1; // equipments cannot be stacked
								slot.Id = StringUtil::ToInt(equipments[j++]);
								slot.InstanceEx.Instance = StringUtil::ToInt64(equipments[j++]);
								slot.InstanceEx.ExpireDate.Parse(StringUtil::ToUnicode(equipments[j++]).c_str());
								info.Equipments.push_back(slot);
							}
						}
					}

					Protocol::FromServer::CharacterSlots slots;
					slots.NumberOfSlots = m_NumberOfSlots;
					slots.CharacterLicenses = m_AccountLicenses;
					Send(slots);

					Protocol::FromServer::ListCharacter sendPacket;
					sendPacket.Characters = m_Characters;
					Send(sendPacket);

					Http::TextPacket request("GetKeySetting");
					request << accountName;
					DB().Request(user, request, *this, &User::KeyLoaded);

				}
			}

			void User::KeyLoaded(std::shared_ptr<User> user, int16 errorNumber, Http::TextPacket* packet, unsigned int senderUniqueSerial)
			{
				Http::TextPacket& parameters = *packet;
				if (errorNumber != 0 || packet == NULL)
				{
					//error
					return;
				}
				Protocol::FromServer::LoadKeySetting result;
				if (parameters.GetCount() < 1) 
				{
					LoggerInstance().Info("There are no KeySetting : parameters NULL");
				}
				else
				{
					result.KeyCodes = StringUtil::ToUnicode(parameters[0]);
				}
				user->Send(result);

				//SendEventData();
			}
			
			void User::Auth_2nd_Check(std::shared_ptr<User> user, int16 errorNumber, Http::TextPacket* packet, unsigned int senderUniqueSerial)
			{
				Protocol::FromServer::SecondPasswordChecked sendPacket;
				sendPacket.Result = Protocol::FromServer::SecondPasswordChecked::NoResponse;
				if (errorNumber == 0 && packet) {
					Http::TextPacket::Subpacket subpackets((*packet)[0]);
					sendPacket.Result = Protocol::FromServer::SecondPasswordChecked::Ok;
					sendPacket.PasswordInUse = StringUtil::ToInt(subpackets[0]);
					sendPacket.FailCount = StringUtil::ToInt(subpackets[1]);
					sendPacket.LockExpired = StringUtil::ToUnicode(subpackets[2]);
					if (sendPacket.PasswordInUse == 0)
						UserRegistry().AuthenticateUser(user);
					user->Send(sendPacket);
					return;
				}
				Terminate(true);
				user->Send(sendPacket);
			}

			uint32 User::GetId() const {
				return m_userId;
			}

			uint32 User::GetUniqueSerial() const {
				return m_userId;
			}

			void User::SetId(const uint32& userId)
			{
				AutoLock lock(mtx);
				m_userId = userId;
			}

			void User::SetLocale(const String& inLocale)
			{
				this->m_Locale = inLocale;
			}

			void User::SetAccountName(const String& inAccountName)
			{
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
}