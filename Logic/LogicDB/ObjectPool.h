#pragma once
#include "ActionDB.h"
#include "PoolTemplate.h"
#include "../Object/NonPlayer.h"
#include "../Object/PlayerPvpItem.h"
#include "../Object/TombStone.h"
#include "../Object/Misc/Misc.h"

namespace Lunia { namespace XRated { namespace Logic {

	class PlayerPvpItemPool : public PoolTemplate<PlayerPvpItem> {
	protected :
		PlayerPvpItem* Create(int type);
	};

	class PlayerPool : public PoolTemplate<Player> {
	protected :
		Player* Create(int type);
	};

	class NPCPool : public PoolTemplate<NonPlayer> {
	protected :
		NonPlayer* Create(int type);
	};

	class MISCPool : public PoolTemplate<Misc> {
	protected :
		Misc* Create(int type=0);
	};

	class ObjectPool {
	public :
		const static int MAXPLAYERCNT = 200;	//플레이어 수에 따른 보정수치. 배열의 갯수를 결정하는데 사용된다.

	private :
		PlayerPvpItemPool playerPvpItemPool;
		PlayerPool playerPool;
		NPCPool npcPool;
		MISCPool miscPool;

		ActionDB* actionDb;

		//Misc* CreateObject(IGameStateDatabase* db, const std::wstring& id );

	public :
		ObjectPool();
		void Init(ActionDB* db);

		Player*		CreatePlayerPvpItem(IGameStateDatabase* db, const ILogic::PlayerInitialData& data);
		Player*		CreatePlayer(IGameStateDatabase* db, const ILogic::PlayerInitialData& data);
		NonPlayer*	CreateNPC(IGameStateDatabase* db, Database::Info::NonPlayerInfo* info, uint8 team, int playerCnt, const Database::Info::StageInfo* stageInfo);
		Misc*		CreateMisc(IGameStateDatabase* db, uint32 id);
		TombStone*	CreateTombstone(IGameStateDatabase* db, const ObjectData& objData);

		bool IsAffectedByPlayerCount(IGameStateDatabase* db, Database::Info::NonPlayerInfo* info);

		template<class T> void Release(T* obj);

	public : //Helper
		void GetPoolInfo(std::vector<std::vector<int> >& l) const;

	private:
        ObjectPool(const ObjectPool&); // should be private: because of members which is unable to be copied
	};

}	}	}
