#pragma once
#include <Core/Runnable.h>
#include "../GameState.h"

#include "ObjectPool.h"


namespace Lunia { namespace XRated {	namespace Logic {

	class ObjectLoadThread : public Lunia::Runnable {
	public :
		struct NPC {
			IGameStateDatabase* stageData;
			Serial serial;
			uint32 stageCnt;
			uint32 id;
			float3 position;
			float3 direction;
			uint8 team;
			Database::Info::StageInfo::Npc::IdleStateType idleType;
            Database::Info::StageInfo::Npc::TransferType transferType;
			int pathGroupIndex;
			int defenceLocation;
			Serial owner;
			int partyCnt;
			Database::Info::StageInfo* stageInfo;
			NPC(IGameStateDatabase* db, Database::Info::StageInfo* sInfo, Serial s, uint32 npcid, const float3& pos, const float3& dir, uint8 t
				, Database::Info::StageInfo::Npc::IdleStateType idle, int pIndex, int defLocation, Serial o, Database::Info::StageInfo::Npc::TransferType transfer, int pCnt)
					: stageData(db), stageInfo(sInfo), serial(s), stageCnt(db->GetStageCnt()), id(npcid), position(pos), direction(dir), team(t)
					, idleType(idle), pathGroupIndex(pIndex), defenceLocation(defLocation), owner(o), transferType(transfer), partyCnt(pCnt) {}
		};

		struct MISC {
			IGameStateDatabase* stageData;
			Serial serial;
			uint32 stageCnt; //방에서 몇번째 스테이지인지를 구분. npc 생성이 logic으로 부터 요청되고 처리 되기 전에 다른 스테이지로 초기화 되는 경우를 막기위해서.
			uint32 id;
			float3 position;
			float3 direction;
			uint8 team;
			float duration;
			float hp;
			MISC(IGameStateDatabase* db, Serial s, uint32 miscid, const float3& pos, const float3& dir, uint8 t, float dur, float h)
				: stageData(db), serial(s), stageCnt(db->GetStageCnt()), id(miscid), position(pos), direction(dir), team(t), duration(dur), hp(h) {}
		};

		typedef std::list<NPC> NPCList;
		typedef std::list<MISC> MISCList;

	protected :
		NPCList npcBuffer;
		MISCList miscBuffer;
		
		std::mutex csNPC, csMISC;
		Lunia::Math::RandomInt rInt; 

	public :
		ObjectLoadThread() : Runnable(L"ObjectLoadThread") { StartThread(); }
		~ObjectLoadThread() { StopThread(); }

		uint32		CreateNPC(IGameStateDatabase* db, uint32 id, const float3& pos, const float3& dir, uint8 team
							, Database::Info::StageInfo::Npc::IdleStateType idleType, int pathGroupIndex, int defenceLocation, Serial owner
							, Database::Info::StageInfo::Npc::TransferType transferType, int partyCnt);
		uint32		CreateMisc(IGameStateDatabase* db, uint32 id, const float3& pos, const float3& dir, uint8 team, float dur, float hp);

	protected :
		void ProcessNPCs(NPCList& l);
		void ProcessMISCs(MISCList& l);

	public : //Lunia::Runnable implement
		void Run();
	};

}	}	}



