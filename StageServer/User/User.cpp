#pragma once
#include <StageServer/fwPacketListener.h>
#include <StageServer/User/UserManager.h>
#include <Network/NetStream.h>
#include <Network/CommonProtocol/Protocol.h>
#include <StageServer/Protocol/FromServer.h>
#include <Info/Info.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer
		{
			template <typename T>
			inline std::shared_ptr<T> shared_from_enabled(std::enable_shared_from_this<T>* base) {
				return base->shared_from_this();
			}
			template <typename T>
			inline std::shared_ptr<T> shared_from(T* derived) {
				return std::static_pointer_cast<T>(shared_from_enabled(derived));
			}
			template<> std::shared_ptr<User> shared_from_enabled(std::enable_shared_from_this<User>* base);
			void User::Send(Protocol::IPacketSerializable& packet) {
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

			User::User(asio::ip::tcp::socket&& socket, const uint32& userId)
				: ClientTcp(std::move(socket))
				, m_UserId(userId)
				, m_GuildState(*this)
				, m_FamilyManager(*this)
			{
				Logger::GetInstance().Info("User :: Hey, I was created!", GetId());
			}

			std::shared_ptr<User> User::shared_from_this() {
				return shared_from(this);
			}

			void User::Init() {
				Protocol::FromServer::Way way;
				way.EncryptKey = Math::Random();
				SetCryptoKey(way.EncryptKey);
				Send(way);
			}

			void User::Flush()
			{
				if (!IsConnected())
					return;
				LoggerInstance().Info("Flushing user:{0}", GetSerial());
				CloseSocket();
			}

			bool User::AuthConnection(const json& result)
			{
				AutoLock lock(mtx);
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
				AutoLock lock(mtx);//self lock.
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
				if (!Auth_SetBagList(result["bags"])) {
					Logger::GetInstance().Error(L"User::Auth_SetBagList={0}", GetSerial());
					return false;
				}
				if (!Auth_SkillLicenses(result["skillLicenses"])) {
					Logger::GetInstance().Error(L"User::Auth_SkillLicenses={0}", GetSerial());
				}
				if (!Auth_SetItemList(result["items"])) {
					Logger::GetInstance().Error(L"User::Auth_SetItemList={0}", GetSerial());
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

					return true;
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

			uint32 User::GetId() const {
				return m_UserId;
			}

			void User::SetSerial(const uint64& userSerial){
				m_UserSerial = userSerial;
			}

			void User::AddPassive(const uint32& hash)
			{
				LoggerInstance().Warn("AddPassive Missing on user");
			}

			void User::RemovePassive(const uint32& hash)
			{
				LoggerInstance().Warn("RemovePassive Missing on user");
			}

			const uint64& User::GetSerial() const
			{
				return m_UserSerial;
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

			void User::Update(const float& dt){
				AutoLock l_(mtx);
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

			const Constants::ClassType& User::GetClassType() const
			{
				if (m_Player == nullptr) {
					LoggerInstance().Error("User::GetClassType() - m_Player==null");
					return std::move(Constants::ClassType::NoClass);
				}
				return m_Player->GetClassType();
			}

			std::mutex& User::GetSyncObject()
			{
				return mtx;
			}

			uint8 User::GetTeamNumber() const
			{
				return m_TeamNumber;
			}

			uint16 User::GetLevel() const
			{
				if (m_Player == nullptr) {
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
				if (m_CharacterStateFlags.IsSpectator)
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

			bool User::ClearGuildInfo()
			{
				LoggerInstance().Warn("User({0})::ClearGuildInfo - NotImplemented", GetSerial());
				return true;
			}

			bool User::PlayerDeleted()
			{
				LoggerInstance().Warn("User({0})::PlayerDeleted - NotImplemented", GetSerial());
				return true;
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