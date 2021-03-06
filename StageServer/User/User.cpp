#pragma once
#include <functional>
#include <mmsystem.h>
#pragma comment( lib, "Winmm.lib" )
#include <algorithm>
#include <StageServer/User/UserManager.h>
#include <Network/NetStream.h>
#include <Network/CommonProtocol/Protocol.h>
#include <StageServer/Protocol/FromServer.h>
#include <Info/Info.h>
#include <StageServer/User/IUserRoom.h>
#include <Core/ErrorDefinition.h>
#include <StageServer/Services/ItemSerial.h>
#include <StageServer/Services/CoinItemPurchaseManager.h>
#include <StageServer/User/Items/Enchanter.h>
#include <StageServer/Services/Communicators.h>
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

			void User::Send(const Protocol::IPacketSerializable& packet) const{
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
				SendAsync((uint8*)buffer.GetData(), buffer.GetLength());
			}

			void User::Send(const Serializer::ISerializable& packet) const
			{
				Send(*(Protocol::IPacketSerializable*)(&packet));
			}
			
			uint32 User::Parse(uint8* buffer, size_t size) {
				/*
					We should technically see but i think that if the server doesn't recognize the packet I say we let the client connection dies.
					That's why I want to keep this call in here
					Parse only known packets. At least the ones which follow NetStream Standarts.
				*/
				if (size < Lunia::Constants::HeaderSize)
					return 0;  // first 4 bytes are just header of NetStream
				Net::StreamReader sReader(buffer);
				int read = sReader.GetNetStreamSize();
				if (read == 0)
					return 0;
				if (read < 0) {
					LoggerInstance().Error("Invalid packet");
					return 0;
				}
				if (sReader.IsNetStream() && read <= size) {
					std::thread t1(
						[&m_Parser= m_Parser,user= shared_from_this()] (Net::StreamReader reader){
							m_Parser.Invoke(user, reader);
						},sReader); 
					// This might create problems in the future. If the operation done before was not completed and something else arrives they gonna work kinda of relying on mutexes. We shall see.
					t1.detach();
				}

				return read;
			}

			void User::SendToAll(Protocol::IPacketSerializable& packet) const
			{
				if (m_Room && IsConnected())
					m_Room->SendToAll(packet);
			}

			User::User(asio::ip::tcp::socket&& socket, const uint32& userId)
				: ClientTcp(std::move(socket))
				, m_UserId(userId)
				, m_GuildState(*this)
				, m_FamilyManager(*this)
				, m_Items(*this)
				, m_FishingManager(*this)
				, playTimeEventCheckIntervalInSec(ConfigInstance().Get("PlayTimeCheckIntervalInSec", 60.0f))
			{
				Logger::GetInstance().Info("User :: Hey, I was created!", GetId());
			}

			void User::Init() {
				BindPackets();
				AutoLock lock(m_UserMtx);
				m_LastDataSavedTime = m_ConnectTime = m_aliveTime = timeGetTime();
				Protocol::FromServer::Way way;
				way.EncryptKey = Math::Random();
				m_FishingManager.Init();
				SetCryptoKey(way.EncryptKey);
				Send(way);
			}

			void User::BindPackets()
			{
				AutoLock lock(m_UserMtx);
				Communicators& communicator = UserManagerInstance().GetCommunicators();
				//Guild
				m_Parser.Bind<Protocol::ToServer::AllMGuild::RefreshMyInfo>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::Invite>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::InviteReply>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::Join>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::Kick>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::Leave>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::ListMembers>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::Remove>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::ValidateName>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::ValidateAlias>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::AddExp>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::AddMaintenancePoint>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::ReflashGuildLevel>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::GetTax>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::ReflashGuildShopItem>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::MessageChange>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::ChangeGrade>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::ChangeGradeName>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::ChangeGradeAuth>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::ChangeGuildMaster>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::GetMyInfo>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::RefreshMyInfo>(communicator.Guild, &GuildManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AllMGuild::ReflashGuildProfitDate>(communicator.Guild, &GuildManager::Dispatch);
				//Family
				m_Parser.Bind<Protocol::ToServer::Family::Invite>(m_FamilyManager, &FamilyManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Family::InviteAnwer>(m_FamilyManager, &FamilyManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Family::Leave>(m_FamilyManager, &FamilyManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Family::RefreshInfo>(m_FamilyManager, &FamilyManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Family::RequestPresent>(m_FamilyManager, &FamilyManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Family::Kick>(m_FamilyManager, &FamilyManager::Dispatch);
				//Trades
				m_Parser.Bind<Protocol::ToServer::RequestTrade>(communicator.Trader, &TradeManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AddTradeItem>(communicator.Trader, &TradeManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::AddTradeMoney>(communicator.Trader, &TradeManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::ReadyToTrade>(communicator.Trader, &TradeManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::CancelTrade>(communicator.Trader, &TradeManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::ConfirmTrade>(communicator.Trader, &TradeManager::Dispatch);
				//PlayerStore
				m_Parser.Bind<Protocol::ToServer::PlayerStore::BuyPet>(communicator.PlayerStore, &PlayerStoreManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::PlayerStore::Buy>(communicator.PlayerStore, &PlayerStoreManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::PlayerStore::List>(communicator.PlayerStore, &PlayerStoreManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::PlayerStore::Close>(communicator.PlayerStore, &PlayerStoreManager::Dispatch);
				//Quests
				m_Parser.Bind<Protocol::ToServer::Quest::Accept>(m_QuestManager, &QuestManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Quest::Complete>(m_QuestManager, &QuestManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Quest::Drop>(m_QuestManager, &QuestManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Quest::ActivityItem>(m_QuestManager, &QuestManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Quest::ShareToPartyPlayer>(m_QuestManager, &QuestManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Quest::SharedAnswer>(m_QuestManager, &QuestManager::Dispatch);
				//Mailbox System
				m_Parser.Bind<Protocol::ToServer::Mail::DisposeMail>(m_MailBoxManager, &MailboxManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Mail::DisposeReadMails>(m_MailBoxManager, &MailboxManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Mail::ReadMail>(m_MailBoxManager, &MailboxManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Mail::RequestMailList>(m_MailBoxManager, &MailboxManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Mail::RetrieveAttached>(m_MailBoxManager, &MailboxManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Mail::SendMail>(m_MailBoxManager, &MailboxManager::Dispatch);
				/*Fishing*/
				m_Parser.Bind<Protocol::ToServer::Fishing::Start>(m_FishingManager, &FishingManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Fishing::End>(m_FishingManager, &FishingManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Fishing::List>(m_FishingManager, &FishingManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Fishing::Do>(m_FishingManager, &FishingManager::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Fishing::ChangeBaitCount>(m_FishingManager, &FishingManager::Dispatch);
				//User
				m_Parser.Bind<Protocol::ToServer::Stage>(*this, &User::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Alive>(*this, &User::Dispatch);
				m_Parser.Bind<Protocol::ToServer::LoadEnd>(*this, &User::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Join>(*this, &User::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Command>(*this, &User::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Chat>(*this, &User::Dispatch);
				m_Parser.Bind<Protocol::ToServer::ListItem>(*this, &User::Dispatch);
				m_Parser.Bind<Protocol::ToServer::ListQuickSlot>(*this, &User::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Use>(*this, &User::Dispatch);
				m_Parser.Bind<Protocol::ToServer::MoveItem>(*this, &User::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Voice>(*this, &User::Dispatch);
				m_Parser.Bind<Protocol::ToServer::EnterShop>(*this, &User::Dispatch);
				m_Parser.Bind<Protocol::ToServer::LeaveShop>(*this, &User::Dispatch);
				m_Parser.Bind<Protocol::ToServer::Family::RefreshInfo>(m_FamilyManager, &FamilyManager::Dispatch);
				//ItemEx
				auto user = shared_from_this(); 
				//General
				m_ItemEx.Register(std::make_shared<BankBag>(user, m_Items));
				m_ItemEx.Register(std::make_shared<InventoryBag>(user, m_Items));
				m_ItemEx.Register(std::make_shared<SkillReset>(m_Player, m_Room));
				m_ItemEx.Register(std::make_shared<SkillResetAll>(m_Player, m_Room));
				m_ItemEx.Register(std::make_shared<Nothing>());
				m_ItemEx.Register(std::make_shared<EnchantRecover>(user, m_Items));
				m_ItemEx.Register(std::make_shared<PlayerStore>(user, m_Items, communicator.PlayerStore));
				m_ItemEx.Register(std::make_shared<CharacterSlot>(user));
				m_ItemEx.Register(std::make_shared<Megaphone>(user));
				m_ItemEx.Register(std::make_shared<TextBoardMessage>(user));
				m_ItemEx.Register(std::make_shared<Store>(user));
				m_ItemEx.Register(std::make_shared<QuestActivity>(user, m_QuestManager));
				m_ItemEx.Register(std::make_shared<ReviveStone>(user));
				m_ItemEx.Register(std::make_shared<DailyItem>(user, m_Items));
				m_ItemEx.Register(std::make_shared<CharacterLicense>(user));
				m_ItemEx.Register(std::make_shared<PetItem>(user, m_Items));
				m_ItemEx.Register(std::make_shared<ExtendExpireItemDate>(user, m_Items));
				m_ItemEx.Register(std::make_shared<ForceItemExpiration>(user, m_Items));
				m_ItemEx.Register(std::make_shared<PetFood>(user, m_Items));
				m_ItemEx.Register(std::make_shared<PetLevelDown>(user, m_Items));
				m_ItemEx.Register(std::make_shared<PetSchoolTicket>(user, m_Items));
				m_ItemEx.Register(std::make_shared<SkillPointPlus>(user));
				m_ItemEx.Register(std::make_shared<AddLicense>(user));
				m_ItemEx.Register(std::make_shared<Rebirth>(user));
				m_ItemEx.Register(std::make_shared<EventStage>(user));
				m_ItemEx.Register(std::make_shared<EventDungeonStage>(user));
				m_ItemEx.Register(std::make_shared<PetRenaming>(user, m_Items));
				m_ItemEx.Register(std::make_shared<CharacterRenaming>(user, m_Items));
				m_ItemEx.Register(std::make_shared<RestoreBelonging>(user, m_Items));
				m_ItemEx.Register(std::make_shared<Appraisal>(user, m_Items));
				m_ItemEx.Register(std::make_shared<RestoreAppraisal>(user, m_Items));
				m_ItemEx.Register(std::make_shared<Extract>(user, m_Items));
				m_ItemEx.Register(std::make_shared<StageServer::AddSkillLicense>(user));
				//Guild ItemEx
				m_ItemEx.Register(std::make_shared<GuildPointUp >(user, communicator.Guild));
				m_ItemEx.Register(std::make_shared<GuildExpUp >(user, communicator.Guild));
				m_ItemEx.Register(std::make_shared<GuildRankingPointUp >(user, communicator.Guild));
				m_ItemEx.Register(std::make_shared<AddItemToGuildShop >(user, communicator.Guild));
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

			void User::ToggleFishingRodVisibility(const bool& state)
			{
				m_IsFishingRodVisible = state;
			}

			bool User::GetFishingRodVisibility() const
			{
				return m_IsFishingRodVisible;
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
				int stageItemCount = m_Items.SetItemList(result["bags"], result["items"]);
				if (stageItemCount > 0)
					m_ItemsChanged = true;
#ifdef BAN_DUPLICATED_ITEM_SERIAL
				else if (stageItemCount < 0)
				{
					CriticalError("duplicated item detected on initializing the character", true, 1); // arcrus. only 1sec blocking
					return false;
				}
#endif
				m_PlayerInitialData.equipments.clear();
				std::vector<Common::ItemEx> equipments;
				m_Items.GetEquip(equipments);
				for (auto& i : equipments)
					m_PlayerInitialData.equipments.emplace_back(i.Info, i.InstanceEx);
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

			void User::AuthedConnection(const UserSharedPtr& user, const Net::Answer& answer)
			{
				if (answer.errorCode == 0 && user->GetSerial() == GetSerial()) {
					if (!answer.resultObject.is_null()) {
						if (!UserManagerInstance().AuthenticateUser(user->GetId(), answer.resultObject)) {
							Logger::GetInstance().Error("UserManager::AuthenticateUser() Could not Authenticate user");
							Terminate();
						}
						else {
							Logger::GetInstance().Info("Authed {0},{1},{2}", user->m_CurrentStage.StageGroupHash, user->m_CurrentStage.Level, user->m_CurrentStage.Difficulty);
							Net::Api api("Auth");
							api << user->GetCharacterName();
							api.GetAsync(this, &User::Authed, user);
						}
					}
					else
						Logger::GetInstance().Warn("Result code is 0 but the data is empty.");
				}
				else
					user->Terminate();
			}

			void User::Authed(const UserSharedPtr& user, const Net::Answer& answer) {
				if (answer.errorCode == 0 && !answer.resultObject.is_null()) {
					if (Auth(answer.resultObject)) {
						UserManagerInstance().RoomAuth(user);
						return;
					}
					else
						Logger::GetInstance().Warn(L"Could not authenticate user={0}", GetSerial());
				}
				else
					Logger::GetInstance().Warn(L"Could not handle the call api to Auth user = {0}", GetSerial());
				Terminate();
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

			void User::RemoveFishingUser()
			{
				if (m_Room)
					m_Room->RemoveFishingUser(GetSerial());
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

			bool User::IsEnableSkill(const uint32& hash) const
			{
				Database::Info::SkillInfoList::SkillInfo* skillinfo = GetSkillInfo(hash);

				if (!skillinfo)
				{
					Protocol::FromServer::Error error;
					error.errorcode = Errors::Unexpected;
					error.errorstring = L"Could not find the specified skill.";
					Send(error);
					return false;
				}

				if (m_CharacterStateFlags.IsRebirthSkillDisable == 1 && skillinfo->RebirthCountLimit > 0)
				{
					Protocol::FromServer::Error error;
					error.errorcode = Errors::NotUseToRebirthSkill;
					Send(error);
					return false;
				}

				if (skillinfo->PrecedingItems.size() > 0)
				{
					for (std::vector<uint32>::iterator itr = skillinfo->PrecedingItems.begin(); itr != skillinfo->PrecedingItems.end(); ++itr)
					{
						if (m_Items.GetItemCount(*itr) + m_Items.GetItemCountInPetInventory(*itr, true) < 1)
						{
							Protocol::FromServer::Error error;
							error.errorcode = Errors::NotHaveSkillItem;
							Send(error);
							return false;
						}
					}
				}
				int size = static_cast<int>(skillinfo->Reagents.size());

				for (int i = 0; i < size; i++)
				{
					int ic = m_Items.GetItemCount(skillinfo->Reagents[i].Hash) + m_Items
						.GetItemCountInPetInventory(skillinfo->Reagents[i].Hash, true);
					if (ic < skillinfo->Reagents[i].Cnt)
					{
						Protocol::FromServer::Error error;
						error.errorcode = Errors::NotHaveReagents;
						Send(error);
						return false;
					}
				}

				return true;
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

			void User::SetPlayerStylePointState(const StylePoint::State& state)
			{
				if (m_Player)
					m_Room->ChangeStylePointStateToLogic(m_Player, state);
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

			void User::SetUltimateSkillFlag(bool used)
			{
				m_UsedUltimateSkill = used;
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

			void User::UpdateExpFactor()
			{
				AutoLock lock(m_PlayerInitialData.SyncRoot);
				m_PlayerInitialData.Factors.ExpFactor = m_ExpFactorManager.TotalExpFactor();

				Protocol::FromServer::ChangeBaseExpFactor packet;
				packet.ExpFactor = ConfigInstance().Get("BaseExp", 1.0f);
				Send(packet);
			}

			void User::LoadStage(const StageLicense& targetStage)
			{
				SavePlayerData(); 
				m_ExpFactorManager.SetGuild(false);


				PlayerDeleted();
				{
					AutoLock lock(m_PlayerInitialData.SyncRoot);
					for(auto& pet: m_PlayerInitialData.pets)
						pet.Appear = false;
				}
				m_ExpFactorManager.ClearItemExpFactor();
				m_ExpFactorManager.ClearGuildExpFactor();
				m_ExpFactorManager.ClearDifficultyExpFactor();

				m_EnterShop = XRated::Constants::ShopType::Decorative;
				m_State = LOAD;
				SetCurrentStage(targetStage);

				m_Items.ResetExpiredBag();

				Protocol::FromServer::Stage stage;
				stage.charactername = GetCharacterName();
				stage.targetStage = targetStage;
				Send(stage);

				m_LoadEnded = false;
			}

			void User::MissionClear(const bool& success, Database::Info::StageGroup* group, const uint16& accessLevel)
			{
				if (m_Room && m_Player)
					m_FamilyManager.MissionClear(success, group, accessLevel);
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

			uint16 User::GetMailCnt() const
			{
				return m_MailBoxManager.GetMailCnt();
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

			void User::ObjectDestroyed(const uint32& id, const uint8& team)
			{
				m_QuestManager.ObjectDestroyed(shared_from_this(), id, team);
				/*if (achievement.IsConnected()) {
					Net::Protocol::Achievement::ServerProtocol::KilledNpc packet;
					packet.characterName = GetName();
					packet.npcHashes.push_back(id);
					achievement.Send(packet);
				}*/
			}

			void User::SetState(const STATE& state)
			{
				m_State = state;
			}

			void User::CheckExpiredItems()
			{
				if (m_Items.IsExpiredItem())
					m_ItemsChanged = true;
			}

			void User::MailAlarmed()
			{
				m_MailBoxManager.MailAlarmed();
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

			void User::EnterShop(const Constants::ShopType& type, const uint32& param)
			{
				AutoLock lock(m_UserMtx);
				if (!m_Room || !m_Player)
					return;
				if (m_IsItemLocked)
					return;
				if (m_State == VOTING)
					return;
				m_State = SHOP;
				m_EnterShop = type;
				if (m_EnterShop == XRated::Constants::ShopType::GuildShop) {
					if (IsPartOfGuild() == false) {
						CriticalError(("this user is not joined guild"));
						return;
					}
					m_ValidGuildInfo.GuildLevel = (uint8)param;
					m_ValidGuildInfo.type = ValidGuildInfo::Requested::EnterGuildShop;
					if (param != (uint32)GetGuildInfo().GuildLevel) {
						UserManagerInstance().RequestValidGuildInfo(shared_from_this());
						return;
					}
				}
				Serial playerSerial;
				{
					AutoLock playerLock(m_PlayerMtx);
					if (!m_Player)
						return;
					playerSerial = m_Player->GetSerial();
					m_Room->Command(m_Player, Constants::Command::Stop, XRated::Constants::None);
				}
				m_Room->EnterShop(playerSerial, type, param);
			}

			void User::SetFamilyExpFactor(const float& factor)
			{
				m_ExpFactorManager.SetFamilyExpFactor(factor);
				if (!m_Player || !m_Room)
					return;
				m_Room->ChangedExpFactor(shared_from_this());
			}

			void User::SetPenaltyExpFactor(const bool& penalty)
			{
				m_ExpFactorManager.SetPenal(penalty);
				UpdateExpFactor();
			}

			void User::InstantCoinRevive(const int16& errorNumber, const uint64& orderNumber)
			{
				AutoLock lock(m_UserMtx);
				Protocol::FromServer::InstantCoinRevive	instantCoinRevive;

				if (errorNumber == 0)
					instantCoinRevive.Result = Protocol::FromServer::InstantCoinRevive::Ok;
				else if (errorNumber == 1)
					instantCoinRevive.Result = Protocol::FromServer::InstantCoinRevive::FailedShopDBError;
				else if (errorNumber == 2)
					instantCoinRevive.Result = Protocol::FromServer::InstantCoinRevive::FailedCGPointError;
				else if (errorNumber == 3)
					instantCoinRevive.Result = Protocol::FromServer::InstantCoinRevive::FailedShopOrderError;

				if (errorNumber != 0)
				{
					LoggerInstance().Warn("InstantCoinRevive - [errorNumber:{}]", errorNumber);
				}
				else
				{
					m_Room->InstantCoinRevive(m_Player, orderNumber); // revive
				}


				Send(instantCoinRevive);
			}

			void User::PurchaseCashItem(const int16& errorNumber, const uint64& orderNumber)
			{
				Protocol::FromServer::PurchaseCashItem purchaseCashItem;

				if (errorNumber == 0)
					purchaseCashItem.Result = Protocol::FromServer::PurchaseCashItem::Ok;
				else if (errorNumber == 1)
					purchaseCashItem.Result = Protocol::FromServer::PurchaseCashItem::WrongOidProduct;
				else if (errorNumber == 2)
					purchaseCashItem.Result = Protocol::FromServer::PurchaseCashItem::NotEnoughCoin;
				else if (errorNumber == 3)
					purchaseCashItem.Result = Protocol::FromServer::PurchaseCashItem::UnknownError;
				else if (errorNumber == 4)
					purchaseCashItem.Result = Protocol::FromServer::PurchaseCashItem::QuantityOverDue;
				else if (errorNumber == 5)
					purchaseCashItem.Result = Protocol::FromServer::PurchaseCashItem::PeriodExceeded;
				else if (errorNumber == 6)
					purchaseCashItem.Result = Protocol::FromServer::PurchaseCashItem::CannotPurchase;
				else if (errorNumber == 7)
					purchaseCashItem.Result = Protocol::FromServer::PurchaseCashItem::NotOnSale;
				else if (errorNumber == 8)
					purchaseCashItem.Result = Protocol::FromServer::PurchaseCashItem::PeriodExceededGoods;

				if (errorNumber != 0)
				{
					LoggerInstance().Error("PurchaseCashItem - [errorNumber:{}]", errorNumber);
				}

				Send(purchaseCashItem);
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

			void User::SetPlayerInitLife(const uint16& initLife)
			{
				AutoLock lock(m_PlayerInitialData.SyncRoot);
				m_PlayerInitialData.life = initLife;
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
				if (!m_Room || !m_Player)
				{
					LoggerInstance().Error("User::ItemsEquip() room == NULL || player == NULL");
					return false;
				}
				//if (achievement.IsConnected() && info != NULL) {
				//	std::wcout << L"ItemsEquip[" << info->Id << L"]" << std::endl;
				//	Net::Protocol::Achievement::ServerProtocol::ItemAcquired packet;
				//	packet.characterName = GetName();
				//	packet.count = Net::Protocol::Achievement::ServerProtocol::ItemAcquired::Wear;
				//	packet.equipPart = info->EquipParts;
				//	packet.addItemType = 0xffffffff/* info->ItemType */;
				//	packet.equipPart = info->EquipParts == 0 ? uint32(-1) : info->EquipParts;
				//	packet.rarity = info->EquipLv;
				//	packet.itemHash = info->Hash;
				//	packet.isEquip = 1;
				//	achievement.Send(packet);
				//}
				return m_Room->Equip(m_Player, info, instance, static_cast<XRated::Constants::Equipment>(equipPos));
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

			Database::Info::SkillInfoList::SkillInfo* User::GetSkillInfo(const uint32& skillGroupHash) const
			{
				if (!m_Player)
				{
					LoggerInstance().Error(L"User({})::GetSkillInfo() player == NULL", GetCharacterName().c_str());
					return nullptr;
				}

				//uint32 classHash = Constants::GetClassHash( playerInitData.type );
				uint32 classHash = Constants::GetClassHash(m_Player->GetPlayerData().Job);

				auto skillgroup = Database::DatabaseInstance().InfoCollections.Skills.GetSkillGroupInfo(classHash, skillGroupHash);

				if (!skillgroup)
				{
					LoggerInstance().Error(L"User({})::GetSkillInfo() skillgroup == NULL", GetCharacterName().c_str());
					return nullptr;
				}

				return skillgroup->GetSkillInfo((uint8)m_Player->GetSkillLevel(skillGroupHash));
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
				auto pair = GetEquipment(XRated::Constants::Equipment::Weapon);
				auto pair2 = GetEquipment(XRated::Constants::Equipment::CashWeapon);

				if (Database::DatabaseInstance().InfoCollections.FishigInfos.IsFishingRod(pair.first) == true)
					return Database::DatabaseInstance().InfoCollections.FishigInfos.RetrieveID(pair.first);
				if (Database::DatabaseInstance().InfoCollections.FishigInfos.IsFishingRod(pair2.first) == true)
					return Database::DatabaseInstance().InfoCollections.FishigInfos.RetrieveID(pair2.first);

				return 0;
			}

			const Database::Info::FishingRodInfo::RodInfo* User::GetFishingRod() const
			{
				auto pair = GetEquipment(XRated::Constants::Equipment::Weapon);
				auto pair2 = GetEquipment(XRated::Constants::Equipment::CashWeapon);

				if (Database::DatabaseInstance().InfoCollections.FishigInfos.IsFishingRod(pair.first) == true)
					return Database::DatabaseInstance().InfoCollections.FishigInfos.Retrieve(pair.first, m_CurrentStageHash);
				if (Database::DatabaseInstance().InfoCollections.FishigInfos.IsFishingRod(pair2.first) == true)
					return Database::DatabaseInstance().InfoCollections.FishigInfos.Retrieve(pair2.first, m_CurrentStageHash);

				return nullptr;
			}

			bool User::IsProperFishingRod(const uint32& rodHash) const
			{
				Database::Info::ItemInfo* info = Database::DatabaseInstance().InfoCollections.Items.Retrieve(rodHash);
				if (info == nullptr)
					return false;
				uint16 lv = GetLevel();
				if (lv < info->Limits.Level || lv > info->Limits.MaxLevel)
					return false;
				return true;
			}

			bool User::IsFishingArea() const
			{
				AutoLock lock(m_UserMtx);
				if (!m_Player) {
					LoggerInstance().Warn(L"Unknow exception - player == NULL ({})", GetName().c_str());
					return false;
				}
				float3 pos = m_Player->GetPosition() + m_Player->GetDirection() * XRated::Fishing::CorkRelativePosition;

				Database::Info::StageInfo* stageInfo = m_Room->GetStageInfo();
				if (!stageInfo) {
					LoggerInstance().Warn(L"Unable to find stageinfo. seems to be changing stage now.({})", GetName().c_str());
					return false;
				}
				else {
					if (stageInfo->GetMoveMapHeigh(pos.x, pos.z) != XRated::Fishing::FishingHeight)
						return false;
				}
				return true;
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
				Send(packet);
			}

			void User::ItemsSendToAll(Protocol::IPacketSerializable& packet)
			{
				SendToAll(packet);
			}

			void User::ItemsDirectSend(Protocol::IPacketSerializable& packet)
			{
				Send(packet);
			}

			bool User::ItemsUse(const uint32& itemHash, const uint32& bag, const uint32& pos)
			{
				if (!m_Room || !m_Player)
				{
					LoggerInstance().Error("User::ItemsUse({}) - Error : room == NULL || player == NULL", GetSerial());
					return false;
				}

				return m_Room->Use(m_Player, itemHash, bag, pos);
			}

			bool User::PetItemUse(const uint32& itemHash, const uint32& bag, const uint32& pos, GlobalSerial itemSerial)
			{
				if (!m_Room || !m_Player)
				{
					LoggerInstance().Error("User::ItemsUse({}) - Error : room == NULL || player == NULL", GetSerial());
					return false;
				}

				return m_Room->PetItemUse(m_Player, itemHash, bag, pos, itemSerial);
			}

			void User::PetInventoryDirectSend(Protocol::IPacketSerializable& packet)
			{
				Send(packet);
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

			bool User::IsEnoughSlotCountByProduceSkill(const uint32& skill)
			{
				uint32 classHash = XRated::Constants::GetClassHash(GetClassType());
				Database::Info::SkillInfoList::SkillGroup* skillGroup = Database::DatabaseInstance().InfoCollections.Skills.GetSkillGroupInfo(classHash, skill);
				if (!skillGroup)
				{
					LoggerInstance().Error("User::IsEnoughSlotCountByProduceSkill() - SkillGroup == NULL");
					return false;
				}

				if (Database::DatabaseInstance().InfoCollections.EnchantTables.IsProduceSkill(skillGroup->Name) == false)
					return false;

				std::vector< std::pair<uint32/*item hash*/, uint32/*count*/> > toRemove;
				std::vector< std::pair<uint32/*item hash*/, uint32/*count*/> > toAdd;

				/*toRemove*/
				Database::Info::SkillInfoList::SkillInfo* skillinfo = GetSkillInfo(skill);
				if (skillinfo == NULL)
					return false;

				size_t size = skillinfo->Reagents.size();
				for (size_t i = 0; i < size; ++i)
				{
					toRemove.emplace_back(skillinfo->Reagents[i].Hash, skillinfo->Reagents[i].Cnt);
				}

				/*toAdd*/
				Database::Enchant::EnchantTableManager::ItemInfosForProduce produceList;
				Database::DatabaseInstance().InfoCollections.EnchantTables.RetrieveMakingSkillMadeItems(skillGroup->Name, m_Player->GetSkillLevel(skill) - 1, produceList);
				if (produceList.empty()) return false;

				Database::Enchant::EnchantTableManager::ItemInfosForProduce::const_iterator iter = produceList.begin();
				while (iter != produceList.end())
				{
					toAdd.emplace_back(iter->first, iter->second);
					++iter;
				}

				/* item validation */
				int requiredSlotCount(GetRequiredSlotCount(toRemove, toAdd));
				if (static_cast<int>(GetBlankSlotCountOfNormalBag()) < requiredSlotCount) // not enough item slots to put-in
				{
					return false;
				}

				return true;
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
				m_ItemsChanged = true;
				return m_Items.ItemAcquired(id, gameObjectSerial, cnt, instance, autoStack, addItemType);
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

			uint16 User::ItemEquiped(bool succeed)
			{
				return m_Items.ItemEquiped(succeed);
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

			float User::GetExtraExpFactor()
			{
				return m_ExpFactorManager.ExtraExpFactor();
			}

			std::wstring User::GetExtraExpString()
			{
				float fExtraExpFactor = m_ExpFactorManager.ExtraExpFactor();
				int iExtraExpFactor = static_cast<int>(m_ExpFactorManager.ExtraExpFactor());

				if ((fExtraExpFactor / 1.0f) == iExtraExpFactor)
					return StringUtil::ToUnicode(iExtraExpFactor);

				else
				{
					float fResult = fExtraExpFactor - iExtraExpFactor;

					int iResult = static_cast<int>(fResult / 0.1f);

					std::wstring result = StringUtil::ToUnicode(iExtraExpFactor);
					result += L".";
					result += StringUtil::ToUnicode(iResult);

					return result;
				}
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

			void User::PlayerDeleted()
			{
				AutoLock playerLock(m_PlayerMtx);
				delete m_Player;
				m_Items.DeletePlayer();
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

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::LoadEnd& packet)
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

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::Stage& packet) {

				Logger::GetInstance().Info("fwPacketListener::protocol@Stage");
				if (packet.Version != Lunia::Constants::Version) {
					Protocol::FromServer::Error sendPacket;
					sendPacket.errorcode = XRated::Errors::InvalidClientVersion;
					Send(sendPacket);
					Terminate();
					UserManagerInstance().RemoveUser(user->GetSerial());
					return;
				}

				user->m_SecuKey = packet.secukey;
				user->m_UsingLocale = packet.Locale.c_str();

				{
					Logger::GetInstance().Info("A connection with (Ip: {0}) is authorizing", user->GetPeerAddress());
					Net::Api api("AuthConnection");
					api << user->m_SecuKey;
					api.GetAsync(this, &User::AuthedConnection, user);
				}
			}

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::Alive& packet) {
				m_aliveTime = timeGetTime();
				user->Send(packet);
			}

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::Join& packet)
			{
				AutoLock lock(m_UserMtx);
				if (!IsLoaded()) {
					LoggerInstance().Error("User({})::Dispatch - Not loaded and sending join", GetSerial());
					Protocol::FromServer::Error error;
					error.errorcode = Errors::InvalidStageCode;
					//Send(error);
				}
			}

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::Command& packet)
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

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::ListItem& packet)
			{
				//Does nothing.
			}

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::ListQuickSlot& packet)
			{
				//Does nothing.
			}

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::Chat& packet)
			{
				AutoLock lock(m_UserMtx);

				if (!m_Room)
					return;

				if (packet.message.size() > 0 && packet.message[0] == L'&' && m_CharacterStateFlags.IsAdmin) // kind of hack
				{
					std::vector<String> parsed;
					StringUtil::Split(packet.message.begin() + 1, packet.message.end(), L' ', parsed);

					// parse each command
					if (parsed[0] == L"item") // give an item
					{
						/* parameters : item_hash [stacked_count=1] [instance=0] */

						uint32 itemHash(0);
						uint16 stackedCount(1);
						InstanceEx instance(0);

						switch (parsed.size())
						{
						case 1: return; // no effects;
						case 4: 
							instance = StringUtil::ToInt64(parsed[3]);
						case 3: 
							stackedCount = StringUtil::ToInt(parsed[2]); 
							if (stackedCount == 0) 
								stackedCount = 1;
						case 2: 
							itemHash = StringUtil::ToInt(parsed[1]);
							break;
						default:
							return;
						}

						ItemAdd(itemHash, 0, stackedCount, instance);
					}
					else if (parsed[0] == L"swap" && parsed.size() == 1)
					{
						Protocol::ToServer::EquipSwap packet;
						packet.Set = StringUtil::ToInt(parsed[1]);
						Dispatch(shared_from_this(), packet);
					}
					else // assume logic commands
					{
						m_Room->DebugCommand(shared_from_this(), packet.message.c_str() + 1);
					}
					LoggerInstance().Info(L"chat:{}-User:{}-{}", (int)packet.chattype, GetSerial(), packet.message);
				}
				else if (!m_Player && m_CharacterStateFlags.IsSpectator)
				{
					m_Room->SpectatorChat(m_CharacterName, packet);
				}
				else if (m_Player)
				{
					m_Room->Chat(m_Player->GetSerial(), packet);
				}
				/* 'chat.message' should not be used after logging because chat.message replaced by filter */
				//std::replace_if(packet.message.begin(), packet.message.end(), std::bind(std::equal_to<wchar_t>(), L','), L'.');
				//Logger().Write(IAdmLogger::ChatLogger, "chat", name, Service::Http::Logger::Parameter() << static_cast<int>(chat.chattype) << chat.message);
			}

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::EquipSwap& packet)
			{
				AutoLock lock(m_UserMtx);
				m_Items.SwapEquipment(packet.Set);
			}

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::Use& packet) 
			{
				AutoLock lock(m_UserMtx);

				if (m_State == LOAD || m_State == NONE)
					return;

				if (IsItemLocked())
				{
					CriticalError("invalid state to use an item");
					return;
				}

				/* validation */
				if (m_Items.IsEnable(false, packet.itemposition.Bag) == false) /* normal inventory bag */
				{
					CriticalError("invaliad bag selected(on using)");
					return;
				}

				if (!m_Items.UseItem(packet.itemposition.Bag, packet.itemposition.Position))
				{
					LoggerInstance().Error("User({})::Dispatch(Use) - items.UseItem({}, {}) == false", GetSerial(), packet.itemposition.Bag, packet.itemposition.Position);
					return;
				}
			}

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::MoveItem& packet)
			{
				AutoLock lock(m_UserMtx);
				/* critical validation */
				if (Items::Validation::IsValidPacketParameter(ItemPosition(packet.pbag, packet.ppos)) == false ||
					Items::Validation::IsValidPacketParameter(ItemPosition(packet.nbag, packet.npos)) == false)
				{
					user->CriticalError(fmt::format("invalid item position to move - ({}, {}) to ({}, {})", packet.pbag, packet.ppos, packet.nbag, packet.npos).c_str(), true);
				}

				if (m_IsItemLocked)
				{
					if (IsLoaded() == false)
					{
						Protocol::FromServer::MoveItem moveitem;
						moveitem.pbag = 0xff;
						moveitem.ppos = 0xff;
						moveitem.nbag = 0xff;
						moveitem.npos = 0xff;
						Send(moveitem);
						return;
					}

					CriticalError("invalid state to move an item");
					return;
				}

				/* validation */
				if (packet.nbag == 0) /* equip item */
				{
					const Common::ItemEx* item = m_Items.GetItem(packet.pbag, packet.ppos);
					if (!item || !item->Info)
					{
						CriticalError("empty item selected");
						return;
					}

					XRated::Constants::Equipment equipSlot = static_cast<XRated::Constants::Equipment>(packet.npos);
					if (item->Info->ItemType != Database::Info::ItemInfo::Type::EQUIPMENT || XRated::Constants::IsEquippable(equipSlot, item->Info->EquipParts) == false)
					{
						CriticalError(fmt::format("wrong equipment(slot:{}, equipment:{}) from ({}, {})"
							, equipSlot, item->Info->EquipParts, packet.pbag, packet.ppos).c_str(), true);
						return;
					}

					if (Database::Enchant::IsEnchantable(item->Info) && static_cast<Database::Enchant::EnchantBitfields>(item->InstanceEx).IsDestoryed())
					{
						CriticalError("destroied equipment requested");
						return;
					}
				}
				else if (m_Items.IsEnable(false, packet.nbag) == false) /* normal inventory bag */
				{
					CriticalError("invaliad bag selected(on moving)");
					return;
				}

				/*
					in case of equipment, ItemsEquip() will be called in items.MoveItems()
				*/
				if (m_Items.MoveItems(packet.pbag, packet.ppos, packet.nbag, packet.npos))
				{
					m_ItemsChanged = true;
				}
			}

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::Voice& packet) {
				AutoLock lock(m_UserMtx);

				if (!m_Room || !m_Player)
					return;

				m_Room->Voice(m_Player->GetSerial(), packet);
			}

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::EnterShop& packet) {
				EnterShop(packet.shopnumber, packet.param);
			}

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::LeaveShop& packet) {
				AutoLock lock(m_UserMtx);

				if (!m_Room || !m_Player)
					return;

				m_State = ACTIVE;
				m_EnterShop = XRated::Constants::ShopType::Decorative;

				XRated::ObjectData& data = m_Player->GetPlayerData().BaseCharacter.BaseObject;

				m_Room->LeaveShop(data.GameObjectSerial, data.Position, data.Direction);
			}

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::CharacterInfo& packet)
			{
				//Does nothing.
			}
			
			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::Cast& packet) 
			{
				AutoLock lock(m_UserMtx);
				if (!m_Room || !m_Player)
					return;

				if (m_State == LOAD || m_State == NONE)
					return;

				if (!IsEnableSkill(packet.skillgroupid))
				{
					LoggerInstance().Error(L"User({})::Dispatch(Cast) - Error : IsEnableSkill({}) == false", GetCharacterName().c_str(), packet.skillgroupid);
					return;
				}

				m_Room->Cast(m_Player, packet.skillgroupid);
			}

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::SetSkillLevel& packet)
			{
				AutoLock lock(m_UserMtx);
				if (!m_Room || !m_Player)
					return;

				if (m_State == LOAD || m_State == NONE)
					return;

				if (HasSkillLicense(packet.skillgroupid) == false) {
					CriticalError(fmt::format("Trying to increase unacquired skill license. - ({})", (int)packet.skillgroupid).c_str(), true);
					return;
				}
				m_Room->LogicSkillPointUp(m_Player, packet);
			}

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::InstantCoinRevive& packet)
			{
				if (!m_Room || !m_Player)
					return;

				CoinItemPurchaseManagerInstance().PurchaseCoinItem(shared_from_this(), GetSerial(), m_Room->GetRoomIndex(), m_Room->GetRoomID(), CoinItemPurchaseManagerInstance().InstantCoinReviveItem);
			}

			void User::Dispatch(const UserSharedPtr user, Protocol::ToServer::CashEquipSwap& packet)
			{
				AutoLock lock(m_UserMtx);
				m_Items.SwapCashEquipment(packet.Set);
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