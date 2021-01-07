#include "Info.h"
#include <Core/Utils/ConfigReader.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			const bool xmlLoad = false;
			void LuniaDatabase::Init() {
				InfoCollections.Actions.Init(xmlLoad);
				//InfoCollections.Items.Init(xmlLoad);
				InfoCollections.Npcs.Init(xmlLoad);
				//InfoCollections.Miscs.Init(xmlLoad);
				//InfoCollections.Projectiles.Init(xmlLoad);
				InfoCollections.Skills.Init(xmlLoad);
				InfoCollections.Stages.Init(xmlLoad);
				InfoCollections.Statuses.Init(xmlLoad);
				InfoCollections.StateBundles.Init(xmlLoad);
				InfoCollections.GeneralInfos.Init(xmlLoad);
				InfoCollections.StageGroups.Init(xmlLoad);
				InfoCollections.StageGroups.SetStageInfo(&InfoCollections.Stages);
				InfoCollections.Quests.Init(xmlLoad);
				InfoCollections.StylePoints.Init(xmlLoad);
				InfoCollections.FishigInfos.Init(xmlLoad);
				InfoCollections.EnchantTables.Init(xmlLoad);
				InfoCollections.DailyItems.Init(xmlLoad);
				InfoCollections.StagePlayTimes.Init(xmlLoad);
				//InfoCollections.BossItems.Init(xmlLoad);
				//InfoCollections.BonusItems.Init(xmlLoad);
				//InfoCollections.BarterItems.Init(xmlLoad);
				InfoCollections.CashStamps.Init(xmlLoad);
				InfoCollections.EquipSet.Init(xmlLoad);
				InfoCollections.Pets.Init(xmlLoad);

				InfoCollections.LicenseProvider.Init(xmlLoad);
				InfoCollections.LuniaChipItems.Init(xmlLoad);
				InfoCollections.GuildInfos.Init(xmlLoad);
				//InfoCollections.ShopInfos.Init(xmlLoad);
				//InfoCollections.EventInfoManager.Init(xmlLoad);

				InfoCollections.UpgradeTables.Init(xmlLoad);

				InfoCollections.PvPRewardItemInfos.Init(xmlLoad);
				InfoCollections.PvPRankRewardItemInfos.Init(xmlLoad);
				InfoCollections.MailAwardItemInfos.Init(xmlLoad);
				InfoCollections.FamilyInfos.Init(xmlLoad);
				InfoCollections.NonPlayerModifierInfos.Init(xmlLoad);
				//InfoCollections.OpenMarketInfos.Init(xmlLoad);
				InfoCollections.BagInfos.Init(xmlLoad);
				InfoCollections.EpisodeSquareAdventageInfos.Init(xmlLoad);

				// EventQuest
				//InfoCollections.EventQuests.Init();

				// EventQuest PostLoadProcess
				{
					//InfoCollections.Quests.Add(InfoCollections.EventQuests.GetQuests());
					//InfoCollections.Quests.AddPreiodQuests( InfoCollections.EventQuests.GetPeriodQuestList() );
				}

				// EventUnidentifiedItem
				//InfoCollections.EventUnidentifiedItemInfos.Init(xmlLoad);
				// EventUnidentifiedItem PostLoadProcess
				{
					std::map<uint32, UnidentifiedItemInfo> x;
					//InfoCollections.EventUnidentifiedItemInfos.GetUnidentifiedItemList( unidentifiedItemInfos );
					//InfoCollections.Items.AddUnidentifiedItemList(unidentifiedItemInfos);
				}

				InfoCollections.EventNonPlayerItemLists.Init();
			}

			LuniaDatabase& LuniaDatabase::GetInstance()
			{
				static LuniaDatabase m_instance;
				return m_instance;
			}

			LuniaDatabase& DatabaseInstance() {
				return LuniaDatabase::GetInstance();
			}
		}
	}
}