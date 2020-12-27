#include "ScriptEp1Stage1.h"
#include "AllM/Debugging/Debugging.h"

using namespace AllM::XRated::Database::Info;

namespace AllM { namespace XRated {	namespace Logic {

	void ScriptEp1Stage1::Update(float dt)
	{
		if (elapsedTime == 0) {
			//첫 호출
			std::wstring text(L"북쪽으로 전진하여 수정을 찾아라");
			stage->DisplayText(3, text );

			//stage->TimerRun(1, 5);

			stage->CreateObject(L"Stone1", float2(385.123077f, 1462.200073f), 100);
			stage->CreateObject(L"Stone1", float2(393.922943f, 1462.200073f), 100);
			stage->CreateObject(L"Stone1", float2(401.522827f, 1462.200073f), 100);
			stage->CreateObject(L"Stone1", float2(411.922668f, 1462.200073f), 100);
		}
		elapsedTime += dt;
	}


	////////////////////////////////////////////////////////////////////////////////////////
	//IStageScript::IStageEventListener implementation
	/* general events */
	void ScriptEp1Stage1::ValueChanged(const std::wstring& /*key*/, int32 /*oldValue*/, int32& /*newVlaue*/)
	{
	}

	void ScriptEp1Stage1::TimerAlarmed(int timer, float alarmAt)
	{
		if (timer == 0) {
			if (alarmAt == 285) {
				stage->CreateNPC(L"OrcThrowerBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//} else if (alarmAt == 280) {
			//	stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//	stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 275) {
				stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 270) {
				stage->CreateNPC(L"OrcThrowerBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//} else if (alarmAt == 265) {
			//	stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//	stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 260) {
				stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 255) {
				stage->CreateNPC(L"OrcThrowerBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//} else if (alarmAt == 250) {
			//	stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//	stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 245) {
				stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 240) {
				stage->CreateNPC(L"OrcThrowerBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//} else if (alarmAt == 235) {
			//	stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//	stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 230) {
				stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcThrowerBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 225) {
				stage->CreateNPC(L"OrcWarriorBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//} else if (alarmAt == 210) {
			//	stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//	stage->CreateNPC(L"OrcThrowerBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 205) {
				stage->CreateNPC(L"OrcThrowerBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 195) {
				stage->CreateNPC(L"OrcWarriorBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcThrowerBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//} else if (alarmAt == 180) {
			//	stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//	stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 170) {
				stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 165) {
				stage->CreateNPC(L"OrcThrowerBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//} else if (alarmAt == 160) {
			//	stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//	stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 150) {
				stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 135) {
				stage->CreateNPC(L"OrcThrowerBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//} else if (alarmAt == 130) {
			//	stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//	stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 125) {
				stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcThrowerBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 120) {
				stage->CreateNPC(L"OrcThrowerBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//} else if (alarmAt == 115) {
			//	stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//	stage->CreateNPC(L"OrcThrowerBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 150) {
				stage->CreateNPC(L"OrcThrowerBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 145) {
				stage->CreateNPC(L"OrcWarriorBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//} else if (alarmAt == 105) {
			//	stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//	stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 90) {
				stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcThrowerBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 85) {
				stage->CreateNPC(L"OrcWarriorBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//} else if (alarmAt == 80) {
			//	stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//	stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 75) {
				stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcThrowerBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 70) {
				stage->CreateNPC(L"OrcThrowerBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//} else if (alarmAt == 65) {
			//	stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//	stage->CreateNPC(L"OrcThrowerBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 60) {
				stage->CreateNPC(L"OrcThrowerBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcThrowerBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 45) {
				stage->CreateNPC(L"OrcWarriorBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//} else if (alarmAt == 40) {
			//	stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			//	stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 35) {
				stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 30) {
				stage->CreateNPC(L"OrcWarriorBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 15) {
				stage->CreateNPC(L"OrcWarriorBasic", 9, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 1, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 15, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 2, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			} else if (alarmAt == 0) { //Mission Succeess
				stage->DestroyObject(L"Stone3", true);
				return;
			}
			stage->TimerRun(timer, alarmAt-5.0f);
		} else if ( timer == 1) {
			//popup monsters
			stage->CreateNPC(L"OrcWarriorBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			stage->CreateNPC(L"OrcWarriorBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
			stage->TimerRun(1, alarmAt+30.0f);
			//stage->TimerPause(1);
		}
		ALLM_INFO(( L"[ScriptEp1Stage1::TimerAlarmed] timer (%d) alarmed at (%f).", timer, alarmAt ));
	}

	/* object related */
	void ScriptEp1Stage1::ObjectEntered(Constants::ObjectType type, const std::wstring& objectId, int location, uint8 /*team*/)
	{
		static bool bBattleStart = false, bMissionStart = false, bGolemActivated = false;
		if (location == 10) {
			if (type == Constants::ObjectType::Player && !bBattleStart) {
				bBattleStart = true;
				stage->CreateNPC(L"OrcWarriorBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->CreateNPC(L"OrcWarriorBasic", 8, 100, StageInfo::Npc::IdleStateType::PATHFINDING, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);
				stage->TimerRun(1, 30);
			}
		} else if (location == 7) {//defence mission start. off monster popup trigger
			if (type == Constants::ObjectType::Player && !bMissionStart) {
				bMissionStart = true;

				//오크 팝업 정지.
				stage->TimerPause(1);

				//미션 디스플레이
				std::wstring text(L"유리아를 5분간 지켜라!");
				stage->DisplayText(3, text);
				//5분 타이머 작동
				stage->TimerRun(0, 295);
				//유리아 케릭터 생성
				stage->CreateNPC(L"YuriaLv1", 14, 1, StageInfo::Npc::IdleStateType::STAND, 0, float4(0,0,0,0), StageInfo::Npc::TransferType::HATELIST);

				//들어온 입구를 막는다.
				stage->CreateObject(L"Stone2", float2(339, 1197.211182f), 100);
				stage->CreateObject(L"Stone2", float2(346, 1197.211182f), 100);
				stage->CreateObject(L"Stone2", float2(353, 1197.211182f), 100);
				stage->CreateObject(L"Stone2", float2(360, 1197.211182f), 100);
				stage->CreateObject(L"Stone2", float2(367, 1197.211182f), 100);
				stage->CreateObject(L"Stone2", float2(374, 1197.211182f), 100);
				stage->CreateObject(L"Stone2", float2(381, 1197.211182f), 100);
				stage->CreateObject(L"Stone2", float2(388, 1197.211182f), 100);

				//골램에게 통하는 길을 막는다.
				stage->CreateObject(L"Stone3", float2(305, 1456.662354f), 100);
				stage->CreateObject(L"Stone3", float2(312, 1456.662354f), 100);
				stage->CreateObject(L"Stone3", float2(319, 1456.662354f), 100);
				stage->CreateObject(L"Stone3", float2(326, 1456.662354f), 100);
				stage->CreateObject(L"Stone3", float2(333, 1456.662354f), 100);
				stage->CreateObject(L"Stone3", float2(340, 1456.662354f), 100);

			}
		} else if (location == 11) {
			if (type == Constants::ObjectType::Player) {
				stage->MovePlayer(objectId, 12);
			}
		} else if (location == 13) {
			if (type == Constants::ObjectType::Player && !bGolemActivated) {
				bGolemActivated = true;
				//teleport zone 을 열어 준다.
				stage->DestroyObject(L"Stone1", true);
				//Golem 을 출현시킨다... 아직 없음

				//입구를 막는다.
				stage->CreateObject(L"Stone1", float2(305, 1551.568848f), 100);
				stage->CreateObject(L"Stone1", float2(312, 1551.568848f), 100);
				stage->CreateObject(L"Stone1", float2(319, 1551.568848f), 100);
				stage->CreateObject(L"Stone1", float2(326, 1551.568848f), 100);
				stage->CreateObject(L"Stone1", float2(333, 1551.568848f), 100);
				stage->CreateObject(L"Stone1", float2(340, 1551.568848f), 100);
				stage->CreateObject(L"Stone1", float2(347, 1551.568848f), 100);

				//최종미션 디스플레이
				std::wstring text(L"스톤 골램을 파괴하라!");
				stage->DisplayText(3, text);
			}
		}
	}

	void ScriptEp1Stage1::ObjectLeft(const std::wstring& /*objectId*/, int /*location*/, uint8 /*team*/)
	{
	}

	void ScriptEp1Stage1::ObjectCreated(const std::wstring& /*objectId*/)
	{
	}

	void ScriptEp1Stage1::ObjectDestroyed(const std::wstring& objectId)
	{
		if ( objectId == L"YuriaLv1") {
			//Mission Failure
			std::wstring text(L"미션 실패!");
			stage->DisplayText(3, text);
		}
	}

}	}	}
