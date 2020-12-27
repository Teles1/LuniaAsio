#include "LogicDB.h"
#include <Core/DeltaTimer.h>


using namespace Lunia::XRated::Database;
using namespace Lunia::XRated::Database::Info;

namespace Lunia { namespace XRated { namespace Logic {

	LogicDB::LogicDB()
	{
		Init();
	}

	NonPlayer* LogicDB::CreateNPC(IGameStateDatabase* db, NonPlayerInfo* info, uint8 team, int partyCnt, const Database::Info::StageInfo* stageInfo)
	{
		NonPlayer* npc = objectPool.CreateNPC(db, info, team, partyCnt, stageInfo);
		return npc;
	}

	Misc* LogicDB::CreateMisc(IGameStateDatabase* db, uint32 id)
	{
		return objectPool.CreateMisc(db, id);
	}

	Misc* LogicDB::CreateMisc(IGameStateDatabase* db, uint32 id, float duration)
	{
		Misc* obj = objectPool.CreateMisc(db, id);
		obj->SetDuration(duration);
		return obj;
	}

	uint32 LogicDB::CreateNPC(IGameStateDatabase* db, uint32 id, const float3& pos, const float3& dir, uint8 team
						, StageInfo::Npc::IdleStateType idleType, int pathGroupIndex, int defenceLocation, Serial owner, StageInfo::Npc::TransferType transferType, int partyCnt)
	{
		return objectThread.CreateNPC(db, id, pos, dir, team, idleType, pathGroupIndex, defenceLocation, owner, transferType, partyCnt);
	}

	uint32 LogicDB::CreateMisc(IGameStateDatabase* db, uint32 id, const float3& pos, const float3& dir, uint8 team, float duration, float hp)
	{
		return objectThread.CreateMisc(db, id, pos, dir, team, duration, hp);
	}

	TombStone* LogicDB::CreateTombstone(IGameStateDatabase* db, const ObjectData& objData)
	{
		return objectPool.CreateTombstone(db, objData);
	}

	Player*	LogicDB::CreatePlayerPvpItem(IGameStateDatabase* db, const ILogic::PlayerInitialData& data)
	{
		return objectPool.CreatePlayerPvpItem(db, data);
	}

	Player* LogicDB::CreatePlayer(IGameStateDatabase* db, const ILogic::PlayerInitialData& data)
	{
		return objectPool.CreatePlayer(db, data);
	}

	void LogicDB::ReleasePlayerPvpItem(Player* obj)
	{
		objectPool.Release((PlayerPvpItem*)obj);
	}

	void LogicDB::ReleasePlayer(Player* obj)
	{
		objectPool.Release(obj);
	}

	void LogicDB::ReleaseNPC(NonPlayer* obj)
	{
		objectPool.Release(obj);
	}

	void LogicDB::ReleaseMisc(Misc* obj)
	{
		objectPool.Release(obj);
	}

	void LogicDB::Init()
	{
		actionDB.Init();
		objectPool.Init( &actionDB );
		stateDB.Init();
	}

}	}	}
