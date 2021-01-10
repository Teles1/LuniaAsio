#pragma once
#include <StageServer/fwPacketListener.h>
#include <StageServer/User/UserManager.h>
#include <Network/NetStream.h>
#include <Network/CommonProtocol/Protocol.h>
#include <StageServer/Protocol/FromServer.h>
#include <Info/Info.h>
#include <StageServer/User/IUserRoom.h>
#include <Core/ErrorDefinition.h>
#include <StageServer/Services/ItemSerial.h>
#include <mmsystem.h>
#include <StageServer/User/Items/Enchanter.h>
#include "User.h"
namespace Lunia {
	namespace XRated {
		namespace StageServer
		{
			template <typename T> inline std::shared_ptr<T> shared_from_enabled(std::enable_shared_from_this<T>* base) {
				return base->shared_from_this();
			}
			
			template <typename T> inline std::shared_ptr<T> shared_from(T* derived) {
				return std::static_pointer_cast<T>(shared_from_enabled(derived));
			}
			
			template<> std::shared_ptr<User> shared_from_enabled(std::enable_shared_from_this<User>* base);
			
			bool User::IsAdmitable(const uint32& receivedBytes, const float& costFactor)
			{
				return m_floodChecker.IsAdmitable(receivedBytes, GetCharacterStateFlags().IsAdmin ? 0.0f : costFactor);
			}

			void User::Send(const Protocol::IPacketSerializable& packet) {
				StaticBuffer<2 << 12> buffer;
				try
				{
					Net::StreamWriter writer(buffer);
					writer.Write(packet);
				}
				catch (Exception&) // catch something like buffer overflow
				{
					Logger::GetInstance().Error("User@{0} Unable to Parse packet", this->GetSerial());
					return;
				}
				Logger::GetInstance().Info(L"Sending packet[{0}] to User@{1}", packet.GetTypeName(), this->GetSerial());
				this->SendAsync(reinterpret_cast<uint8*>(buffer.GetData()), buffer.GetLength());
			}
			
			uint32 User::Parse(uint8* buffer, size_t size) {
				/*
					We should technically see but i think that if the server doesn't recognize the packet I say we let the client connection dies.
					That's why I want to keep this call in here
					Parse only known packets. At least the ones which follow NetStream Standarts.
				*/

				Net::StreamReader sReader(buffer);
				if (!IsAuthenticated())
					fwPacketListener::GetInstance().Invoke(UserManagerInstance().GetUserByConnectionId(m_UserId), sReader.GetSerializedTypeHash(), sReader);
				else
					fwPacketListener::GetInstance().Invoke(UserManagerInstance().GetUserBySerial(m_UserSerial), sReader.GetSerializedTypeHash(), sReader);
				HandleRead();
				return (uint32)size;
			}

			void User::SendToAll(Protocol::IPacketSerializable& packet)
			{
				if (m_Room && IsConnected())
					m_Room->SendToAll(packet);
			}

			void User::Dispatch(Protocol::ToServer::LoadEnd& packet)
			{
				AutoLock lock(m_UserMtx);
				if (!IsAuthed()) {
					LoggerInstance().Error("User({0})::LoadEnd - User Not Authed and sent LoadEnd", GetSerial());
					return;
				}
				if ((packet.progress < 0.0f) || (packet.progress > 1.0f) || IsLoaded()) {
					LoggerInstance().Error("User({0})::LoadEnd - User sent LoadEnd({1}) and LoadedEnd({2}) not valid", GetSerial(), packet.progress, IsLoaded());
					return;
				}
				if (m_Room == nullptr) {
					LoggerInstance().Error("User({0})::LoadEnd - User sent LoadEnd({1}) room is nullptr", GetSerial(), packet.progress);
					return;
				}
				if (packet.progress >= 1.0f) {
					m_LoadEnded = true;
					m_IsItemLocked = false;
				}
				m_Room->JoinEndUser(shared_from_this(), packet.progress);
			}

			void User::Dispatch(Protocol::ToServer::Family::RefreshInfo& packet)
			{
				m_FamilyManager.Dispatch(shared_from_this(), packet);
			}

			void User::Dispatch(StageServer::Protocol::ToServer::Join& packet)
			{
				AutoLock lock(m_UserMtx);
				if (!IsLoaded()) {
					LoggerInstance().Error("User({})::Dispatch - Not loaded and sending join", GetSerial());
					Protocol::FromServer::Error error;
					error.errorcode = Errors::InvalidStageCode;
					//Send(error);
				}
			}

			void User::Dispatch(Protocol::ToServer::Command& packet)
			{
				AutoLock lock(m_UserMtx);
				AutoLock playerLock(m_PlayerMtx);
				if (!m_Room || !m_Player)
					return;
				if (m_State == LOAD || m_State == NONE)
					return;
				if (m_FishingManager.IsFishing())
					return;
				if (!m_Room->Command(m_Player, (Constants::Command)packet.command, packet.direction)) {
					// invalid command like an action in square
					LoggerInstance().Error("closing an user by wrong command (modified client or its data) : {}", GetSerial());
					Close();
				}
			}

			void User::LicenseAquired(const UserSharedPtr& user, const Net::Answer& answer)
			{
				Protocol::FromServer::AcquireCharacterLicense response;
				if (answer.errorCode == 0)
					answer.get_to("characterClass", response.CharacterClass);
				response.result = static_cast<Protocol::FromServer::AcquireCharacterLicense::Result>(answer.errorCode);
				Send(response);
			}

			void User::PetCreated(const UserSharedPtr& user, const Net::Answer& answer)
			{
				if (answer.errorCode != 0) {
					Protocol::FromServer::Error error;
					switch (answer.errorCode) {
					case 2:
						error.errorcode = XRated::Errors::PetCreateFailed_InvalidItem;
						Send(error);
						break;
					case 3:
						error.errorcode = XRated::Errors::PetCreateFailed_ExsistName;
						Send(error);
						break;
					}
					return;
				}
				{
					AutoLock lock(m_UserMtx);
					LockItems(false);
				}
				{
					auto& pet = answer.resultObject;
					XRated::Pet petData;

					//response : pet(separated : serial, petName, stageLevel, stageExp, satisfiedOfFood, satisfiedOfPresent, rareProbability, petHash)
					if (!pet.is_null()) {
						pet.at("serial").get_to(petData.PetSerial);
						pet.at("petName").get_to(petData.PetName);
						pet.at("level").get_to(petData.Level);
						pet.at("exp").get_to(petData.Exp);
						pet.at("full").get_to(petData.Full);
						pet.at("rareProbability").get_to(petData.RareProbability);
						XRated::ItemPosition itemPosition = m_Items.FindPetItem(petData.PetSerial);
						if (itemPosition == XRated::ItemPosition::Invalid) {
							CriticalError(fmt::format("can not found pet item from DBCreatedPet : {}", petData.PetSerial).c_str(), false);
							return;
						}
						pet.at("hash").get_to(petData.PetHash);
						petData.Appear = false;

						try {
							m_Items.AddInventory(petData);
						}
						catch (Exception& e) {
							LoggerInstance().Exception("PetCreated: Add Pet Inventory Failed {}", e.what());
							CriticalError("PetCreated: Add Pet Inventory Failed", true);
							return;
						}

						if (m_Player)
							SendUpdatedPetInfo(petData, m_Player->GetSerial(), 0, L"PET_CREATE");
						else
							SendUpdatedPetInfo(petData, 0, 0, L"PET_CREATE");
						XRated::Pet pet(petData);
						if (m_Room) {
							m_Room->CreatePet(m_Player, pet);
						}

						{
							AutoLock lock(m_PlayerInitialData.SyncRoot);
							m_OriginPets.push_back(petData);
						}

						const Common::ItemEx* item = m_Items.GetItem(itemPosition.Bag, itemPosition.Position);

						/* Pet Created Logging */
						/*Logger().Write(IAdmLogger::NormalLogger, "pet-create", GetName(), Http::Logger::Parameter()
							<< petData.PetSerial << petData.PetHash << item->InstanceEx << petData.PetName);*/
					}
					Protocol::FromServer::UseItemEx response;
					response.Result = Protocol::FromServer::UseItemEx::Ok;
					Send(response);

				}
			}

			User::User(asio::ip::tcp::socket&& socket, const uint32& userId)
				: ClientTcp(std::move(socket))
				, m_UserId(userId)
				, m_GuildState(*this)
				, m_FamilyManager(*this)
				, m_Items(*this)
				, playTimeEventCheckIntervalInSec(ConfigInstance().Get("PlayTimeCheckIntervalInSec", 60.0f))
			{
				Logger::GetInstance().Info("User :: Hey, I was created!", GetId());
			}

			void User::AddTradedPet(const XRated::Pet& pet)
			{
				if (m_Player)
					SendUpdatedPetInfo(pet, m_Player->GetSerial(), 0, L"");
				else
					SendUpdatedPetInfo(pet, 0, 0, L"");
				m_Room->CreatePet(m_Player, pet);

				PetOrigin origin(pet);
				origin.needChangeOwner = true;
				{
					AutoLock lock(m_PlayerInitialData.SyncRoot);
					m_OriginPets.push_back(origin);
				}
			}

			std::shared_ptr<User> User::shared_from_this() {
				return shared_from(this);
			}

			void User::RemoveTradedPet(const XRated::GlobalSerial& petSerial)
			{
				AutoLock lock(m_PlayerInitialData.SyncRoot);
				std::vector<PetOrigin>::iterator iter = m_OriginPets.begin();
				std::vector<PetOrigin>::iterator end = m_OriginPets.end();
				while (iter != end) {
					if (iter->PetSerial == petSerial) {
						m_OriginPets.erase(iter);
						SendPetDeleted(petSerial);
						m_Room->DropPetItem(m_Player, petSerial);
						break;
					}
					++iter;
				}
			}

			bool User::IsItemInPetInventory(const GlobalSerial& petSerial)
			{
				return m_Items.IsItemInPetInventory(petSerial);
			}

			void User::CreatePet(const String& petName, const GlobalSerial& petSerial, const uint32& petHash)
			{
				if (XRated::Pet::IsValidPetNameSize(petName) == false) {
					LoggerInstance().Warn("Create Pet Failed : User[{0}] limit over Pet name Size : {1}", GetSerial(), petName.size());
					return;
				}
				XRated::ItemPosition itemPosition = m_Items.FindPetItem(petSerial);
				if (itemPosition == XRated::ItemPosition::Invalid) {
					CriticalError(fmt::format("can not found pet item from CreatePet : {0}", petSerial).c_str(), false);
					return;
				}

				if (petHash == 0)
				{
					throw Exception("User::CreatePet() Serial is not for pet item ( serial : {0} )", petSerial);
				}

				const Database::Info::Pet::Species* species = XRated::Database::DatabaseInstance().InfoCollections.Pets.GetSpecies(petHash);
				if (!species) {
					throw Exception("User::CreatePet() can not found species {0}", petHash);
				}

				LockItems(true);

				Net::Api request("Pet/Create");
				request << GetSerial() << petSerial << petName << petHash;
				request.GetAsync(this, &User::PetCreated, shared_from_this());
			}

			void User::SummonPet(const uint64& petSerial, const uint8& bag, const uint8& position)
			{
				if (!m_Player || m_CharacterStateFlags.IsPetNotUsable == 1)
					return;

				if (!m_Room)
				{
					Database::Info::StageInfo* stageInfo = m_Room->GetStageInfo();

					if (!stageInfo)
					{
						if (stageInfo->isSkillAvailable == false)
						{
							return;
						}
					}
				}

				const XRated::Pet* pet = m_PetDatas.GetPetData(petSerial);
				const Common::ItemEx* slot = GetItem(XRated::ItemPosition(bag, position));

				m_Room->UnsummonPet(m_Player, GetSerial());

				if (!pet || !slot || !slot->Info || Database::Enchant::IsExpired(slot->Info, slot->InstanceEx))
					return;

				GetPetDatas().RequestedSummonPet(petSerial);
				m_Items.UsePetItem(bag, position);
			}

			void User::UnsummonPet(void)
			{
			}

			void User::SendUpdatedPetInfo(const XRated::Pet& petData, const XRated::Serial& playerSerial, const XRated::Serial& petSerial, const std::wstring& petExpression)
			{
			}

			void User::SendPetDeleted(const XRated::GlobalSerial& petSerial)
			{
			}

			Protocol::FromServer::PetUpdated User::GetUpdatedPetInfo(const XRated::Pet& petData, const XRated::Serial& playerSerial, const XRated::Serial& petSerial, const std::wstring& petExpression)
			{
				return Protocol::FromServer::PetUpdated();
			}

			void User::PetChange(const uint32 petItemHash, const XRated::GlobalSerial& petItemSerial)
			{
			}

			void User::PetOwnerChange(const XRated::GlobalSerial& petItemSerial)
			{
			}

			bool User::PetFeed(const GlobalSerial& petSerial, const uint32& foodAmount, const uint32& foodCount, bool overFeed)
			{
				if (!m_Room || !m_Player)
				{
					LoggerInstance().Error("User::PetFeed() - Error : room == NULL || player == NULL");
					return false;
				}

				m_Room->PetFeed(m_Player, petSerial, foodAmount, foodCount, overFeed);
				return true;
			}

			bool User::GivePresentToPet(const GlobalSerial& petSerial, const uint32& sellPrice, const uint32& count)
			{
				if (!m_Room || !m_Player)
				{
					LoggerInstance().Error("User::GivePresentToPet() - Error : room == NULL || player == NULL");
					return false;
				}
				m_Room->GivePresentToPet(m_Player, petSerial, sellPrice, count);
				return true;
			}

			bool User::PetLevelDecrease(const GlobalSerial& petSerial, const uint32& downLevel, const bool& onlyMaxLevel)
			{
				if (!m_Room || !m_Player)
				{
					LoggerInstance().Error("User::PetLevelDecrease() - Error : room == NULL || player == NULL");
					return false;
				}
				m_Room->PetLevelDecrease(m_Player, petSerial, downLevel, onlyMaxLevel);
				return false;
			}

			void User::TakeCarePetBySchool(const uint32& petItemHash, const GlobalSerial& petSerial, const Database::Enchant::EnchantBitfields& instance, const uint8& count, const uint32& period, const float& expFactor, const uint32& bag, const uint32& pos, uint32 schoolTicketHash)
			{
				if (!m_Room || m_Player)
				{
					LoggerInstance().Error("User::TakeCarePetBySchool() - Error : room == NULL || player == NULL");
					return;
				}

				m_Room->TakeCarePetBySchool(m_Player, petItemHash, petSerial, instance, count, period, expFactor);

				{

					AutoLock lock(m_PetDatas.GetSyncObject());
					const XRated::Pet* pet = m_PetDatas.GetPetData(petSerial);
					if (pet) {
						/*Logger().Write(IAdmLogger::NormalLogger, "pet-training-in", GetName(), Http::Logger::Parameter()
							<< petSerial << pet->Level << pet->Exp << schoolTicketHash);*/
					}
				}

				if (m_Items.IsItemRequirement())
				{
					LoggerInstance().Error(L"TakeACrePetBySchool Failed, warning pet data copy");
					return;
				}
				if (m_IsItemLocked)
				{
					CriticalError("invalid state to drop item");
					return;
				}

				Common::ItemEx droppedItem;
				droppedItem.Info = nullptr;
				if ((m_Items.DropItems(bag, pos, count, true, &droppedItem) == true) && droppedItem.Info ) {
				}

			}

			void User::PetRename(const XRated::GlobalSerial petSerial, const std::wstring& newName)
			{
				if (!m_Room || !m_Player)
				{
					LoggerInstance().Error("User::PetLevelDecrease() - Error : room == NULL || player == NULL");
					return;
				}
				m_Room->PetRenaming(GetSerial(), petSerial, newName);
			}

			void User::Init() {
				m_LastDataSavedTime = m_ConnectTime = m_AliveTime = timeGetTime();
				Protocol::FromServer::Way way;
				way.EncryptKey = Math::Random();
				m_FishingManager.Init();
				SetCryptoKey(way.EncryptKey);
				Send(way);
			}

			void User::Flush()
			{
				if (!IsConnected())
					return;
				//LoggerInstance().Info("Flushing user:{0}", GetSerial());
				//CloseSocket();
			}

			bool User::AuthConnection(const json& result)
			{
				AutoLock lock(m_UserMtx);
				m_CharacterName = StringUtil::ToUnicode(result["characterName"].get<std::string>());
				m_RoomIndex = result["roomNumber"].get<uint32>();
				m_CurrentStage.StageGroupHash = result["stageGroupHash"].get<uint32>();
				m_CurrentStage.Level = result["accessLevel"].get<uint16>();
				m_CurrentStage.Difficulty = result["difficulty"].get<uint8>();
				m_RoomPass = result["password"].get<std::string>();
				m_RoomCapacity = result["capacity"].get<uint16>();
				m_TeamNumber = result["teamNumber"].get<uint8>();
				m_StageStateFlags = result["stageStates"].get<int>();
				m_LastStage.StageGroupHash = result["lastPlayedStageGroupHash"].get<uint32>();
				m_LastStage.Level = result["lastPlayedStageLevel"].get<uint16>();
				m_RoomActivationSerial = result["roomActivationSerial"].get<int64>();
				return true;
			}

			bool User::Auth(const json& result)
			{
				AutoLock lock(m_UserMtx);//self lock.
				AutoLock stageLock(m_PlayerInitialData.SyncRoot); // prevent the room/stagescript from modifiyng the userData
				if (!Auth_CharacterInfos(result["characterInfo"])) {
					Logger::GetInstance().Error(L"User::Auth_CharacterInfos={0}", GetSerial());
					return false;
				}
				if (!Auth_CharacterRebirth(result["characterRebirth"])) {
					Logger::GetInstance().Error(L"User::Auth_CharacterRebirth={0}", GetSerial());
					return false;
				}
				if (!Auth_CharacterLicenses(result["accountsLicenses"])) {
					Logger::GetInstance().Error(L"User::Auth_CharacterLicenses={0}", GetSerial());
					return false;
				}
				m_Items.SetItemList(result["bags"], result["items"]);
				if (!Auth_SkillLicenses(result["skillLicenses"])) {
					Logger::GetInstance().Error(L"User::Auth_SkillLicenses={0}", GetSerial());
				}
				if (!Auth_Skills(result["skills"])) {
					Logger::GetInstance().Error(L"User::Auth_Skills={0}", GetSerial());
				}
				if (!Auth_QuickSlots(result["quickSlots"])) {
					Logger::GetInstance().Error(L"User::Auth_QuickSlots={0}", GetSerial());
				}
				if (!Auth_PetsInfo(result["petsInfo"])) {
					Logger::GetInstance().Error(L"User::Auth_PetsInfo={0}", GetSerial());
				}
				if (!Auth_PetsItems(result["petsItems"])) {
					Logger::GetInstance().Error(L"User::Auth_PetsItems={0}", GetSerial());
				}
				if (!Auth_PetsTraining(result["petsTraining"])) {
					Logger::GetInstance().Error(L"User::Auth_PetsTraining={0}", GetSerial());
				}
				m_ConnectDateTime = DateTime::Now();
				return true;
			}

			bool User::Auth_CharacterInfos(const json& characterinfo)
			{
				//StageGroup. If this is not valid we don't even need to bother authing the user.
#pragma region StageGroupVerification
				auto stageGroup = Database::DatabaseInstance().InfoCollections.StageGroups.Retrieve(m_CurrentStage.StageGroupHash);
				if (stageGroup == NULL) {
					Logger::GetInstance().Error("User::Auth({0}) - stageGroup({1}) == NULL", GetSerial(), m_CurrentStage.StageGroupHash);
					return false;
				}
				if (stageGroup->Stages.size() <= m_CurrentStage.Level) {
					Logger::GetInstance().Error("User::Auth({0}) - StageLevel={1} not found on current StageGroup={2}", GetSerial(), m_CurrentStage.Level, m_CurrentStage.StageGroupHash);
					return false;
				}
				m_CurrentStageHash = stageGroup->StageHashes[m_CurrentStage.Level];
				auto stageInfo = stageGroup->Stages.at(m_CurrentStage.Level);

				if (!stageInfo || !stageInfo->isOpened) {
					Logger::GetInstance().Error("User::Auth({0}) - Stage{1} is closed.", GetSerial(), m_CurrentStageHash);
					return false;
				}
#pragma endregion
				/*
					tempId,characterName
					classNumber,stageLevel,stageExp,pvpLevel,pvpExp,warLevel,warExp,gameMoney,bankMoney,skillPoint,instantStateFlags,createDate
					addedSkillPoint
					rebirthCount,storedLevel,storedSkillPoint,lastRebirthDate
					achievementPoint
					characterLicenses(subseparated: classNumber)
					StageLicense(subseparated: stageGroupHash,accessLevel)
					skilllicenses(subseparated: skillGroupHash)
					Items(subseparated: itemHash,bagNumber,positionNumber,stackedCount,itemSerial,instance,itemExpire)
					skills(subseparated: skillGroupHash,skillLevel)
					quickslots(subseparated: hash,position,isSkill,instance,itemExpire)
					bags(subseparated: bagNumber,isBank,expireDate)
					bankBags(subseparated: bagNumber,expireDate)
					pets(subseparated: petSerial,petName,petLevel,petExp,fullValue,isRare,rareProbability,fullRateSum,dtSum,petHash,enchantSerial)
					petItems(subseparated: petSerial,bagNumber,positionNumber,itemHash,stackedCount,itemSerial,instance,itemExpire)
					petTraining(subseparated: petSerial,itemHash,stackedCount,instance,itemExpire,expFactor,startTime,endTime)

				*/
				// CharacterInfos
				{
					m_PlayerInitialData.team = m_TeamNumber;
					m_PlayerInitialData.name = GetCharacterName();
					//Character

					auto& mPlayerData = characterinfo["character"];
					if (mPlayerData.is_null())
						return false;

					m_PlayerInitialData.type = (XRated::Constants::ClassType)mPlayerData["classNumber"].get<int>();
					m_PlayerInitialData.level = mPlayerData["stageLevel"].get<uint16>();
					m_PlayerInitialData.xp = mPlayerData["stageExp"].get<uint32>();
					m_PlayerInitialData.pvpLevel = mPlayerData["pvpLevel"].get<uint16>();
					m_PlayerInitialData.pvpXp = mPlayerData["pvpExp"].get<uint32>();
					m_PlayerInitialData.warLevel = mPlayerData["warLevel"].get<uint32>();
					m_PlayerInitialData.warXp = mPlayerData["warExp"].get<uint32>();
					m_PlayerInitialData.money = mPlayerData["gameMoney"].get<uint32>();
					m_PlayerInitialData.bankMoney = mPlayerData["bankMoney"].get<uint32>();
					m_PlayerInitialData.skillpoint = mPlayerData["skillPoint"].get<uint16>();
					m_CharacterStateFlags = mPlayerData["instantStateFlag"].get<int>();
					m_CreateDate.Parse(mPlayerData["createDate"].get<std::string>());

					//The one that gives you +1 that paid op one.
					mPlayerData["addedSkillPoint"].get_to(m_PlayerInitialData.addedSkillPointPlus);

					m_PlayerInitialData.characterStateFlags = m_CharacterStateFlags;

					//Pvp
					auto& pvp = characterinfo["ladderPoint"];
					if (pvp.is_null())
						Logger::GetInstance().Exception(L"Could not parse LadderPoint on Auth user = {0}", GetCharacterName());
					m_PlayerInitialData.ladderPoint = pvp["points"].get<uint32>();
					m_PlayerInitialData.ladderMatchCount = pvp["dailyPlayCount"].get<uint16>();
					m_PlayerInitialData.ladderWinCount = pvp["winCount"].get<uint32>();
					m_PlayerInitialData.ladderLoseCount = pvp["loseCount"].get<uint32>();

					//Achivement
					if (mPlayerData.at("achievementPoints").is_null())
						Logger::GetInstance().Exception(L"Could not parse achievementPoint on Auth user = {0}", GetCharacterName());
					mPlayerData.at("achievementPoints").get_to(m_PlayerInitialData.achievementScore);
				}
				//Setting the next position if square
				{
					if (stageGroup->GameType & XRated::Constants::SquareType) {
						if (m_LastStage.StageGroupHash != 0) { // there is no last stage
							const auto squareInfo = stageGroup->Stages.at(m_CurrentStage.Level);
							const auto laststageGroup = Database::DatabaseInstance().InfoCollections.StageGroups.Retrieve(m_LastStage.StageGroupHash);
							Database::Info::StageInfo* lastStageInfo = nullptr;
							if (laststageGroup == NULL) {
								Logger::GetInstance().Warn("User::Auth({0}) - LastStageGroup isn't valid ", GetSerial());
								return false;
							}
							else {
								if (laststageGroup->Stages.size() <= m_LastStage.Level)
									lastStageInfo = laststageGroup->Stages.back();
								else
									lastStageInfo = laststageGroup->Stages[m_LastStage.Level];
								auto it = squareInfo->SquareStartingPoints.find(lastStageInfo->Episode);
								if (it != squareInfo->SquareStartingPoints.end()) {
									m_PlayerInitialData.StartPosition.UseCustomStartPoint = true;
									m_PlayerInitialData.StartPosition.Place = it->second;
								}
							}
						}
					}
				}
				//Granted
				{
					Protocol::FromServer::Granted sendPacket;
					sendPacket.charactername = GetCharacterName();
					sendPacket.targetStage = m_CurrentStage;
					Send(sendPacket);
				}
				//CharacterInfo -Packet being sent to user
				{
					Protocol::FromServer::CharacterInfo characterinfo;
					characterinfo.classtype = m_PlayerInitialData.type;
					characterinfo.level = m_PlayerInitialData.level;
					characterinfo.xp = m_PlayerInitialData.xp;
					characterinfo.pvpLevel = m_PlayerInitialData.pvpLevel;
					characterinfo.pvpXp = m_PlayerInitialData.pvpXp;
					characterinfo.warLevel = m_PlayerInitialData.warLevel;
					characterinfo.warXp = m_PlayerInitialData.warXp;
					characterinfo.storedLevel = m_PlayerInitialData.storedLevel;
					characterinfo.rebirthCount = m_PlayerInitialData.rebirthCount;
					characterinfo.money = m_PlayerInitialData.money;
					characterinfo.bankMoney = m_PlayerInitialData.bankMoney;
					characterinfo.life = m_PlayerInitialData.life;
					characterinfo.skillpoint = m_PlayerInitialData.skillpoint;
					characterinfo.ladderPoint = m_PlayerInitialData.ladderPoint;
					characterinfo.ladderMatchCount = m_PlayerInitialData.ladderMatchCount;
					characterinfo.ladderWinCount = m_PlayerInitialData.ladderWinCount;
					characterinfo.ladderLoseCount = m_PlayerInitialData.ladderLoseCount;
					characterinfo.addedSkillPointPlus = m_PlayerInitialData.addedSkillPointPlus;
					characterinfo.storedSkillPoint = m_PlayerInitialData.storedSkillPoint;
					characterinfo.achievementScore = m_PlayerInitialData.achievementScore;
					characterinfo.IsSpectator = m_PlayerInitialData.characterStateFlags.IsSpectator == 1 ? true : false;
					Send(characterinfo);
				}
				if (m_CharacterStateFlags.IsPcRoom && ((stageGroup->GameType & Constants::GameTypes::PvpGameTypeMask) == 0))
				{
					m_PlayerInitialData.life = static_cast<uint16>(stageGroup->InitailLife + 1);// one more life for PC room
					m_PlayerInitialData.Factors.MaxHp = 1.2f;
					m_PlayerInitialData.Factors.MaxMp = 1.2f;
				}
				else { //normal user
					m_PlayerInitialData.life = static_cast<uint16>(stageGroup->InitailLife);
					m_PlayerInitialData.Factors.MaxHp = 1.0f;
					m_PlayerInitialData.Factors.MaxMp = 1.0f;
				}

				m_ExpFactorManager.SetPenal(m_StageStateFlags.IsPrivate);
				m_ExpFactorManager.SetExtra(m_CharacterStateFlags.IsPcRoom);

				m_PlayerInitialData.Factors.ExpFactor = m_ExpFactorManager.TotalExpFactor();
				m_Authed = true; // This is where we verify if the user has its data all setup and it's allowed to proceed
				return true;
			}

			bool User::Auth_CharacterLicenses(const json& licenses)
			{
				if (licenses.is_null())
					return false;
				//CharacterLicense // Which characters the user is allowed to create.
				licenses.get_to(m_CharacterLicenses);
				/*for (int i = 0; i < 16; i++) {
					if (aux & (1 << i))
						m_CharacterLicenses.push_back(i);
				}*/
				return true;
			}

			bool User::Auth_StageLicenses(const json& licenses)
			{
				if (licenses.is_null())
					return false;
				//StageLicense // What Stages the users is allowed to go and what AccessLevel the user has
				for (auto& x : licenses["stageLicenses"]) {
					m_StageLicenses.push_back(XRated::StageLicense(x["stageGroupHash"].get<uint32>(), x["accessLevel"].get<uint16>()));
					SendRewardMailByGainStageLicense(m_StageLicenses.back());
				}
				return true;
			}

			bool User::Auth_CharacterRebirth(const json& rebirth)
			{
				if (rebirth.is_null())
					return false;
				m_PlayerInitialData.rebirthCount = rebirth["rebirthCount"].get<uint16>();
				m_PlayerInitialData.storedLevel = m_PlayerInitialData.rebirthCount == 0 ? m_PlayerInitialData.level : rebirth["storedLevel"].get<uint16>();
				m_PlayerInitialData.storedSkillPoint = rebirth["storedSkillPoint"].get<uint16>();
				m_PlayerInitialData.lastRebirthDateTime.Parse(rebirth["lastRebirth"].get<std::string>());
				return true;
			}

			bool User::Auth_SetItemList(const json& items)
			{
				//We should check weather the position where the item is at is valid.
				Protocol::FromServer::ListItem sendPacket;
				for (auto& x : items)
					sendPacket.listitem.push_back(
						XRated::ItemSlot(
							x["itemHash"].get<uint32>(),
							x["stackedCount"].get<uint16>(),
							XRated::InstanceEx(
								x["instance"].get<int64>(),
								x["itemExpire"].get<std::string>())
						));
				Send(sendPacket);
				return true;
			}

			bool User::Auth_SetBagList(const json& bags)
			{
				if (bags.is_null())
					return false;
				//Check the reason why we have two tables doing the same damn thing.
				Protocol::FromServer::BagStates sendPacket;
				//Bags Character
				for (auto& x : bags["bags"])
					if (!x["isBank"].get<bool>())
						sendPacket.Bags.push_back(XRated::BagState(x["bagNumber"].get<int>(), x["expireDate"].get<std::string>()));

				//Bags Bank
				for (auto& x : bags["bankBags"])
					sendPacket.BankBags.push_back(XRated::BagState(x["bagNumber"].get<int>(), x["expireDate"].get<std::string>()));

				Send(sendPacket);
				return true;
			}

			bool User::Auth_SkillLicenses(const json& licenses)
			{
				for (auto& x : licenses) {
					m_SkillLicenses.push_back(x["skillGrouphash"].get<uint32>());
				}
				Protocol::FromServer::ListSkillLicenses sendPacket;
				sendPacket.listSkillLicenses = m_SkillLicenses;
				Send(sendPacket);
				return true;
			}

			bool User::Auth_Skills(const json& skills)
			{
				for (auto& x : skills) {
					m_PlayerInitialData.skills.push_back(XRated::Skill(x["skillGroupHash"].get<uint32>(), x["skillLevel"].get<uint8>()));
				}
				Protocol::FromServer::ListSkill sendPacket;
				sendPacket.listskill = m_PlayerInitialData.skills;
				Send(sendPacket);
				return true;
			}

			bool User::Auth_QuickSlots(const json& quickslots)
			{
				Protocol::FromServer::ListQuickSlot sendPacket;
				for (auto& x : quickslots) {
					XRated::QuickSlot tempSlot;
					tempSlot.Id = x["hash"].get<uint32>();
					tempSlot.Pos = x["position"].get<uint8>();
					tempSlot.IsSkill = x["isSkill"].get<bool>();
					if (!tempSlot.IsSkill) {
						tempSlot.InstanceEx.Instance = x["instance"].get<int64>();
						tempSlot.InstanceEx.ExpireDate.Parse(x["itemExpire"].get<std::string>());
					}
					else
						tempSlot.InstanceEx = XRated::InstanceEx(0);
					m_QuickSlot.SetSlot(tempSlot);
					sendPacket.quickslotlist.push_back(tempSlot);
				}
				m_QuickSlot.UpdateOriginData(); // What is this for?!
				Send(sendPacket);
				return true;
			}

			bool User::Auth_PetsInfo(const json& petsinfo)
			{
				Protocol::FromServer::PetInfo sendPacket;
				for (auto& x : petsinfo) {
					XRated::PetDataWithItemPos petDataWithPos;
					petDataWithPos.Pet.PetSerial = x["id"].get<int64>();
					petDataWithPos.Pet.PetName = StringUtil::ToUnicode(x["petName"].get<std::string>());
					petDataWithPos.Pet.Level = x["petLevel"].get<uint16>();
					petDataWithPos.Pet.Exp = x["petExp"].get<uint32>();
					petDataWithPos.Pet.Full = x["fullValue"].get<float>();
					petDataWithPos.Pet.PrevFull = petDataWithPos.Pet.Full;
					petDataWithPos.Pet.IsRarePet = x["isRare"].get<bool>();
					petDataWithPos.Pet.RareProbability = x["rareProbability"].get<float>();
					petDataWithPos.Pet.FullSum = x["fullRateSum"].get<float>();
					petDataWithPos.Pet.LevelUpPeriod = x["dtSum"].get<float>();
					petDataWithPos.Pet.PetHash = x["petHash"].get<uint32>();
					petDataWithPos.Pet.Appear = false;

					//XRated::ItemPosition itemPosition = items.FindPetItem(petDataWithPos.Pet.PetSerial);
					petDataWithPos.PetItemPosition = XRated::ItemPosition();//itemPosition;
					//packet
					sendPacket.PetDataWithPos.push_back(petDataWithPos);
					m_PlayerInitialData.pets.push_back(petDataWithPos.Pet);
					m_PetDatas.Add(petDataWithPos.Pet);
				}
				Send(sendPacket);
				return true;
			}

			bool User::Auth_PetsItems(const json& petsitems)
			{
				//We should check weather the position where the item is at is valid.
				Protocol::FromServer::ListPetItem sendPacket;
				for (auto& x : petsitems)
					sendPacket.PetsItems[x["petId"].get<int64>()].push_back(XRated::PetItemSlot(
						static_cast<XRated::PetItemSlot::PositionType>(x["bagNumber"].get<int>()),
						x["positionNumber"].get<uint8>(),
						x["itemHash"].get<uint32>(),
						XRated::InstanceEx(
							x["instance"].get<int64>(),
							x["itemExpire"].get<std::string>()
						),
						x["stackedCount"].get<uint16>()
					));
				Send(sendPacket);
				return true;
			}

			bool User::Auth_PetsTraining(const json& petstraining)
			{
				Protocol::FromServer::PetsCaredBySchool sendPacket;
				sendPacket.OwnerSerial = 0;
				for (auto& x : petstraining) {
					XRated::PetCaredBySchool petTraining;
					petTraining.PetItemSerial = x["petId"].get<int64>();
					petTraining.PetItemHash = x["itemHash"].get<uint32>();
					petTraining.PetItemCount = x["stackedCount"].get<uint16>();
					petTraining.PetItemInstanceEx = x["instance"].get<int64>();
					petTraining.ExpFactor = x["expFactor"].get<float>();
					petTraining.Start.Parse(x["startTime"].get<std::string>());
					petTraining.End.Parse(x["endTime"].get<std::string>());

					sendPacket.CaredPets.push_back(petTraining);
					m_PlayerInitialData.petCared.push_back(petTraining);
				}
				Send(sendPacket);
				return true;
			}

			void User::SendRewardMailByGainStageLicense(const XRated::StageLicense& license)
			{
				//Gotta have the mail system working beforehand.
			}

			void User::SetSerial(const uint64& userSerial){
				m_UserSerial = userSerial;
			}

			const int64& User::GetActivationSerial() const
			{
				return m_RoomActivationSerial;
			}

			bool User::IsAuthenticated() const
			{
				return m_UserSerial == 0 ? false : true;
			}

			bool User::IsAbleToJoinStage(const StageLicense& targetStage) const
			{
				//Data By Level License Check
				uint16 level = 0;
				if (m_Player != nullptr)
					level = m_Player->GetPlayerData().BaseCharacter.Level;
				else
					level = m_PlayerInitialData.level;
				uint16 rebitrhCount = 0;
				if (m_Player != nullptr)
					rebitrhCount = m_Player->GetPlayerData().RebirthCount;
				else 
					rebitrhCount = m_PlayerInitialData.rebirthCount;
				if (Database::DatabaseInstance().InfoCollections.LicenseProvider.IsEnableJoinToStage(level, rebitrhCount, targetStage))
					return true;
				//DB License Check
				for (auto& x : m_StageLicenses)
				{
					if (targetStage.StageGroupHash == x.StageGroupHash && targetStage.Level <= x.Level)
						return true;
				}
				return false;
			}

			bool User::IsAuthed() const
			{
				return m_Authed;
			}

			bool User::IsLoaded() const
			{
				return m_LoadEnded;
			}

			bool User::IsItemLocked() const
			{
				return m_IsItemLocked;
			}

			bool User::IsPartOfGuild() const
			{
				return m_IsPartOfGuild;
			}

			bool User::IsFishing() const
			{
				return m_FishingManager.IsFishing();
			}

			bool User::IsConnected() const
			{
				//Gotta figure out a good way to do this return.
				return true;
			}

			void User::SetCharacterName(const String& name) {
				this->m_CharacterName = name;
			}

			String User::GetCharacterName() const {
				return m_CharacterName;
			}

			const uint64& User::GetSerial() const
			{
				return m_UserSerial;
			}

			uint32 User::GetId() const {
				return m_UserId;
			}

			uint32 User::GetMoney() const
			{
				if (!m_Player)
				{
					LoggerInstance().Error("User::GetMoney() - Error : m_Player == NULL");
					return 0;
				}

				return m_Player->GetPlayerData().BaseCharacter.Money;
			}

			uint32 User::GetMaxAirCombo() const
			{
				if (!m_Player)
				{
					LoggerInstance().Error("User::GetMaxAirAttackComboCnt() - Error : m_Player == NULL");
					return 0;
				}

				return m_Player->GetMaxAirAttackComboCnt();
			}

			uint64 User::GetStageXp() const
			{
				if (!m_Player)
				{
					LoggerInstance().Error("User::GetStageXp() - Error : m_Player == NULL");
					return 0;
				}

				return m_Player->GetStageXp();
			}

			uint32 User::GetClearXp() const
			{
				if (!m_Player)
				{
					LoggerInstance().Error("User::GetClearXp() - Error : m_Player == NULL");
					return 0;
				}

				return m_Player->GetClearXp();
			}

			uint16 User::GetLife() const
			{
				if (!m_Player)
				{
					LoggerInstance().Error("User::GetLife() - Error : m_Player == NULL");
					return 0;
				}

				return m_Player->GetLife();
			}

			uint32 User::GetSPPoint() const
			{
				if (!m_Player)
				{
					LoggerInstance().Error("User::GetSPPoint() - Error : m_Player == NULL");
					return 0;
				}

				return m_Player->GetStylePoint();
			}

			uint16 User::GetRebirthCount() const
			{
				if (!m_Player)
				{
					LoggerInstance().Error("User::GetRebirthCount() - Error : m_Player == NULL");
					return 0;
				}

				return m_Player->GetRebirthCount();
			}

			uint32 User::GetRemainStackatbleItem(const Database::Info::ItemInfo* info, const InstanceEx& instance, const uint32& count) const
			{
				return m_Items.CountRemainStackableItem(info, instance, count);
			}

			uint32 User::GetBlankSlotCount() const
			{
				return m_Items.GetBlankSlotCountOfNormalBag();
			}

			uint32 User::GetBlankSlotCountOfNormalBag() const
			{
				return m_Items.GetEmptySlotCount();
			}

			std::shared_ptr<IUserRoom> User::GetRoom() const
			{
				return m_Room;
			}

			const DateTime& User::GetLastRebirthDate() const
			{
				if (!m_Player)
					return m_PlayerInitialData.lastRebirthDateTime;
				return m_Player->GetPlayerData().LastRebirthDateTime;
			}

			const DateTime& User::GetConnectDateTime() const
			{
				return m_ConnectDateTime;
			}

			const DateTime::Date User::GetConnectDate() const
			{
				return m_ConnectDateTime.GetDate();
			}

			const DateTime::Time User::GetConnectTime() const
			{
				return m_ConnectDateTime.GetTime();
			}

			const DateTime::Week::type User::GetConnectDayOfWeek() const
			{
				return m_ConnectDateTime.GetDayOfWeek();
			}

			void User::Update(const float& dt){
				UpdateStackedPlayTime(dt);
				//Update the user with the time.
			}

			void User::RoomJoined(std::shared_ptr<IUserRoom> room, StageLicense& targetStage)
			{
				this->m_Room = room;
				m_State = LOAD;

				Protocol::FromServer::Stage packet;
				packet.charactername = GetCharacterName();
				packet.targetStage = targetStage;
				Send(packet);
				//quest.RefreshWorkingQuests(this);
				m_FamilyManager.Init(GetCharacterName());
			}

			void User::PlayerCreated(Logic::Player* player)
			{
				AutoLock playerLock(m_PlayerMtx);
				this->m_Player = player;
				m_Items.SetPlayer(player);
				m_ExpFactorManager.SetGuild(false);
				m_ExpFactorManager.ClearItemExpFactor();
				m_ExpFactorManager.ClearGuildExpFactor();
				m_ExpFactorManager.ClearFamilyExpFactor();
				m_GuildState.Clear();
				if (IsPartOfGuild()) {
					m_ExpFactorManager.SetGuild(true);
					m_Room->AddGuildUser(shared_from_this());
				}
				if (m_Room->GetRoomKind() != Common::SQUARE) // passive item cannot be activated at square
					;//passiveItems.Use(items.GetPassiveItems());
				else
					;//passiveItems.Use(std::vector<const Database::Info::ItemInfo*>()); // empty passive
				m_QuestManager.RefreshCompletedQuests(shared_from_this(), m_CurrentStage);
				m_Player->InitAirAttackComboCnt();

				UpdatePlayerInfoByUserData();
				Database::Info::StageInfo* stageInfo = m_Room->GetStageInfo();
				if (stageInfo)
				{
					// Square
					if (m_Room->GetRoomKind() == Common::SQUARE)
					{
						m_CharacterStateFlags.LastSquareLocation = Database::DatabaseInstance().InfoCollections.EpisodeSquareAdventageInfos.GetEpisodeSquareAdventageFlag(stageInfo->ChannelName);
						m_PlayerInitialData.characterStateFlags.LastSquareLocation = GetCharacterStateFlags().LastSquareLocation;
						//ALLM_IMPORTANT((L"LastSquare : %s entered to %s(%s) square", player->GetName().c_str() , stageInfo->Title.c_str() , stageInfo->ChannelName.c_str()));
					}
					else
					{
						//ALLM_IMPORTANT((L"LastSquare : %s was at %s(%s) square", player->GetName().c_str() , stageInfo->Title.c_str() , stageInfo->ChannelName.c_str()));
						if (GetCharacterStateFlags().LastSquareLocation != 0)
						{
							if (GetCharacterStateFlags().LastSquareLocation == Database::DatabaseInstance().InfoCollections.EpisodeSquareAdventageInfos.GetEpisodeSquareAdventageFlag(stageInfo->ChannelName))
							{
								m_ExpFactorManager.SetEpisodeStageAdventage(true);


								if (m_Room != NULL && GetPlayer() != NULL)
								{
									m_Room->ChangedExpFactor(shared_from_this());
								}

								Protocol::FromServer::DisplayText displaytext;
								displaytext.type = XRated::Constants::DisplayTextType::SystemNotice;
								displaytext.message = L"$UI.Global.SystemChat.EpisodeSquareAdventage$";
								Send(displaytext);
							}
							else
								m_ExpFactorManager.SetEpisodeStageAdventage(false);
						}

						Database::Info::StageGroup* stageGroup = Database::DatabaseInstance().InfoCollections.StageGroups.Retrieve(m_CurrentStage.StageGroupHash);
						if (stageGroup != NULL && stageGroup->HasDifficulty == true && stageGroup->Difficulties.size() > m_CurrentStage.Difficulty)
						{
							m_ExpFactorManager.SetDifficultyExpFactor(stageGroup->Difficulties[m_CurrentStage.Difficulty].XpFactor - 1.0f);
							if (m_Room != NULL && GetPlayer() != NULL)
							{
								m_Room->ChangedExpFactor(shared_from_this());
							}
						}
					}
				}

			}

			void User::UpdatePlayerInfoByUserData()
			{
				if (m_Player)
				{
					m_Player->SetEnableNextLicense(m_IsEnableToJoinNextLicense);
					Database::Info::StageGroup* stageGroup = Database::DatabaseInstance().InfoCollections.StageGroups.Retrieve(m_CurrentStage.StageGroupHash);
					if (stageGroup)
					{
						Database::Info::StageInfo* stageInfo = Database::DatabaseInstance().InfoCollections.Stages.Retrieve(stageGroup->StageHashes[m_CurrentStage.Level]);
						if (stageInfo) {
							m_Player->SetGainNormalExp(stageInfo->ProprietyLevel);
							return;
						}
					}
					m_Player->SetGainNormalExp(0);
				}
			}

			void User::UpdateStackedPlayTime(const float& dt)
			{
				m_StackedTodayPlayTime += (DWORD)dt;

				m_StackedPlayTimeEventCheckTime += (DWORD)dt;

				if (m_StackedPlayTimeEventCheckTime >= playTimeEventCheckIntervalInSec)
				{
					//SendPlayTimePresentEventMailBy(m_StackedTodayPlayTime);

					m_StackedPlayTimeEventCheckTime = 0;
				}
			}

			void User::UpdateInfos()
			{
				m_LastDataSavedTime = timeGetTime();
				SavePlayerData();
			}

			void User::ClearFamilyExpFactor()
			{
				m_ExpFactorManager.ClearFamilyExpFactor();
				if (!m_Player || !m_Room)
					return;
				m_Room->ChangedExpFactor(shared_from_this());
			}

			void User::SendSystemMail(const std::wstring& receiver, const std::wstring& title, const std::wstring& text, const uint32 attachedMoney, const std::vector<XRated::ItemBasicInfo>& sendItems, const std::wstring& sender)
			{
				m_MailBoxManager.SendSystemMail(receiver, title, text, attachedMoney, sendItems, sender);
			}

			Protocol::FromServer::Family::InviteResult::Type User::RequestInviteFamily(XRated::Family::FamilySerial familySerial, DateTime createdDate, XRated::Serial targetSerial, const std::wstring& targetname)
			{
				return m_Room->RequestInviteFamily(familySerial, createdDate, targetSerial, targetname, GetName());
			}

			Protocol::FromServer::Family::InviteResult::Type User::RequestInviteFamily(XRated::Serial targetSerial, const std::wstring& targetname)
			{
				return m_Room->RequestInviteFamily(targetSerial, targetname, GetName());
			}

			Protocol::FromServer::Family::InviteResult::Type User::RequestedInviteFamily(const std::wstring& ownerName, XRated::Family::FamilySerial familySerial, DateTime createdDate)
			{
				return m_FamilyManager.Invite( ownerName );
			}

			Protocol::FromServer::Family::InviteResult::Type User::RequestedInviteFamily(const std::wstring& ownerName)
			{
				return m_FamilyManager.Invite(ownerName);
			}

			void User::ReflashGuildProfit()
			{
				if (!m_Player || !m_Room)
					return;

				if (IsPartOfGuild()) {
					const Database::Info::GuildInfo::Level* newLevelInfo = Database::DatabaseInstance().InfoCollections.GuildInfos.GetLevelInfo(GetGuildInfo().GuildLevel);
					SetGuildExpFactor(newLevelInfo->AddExpFactor);

					DateTime now(DateTime::Now());
					if (GetGuildInfo().ShopOpenDate < now && GetGuildInfo().ShopCloseDate > now) {
						const Database::Info::GuildInfo::Level* info = Database::DatabaseInstance().InfoCollections.GuildInfos.GetLevelInfo(GetGuildInfo().GuildLevel);
						if (info == NULL) {
							CriticalError("can not found guild level Info", false);
							return;
						}
						m_GuildState.SetGuildLevelInfo(info);
					}
					else {
						m_GuildState.ClearGuildPassive();
					}
				}
			}

			void User::ClearCharacterRewardStateFlags()
			{
				m_CharacterRewardStateFlags = 0;
			}

			void User::NoticeStylePointState(const XRated::StylePoint::State& state)
			{
				if (!m_Player) return;

				Protocol::FromServer::StylePoint::ChangeState packet;
				packet.State = state;
				packet.CurrentPoint = m_Player->GetStylePoint();

				Send(packet);
			}			

			void User::AutoSaveUserInfo(const DWORD& nowTime, const uint32& synchronizePlayerDataInMs)
			{
				if (IsAuthed() && nowTime - m_LastDataSavedTime > synchronizePlayerDataInMs) {
					LoggerInstance().Info("Auto saving user:{0} at {1} ms", GetSerial(), nowTime - m_ConnectTime);
					m_LastDataSavedTime = nowTime;
					SavePlayerData();
				}
			}

			void User::SavePlayerData()
			{
			}

			void User::RebirthCharacter(const XRated::ItemPosition& itemPosition)
			{
				if (!m_Player) {
					LoggerInstance().Error("User::RebirthCharacter() - m_Player==null");
					return;
				}
				XRated::ItemPosition rebirthItemPosition = itemPosition;

				m_LastDataSavedTime = timeGetTime();
				SavePlayerData();
				//InfoReport(false, SNAPSHOTTYPE::ABSOLUTELY, true);

				auto tempRebirthLevel = GetLevel();
				auto tempStoredSkillPoint = XRated::Database::DatabaseInstance().InfoCollections.Statuses.GetRebirthSkillPoint(GetLevel());
				const Common::ItemEx* item = m_Items.GetItem(itemPosition.Bag, itemPosition.Position);
				auto tempRebirthItemHash = 0;
				auto levelAfterRebirth = 1;
				auto IsStageLicenseReset = true;
				auto IsQuestReset = true;

				if (item && item->Info )
				{
					tempRebirthItemHash = item->Info->Hash;

					std::vector< uint32 > statusTypeInfo;
					Database::Info::ActionInfo::StateList::const_iterator nologicStates = std::find_if(item->Info->States.begin(), item->Info->States.end(), Database::Info::ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());

					//std::map<std::wstring, std::wstring>::const_iterator param = nologicStates->params.find( L"LevelAfterRebirth" );
					std::map<std::wstring, std::wstring>::const_iterator param = nologicStates->params.find(L"IsLevelChange");
					if (param != nologicStates->params.end())
					{
						uint32 temp = StringUtil::To< uint32 >(param->second);
						if (temp == 0) {
							levelAfterRebirth = GetLevel();
						}
						else {
							levelAfterRebirth = 1;
						}
					}
					// stageLicense
					std::map<std::wstring, std::wstring>::const_iterator param2 = nologicStates->params.find(L"IsStageLicenseReset");
					if (param2 != nologicStates->params.end())
					{
						uint32 temp2 = StringUtil::To< uint32 >(param2->second);
						if (temp2 == 0) {
							IsStageLicenseReset = 0;
						}
					}
					// quests
					std::map<std::wstring, std::wstring>::const_iterator param3 = nologicStates->params.find(L"IsQuestReset");
					if (param3 != nologicStates->params.end())
					{
						uint32 temp3 = StringUtil::To< uint32 >(param3->second);
						if (temp3 == 0) {
							IsQuestReset = 0;
						}
					}
				}

			}

			void User::RemoveGuildAdvantage()
			{
				AutoLock lock(m_UserMtx);
				//m_Quests.DropGuildQuest(this);
				m_Items.RemoveItemsByAttribute(Database::Info::ItemInfo::ATTRIBUTES::GUILDITEM, true, true);
			}

			void User::SendRewardMailByGuildContibutedUp()
			{
				const Database::Info::AwardInfo* awardInfo = Database::DatabaseInstance().InfoCollections.MailAwardItemInfos.GetGuildContributedAwardInfo(m_AllMGuildInfo.MyContributed);
				if (awardInfo != NULL)
				{
					std::vector< XRated::ItemBasicInfo > items;

					items = awardInfo->Items;
					std::vector< XRated::ItemBasicInfo >::iterator iter = items.begin();
					for (; iter != items.end(); ++iter)
					{
						iter->ItemSerial = ItemSerialInstance().MakeItemSerial();
					}

					SendMailSelf(awardInfo->Title, awardInfo->Text, 0, items, awardInfo->Sender);
				}
			}

			void User::SendMailSelf(const std::wstring& title, const std::wstring& text, const uint32& attachedMoney, const std::vector<XRated::ItemBasicInfo>& sendItems, const std::wstring& sender)
			{
				//m_MailBoxManager.SendMailSelf(this, title, text, attachedMoney, sendItems, sender);
			}

			void User::LockItems(bool state)
			{
				m_IsItemLocked = state;
			}

			void User::ItemDrop(const ItemPosition& pos)
			{
				m_ItemsChanged = true; // change flag makes sure to save data
				Common::ItemEx item;
				item.Info = nullptr; // empty slot
				m_Items.SetItem(pos.Bag, pos.Position, item);
			}

			void User::DropItems(const uint32& bag, const uint32& pos, const uint32& count)
			{
				if (m_Items.IsItemRequirement())
					return;

				if (m_IsItemLocked)
				{
					CriticalError("invalid state to drop item");
					return;
				}

				Common::ItemEx droppedItem;
				droppedItem.Info = nullptr;
				if ((m_Items.DropItems(bag, pos, count, true, &droppedItem) == true) && (droppedItem.Info != NULL)) {
					if (Database::DatabaseInstance().InfoCollections.Pets.IsPetItem(droppedItem.Info->Hash) == true)
					{
						AutoLock lock(m_PlayerInitialData.SyncRoot);
						std::vector<PetOrigin>::iterator iter = std::find_if(m_OriginPets.begin(), m_OriginPets.end(), XRated::Pet::Finder(droppedItem.Serial));
						if (m_OriginPets.end() != iter) {
							{
								AutoLock lock(m_PetDatas.GetSyncObject());
								const XRated::Pet* pet = m_PetDatas.GetPetData(droppedItem.Serial);
								if (pet) {
									*iter = *pet;
								}

							}
							iter->isDeleted = true;

							/*Logger().Write(IAdmLogger::ImportantLogger, "pet-delete", GetName(), Service::Http::Logger::Parameter()
								<< (*iter).PetSerial << (*iter).Level << static_cast<int>((*iter).Exp));*/
						}
						m_Room->DropPetItem(m_Player, droppedItem.Serial);
					}

					m_ItemsChanged = true;
				}

			}

			void User::SetCurrentStage(const StageLicense& targetStage)
			{
				/*
					What is the world is this?!
					Teles
				*/
				AutoLock lock(m_UserMtx);
				m_CurrentStage = targetStage;
				if (IsAbleToJoinStage(m_CurrentStage))
					m_IsEnableToJoinNextLicense = true;
				else
					m_IsEnableToJoinNextLicense = false;
			}

			void User::SetState(const STATE& state)
			{
				m_State = state;
			}

			void User::SetGuildInfo(const AllMGuildInfo& info)
			{
				AutoLock lock(m_UserMtx);
				m_AllMGuildInfo = info;
				if ((m_Room == NULL) || (m_Player == NULL))
					return;
				if (!m_DoesHaveGuildInfo) {
					m_DoesHaveGuildInfo = true;
					m_ExpFactorManager.SetGuild(true);
					m_Room->AddGuildUser(shared_from_this());
				}
				ReflashGuildProfit();
			}

			void User::SetGuildExpFactor(const float& factor)
			{
				if (m_ExpFactorManager.GetGuildExpFactor() != factor) {
					m_ExpFactorManager.SetGuildExpFactor(factor);
					if (!m_Room || !m_Player) {
						return;
					}
					m_Room->ChangedExpFactor(shared_from_this());
				}
			}

			void User::SetMoney(const uint32& money)
			{
				if (!m_Player) return;
				m_Player->GetPlayerData().BaseCharacter.Money = money;
				m_CharacterInfoChanged = m_CharacterInfoChanged = true; // most of all it occurs
			}

			void User::GiveCharacterLicense(const Constants::ClassType& classType)
			{
				Net::Api request("UpdateCharacterLicense");
				{
					AutoLock lock(m_UserMtx);
					m_CharacterLicenses += (1 << (int)classType);
				}
				request << GetSerial() << m_CharacterLicenses;
				request.GetAsync(this, &User::LicenseAquired, shared_from_this());
			}

			bool User::AddStageLicense(const StageLicense& stageLicense, bool initial)
			{
				for (auto& itr : m_StageLicenses)
				{
					if (stageLicense.StageGroupHash == itr.StageGroupHash)
					{
						if (stageLicense.Level > itr.Level)
						{
							itr.Level = stageLicense.Level;

							SendRewardMailByGainStageLicense(stageLicense);
						}
						else {
							return false;
						}

						return true;


					}
				}

				m_StageLicenses.push_back(stageLicense);
				SendRewardMailByGainStageLicense(stageLicense);

				return true;
			}

			void User::SetExpFactorFromItem(XRated::Constants::ExpFactorCategoryFromItem category, const float& factor)
			{
				if (!m_Player || !m_Room) {
					LoggerInstance().Error("User::SetExpFactorFromItem() - m_Player==null || m_Room ==null");
					return;
				}
				m_Room->ChangedExpFactorFromItem(shared_from_this(), category, factor);
			}

			void User::SetFamilyExpFactor(const float& factor)
			{
				m_ExpFactorManager.SetFamilyExpFactor(factor);
				if (!m_Player || !m_Room)
					return;
				m_Room->ChangedExpFactor(shared_from_this());
			}

			void User::AddSkillPointPlus(const uint16& point)
			{
				m_Room->AddSkillPointPlus(m_Player, point);
			}

			void User::AddSkillLicense(const uint32& hash)
			{
				m_SkillLicenses.push_back(hash);
				Net::Api request("AddSkillLicense");
				request << GetSerial() << hash;
				request.GetAsync();
			}

			void User::SetRequestedInitGuildInfo(bool request)
			{
				m_RequestedInitGuildInfoToDB = request;
			}

			bool User::GetRequestedInitGuildInfo() const {
				return m_RequestedInitGuildInfoToDB;
			}

			void User::ChangedGuildLevel(const uint8& guildLevel, const uint32& guildExp)
			{
				AutoLock lock(m_UserMtx);
				if (IsPartOfGuild()) {
					if ((GetGuildInfo().GuildLevel != guildLevel)) {
						GetGuildInfo().GuildLevel = guildLevel;
						GetGuildInfo().GuildExp = guildExp;
						ReflashGuildProfit();
					}
					else {
						GetGuildInfo().GuildLevel = guildLevel;
						GetGuildInfo().GuildExp = guildExp;
					}
				}
			}

			void User::ChangedGuildProfitDate(const DateTime& openDate, const DateTime& closeDate)
			{
				AutoLock lock(m_UserMtx);
				if (IsPartOfGuild()) {
					if ((GetGuildInfo().ShopOpenDate != openDate) || (GetGuildInfo().ShopCloseDate != closeDate)) {
						GetGuildInfo().ShopOpenDate = openDate;
						GetGuildInfo().ShopCloseDate = closeDate;
						ReflashGuildProfit();
					}
				}
			}

			void User::RaiseEvent(const std::vector<std::wstring>& eventInfo, const uint32& lowRank, const uint32& highRank, const DateTime& endTime)
			{
				uint32 stageGroupHash = StringUtil::ToInt(eventInfo[0].c_str());
				uint16 level = StringUtil::ToInt(eventInfo[1].c_str());

				if (IsAbleToJoinStage(StageLicense(stageGroupHash, level)))
				{
					Protocol::FromServer::RaiseEvent raiseEvent;
					raiseEvent.eventInfo = eventInfo;
					raiseEvent.endTime = endTime;

					if ((lowRank == 0) || (highRank == 0))
					{
						Send(raiseEvent);
						return;
					}

					if ((highRank <= GetGuildInfo().Rank) && (lowRank >= GetGuildInfo().Rank))
					{
						Send(raiseEvent);
					}
				}
			}

			StageLicense& User::GetCurrentStage(){
				return m_CurrentStage;
			}

			CharacterStateFlags User::GetCharacterStateFlags() const
			{
				return m_CharacterStateFlags;
			}

			GuildState& User::GetGuildState()
			{
				return m_GuildState;
			}

			Logic::ILogic::PlayerInitialData* User::GetInitialPlayerData()
			{
				return &m_PlayerInitialData;
			}

			bool User::ItemsEquip(const Database::Info::ItemInfo* info, const Database::Enchant::EnchantBitfields& instance, const uint32& equipPos)
			{
				return false;
			}

			bool User::ItemsEquipToEquip(const uint32& from, const uint32& to)
			{
				return false;
			}

			bool User::EquipmentSwap(const std::vector<EquippedItem>& newEquipments)
			{
				return false;
			}

			bool User::CashEquipmentSwap(const std::vector<EquippedItem>& newEquipments)
			{
				return false;
			}

			Database::Info::SkillInfoList::SkillInfo* User::GetSkillInfo(const uint32& skillGroupName)
			{
				return nullptr;
			}

			const Constants::ClassType& User::GetClassType() const
			{
				if (m_Player == nullptr) {
					LoggerInstance().Error("User::GetClassType() - m_Player==null");
					return std::move(Constants::ClassType::NoClass);
				}
				return m_Player->GetClassType();
			}

			void User::CriticalError(const char* logMessage, const bool& block, const uint32& blockDuration)
			{
				LoggerInstance().Error("Critical error detected - {0} : {1}", GetSerial(), logMessage);
				if (block && GetSerial() != 0)
				{
					Net::Api request("Block");
					request << GetSerial() << blockDuration << logMessage;
					request.GetAsync();
				}
				if (IsAuthed()) // you should not close directly before authorize. it causes serious unexpectable problem.
					Close();
			}

			void User::SendSystemChat(const std::wstring& message)
			{
			}

			const Database::Info::StageInfo* User::GetStageInfo() const
			{
				return nullptr;
			}

			void User::SetEquipmentIndex(const uint8& newIndex)
			{
			}

			void User::SetCashEquipmentIndex(const uint8& newIndex)
			{
			}

			void User::SendItemMail(const uint32& itemHash, uint16 stackCount, const InstanceEx& instance)
			{
				m_ItemsChanged = true;

				std::vector<XRated::ItemBasicInfo> sendItems;
				XRated::ItemBasicInfo itemBasicInfo;
				itemBasicInfo.ItemHash = itemHash;
				itemBasicInfo.StackCount = stackCount;
				itemBasicInfo.ItemSerial = ItemSerialInstance().MakeItemSerial();
				itemBasicInfo.InstanceEx = instance;

				sendItems.push_back(itemBasicInfo);

				SendMailSelf(L"$UI.Mail.Title.IFT$", L"$UI.Mail.Title.IFX$", 0, sendItems);

				Protocol::FromServer::DisplayText displaytext;
				displaytext.type = XRated::Constants::DisplayTextType::SystemNotice;
				displaytext.message = L"$UI.Mail.ChatMessage.SendItemMail_InventoryFull$";
				Send(displaytext);

				Protocol::FromServer::AcquireItem acquireitem;
				acquireitem.Bag = XRated::ItemPosition::Invalid.Bag;
				acquireitem.Pos = XRated::ItemPosition::Invalid.Position;
				Send(acquireitem);

				/* log the result from unidentified box */
				//Logger().Write(IAdmLogger::ImportantLogger, "Identifyitem", GetName(), Service::Http::Logger::Parameter()
				//	<< itemHash << itemBasicInfo.ItemSerial << stackCount << instance << 1/*means a result*/);
			}

			std::mutex& User::GetSyncObject()
			{
				return m_UserMtx;
			}

			void User::UpdateFishing(const float& dt)
			{
				AutoLock lock(m_FishingManager.GetSync());

				if (m_FishingManager.Update(dt)) {
					Protocol::FromServer::Fishing::FishingResult packet;
					packet.PlayerSerial = GetSerial();

					const Database::Info::FishingRodInfo::RodInfo* rodDetailInfo = GetFishingRod();

					if (rodDetailInfo == NULL)
					{
						packet.Result = Protocol::FromServer::Fishing::FishingResult::Results::FishingRodNeeded;
						Send(packet);
						return;
					}

					uint16 baitCnt, removedCnt;
					baitCnt = ItemExamineSlot(m_FishingManager.GetBaitPos(), m_FishingManager.GetBaitHash(), m_FishingManager.GetBaitCount());
					baitCnt = baitCnt > m_FishingManager.GetBaitCount() ? m_FishingManager.GetBaitCount() : baitCnt;
					if (baitCnt == 0) {
						packet.Result = Protocol::FromServer::Fishing::FishingResult::Results::BaitNeeded;
						Send(packet);
						return;
					}

					{
						AutoLock lock(m_UserMtx);
						if (GetBlankSlotCount() < 1) {
							packet.Result = Protocol::FromServer::Fishing::FishingResult::Results::NotEnoughInventory;
							Send(packet);
							return;
						}
					}

					if (rodDetailInfo->SuccessProb >= m_FishingManager.GetRandomFloat()) { //Succeed
						//uint8 baitCount = fishing.GetBaitCount();
						double selectItemProb = m_FishingManager.GetRandomFloat();
						std::vector<Database::Info::FishingRodInfo::ProbabilityTable>::const_iterator i = rodDetailInfo->ProbabilityTables.begin();
						std::vector<Database::Info::FishingRodInfo::ProbabilityTable>::const_iterator iEnd = rodDetailInfo->ProbabilityTables.end();
						double tot = 0;
						for (; i != iEnd; ++i) {
							tot += (*i).Probability;
							if (tot >= selectItemProb) {
								{
									if ((*i).Rare >= 4) {
										LoggerInstance().Warn("Random value= {0}/{1}/{2}", tot, selectItemProb, (*i).Probability);
									}
									uint32 fishingItemHash = (*i).ItemHash;
									uint16 a;
									{

										AutoLock lock(m_UserMtx);
										for (a = 0; a < baitCnt; ++a) {
											if (!ItemAdd((*i).ItemHash, 0, 1, 0, true, XRated::Constants::AddItemType::Fish)) {
												break;
											}
										}
									}
									
									if (a == 0) {
										packet.Result = Protocol::FromServer::Fishing::FishingResult::Results::NotEnoughInventory;
										Send(packet);
										return;
									}
									else {
										m_ItemsChanged = true;
										{
											AutoLock lock(m_UserMtx);
											removedCnt = ItemRemove(m_FishingManager.GetBaitPos(), m_FishingManager.GetBaitHash(), a);
											//removedCnt = a; //For Dummy test
										}
										if (removedCnt != a) {
											LoggerInstance().Exception(L"Unable to remove required numbers of bait.({0}/{1}/{2})", GetSerial(), a, removedCnt);
											packet.Result = Protocol::FromServer::Fishing::FishingResult::Results::UnknownError;
											Send(packet);
											return;
										}

										AddExp(XRated::Constants::ExpAcquiredType::ExpPlayer, rodDetailInfo->Exp * removedCnt, false);
										m_FamilyManager.AcquiredFishItem(fishingItemHash);

									}
								}
								m_Room->FishingInfo(shared_from_this(), (*i).ItemHash, baitCnt, (*i).Rare);

								packet.Result = Protocol::FromServer::Fishing::FishingResult::Results::Ok;

								m_Room->SendToAll(packet);
								/*if (achievement.IsConnected()) {
									Net::Protocol::Achievement::ServerProtocol::FishingCountChanged pkt;
									pkt.characterName = GetName();
									pkt.fishingCount = uint32(1);
								}*/
								return;
							}
						}
						if (i == iEnd) {
							LoggerInstance().Info(L"Critical Error when fishing. Random value= {}/{}", tot, selectItemProb);
						}

					}
					m_ItemsChanged = true;
					{
						AutoLock lock(m_UserMtx);
						removedCnt = ItemRemove(m_FishingManager.GetBaitPos(), m_FishingManager.GetBaitHash(), baitCnt);
					}
					if (removedCnt != baitCnt) {
						LoggerInstance().Info(L"Unable to remove required numbers of bait.({}/{}/{})", m_CharacterName, baitCnt, removedCnt);
						packet.Result = Protocol::FromServer::Fishing::FishingResult::Results::UnknownError;
						Send(packet);
						return;
					}

					packet.Result = Protocol::FromServer::Fishing::FishingResult::Results::Failed;
					m_Room->SendToAll(packet);
					return;
				}

			}

			uint32 User::GetFishingRodId() const
			{
				return uint32();
			}

			const Database::Info::FishingRodInfo::RodInfo* User::GetFishingRod() const
			{
				return nullptr;
			}

			bool User::IsProperFishingRod() const
			{
				return false;
			}

			bool User::IsFishingArea() const
			{
				return false;
			}

			void User::AddExp(const XRated::Constants::ExpAcquiredType& type, const uint32& exp, const bool& withFactor)
			{
				if (m_Player && m_Room)
					m_Room->AddExp(type, shared_from_this(), exp, withFactor);
			}

			void User::AddPvpExp(const XRated::Constants::ExpAcquiredType& type, const uint32& exp)
			{
				if (m_Player && m_Room)
					m_Room->AddPvpExp(type, shared_from_this(), exp);
			}

			void User::AddWarExp(const XRated::Constants::ExpAcquiredType& type, const uint32& exp)
			{
				if (m_Player && m_Room)
					m_Room->AddWarExp(type, shared_from_this(), exp);
			}

			void User::AddStateBundle(const uint32& hash)
			{
				if (m_Player && m_Room)
					m_Room->AddStateBundle(shared_from_this(), hash);
			}

			void User::AddPassive(const uint32& hash)
			{
				if (m_Player && m_Room)
					m_Room->AddPassive(shared_from_this(), hash);
			}

			void User::RemovePassive(const uint32& hash)
			{
				if (m_Player && m_Room)
					m_Room->RemovePassive(shared_from_this(), hash);
			}

			float User::GetObjectDistance(const uint32& targetHash)
			{
				if (m_Player && m_Room)
					return m_Room->GetObjectDistance(shared_from_this(), targetHash);
				return -1.0f;
			}

			const std::wstring& User::GetName() const
			{
				return m_CharacterName;
			}

			void User::ItemsSend(Protocol::IPacketSerializable& packet)
			{
			}

			void User::ItemsSendToAll(Protocol::IPacketSerializable& packet)
			{
			}

			void User::ItemsDirectSend(Protocol::IPacketSerializable& packet)
			{
				Send(packet);
			}

			bool User::ItemsUse(const uint32& itemHash, const uint32& bag, const uint32& pos)
			{
				return false;
			}

			bool User::PetItemUse(const uint32& itemHash, const uint32& bag, const uint32& pos, GlobalSerial itemSerial)
			{
				return false;
			}

			void User::PetInventoryDirectSend(Protocol::IPacketSerializable& packet)
			{
			}

			bool User::PetItemsEquip(XRated::GlobalSerial petSerial, const Database::Info::ItemInfo* info, const Database::Enchant::EnchantBitfields& instance, const uint32& equipPos)
			{
				return false;
			}

			bool User::PetItemPositionUpdate(XRated::GlobalSerial petSerial, const uint8& bag, const uint8& position)
			{
				return false;
			}

			void User::RemoveDuplicatedItem(const uint32& hash, const uint64 serial, const uint8& bag, const uint8& pos, const uint32& stackcount, const InstanceEx& instance, int isBankItem, bool insertAgainAfterDelete)
			{
			}

			bool User::PetVisitor(IPetItemVisitor& visitor)
			{
				return visitor.Run(this, m_Room.get());
			}

			PetDatas& User::GetPetDatas()
			{
				return m_PetDatas;
			}

			User::STATE User::GetState() const
			{
				return m_State;
			}

			AllMGuildInfo& User::GetGuildInfo()
			{
				return m_AllMGuildInfo;
			}

			uint8 User::GetTeamNumber() const
			{
				return m_TeamNumber;
			}

			uint8 User::GetTotalRank() const
			{
				if (!m_Player) {
					LoggerInstance().Error("User::GetTotalRank() - m_Player==null");
					return 0;
				}
				return m_Player->GetTotalRank();
			}

			uint8 User::GetTimeRank() const
			{
				if (!m_Player) {
					LoggerInstance().Error("User::GetTimeRank() - m_Player==null");
					return 0;
				}
				return m_Player->GetTimeRank();
			}

			uint8 User::GetLifeRank() const
			{
				if (!m_Player) {
					LoggerInstance().Error("User::GetLifeRank() - m_Player==null");
					return 0;
				}
				return m_Player->GetLifeRank();
			}

			uint8 User::GetSecretRank() const
			{
				if (!m_Player) {
					LoggerInstance().Error("User::GetSecretRank() - m_Player==null");
					return 0;
				}
				return m_Player->GetSecretRank();
			}

			uint8 User::GetAirComboRank() const
			{
				if (!m_Player) {
					LoggerInstance().Error("User::GetAirComboRank() - m_Player==null");
					return 0;
				}
				return m_Player->GetAirComboRank();
			}

			uint8 User::GetStylePointRank() const
			{
				if (!m_Player) {
					LoggerInstance().Error("User::GetStylePointRank() - m_Player==null");
					return 0;
				}
				return m_Player->GetStylePointRank();
			}

			uint16 User::GetLevel() const
			{
				if (!m_Player) {
					LoggerInstance().Error("User::GetLevel() - m_Player==null");
					return 0;
				}
				return m_Player->GetLevel();
			}

			uint16 User::GetPvpLevel() const
			{
				if (m_Player == nullptr) {
					LoggerInstance().Error("User::GetPvpLevel() - m_Player==null");
					return 0;
				}
				return m_Player->GetPvpLevel();
			}

			uint16 User::GetRoomCapacity() const
			{
				return m_RoomCapacity;
			}

			uint32 User::GetRoomIndex() const
			{
				return m_RoomIndex;
			}

			const Common::ItemEx* User::GetItem(const ItemPosition& position) const
			{
				return m_Items.GetItem(position.Bag, position.Position);
			}

			std::pair<uint32, int64> User::GetEquipment(const uint16& position) const
			{
				const Common::ItemEx* item = m_Items.GetItem(0/*equipment*/, position);
				if (!item || !item->Info)
					return std::pair<uint32, int64>(0, 0);
				return std::pair<uint32, int64>(item->Info->Hash, item->InstanceEx);
			}

			bool User::ItemExamineSlot(const uint32& id, const int& cnt) const
			{
				return false;
			}

			uint16 User::ItemExamineSlot(const XRated::ItemPosition& pos, const uint32& id, const int& cnt) const
			{
				return uint16();
			}

			bool User::ItemAdd(const uint32& id, Serial gameObjectSerial, const int& cnt, const InstanceEx& instance, bool autoStack, XRated::Constants::AddItemType addItemType)
			{
				return false;
			}

			uint32 User::ItemFill(const uint32& hash, const uint32& amount)
			{
				return uint32();
			}

			ItemPosition User::ItemAdd(const Common::ItemEx& item)
			{
				return ItemPosition();
			}

			ItemPosition User::ItemAdd(const XRated::Item& item, const uint16& count)
			{
				return ItemPosition();
			}

			bool User::ItemAcquire(const uint32& itemId, Serial serial, const uint16& stackCount, const InstanceEx& instance, const uint64& itemSerial, bool addSeparate, XRated::Item* identifiedAcquireItem)
			{
				return false;
			}

			bool User::ItemAcquireWithMailSupport(const uint32& itemId, const uint16& stackCount, const InstanceEx& instance, const uint64& itemSerial, bool addSeparate)
			{
				return false;
			}

			uint32 User::GetItemCount(const Database::Info::ItemInfo::IFindCondition& condition, bool findPetInven) const
			{
				return m_Items.GetItemCount(condition) + (findPetInven == false ? 0 : m_Items.GetItemCountInPetInventory(condition, true));
			}

			uint32 User::GetItemCount(const uint32& hash) const
			{
				return m_Items.GetItemCount(hash);
			}

			uint32 User::GetEmptyItemSlotCount() const
			{
				return m_Items.GetEmptySlotCount();
			}

			bool User::ItemRemove(const uint32& id, int cnt)
			{
				return false;
			}

			uint16 User::ItemRemove(const ItemPosition& position, const uint32& id, const uint16& cnt, bool sendPacket, bool writeLog)
			{
				return uint16();
			}

			void User::DoRestoreBelonging(const XRated::ItemPosition& usingPos, const XRated::ItemPosition& targetPos)
			{
				const Common::ItemEx* usingItem = m_Items.GetItem(usingPos.Bag, usingPos.Position, false);
				const Common::ItemEx* targetItem = m_Items.GetItem(targetPos.Bag, targetPos.Position, false);


				if (!usingItem || !targetItem)
				{
					CriticalError("DoRestoreBelonging : null-item");
					return;
				}

				if (m_Items.IsEnable(false, usingPos.Bag) == false || m_Items.IsEnable(false, targetPos.Bag) == false)
				{
					CriticalError("DoRestoreBelonging : the target bag in inventory is not enable");
					return;
				}

				if (Database::Enchant::IsRestorableBelonging(targetItem->Info, targetItem->InstanceEx) == false)
				{
					CriticalError("DoRestoreBelonging : not proper target item");
					return;
				}

				Database::Enchant::Condition condition;
				condition = Database::Enchant::GetRestoreBelongingRequirement(targetItem->Info, targetItem->InstanceEx);


				Common::ItemEx newItem; // new item
				newItem.Serial = ItemSerialInstance().MakeItemSerial();
				newItem.StackedCount = 1;
				newItem.Info = targetItem->Info;
				newItem.InstanceEx = targetItem->InstanceEx;
				Database::Enchant::RestoreBelonging(newItem.Info, newItem.InstanceEx);

				m_Items.SetItem(targetPos.Bag, targetPos.Position, newItem);
				m_Items.DropItems(usingPos.Bag, usingPos.Position, 1, true);

				m_ItemsChanged = true; // mark as item changed to report to db

				Protocol::FromServer::RestoreBelonging response;
				response.Position = XRated::ItemPosition(targetPos.Bag, targetPos.Position);
				response.ItemHash = newItem.Info->Hash;
				response.instanceEx = newItem.InstanceEx;
				Send(response);

				Database::Enchant::EnchantBitfields tmp(newItem.InstanceEx);
				/*Logger().Write(IAdmLogger::ImportantLogger, "restore-belonging", GetName(), Http::Logger::Parameter()
					<< newItem.Info->Hash << newItem.Serial << newItem.InstanceEx);*/
			}
			
			void User::DoAppraisal(const XRated::ItemPosition& usingPos, const XRated::ItemPosition& targetPos)
			{
				const Common::ItemEx* usingItem = m_Items.GetItem(usingPos.Bag, usingPos.Position, false);
				const Common::ItemEx* targetItem = m_Items.GetItem(targetPos.Bag, targetPos.Position, false);

				if (!usingItem || !targetItem)
				{
					CriticalError("DoAppraisal : null-item");
					return;
				}

				if (m_Items.IsEnable(false, usingPos.Bag) == false || m_Items.IsEnable(false, targetPos.Bag) == false)
				{
					CriticalError("DoAppraisal : the target bag in inventory is not enable");
					return;
				}

				if (targetItem->Info->ItemType != Database::Info::ItemInfo::Type::EQUIPMENT || Database::Enchant::IsEnchantable(targetItem->Info) == false)
				{
					CriticalError("DoAppraisal : not proper target item");
					return;
				}

				Database::Enchant::EnchantBitfields instance(targetItem->InstanceEx);

				if (instance.Identified)
				{
					CriticalError("DoAppraisal : target item cannot be appraisaled");
					return;
				}

				Common::ItemEx newItem;

				newItem.Serial = ItemSerialInstance().MakeItemSerial();
				newItem.StackedCount = 1;
				newItem.Info = targetItem->Info;

				if (instance.NormalIdentified == 1)
				{
					AdvancedIdentifyEnchanter enchant;
					newItem.InstanceEx = enchant(*(targetItem->Info), instance);
				}
				else
				{
					/* make identifing enchant */
					IdentifyEnchanter enchant;
					newItem.InstanceEx = enchant(*(targetItem->Info), instance);

				}

				m_Items.SetItem(targetPos.Bag, targetPos.Position, newItem);
				m_Items.DropItems(usingPos.Bag, usingPos.Position, 1, true);

				m_ItemsChanged = true; // mark as item changed to report to db

				Protocol::FromServer::Identify response;
				response.Position = XRated::ItemPosition(targetPos.Bag, targetPos.Position);
				response.ItemHash = newItem.Info->Hash;
				response.instanceEx = newItem.InstanceEx;
				Send(response);

				UpdateInfos();
				/*Logger().Write(IAdmLogger::ImportantLogger, "enchant-identify", GetName(), Http::Logger::Parameter()
					<< newItem.Info->Hash << newItem.Serial << newItem.InstanceEx);*/
			}

			void User::DoCashAppraisal(const XRated::ItemPosition& usingPos, const XRated::ItemPosition& targetPos)
			{
				const Common::ItemEx* usingItem = m_Items.GetItem(usingPos.Bag, usingPos.Position, false);
				const Common::ItemEx* targetItem = m_Items.GetItem(targetPos.Bag, targetPos.Position, false);

				if (!usingItem || !targetItem)
				{
					CriticalError("DoCashAppraisal : null-item");
					return;
				}

				if (m_Items.IsEnable(false, usingPos.Bag) == false || m_Items.IsEnable(false, targetPos.Bag) == false)
				{
					CriticalError("DoCashAppraisal : the target bag in inventory is not enable");
					return;
				}

				if (targetItem->Info->ItemType != Database::Info::ItemInfo::Type::EQUIPMENT || Database::Enchant::IsEnchantable(targetItem->Info) == false)
				{
					CriticalError("DoCashAppraisal : not proper target item");
					return;
				}

				Database::Enchant::EnchantBitfields instance(targetItem->InstanceEx);

				if (instance.Identified)
				{
					CriticalError("DoCashAppraisal : target item cannot be cash-appraisaled");
					return;
				}

				Common::ItemEx newItem;

				newItem.Serial = ItemSerialInstance().MakeItemSerial();
				newItem.StackedCount = 1;
				newItem.Info = targetItem->Info;

				{
					CashIdentifyEnchanter enchant;
					newItem.InstanceEx = enchant(*(targetItem->Info), targetItem->InstanceEx);
				}

				m_Items.SetItem(targetPos.Bag, targetPos.Position, newItem);
				m_Items.DropItems(usingPos.Bag, usingPos.Position, 1, true);

				m_ItemsChanged = true; // mark as item changed to report to db

				Protocol::FromServer::Identify response;
				response.Position = XRated::ItemPosition(targetPos.Bag, targetPos.Position);
				response.ItemHash = newItem.Info->Hash;
				response.instanceEx = newItem.InstanceEx;
				Send(response);

				UpdateInfos();
				/*Logger().Write(IAdmLogger::ImportantLogger, "enchant-identify", GetName(), Http::Logger::Parameter()
					<< newItem.Info->Hash << newItem.Serial << newItem.InstanceEx);*/
			}

			void User::DoRestoreAppraisal(const XRated::ItemPosition& usingPos, const XRated::ItemPosition& targetPos)
			{
			}

			void User::DoCashRestoreAppraisal(const XRated::ItemPosition& usingPos, const XRated::ItemPosition& targetPos)
			{
			}

			void User::DoExtract(const XRated::ItemPosition& usingPos, const XRated::ItemPosition& targetPos)
			{
			}

			void User::GetActivatedPassiveItems(std::vector<uint32>& result) const
			{
				//const std::vector<const Database::Info::ItemInfo*>& items = passiveItems.GetActivatedItems();
				//for (std::vector<const Database::Info::ItemInfo*>::const_iterator i = items.begin(); i != items.end(); ++i)
					//result.push_back((*i)->Hash);
			}

			const std::vector<XRated::StageLicense>& User::GetStageLicenses() const
			{
				return m_StageLicenses;
			}

			std::string User::GetRoomPass() const
			{
				return m_RoomPass;
			}

			Logic::Player* User::GetPlayer() const
			{
				return m_Player;
			}

			float User::GetExpFactor()
			{
				return m_ExpFactorManager.TotalExpFactor();
			}

			void User::RoomParted()
			{
				UpdateInitialPlayerData();
				ClearGuildInfo();
				PlayerDeleted();

				m_Room = NULL;

				/* release quest information */
				//quest.Release(this);

				m_State = NONE; // this should be set NONE when the player is parted from game logic completely.
			}

			bool User::UpdateInitialPlayerData()
			{
				AutoLock lock(m_PlayerInitialData.SyncRoot);//Prevent Room from modifying the initial values.
				if (!IsAuthed() || m_CharacterStateFlags.IsSpectator)
					return false;
				if (m_Player == nullptr) {
					LoggerInstance().Error("User({0})::UpdateInitialPlayerData() m_Player == null", GetSerial());
					return false;
				}
				if (m_Player->GetName() != GetCharacterName()) {
					LoggerInstance().Exception("User({0})::UpdateInitialPlayerData - PlayerName != UserName {0}");
					return false;
				}
				auto& newData = m_Player->GetPlayerData();
				m_PlayerInitialData.type = newData.Job;
				m_PlayerInitialData.level = newData.BaseCharacter.Level;
				m_PlayerInitialData.xp = newData.BaseCharacter.Xp;
				m_PlayerInitialData.pvpLevel = newData.PvpLevel;
				m_PlayerInitialData.pvpXp = newData.PvpXp;
				m_PlayerInitialData.warLevel = newData.WarLevel;
				m_PlayerInitialData.warXp = newData.WarXp;
				m_PlayerInitialData.storedLevel = newData.StoredLevel;
				m_PlayerInitialData.rebirthCount = newData.RebirthCount;
				m_PlayerInitialData.storedSkillPoint = newData.StoredSkillPoint;
				m_PlayerInitialData.money = newData.BaseCharacter.Money;
				m_PlayerInitialData.bankMoney = newData.BaseCharacter.BankMoney;
				m_PlayerInitialData.skillpoint = newData.SkillPoint;
				m_PlayerInitialData.skills = newData.Skills;
				m_PlayerInitialData.lastRebirthDateTime = newData.LastRebirthDateTime;
				m_PlayerInitialData.ladderPoint = newData.LadderPoint;
				m_PlayerInitialData.ladderMatchCount = newData.LadderMatchCount;
				m_PlayerInitialData.ladderWinCount = newData.LadderWinCount;
				m_PlayerInitialData.ladderLoseCount = newData.LadderLoseCount;
				m_PlayerInitialData.achievementScore = newData.achievementScore;
				m_PlayerInitialData.equipments.clear();
				return true;
			}

			bool User::ValidGuildInfoAndRun(const XRated::AllMGuildInfo& info)
			{
				/*if (info.GuildLevel < m_ValidGuildInfo.GuildLevel) {
					CriticalError("request client guild level is wrong", false);
					return false;
				}
				switch (m_ValidGuildInfo.type)
				{
				case ValidGuildInfo::Requested::EnterGuildShop:
					EnterShop(XRated::Constants::ShopType::GuildShop, static_cast<uint32>(info.GuildLevel));
					break;
				}*/
				return true;
			}

			void User::ShareQuestToParty(const Database::Info::QuestInfo& info, const uint32& questHash)
			{
				class ShareQuestToPartyFuncter : public IUserRoom::IExcuter
				{
					uint32 questHash;
					std::wstring owner;
					const Database::Info::QuestInfo& info;
				public:
					ShareQuestToPartyFuncter(const std::wstring& owner, const Database::Info::QuestInfo& info, uint32 shareQuestHash)
						:questHash(shareQuestHash), info(info), owner(owner) {}

					bool Excute(UserSharedPtr in) {
						AutoLock lock(in->GetSyncObject());
						if (owner == in->GetName()) {
							return false;
						}
						in->ShareQuest(owner, info, questHash);
						return true;
					}
				};
				if (m_Room->GetRoomKind() == Common::STAGE) {
					Protocol::FromServer::Quest::ShareResult packet;
					packet.QuestHash = questHash;
					packet.SuccessPlayerCount = m_Room->ExcuteRoomSerialUsers(ShareQuestToPartyFuncter(GetName(), info, questHash));
					Send(packet);
				}
			}

			void User::ShareQuest(const std::wstring& shareOwner, const Database::Info::QuestInfo& info, const uint32& questHash)
			{
				m_QuestManager.ShareQuest(shared_from_this(), shareOwner, info, questHash);
			}

			bool User::ClearGuildInfo()
			{
				LoggerInstance().Warn("User({0})::ClearGuildInfo - NotImplemented", GetSerial());
				return true;
			}

			void User::CompletedQuest(const uint32& questHash)
			{
				if (m_Room) {
					m_Room->CompletedQuest(shared_from_this(), questHash);
				}
			}

			void User::AcceptedQuest(const uint32& questHash)
			{
				if (m_Room) {
					m_Room->AcceptedQuest(shared_from_this(), questHash);
				}
			}

			bool User::PlayerDeleted()
			{
				LoggerInstance().Warn("User({0})::PlayerDeleted - NotImplemented", GetSerial());
				return true;
			}

			bool User::HasCharacterLicense(const  Constants::ClassType& classType) const
			{
				return m_CharacterLicenses & (1 << (int)classType);
			}

			bool User::HasSkillLicense(const uint32& hash) //IsAcquiredSkillLicense
			{
				uint32 classHash = XRated::Constants::GetClassHash(GetClassType());
				Database::Info::SkillInfoList::SkillGroup* skill = Database::DatabaseInstance().InfoCollections.Skills.GetSkillGroupInfo(classHash, hash);
				if (!skill) return false;
				if (!skill->NeedLicense) return true;
				//AutoLock lock(m_UserMtx);
				auto result = std::find(m_SkillLicenses.begin(), m_SkillLicenses.end(), hash);
				if (result != m_SkillLicenses.end())
					return true;
				return false;
			}

			bool User::RemoveStageItem()
			{
				if (m_Items.RemoveStageItems())
				{
					m_ItemsChanged = true;
					return true;
				}
				return false;
			}

			int User::GetRequiredSlotCount(const std::vector<std::pair<uint32, uint32>>& toRemove, const std::vector<std::pair<uint32, uint32>>& toAdd, const uint32& availablecount) const
			{
				return m_Items.GetRequiredSlotCount(toRemove, toAdd, availablecount);
			}

			bool User::RemoveItems(const uint32& itemHash, const uint32& count)
			{
				return m_Items.Remove(itemHash, count);
			}

			void User::Terminate(){
				Logger::GetInstance().Warn("Terminate has to be implemented.");
				Net::Api api("Terminate");
				api << GetCharacterName();
				api.GetAsync();
				CloseSocket();
			}

			void User::Close()
			{
				if (!IsConnected())
					return;
				Terminate();
			}

		}
	}
}