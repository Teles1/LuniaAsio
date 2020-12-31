#include <StageServer/fwPacketListener.h>
#include <StageServer/User/UserManager.h>
#include <Network/NetStream.h>
#include <Network/CommonProtocol/Protocol.h>
#include <StageServer/StageServerProtocol/StageServerProtocol.h>
#include <Info/Info.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer
		{
			void User::Send(Serializer::ISerializable& packet) {
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

				Logger::GetInstance().Info("Sending packet[{0:#04x}] to User@{1}", *((HashType*)buffer.GetData() + 2), this->GetSerial());
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

			void User::Init() {
				Protocol::Way way;
				way.EncryptKey = Math::Random();
				SetCryptoKey(way.EncryptKey);
				Send(way);
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
				m_RoomFullCount = result["capacity"].get<uint32>();
				m_TeamNumber = result["teamNumber"].get<int>();
				m_StageStateFlags = result["stageStates"].get<int>();
				m_LastStage.StageGroupHash = result["lastPlayedStageGroupHash"].get<uint32>();
				m_LastStage.Level = result["lastPlayedStageLevel"].get<uint16>();
				m_RoomActivateSerial = result["uniqueKey"].get<int64>();
				return true;
			}

			bool User::Auth(const json& result)
			{
				AutoLock lock(mtx);//self lock.
				AutoLock stageLock(m_PlayerData.SyncRoot); // prevent the room/stagescript from modifiyng the userData
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
					m_PlayerData.team = m_TeamNumber;
					m_PlayerData.name = GetCharacterName();
					//Character

					auto& mPlayerData = characterinfo["character"];
					if (mPlayerData.is_null())
						return false;

					m_PlayerData.type = (XRated::Constants::ClassType)mPlayerData["classNumber"].get<int>();
					m_PlayerData.level = mPlayerData["stageLevel"].get<uint16>();
					m_PlayerData.xp = mPlayerData["stageExp"].get<uint32>();
					m_PlayerData.pvpLevel = mPlayerData["pvpLevel"].get<uint16>();
					m_PlayerData.pvpXp = mPlayerData["pvpExp"].get<uint32>();
					m_PlayerData.warLevel = mPlayerData["warLevel"].get<uint32>();
					m_PlayerData.warXp = mPlayerData["warExp"].get<uint32>();
					m_PlayerData.money = mPlayerData["gameMoney"].get<uint32>();
					m_PlayerData.bankMoney = mPlayerData["bankMoney"].get<uint32>();
					m_PlayerData.skillpoint = mPlayerData["skillPoint"].get<uint16>();
					m_CharacterStateFlags = mPlayerData["instantStateFlag"].get<int>();
					m_CreateDate.Parse(mPlayerData["createDate"].get<std::string>());

					//The one that gives you +1 that paid op one.
					if (!characterinfo["addedSkillPoint"].is_null())
						m_PlayerData.addedSkillPointPlus = characterinfo["addedSkillPoint"]["skillPoint"].get<uint16>();
					else
						m_PlayerData.addedSkillPointPlus = 0;

					m_PlayerData.characterStateFlags = m_CharacterStateFlags;

					//Pvp
					auto& pvp = characterinfo["ladderPoint"];
					if (pvp.is_null())
						Logger::GetInstance().Exception(L"Could not parse LadderPoint on Auth user = {0}", GetCharacterName());
					m_PlayerData.ladderPoint = pvp["ladderPoint"].get<uint32>();
					m_PlayerData.ladderMatchCount = pvp["dailyPlayCount"].get<uint16>();
					m_PlayerData.ladderWinCount = pvp["winCount"].get<uint32>();
					m_PlayerData.ladderLoseCount = pvp["loseCount"].get<uint32>();

					//Achivement
					if (characterinfo["achievementPoint"].is_null())
						Logger::GetInstance().Exception(L"Could not parse achievementPoint on Auth user = {0}", GetCharacterName());

					m_PlayerData.achievementScore = characterinfo["achievementPoint"]["achievementPoint"].get<uint32>();

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
									m_PlayerData.StartPosition.UseCustomStartPoint = true;
									m_PlayerData.StartPosition.Place = it->second;
								}
							}
						}
					}
				}
				//Granted
				{
					Protocol::Granted sendPacket;
					sendPacket.charactername = GetCharacterName();
					sendPacket.targetStage = m_CurrentStage;
					Send(sendPacket);
				}
				//CharacterInfo -Packet being sent to user
				{
					Protocol::CharacterInfo characterinfo;
					characterinfo.classtype = m_PlayerData.type;
					characterinfo.level = m_PlayerData.level;
					characterinfo.xp = m_PlayerData.xp;
					characterinfo.pvpLevel = m_PlayerData.pvpLevel;
					characterinfo.pvpXp = m_PlayerData.pvpXp;
					characterinfo.warLevel = m_PlayerData.warLevel;
					characterinfo.warXp = m_PlayerData.warXp;
					characterinfo.storedLevel = m_PlayerData.storedLevel;
					characterinfo.rebirthCount = m_PlayerData.rebirthCount;
					characterinfo.money = m_PlayerData.money;
					characterinfo.bankMoney = m_PlayerData.bankMoney;
					characterinfo.life = m_PlayerData.life;
					characterinfo.skillpoint = m_PlayerData.skillpoint;
					characterinfo.ladderPoint = m_PlayerData.ladderPoint;
					characterinfo.ladderMatchCount = m_PlayerData.ladderMatchCount;
					characterinfo.ladderWinCount = m_PlayerData.ladderWinCount;
					characterinfo.ladderLoseCount = m_PlayerData.ladderLoseCount;
					characterinfo.addedSkillPointPlus = m_PlayerData.addedSkillPointPlus;
					characterinfo.storedSkillPoint = m_PlayerData.storedSkillPoint;
					characterinfo.achievementScore = m_PlayerData.achievementScore;
					characterinfo.IsSpectator = m_PlayerData.characterStateFlags.IsSpectator == 1 ? true : false;
					Send(characterinfo);
				}
				if (m_CharacterStateFlags.IsPcRoom && ((stageGroup->GameType & Constants::GameTypes::PvpGameTypeMask) == 0))
				{
					m_PlayerData.life = static_cast<uint16>(stageGroup->InitailLife + 1);// one more life for PC room
					m_PlayerData.Factors.MaxHp = 1.2f;
					m_PlayerData.Factors.MaxMp = 1.2f;
				}
				else { //normal user
					m_PlayerData.life = static_cast<uint16>(stageGroup->InitailLife);
					m_PlayerData.Factors.MaxHp = 1.0f;
					m_PlayerData.Factors.MaxMp = 1.0f;
				}

				m_ExpFactorManager.SetPenal(m_StageStateFlags.IsPrivate);
				m_ExpFactorManager.SetExtra(m_CharacterStateFlags.IsPcRoom);

				m_PlayerData.Factors.ExpFactor = m_ExpFactorManager.TotalExpFactor();

				return true;
			}

			bool User::Auth_CharacterLicenses(const json& licenses)
			{
				if (licenses.is_null())
					return false;
				//CharacterLicense // Which characters the user is allowed to create.
				uint32 aux = licenses["licenses"].get<uint32>();
				for (int i = 0; i < 16; i++) {
					if (static_cast<bool>(aux & (1 << i)))
						m_CharacterLicenses.push_back(i);
				}
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
				m_PlayerData.rebirthCount = rebirth["rebirthCount"].get<uint16>();
				m_PlayerData.storedLevel = m_PlayerData.rebirthCount == 0 ? m_PlayerData.level : rebirth["storedLevel"].get<uint16>();
				m_PlayerData.storedSkillPoint = rebirth["storedSkillPoint"].get<uint16>();
				m_PlayerData.lastRebirthDateTime.Parse(rebirth["lastRebirth"].get<std::string>());
				return true;
			}

			bool User::Auth_SetItemList(const json& items)
			{
				//We should check weather the position where the item is at is valid.
				Protocol::ListItem sendPacket;
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
				Protocol::BagStates sendPacket;
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
				Protocol::ListSkillLicenses sendPacket;
				sendPacket.listSkillLicenses = m_SkillLicenses;
				Send(sendPacket);
				return true;
			}

			bool User::Auth_Skills(const json& skills)
			{
				for (auto& x : skills) {
					m_PlayerData.skills.push_back(XRated::Skill(x["skillGroupHash"].get<uint32>(), x["skillLevel"].get<uint8>()));
				}
				Protocol::ListSkill sendPacket;
				sendPacket.listskill = m_PlayerData.skills;
				Send(sendPacket);
				return true;
			}

			bool User::Auth_QuickSlots(const json& quickslots)
			{
				Protocol::ListQuickSlot sendPacket;
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
				Protocol::PetInfo sendPacket;
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
					m_PlayerData.pets.push_back(petDataWithPos.Pet);
					m_PetDatas.Add(petDataWithPos.Pet);
				}
				Send(sendPacket);
				return true;
			}

			bool User::Auth_PetsItems(const json& petsitems)
			{
				//We should check weather the position where the item is at is valid.
				Protocol::ListPetItem sendPacket;
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
				Protocol::PetsCaredBySchool sendPacket;
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
					m_PlayerData.petCared.push_back(petTraining);
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

			const uint64& User::GetSerial() const
			{
				return m_UserSerial;
			}

			bool User::IsAuthenticated() const
			{
				return m_UserSerial == 0 ? false : true;
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

			StageLicense& User::GetCurrentStage(){
				return m_CurrentStage;
			}

			uint32 User::GetRoomIndex() const
			{
				return m_RoomIndex;
			}

			std::string User::GetRoomPass() const
			{
				return m_RoomPass;
			}

			void User::Terminate(){
				Logger::GetInstance().Warn("Terminate has to be implemented.");
				AutoLock _l(mtx);
				Net::Api api("Terminate");
				api << GetCharacterName();
				api.GetAsync();
				CloseSocket();
			}
		}
	}
}