#pragma once
#include "StateDB.h"
#include "ObjectThread.h"

//Npc
//remove Action binding
//State runtime create

namespace Lunia { namespace XRated { namespace Logic {

	class LogicDB {
		ActionDB actionDB;
		ObjectPool objectPool;
		ObjectLoadThread objectThread;

	public :
		StateDB stateDB;

	public :
		LogicDB();
		~LogicDB() {}

		void Init();

		Player*		CreatePlayerPvpItem(IGameStateDatabase* db, const ILogic::PlayerInitialData& data);
		Player*		CreatePlayer(IGameStateDatabase* db, const ILogic::PlayerInitialData& data);
		//Old functions
		NonPlayer*	CreateNPC(IGameStateDatabase* db, Database::Info::NonPlayerInfo* info, uint8 team, int partyCnt, const Database::Info::StageInfo* stageInfo);
		Misc*		CreateMisc(IGameStateDatabase* db, uint32 id);	//일반 object.
		Misc*		CreateMisc(IGameStateDatabase* db, uint32 id, float duration); //시간이 지나면 사라지는 놈들.
		TombStone*	CreateTombstone(IGameStateDatabase* db, const ObjectData& objData);
		//New functions
		uint32		CreateNPC(IGameStateDatabase* db, uint32 id, const float3& pos, const float3& dir, uint8 team
							, Database::Info::StageInfo::Npc::IdleStateType idleType, int pathGroupIndex, int defenceLocation, Serial owner
							, Database::Info::StageInfo::Npc::TransferType transferType, int partyCnt);
		uint32		CreateMisc(IGameStateDatabase* db, uint32 id, const float3& pos, const float3& dir, uint8 team, float duration, float hp);

		void ReleasePlayerPvpItem(Player* obj);
		void ReleasePlayer(Player* obj);
		void ReleaseNPC(NonPlayer* obj);
		void ReleaseMisc(Misc* obj);

		ActionDB& GetActionDB() { return actionDB; }

	private :
		LogicDB(const LogicDB&);
		void operator= (const LogicDB&);
	};

	static inline LogicDB& LogicDBInstance()
	{
		static LogicDB m_instance;
		return m_instance;
	}

}	}	}
