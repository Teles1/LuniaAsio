#include <Core/Resource/Resource.h>
#include "ObjectPool.h"
#include "../Object/NPCs/Boss/StoneGolem.h"
#include "../Object/NPCs/Orcs/OrcWarriorBase.h"
#include "../Object/NPCs/Orcs/OrcThrowerBase.h"
#include "../Object/NPCs/Instruments/Catapult.h"
#include "../Object/NPCs/Instruments/Tower.h"
#include "../Object/NPCs/Monster/Mimic.h"
#include "../Object/NPCs/Monster/Slime.h"
#include "../Object/NPCs/Animal/WolfBoss.h"
#include "../Object/NPCs/DnF/DnFNPC.h"
#include "../Object/NPCs/Allied/BradFord.h"
#include "../Object/NPCs/NPCChecker.h"
#include "../Object/Misc/Misc.h"
#include <Info/Info/Database.h>

#include <Core/DeltaTimer.h>

using namespace Lunia::XRated::Database;

namespace Lunia { namespace XRated { namespace Logic {

	PlayerPvpItem* PlayerPvpItemPool::Create(int type)
	{
		Constants::ClassType classType = (Constants::ClassType)type;
		PlayerPvpItem* player = new PlayerPvpItem( classType );
		return player;
	}

	Player* PlayerPool::Create(int type)
	{
		Constants::ClassType classType = (Constants::ClassType)type;
		Player* player = new Player( classType );
		return player;
	}

	NonPlayer* NPCPool::Create(int type)
	{
		switch ( (Info::NonPlayerInfo::NPCType::ClassType)type) {
		case Info::NonPlayerInfo::NPCType::Warrior :
			return new OrcWarriorBase();
		case Info::NonPlayerInfo::NPCType::Thrower :
			return new OrcThrowerBase();
		case Info::NonPlayerInfo::NPCType::StoneGolem :
			return new StoneGolem();
		case Info::NonPlayerInfo::NPCType::Catapult :
			return new Catapult();
		case Info::NonPlayerInfo::NPCType::Mimic :
			return new Mimic();
		case Info::NonPlayerInfo::NPCType::WolfBoss :
			return new WolfBoss();
		case Info::NonPlayerInfo::NPCType::Wolf :
			return new Wolf();
		case Info::NonPlayerInfo::NPCType::SlimeBoss :
			return new SlimeBoss();
		case Info::NonPlayerInfo::NPCType::Tower :
			return new Tower();
		case Info::NonPlayerInfo::NPCType::DnD :
			return new DnFNPC();
		case Info::NonPlayerInfo::NPCType::BradFord :
			return new BradFord();
		case Info::NonPlayerInfo::NPCType::Checker :
			return new NPCChecker();
		case Info::NonPlayerInfo::NPCType::Default :
		default :
			return new NonPlayer();
		}
	}

	Misc* MISCPool::Create(int /*type*/)
	{
		return new Misc();
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	//public methods
	ObjectPool::ObjectPool()
	{
	}

	void ObjectPool::Init(ActionDB* db)
	{
		actionDb = db;

		//초기치에 따라서 풀을 생성해준다.
		playerPvpItemPool.Init( DatabaseInstance().InfoCollections.GeneralInfos.Retrieve().PoolSize[0] );
		playerPool.Init( DatabaseInstance().InfoCollections.GeneralInfos.Retrieve().PoolSize[0] );
		npcPool.Init( DatabaseInstance().InfoCollections.GeneralInfos.Retrieve().PoolSize[1] );

		std::vector<int> l;
		l.push_back(3500);
		miscPool.Init( l );
	}

	Player*	ObjectPool::CreatePlayerPvpItem(IGameStateDatabase* db, const ILogic::PlayerInitialData& data)
	{
		Player* player = playerPvpItemPool.Get((int)data.type);

#ifdef _SINGLE
		player->LoadActions(data.name, L"Stand", actionDb->GetActionList( Constants::GetClassStr(data.type), Constants::GetClassStr(data.type)) );
#else
		uint32 hash = Constants::GetClassHash(data.type);
		player->LoadActions(data.name, L"Stand", actionDb->GetActionList( hash, hash));
#endif
		player->Initialize(db, data.name, data.level, data.xp, data.money, data.bankMoney, data.storedLevel, data.rebirthCount
			, data.storedSkillPoint, data.ladderPoint, data.ladderMatchCount, data.ladderWinCount, data.ladderLoseCount, data.achievementScore, data.life, data.bonusLife			
			, data.skillpoint, data.addedSkillPointPlus, data.team, data.Factors.ExpFactor, data.Factors.MaxHp, data.Factors.MaxMp, data.characterStateFlags
			, data.pets, data.petCared, data.lastRebirthDateTime);
		return player;
	}

	Player* ObjectPool::CreatePlayer(IGameStateDatabase* db, const ILogic::PlayerInitialData& data)
	{
		Player* player = playerPool.Get((int)data.type);

#ifdef _SINGLE
		player->LoadActions( data.name, L"Stand", actionDb->GetActionList( Constants::GetClassStr(data.type), Constants::GetClassStr(data.type)) );
#else
		uint32 hash = Constants::GetClassHash(data.type);
		player->LoadActions(data.name, L"Stand", actionDb->GetActionList( hash, hash));
#endif
		uint16 life = data.life;
		Database::Info::StageGroup* stageGroup = db->GetStageGroupInfo();
		if( stageGroup != NULL && stageGroup->HasDifficulty == true )
		{
			uint8 difficulty = db->GetDifficulty();
			if( stageGroup->Difficulties.size() > difficulty )
			{
				life -= ( 2 - stageGroup->Difficulties[ difficulty ].Life );
			}
		}

		player->Initialize(db, data.name, data.level, data.xp, data.money, data.bankMoney, data.storedLevel, data.rebirthCount
							, data.storedSkillPoint, data.ladderPoint , data.ladderMatchCount , data.ladderWinCount, data.ladderLoseCount, data.achievementScore
							, life, data.bonusLife, data.skillpoint, data.addedSkillPointPlus, data.team, data.Factors.ExpFactor, data.Factors.MaxHp, data.Factors.MaxMp, data.characterStateFlags
							, data.pets, data.petCared, data.lastRebirthDateTime);
		return player;
	}

	NonPlayer* ObjectPool::CreateNPC(IGameStateDatabase* db, Database::Info::NonPlayerInfo* info, uint8 team, int playerCnt, const Database::Info::StageInfo* stageInfo)
	{
		bool isAffectedbyPlayerCnt = IsAffectedByPlayerCount(db, info);

		if(isAffectedbyPlayerCnt == true && playerCnt > MAXPLAYERCNT) {
			Logger::GetInstance().Error( L"[ObjectPool::CreateNPC] out of range. (player count : {0})", playerCnt );
			playerCnt = MAXPLAYERCNT;
		}
		NonPlayer* npc = npcPool.Get((int)info->Class);
		if ( !npc ) {
			Logger::GetInstance().Error( L"[ObjectPool::CreateNPC] unable to get npc({0}) instance from npc pool", info->Name.c_str() );
			return NULL;
		}
		
		unsigned int nonPlayerLv = info->Lv;
		const Database::Info::NonplayerLvModifierInfo::ModifierInfo* lvModifierInfo = Database::DatabaseInstance().InfoCollections.NonPlayerModifierInfos.GetLv(stageInfo->RaidStageDifficulty, info->Lv);
		if (lvModifierInfo)
		{
			npc->ModifyNpcInfoByRaidStageDiffy(lvModifierInfo->Lv, lvModifierInfo->BasicStat, lvModifierInfo->Exp);
			nonPlayerLv = lvModifierInfo->Lv;
		}
		else
		{
			npc->ModifyNpcInfoByRaidStageDiffy(info->Lv, info->BasicStat, info->Exp);
		}

#ifdef _SINGLE
		npc->LoadActions(info->Name, info->StartAction, actionDb->GetActionList( info->TemplateName, info->ActionListName ) );
#else
		npc->LoadActions(info->Name, info->StartAction, actionDb->GetActionList( info->HashcodeTemplateName, info->HashcodeActionListName ) );
#endif
		npc->LoadStatus( db, info );

		if(isAffectedbyPlayerCnt == true)
		{
			npc->SetPartyCntWhenCreated(playerCnt);

			if (!playerCnt)
			{
				const Database::Info::NonPlayerModifierInfo::ModifierInfo* modifierInfo = Database::DatabaseInstance().InfoCollections.NonPlayerModifierInfos.GetModifierInfo(npc->GetNPCInfo()->NpcType, 1, nonPlayerLv);
				if (modifierInfo)
				{
					npc->ModifyNpcInfoByPlayerCnt(modifierInfo->HpMultiply, modifierInfo->XpMultiply, modifierInfo->AtkMultiply, modifierInfo->HitIgnoreMultiply);
				}
			}
			else
			{
				const Database::Info::NonPlayerModifierInfo::ModifierInfo* modifierInfo = Database::DatabaseInstance().InfoCollections.NonPlayerModifierInfos.GetModifierInfo(npc->GetNPCInfo()->NpcType, playerCnt, nonPlayerLv);
				if (modifierInfo) 
				{
					npc->ModifyNpcInfoByPlayerCnt(modifierInfo->HpMultiply, modifierInfo->XpMultiply, modifierInfo->AtkMultiply, modifierInfo->HitIgnoreMultiply);
				}
			}
		}

		if( db->IsPlayerTeam( team ) == false )
		{
			Database::Info::StageGroup* stageGroup = db->GetStageGroupInfo();
			if( stageGroup != NULL && stageGroup->HasDifficulty == true )
			{
				uint8 difficulty = db->GetDifficulty();
				if( stageGroup->Difficulties.size() > difficulty )
				{
					Database::Info::DifficultyInfo difficultyInfo = stageGroup->Difficulties[ difficulty ];
					npc->ModifyNpcInfo( difficultyInfo.HpFactor, difficultyInfo.XpFactor, difficultyInfo.IntFactor, difficultyInfo.DmgFactor, difficultyInfo.UpdateTick, difficultyInfo.UpdateFightTick );
				}
			}
		}

		return npc;
	}

	Misc* ObjectPool::CreateMisc(IGameStateDatabase* db, uint32 id)
	{
		Misc* obj = miscPool.Get(0);
		if ( !obj )
			return NULL;
		obj->Initialize(db);
		if ( !obj->LoadStatus(id) )
			return NULL;
		return obj;
	}

	TombStone* ObjectPool::CreateTombstone(IGameStateDatabase* db, const ObjectData& objData)
	{
		TombStone* tomb = new TombStone(objData.Name.c_str() /*+ L"Tomb"*/, objData.GameObjectSerial);
		tomb->Initialize(db);
		tomb->InitPosition( float3(objData.Position.x, 0, objData.Position.z) );
		tomb->SetRadius( objData.Radius );
		return tomb;
	}

	bool ObjectPool::IsAffectedByPlayerCount(IGameStateDatabase* db, Database::Info::NonPlayerInfo* info)
	{
		const Info::Pet* petInfo = DatabaseInstance().InfoCollections.Pets.GetPet(info->Name);

		if(petInfo == NULL)
			return true;

		return false;
	}

	template<> void ObjectPool::Release(PlayerPvpItem* obj)
	{
		playerPvpItemPool.Release(obj);
	}

	template<> void ObjectPool::Release(Player* obj)
	{
		playerPool.Release(obj);
	}

	template<> void ObjectPool::Release(NonPlayer* obj)
	{
		npcPool.Release(obj);
	}

	template<> void ObjectPool::Release(Misc* obj)
	{
		miscPool.Release(obj);
	}

	void ObjectPool::GetPoolInfo(std::vector<std::vector<int> >& l) const
	{
		if (l.empty()==false)
			Logger::GetInstance().Exception(L"uncleaned result (size:{0})", l.size());

		l.push_back( std::vector<int>() );
		playerPool.GetAvailableSize( l[0] );
		l.push_back( std::vector<int>() );
		npcPool.GetAvailableSize( l[1] );
	}

}	}	}





