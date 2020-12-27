#pragma once
namespace Lunia { namespace XRated { namespace Logic {
	class Player;

	struct StatusCharacter
	{
		Database::Info::StatusInfo* info;
		Database::Info::StatusInfoManager::StatusGroup* statusGroup;

		float hitGauge;
		float regenHp, regenMp;
		StatusCharacter() {}
		void operator= (StatusCharacter&) {}
	};

	struct StatusNonPlayer
	{
		std::map<Serial, float> hateList;
		Serial target;
		float tauntDuration;
		float hpRate;
		uint32 xp;
		unsigned int Lv; 
		std::wstring BasicStat;

		Database::Info::StageInfo::Npc::IdleStateType idleType;
		Database::Info::PathGroup* paths;
		int pathCnt;
		float4 area;
		Serial familiar;
		Player* familiarPtr;
		Database::Info::StageInfo::Npc::TransferType transType;

		float sight, attackRange;

		Database::Info::StatusInfo* info;
		Database::Info::StatusInfoManager::StatusGroup* statusGroup;
		Database::Info::NonPlayerInfo* npcInfo;
		Database::Info::NPCInfoManager::BasicResist::Resist* raceResist;

		void Init(Database::Info::NonPlayerInfo* i) {
			npcInfo = i;
			raceResist = Database::DatabaseInstance().InfoCollections.Npcs.Retrieve(npcInfo->Race);

			//statusGroup = Database::DatabaseInstance().InfoCollections.Statuses.Retrieve(npcInfo->BasicStat);
			//info = statusGroup->statusList.at(npcInfo->Lv-1);
			statusGroup = Database::DatabaseInstance().InfoCollections.Statuses.Retrieve(BasicStat);
			info = statusGroup->statusList.at(Lv-1);

			hateList.clear();
			tauntDuration = 0;
			hpRate = 1;
			target = 0;
			sight = npcInfo->Sight;
			attackRange = npcInfo->AttackRange;
			familiar = 0;
			familiarPtr = NULL;
		}
	};

}	}	}
