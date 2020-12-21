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
		void UserManager::Auth(UserSharedPtr& user, json data) {
			if (UserRegistry::GetInstance().GetUserByUserId(user->GetId())) {
				// CharacterInfos
				{
					if (user->GetCharacterName() != StringUtil::ToUnicode(data["character"]["characterName"].get<std::string>())) {
						Logger::GetInstance().Exception("UserManager::Auth() GetCharacterName != CharacterName from page");
						throw "UserManager::Auth() GetCharacterName != CharacterName from page";
					}

					user->m_PlayerData.team = user->m_TeamNumber;
					user->m_PlayerData.name = user->GetCharacterName();
					user->m_PlayerData.type = (XRated::Constants::ClassType)data["character"]["classNumber"].get<int>();
					user->m_PlayerData.level = data["character"]["stageLevel"].get<uint16>();
					user->m_PlayerData.xp = data["character"]["stageExp"].get<int>();
					user->m_PlayerData.pvpLevel = data["character"]["pvpLevel"].get<int>();
					user->m_PlayerData.pvpXp = data["character"]["pvpExp"].get<int>();
					user->m_PlayerData.warLevel = data["character"]["warLevel"].get<int>();
					user->m_PlayerData.warXp = data["character"]["warExp"].get<int>();
					user->m_PlayerData.money = data["character"]["gameMoney"].get<int>();
					user->m_PlayerData.bankMoney = data["character"]["bankMoney"].get<int>();
					if(!data["character"]["addedSkillPoint"].is_null())
						user->m_PlayerData.skillpoint = data["character"]["addedSkillPoint"]["skillPoint"].get<int>();

					XRated::Database::Info::StageGroup* stageGroup;
					stageGroup = XRated::Database::DatabaseInstance().InfoCollections.StageGroups.Retrieve(user->m_CurrentStage.StageGroupHash);
					XRated::Database::DatabaseInstance().InfoCollections.StageGroups.Retrieve(user->m_CurrentStage.StageGroupHash);
					if (stageGroup == NULL){
						Logger::GetInstance().Error("UserManager::Auth()- stageGroup({0}) == NULL", user->m_CurrentStage.StageGroupHash);
						return;
					}
					if (stageGroup->Stages.size() <= user->m_CurrentStage.Level)
					{
						Logger::GetInstance().Exception("UserManager::Auth() - can not find stageData,  StageGroupHash({0}), AccessLevel({1})", user->m_CurrentStage.StageGroupHash, user->m_CurrentStage.Level);
						return ;
					}
					user->m_CurrentStageHash = stageGroup->StageHashes[user->m_CurrentStage.Level];
					const XRated::Database::Info::StageInfo* stageInfo = stageGroup->Stages.at(user->m_CurrentStage.Level);
					if (!stageInfo || !stageInfo->isOpened)
					{
						Logger::GetInstance().Exception("UserManager::Auth() - closed Stage({0}, {1}) == NULL", user->m_CurrentStage.StageGroupHash, user->m_CurrentStage.Level);
						return ;
					}

					user->m_CharacterStateFlags = data["character"]["instantStateFlag"].get<int>();
					user->m_PlayerData.characterStateFlags = user->m_CharacterStateFlags;
				}
				// CharacterLicenses
				{

				}
				// Licenses
				{

				}
				//SkillLicenses
				{

				}
				//Skills
				{

				}
				//QuickSlots
				{

				}
				//Pets
				{

				}
			}
		}
	}
}
