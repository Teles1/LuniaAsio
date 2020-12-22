#include <StageServer/User/UserManager.h>
#include <Info/Info.h>
#include <Info/Info/StageGroup/StageGroup.h>
namespace Lunia {
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
		void UserManager::Auth(UserSharedPtr& user, const json& data) {
			// CharacterInfos
			{
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
			for(auto& x : data["stageLicenses"])
				user->m_StageLicenses.push_back( XRated::StageLicense( x["stageGroupHash"].get<uint32>(), x["accessLevel"].get<uint16>() ) );
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
				//Tobinhas
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
			//Pet
			{

			}
		}
	}
}
