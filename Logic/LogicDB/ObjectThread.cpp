#include "ObjectThread.h"
#include "LogicDB.h"

using namespace Lunia::XRated::Database;
using namespace Lunia::XRated::Database::Info;

namespace Lunia { namespace XRated {	namespace Logic {


	uint32 ObjectLoadThread::CreateNPC(IGameStateDatabase* db, uint32 id, const float3& pos, const float3& dir, uint8 team
							, Database::Info::StageInfo::Npc::IdleStateType idleType, int pathGroupIndex, int defenceLocation, Serial owner
							, Database::Info::StageInfo::Npc::TransferType transferType, int partyCnt)
	{
		Serial s = db->GetNextObjectSerial();
		AutoLock locker(csNPC);
		npcBuffer.push_back( NPC(db, db->GetStageInfo(), s, id, pos, dir, team, idleType, pathGroupIndex, defenceLocation, owner, transferType, partyCnt) );
		return s;
	}

	uint32 ObjectLoadThread::CreateMisc(IGameStateDatabase* db, uint32 id, const float3& pos, const float3& dir, uint8 team, float dur, float hp)
	{
		Serial s = db->GetNextObjectSerial();
		AutoLock locker(csMISC);
		miscBuffer.push_back( MISC(db, s, id, pos, dir, team, dur, hp) );
		return s;
	}

	void ObjectLoadThread::ProcessNPCs(NPCList& l)
	{
		NonPlayer* obj=NULL;
		NonPlayerInfo* npcInfo;
		for ( NPCList::iterator i = l.begin() ; i != l.end(); ++i) {
			NPC& item = (*i);
			npcInfo = DatabaseInstance().InfoCollections.Npcs.Retrieve(item.id);
			if (!npcInfo) {
				Logger::GetInstance().Error( L"[ObjectLoadThread::ProcessNPCs] npc [{0}] info not founded in database.", item.id );
				continue;
			}
			obj= LogicDBInstance().CreateNPC(item.stageData, npcInfo, item.team, item.partyCnt, item.stageInfo);
			if ( !obj ) {
				continue;
			}

			obj->SetSerial( item.serial );
			obj->SetTeam( item.team );

			//StageInfo* stageInfo = item.stageData->GetStageInfo();
			switch ( item.idleType ) {
			case StageInfo::Npc::IdleStateType::STAND :
				obj->SetAIInfo(StageInfo::Npc::IdleStateType::STAND, NULL, float4(0,0,0,0), item.position, item.transferType, item.owner);
				break;
			case StageInfo::Npc::IdleStateType::PATHFINDING :
				{
					if (!item.stageInfo || (int)item.stageInfo->PathGroups.size() <= item.pathGroupIndex || item.pathGroupIndex < 0) {
						Logger::GetInstance().Exception(L"[ObjectLoadThread::ProcessNPCs] Wrong stage info. [{0}/{1}]", item.stageInfo->Id.c_str(), item.pathGroupIndex);
						LogicDBInstance().ReleaseNPC(obj);
						continue;
					} else
						obj->SetAIInfo(StageInfo::Npc::IdleStateType::PATHFINDING, &item.stageInfo->PathGroups[item.pathGroupIndex], float4(0,0,0,0), item.position, item.transferType, item.owner);
				}
				break;
			case StageInfo::Npc::IdleStateType::AREADEFENCE :
				if (!item.stageInfo || (int)item.stageInfo->Locations.size() <= item.defenceLocation || item.defenceLocation < 0) {
					Logger::GetInstance().Exception(L"[ObjectLoadThread::ProcessNPCs] Wrong location index. [{0}/{1}]", item.stageInfo->Id.c_str(), item.defenceLocation);
					LogicDBInstance().ReleaseNPC(obj);
					continue;
				} else
					obj->SetAIInfo(StageInfo::Npc::IdleStateType::AREADEFENCE, NULL, item.stageInfo->Locations[item.defenceLocation], item.position, item.transferType, item.owner);
				break;
			case StageInfo::Npc::IdleStateType::DOLL :
			case StageInfo::Npc::IdleStateType::SOLDIEROFFORTUNE :
			case StageInfo::Npc::IdleStateType::PET :
				if (!item.stageInfo) {
					LogicDBInstance().ReleaseNPC(obj);
					continue;
				} else {
					obj->SetAIInfo(item.idleType, NULL, float4(0,0,0,0), item.position, item.transferType, item.owner);
				}
				break;
			default :
				Logger::GetInstance().Info( L"[ObjectLoadThread::ProcessNPCs] [{0}] Unknown IdleType [{1}]", item.id, (int)item.idleType);
				obj->SetAIInfo(StageInfo::Npc::IdleStateType::STAND, NULL, float4(0,0,0,0), item.position, item.transferType, item.owner);
			}

			obj->Initialize(rInt);
			obj->SetDirection(item.direction);
			if ( !item.stageData->AddToNPCBuffer(item.stageCnt, obj) )
				LogicDBInstance().ReleaseNPC(obj);

			Sleep(10);
		}
	}

	void ObjectLoadThread::ProcessMISCs(MISCList& l)
	{
		Misc* obj;
		for ( MISCList::iterator i = l.begin() ; i != l.end(); ++i) {
			MISC& item = (*i);
			obj = LogicDBInstance().CreateMisc(item.stageData, item.id);
			if ( !obj || item.position.x < 0 || item.position.z < 0 ) {
			//if ( !obj ) {
				Logger::GetInstance().Error( L"[ObjectLoadThread::ProcessMISCs] fail to create misc.[{0}, {1}, ({2},{3})]",  item.id, item.team, item.position.x, item.position.z );
				continue;
			}

			obj->SetSerial( item.serial );
			obj->SetTeam(item.team);
			obj->InitPosition( item.position );
			obj->SetDirection(item.direction);
			if ( item.duration )
				obj->SetDuration(item.duration);
			if ( item.hp )
				obj->SetHP(item.hp);
			if ( !item.stageData->AddToObjectBuffer(item.stageCnt, obj) )
				LogicDBInstance().ReleaseMisc(obj);
			Sleep(10);
		}
	}

	void ObjectLoadThread::Run()
	{
		Sleep(100);
		NPCList npcs;
		{
			AutoLock locker(csNPC);
			npcs.swap(npcBuffer);
		}
		ProcessNPCs(npcs);

		MISCList miscs;
		{
			AutoLock locker(csMISC);
			miscs.swap(miscBuffer);
		}
		ProcessMISCs(miscs);
	}

}	}	}
