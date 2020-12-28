#include <StageServer/User/UserManager.h>
#include <Info/Info.h>
#include <Info/Info/StageGroup/StageGroup.h>
#include <Network/Api/Api.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer
		{
			static UserManager& UserManagerInstance() {
				static UserManager m_Instance;
				return m_Instance;
			};

			UserManager::UserManager() {

			}
			UserManager::~UserManager() {

			}
			bool UserManager::Auth(UserSharedPtr& user, const json& data) {
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
					user->m_PlayerData.team = user->m_TeamNumber;
					user->m_PlayerData.name = user->GetCharacterName();
					//Character
					{
						auto& mPlayerData = data["character"];
						if (mPlayerData.is_null())
							Logger::GetInstance().Exception(L"Could not parse character on Auth user = {0}", user->GetCharacterName());

						user->m_PlayerData.type = (XRated::Constants::ClassType)mPlayerData["classNumber"].get<int>();
						user->m_PlayerData.level = mPlayerData["stageLevel"].get<uint16>();
						user->m_PlayerData.xp = mPlayerData["stageExp"].get<uint32>();
						user->m_PlayerData.pvpLevel = mPlayerData["pvpLevel"].get<uint16>();
						user->m_PlayerData.pvpXp = mPlayerData["pvpExp"].get<uint32>();
						user->m_PlayerData.warLevel = mPlayerData["warLevel"].get<uint32>();
						user->m_PlayerData.warXp = mPlayerData["warExp"].get<uint32>();
						user->m_PlayerData.money = mPlayerData["gameMoney"].get<uint32>();
						user->m_PlayerData.bankMoney = mPlayerData["bankMoney"].get<uint32>();
						user->m_PlayerData.skillpoint = mPlayerData["skillPoint"].get<uint16>();
						user->m_CharacterStateFlags = mPlayerData["instantStateFlag"].get<int>();
						user->m_CreateDate.Parse(mPlayerData["createDate"].get<std::string>());

						//The one that gives you +1 that paid op one.
						if (!data["addedSkillPoint"].is_null())
							user->m_AddedSkillPoint = data["addedSkillPoint"]["skillPoint"].get<uint16>();

						user->m_PlayerData.characterStateFlags = user->m_CharacterStateFlags;
					}
					//CharacterRebirth
					{
						auto& rebirth = data["characterRebirth"];
						if (rebirth.is_null())
							Logger::GetInstance().Exception(L"Could not parse CharacterRebirth on Auth user = {0}", user->GetCharacterName());
						user->m_PlayerData.rebirthCount = rebirth["rebirthCount"].get<uint16>();
						user->m_PlayerData.storedLevel = user->m_PlayerData.rebirthCount == 0 ? user->m_PlayerData.level : rebirth["storedLevel"].get<uint16>();
						user->m_PlayerData.storedSkillPoint = rebirth["storedSkillPoint"].get<uint16>();
						user->m_PlayerData.lastRebirthDateTime.Parse(rebirth["lastRebirth"].get<std::string>());
					}
					//Pvp
					{
						auto& pvp = data["ladderPoint"];
						if (pvp.is_null())
							Logger::GetInstance().Exception(L"Could not parse LadderPoint on Auth user = {0}", user->GetCharacterName());
						user->m_PlayerData.ladderPoint = pvp["ladderPoint"].get<uint32>();
						user->m_PlayerData.ladderMatchCount = pvp["dailyPlayCount"].get<uint16>();
						user->m_PlayerData.ladderWinCount = pvp["winCount"].get<uint32>();
						user->m_PlayerData.ladderLoseCount = pvp["loseCount"].get<uint32>();
					}
					//Achivement
					{
						if (data["achievementPoint"].is_null())
							Logger::GetInstance().Exception(L"Could not parse achievementPoint on Auth user = {0}", user->GetCharacterName());

						user->m_PlayerData.achievementScore = data["achievementPoint"]["achievementPoint"].get<uint32>();
					}
				}
				//Granted
				{
					Protocol::Granted sendPacket;
					sendPacket.charactername = user->GetCharacterName();
					sendPacket.targetStage = user->m_CurrentStage;
					user->Send(sendPacket);
				}
				//CharacterInfo
				{
					Protocol::CharacterInfo characterinfo;
					characterinfo.classtype = user->m_PlayerData.type;
					characterinfo.level = user->m_PlayerData.level;
					characterinfo.xp = user->m_PlayerData.xp;
					characterinfo.pvpLevel = user->m_PlayerData.pvpLevel;
					characterinfo.pvpXp = user->m_PlayerData.pvpXp;
					characterinfo.warLevel = user->m_PlayerData.warLevel;
					characterinfo.warXp = user->m_PlayerData.warXp;
					characterinfo.storedLevel = user->m_PlayerData.storedLevel;
					characterinfo.rebirthCount = user->m_PlayerData.rebirthCount;
					characterinfo.money = user->m_PlayerData.money;
					characterinfo.bankMoney = user->m_PlayerData.bankMoney;
					characterinfo.life = user->m_PlayerData.life;
					characterinfo.skillpoint = user->m_PlayerData.skillpoint;
					characterinfo.ladderPoint = user->m_PlayerData.ladderPoint;
					characterinfo.ladderMatchCount = user->m_PlayerData.ladderMatchCount;
					characterinfo.ladderWinCount = user->m_PlayerData.ladderWinCount;
					characterinfo.ladderLoseCount = user->m_PlayerData.ladderLoseCount;
					characterinfo.addedSkillPointPlus = user->m_PlayerData.addedSkillPointPlus;
					characterinfo.storedSkillPoint = user->m_PlayerData.storedSkillPoint;
					characterinfo.achievementScore = user->m_PlayerData.achievementScore;
					characterinfo.IsSpectator = user->m_PlayerData.characterStateFlags.IsSpectator == 1 ? true : false;
					user->Send(characterinfo);
				}

				//CharacterLicense // Which characters the user is allowed to create. yikes oWo
				{
					if (data["accountsLicenses"]["licenses"].is_null())
						Logger::GetInstance().Exception(L"Could not parse LadderPoint on Auth user = {0}", user->GetCharacterName());
					uint32 aux = data["accountsLicenses"]["licenses"].get<uint32>();
					for (int i = 0; i < 16; i++) {
						if (static_cast<bool>(aux & (1 << i)))
							user->m_CharacterLicenses.push_back(i);
					}
				}
				//StageLicense // What Stages the users is allowed to go and what AccessLevel the user has
				for (auto& x : data["stageLicenses"])
					user->m_StageLicenses.push_back(XRated::StageLicense(x["stageGroupHash"].get<uint32>(), x["accessLevel"].get<uint16>()));
				//Skilllicenses
				{
					for (auto& x : data["skillLicenses"]) {
						user->m_SkillLicenses.push_back(x["skillGrouphash"].get<uint32>());
					}
					Protocol::ListSkillLicenses sendPacket;
					sendPacket.listSkillLicenses = user->m_SkillLicenses;
					user->Send(sendPacket);
				}
				//Items
				{
					//Bags
					{
						//Check the reason why we have two tables doing the same damn thing.
						Protocol::BagStates sendPacket;
						//Bags Character
						for (auto& x : data["bags"])
							if (!x["isBank"].get<bool>())
								sendPacket.Bags.push_back(XRated::BagState(x["bagNumber"].get<int>(), x["expireDate"].get<std::string>()));
						//Bags Bank
						for (auto& x : data["bankBags"])
							sendPacket.BankBags.push_back(XRated::BagState(x["bagNumber"].get<int>(), x["expireDate"].get<std::string>()));

						user->Send(sendPacket);
					}
					//Items
					{
						//We should check weather the position where the item is at is valid.
						Protocol::ListItem sendPacket;
						for (auto& x : data["items"])
							sendPacket.listitem.push_back(
								XRated::ItemSlot(
									x["itemHash"].get<uint32>(),
									x["stackedCount"].get<uint16>(),
									XRated::InstanceEx(
										x["instance"].get<int64>(),
										x["itemExpire"].get<std::string>())
								));
						user->Send(sendPacket);
					}
				}
				//Skills // The ones that the user has levelup and has license basedon the class it's
				{
					for (auto& x : data["skills"]) {
						user->m_PlayerData.skills.push_back(XRated::Skill(x["skillGroupHash"].get<uint32>(), x["skillLevel"].get<uint8>()));
					}
					Protocol::ListSkill sendPacket;
					sendPacket.listskill = user->m_PlayerData.skills;
					user->Send(sendPacket);
				}
				//QuickSlots
				{
					Protocol::ListQuickSlot sendPacket;
					for (auto& x : data["quickSlots"]) {
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
						user->m_QuickSlot.SetSlot(tempSlot);
						sendPacket.quickslotlist.push_back(tempSlot);
					}
					user->m_QuickSlot.UpdateOriginData(); // What is this ugly shit?!
					user->Send(sendPacket);
				}
				//Pets
				{
					//PetItems
					{
						//We should check weather the position where the item is at is valid.
						Protocol::ListPetItem sendPacket;
						for (auto& x : data["petItems"])
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
						user->Send(sendPacket);
					}
					//PetTraining
					{
						Protocol::PetsCaredBySchool sendPacket;
						sendPacket.OwnerSerial = 0;
						for (auto& x : data["petTraining"]) {
							XRated::PetCaredBySchool petTraining;
							petTraining.PetItemSerial = x["petId"].get<int64>();
							petTraining.PetItemHash = x["itemHash"].get<uint32>();
							petTraining.PetItemCount = x["stackedCount"].get<uint16>();
							petTraining.PetItemInstanceEx = x["instance"].get<int64>();
							petTraining.ExpFactor = x["expFactor"].get<float>();
							petTraining.Start.Parse(x["startTime"].get<std::string>());
							petTraining.End.Parse(x["endTime"].get<std::string>());

							sendPacket.CaredPets.push_back(petTraining);
						}

						user->Send(sendPacket);
					}
					Protocol::PetInfo sendPacket;
					for (auto& x : data["pets"]) {
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
					}
					user->Send(sendPacket);
				}
				//After this is all done and good we have to send this user to the "Room" responsible for the stage it'll be joining. but for now since we dont have any room
				//anything really i'll just "fake it"
				//Load
				{
					Protocol::LoadEnd sendPacket;
					sendPacket.charName = user->GetCharacterName();
					sendPacket.progress = 0;
					user->Send(sendPacket);
				}
				//RoomJoined
				{
					//Stage
					{
						Protocol::Stage sendPacket;
						sendPacket.charactername = user->GetCharacterName();
						sendPacket.targetStage = user->m_CurrentStage;
						user->Send(sendPacket);
					}
					//QuestManager
					{
						Net::Api api("Quest/WorkingList");
						api << user->GetCharacterName();
						api.GetAsync(
							[&](const Net::Answer& result) {
								if (result.errorCode == 0) {

									Protocol::Quest::WorkingList sendPacket; // even with no quests it has to be sent.
									for (auto& x : result.resultObject) {
										XRated::Quest quest;
										quest.Id = x["questHash"].get<uint32>();
										quest.ExpiredDate.Parse(x["expiredDate"].get<std::string>());
										quest.CurrentState = x["currentState"].get<uint8>();
										quest.Params.push_back(x["param1"].get<uint32>());
										quest.Params.push_back(x["param2"].get<uint32>());
										quest.Params.push_back(x["param3"].get<uint32>());
									}
									user->Send(sendPacket);
								}
								else
									Logger::GetInstance().Error(L"Could not parse WorkingQuests for the user@{0}", user->GetCharacterName());
							});
					}
					//FamilyManager // Im ignoring this for now. Let's do it later.
					if (1 == 0) {
						Net::Api api("Family/JoinedInfo");
						api << user->GetCharacterName();
						api.GetAsync(
							[&](const Net::Answer& result) {
								if (result.errorCode == 0) {

									Protocol::Family::Info sendPacket;
									for (auto& x : result.resultObject) {
									}
									user->Send(sendPacket);
								}
								else
									Logger::GetInstance().Error(L"Could not parse FamilyInfo for the user@{0}", user->GetCharacterName());
							});
					}
					//Load
					{
						Protocol::LoadEnd sendPacket;
						sendPacket.charName = user->GetCharacterName();
						sendPacket.progress = 1;
						user->Send(sendPacket);
					}
				}
				//EndOfRoomJoined
				{
					Protocol::CreatePlayer createplayer;
					createplayer.playerserial = user->GetId() + 1000;
					createplayer.classtype = user->m_PlayerData.type;
					createplayer.charactername = user->GetCharacterName();//pPlayerData->BaseCharacter.BaseObject.Name;
					createplayer.level = user->m_PlayerData.level;//pPlayerData->BaseCharacter.Level;
					createplayer.pvpLevel = user->m_PlayerData.pvpLevel;//pPlayerData->PvpLevel;
					createplayer.warLevel = user->m_PlayerData.warLevel;//pPlayerData->WarLevel;
					createplayer.storedLevel = user->m_PlayerData.storedLevel;//pPlayerData->StoredLevel;
					createplayer.rebirthCount = user->m_PlayerData.rebirthCount;//pPlayerData->RebirthCount;
					createplayer.ladderPoint = user->m_PlayerData.ladderPoint;//pPlayerData->LadderPoint;
					createplayer.ladderMatchCount = user->m_PlayerData.ladderMatchCount;//pPlayerData->LadderMatchCount;
					createplayer.ladderWinCount = user->m_PlayerData.ladderWinCount;//pPlayerData->LadderWinCount;
					createplayer.ladderLoseCount = user->m_PlayerData.ladderLoseCount;//pPlayerData->LadderLoseCount;
					createplayer.achievementScore = user->m_PlayerData.achievementScore;//pPlayerData->achievementScore; // 3.1 by Robotex
					createplayer.addedSkillPointByRebirth = user->m_PlayerData.storedSkillPoint;//pPlayerData->StoredSkillPoint;
					createplayer.position = float3(1200, 0, 610);//pPlayerData->BaseCharacter.BaseObject.Position;
					createplayer.direction = float3(0, 0, -1);//pPlayerData->BaseCharacter.BaseObject.Direction;
					createplayer.hp = 1000;//pPlayerData->BaseCharacter.Hp;
					createplayer.team = 1;//pPlayerData->BaseCharacter.Team;

					createplayer.shopping = true;
					createplayer.stageLicenses = user->m_StageLicenses;

					createplayer.lives = static_cast<uint8>(user->m_PlayerData.life);
					createplayer.bonusLife = static_cast<uint8>(user->m_PlayerData.bonusLife);
					//createplayer.stateflags = pPlayerData->BaseCharacter.States;
					createplayer.lastRebirthDateTime = user->m_PlayerData.lastRebirthDateTime;//pPlayerData->LastRebirthDateTime;
					createplayer.partyChannelName = L"";//partyChannelName;
					createplayer.eventExpFactor = 1.0;
					user->Send(createplayer);
				}
				return true;
			}
		}
	}
}
