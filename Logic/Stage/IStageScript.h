
#pragma once
#include "Info/Info.h"

namespace Lunia 
{ 
	namespace XRated 
	{	
		namespace Logic 
		{

	class IStageScript {
	public :
		virtual ~IStageScript() {}

		virtual ILockable& GetSyncRoom()=0;
		virtual void Initialized(uint16 uniqueId)=0;
		// Addby Caspian 20071115
		virtual uint16 GetPlayerCnt(uint8 team) const=0;    // ���� �ش� ���� �Ҽӵ� ���� �� �Դϴ�.
		virtual int GetPlayerSerial( uint8 team, uint16 index ) const=0;   // �ش� ���� �Ҽӵ� �÷��̾��� serial�� ���ɴϴ�. 
		// (index�� 0���� �ش����� GetPlayerCnt() -1 ���� �����մϴ�. ���� ���ϵǴ� �ø����� �������� �Դϴ�. )
		// Ex. Team 1�� 1001, 1005, 1002, 1009�� ���� �� ��� GetPlayerSerial( 1, 2 ) �� 1005�� GetPlayerSerial(1, 0)�� 1001 �� ���ϵ˴ϴ�.
		// -1�ϰ�� ���� �ش� ���� ����

		// Addby Caspian 20071115
		virtual uint16 GetPlayablePlayerCnt() const =0;
		virtual uint16 GetPlayablePlayerCnt(uint8 team) const=0;   // ���� �ش� ���� *�÷��� ������ ���� �� �Դϴ�. 
		virtual bool IsPlayable( uint32 playerSerial ) const=0; // �ش� �ø����� ������ *�÷��� �������� üũ�Ͽ� ������ ��� true�� �����մϴ�.

		// AddBy Caspian 20071120
		virtual void SetPvpMissionClearDelay( float delay ) =0;

		virtual uint32 GetStageId() const=0;
		virtual bool IsPlayer(uint32 objectId)=0;
		virtual int GetPlayerCnt() const=0;
		virtual int GetActivePlayerCnt() const=0;
		virtual uint8 GetPlayerTeam(uint32) const=0;	//�̰� �ش� �ø����� �÷��̾��� ���� �������� �Ű���? caspian
		virtual int GetPlayerClass(uint32) const=0;
		virtual int	GetRandomInt(int min, int max)=0;
		virtual void GetNPCPosition(Serial serial, float& x, float& y)=0;
		virtual int GetPlayerLv(uint32 userSerial) const=0;

		/* stage */
		virtual void SetPvpItemInfo(uint32 hash)=0;
		virtual void DropPvpItem()=0;
		virtual void AddInterestLocation(int location, bool bAdd)=0;
		virtual void AddInterestObjectLocation(uint32 npcSerial, float range, bool bAdd)=0;
		virtual bool IsFulfilledInterestLocation(int location) =0;
		virtual void AddInterestObjects(uint32 hash, bool bAdd)=0;
		virtual void AddInterestNPCPath(uint32 hash, bool bAdd)=0;
		virtual void AddInterestObjects(uint32)=0; //Ư�� serial�� ���� object�� �׾��� �� Ư���� �̺�Ʈ�� ��û
		virtual void Pause(float time)=0;
		virtual void LocPlayers(bool lock)=0; // ���̻� ���ο� �÷��̾� ������ ���ϵ���
		virtual void TimerRun(int id, float time)=0;
		virtual void TimerPause(int id)=0;
		virtual int SecretFound()=0; // �ߺ�ȣ���̵ǰų� �ؼ� �������� �� �� secret������ ���� ȣ��Ǵ����� ���� ��������.
		virtual void MissionClear(bool success, uint8 team)=0;
		virtual void ActivateFastMatch()=0;
		virtual void SetFloor( int floor )=0;

		/* object */
		virtual void ClearItem()=0;
		virtual void CreateItem(uint32 id, int location, int cnt, uint32 userSerial = 0)=0;
		virtual void CreateItem(uint32 id, const float3& pos, uint32 userSerial = 0, bool isPrivateItem = false)=0;

		virtual uint32 CreateObject(uint32 id, float2 pos, uint8 team)=0;
		virtual uint32 CreateObject(uint32 id, float2 pos, const float3& direction, uint8 team)=0;
		virtual uint32 CreateNPC(uint32 id, float3 pos, uint8 team, Database::Info::StageInfo::Npc::IdleStateType idleType,
								int pathGroupIndex, int defenceLocation, Database::Info::StageInfo::Npc::TransferType transferType)=0;
		virtual uint32 CreateNPC(uint32 id, float3 pos, const float3& direction, uint8 team, Database::Info::StageInfo::Npc::IdleStateType idleType,
								int pathGroupIndex, int defenceLocation, Database::Info::StageInfo::Npc::TransferType transferType)=0;
		virtual void DestroyObject(uint32 id, bool silent)=0; // silent==true�̸�, ������ ����Ʈ ���� ������ �������
		virtual void DestroyObject(const int& location, uint32 id, bool silent)=0; // silent==true�̸�, ������ ����Ʈ ���� ������ �������. player�� ���ټ� ����.
		virtual void DestroyObject(const int& location, const uint8& team, bool silent)=0; // silent==true�̸�, ������ ����Ʈ ���� ������ �������. player�� ���ټ� ����.
		virtual void DestroyNPC(const int& location, uint32 id, bool silent)=0; // silent==true�̸�, ������ ����Ʈ ���� ������ �������. player�� ���ټ� ����.
		virtual void DestroyNPC(const int& location, const uint8& team, bool silent)=0; // silent==true�̸�, ������ ����Ʈ ���� ������ �������. player�� ���ټ� ����.
		virtual void ChangePlayerTeam(uint32 userSerial, uint8 team)=0;
		virtual void ChangeNpcTeam(uint32 serial, uint8 team)=0;
		virtual void ChangeNpcAI(int location, uint32 id, int idleType, int value)=0;
		virtual void ChangeNpcAI(uint32 id, int idleType, int value)=0;
		virtual void MoveObject(int locationFrom, int locationTo, uint16 objectType)=0;
		virtual void MoveObject(int locationFrom, int locationTo, uint32 id, uint16 objectType)=0;
		virtual void MovePlayer(uint32 serial, int location)=0;
		virtual void MovePlayer(int location)=0;
		virtual void ChangeNpcSight(uint32 seial, float range)=0;
		virtual void ChangeNPCAction( uint32 hash, uint32 action)=0;
		virtual void ChangeNPCAction(int location, uint32 hash, uint32 action)=0;
		virtual void ChangeMISCState(uint32 id, uint32 type, bool bAdd)=0;
		virtual void ChangeMISCState(uint32 id, int location, uint32 type, bool bAdd)=0;
		virtual void ChangeNPCState(uint32 id, uint32 type, bool bAdd)=0;
		virtual void ChangeNPCState(uint32 id, int location, uint32 type, bool bAdd)=0;

		// �°� ������ StateBundle�� �����ݴϴ�.
		virtual uint32 CheckMinimumPlayerCount()=0;
		virtual void CheckStateBundleForAchievement( uint32 stateBundleHash )=0;
		virtual void GiveStateBundleToObject( uint32 objectSerial, uint32 id )=0;
		virtual void ChangePlayerStatusLimit( std::vector<uint32>* statuses, const std::vector<uint32>& limits )=0;
		virtual void ChangePlayerStatusRate( std::vector<uint32>* statusId, const std::vector<float>& rate )=0;

		virtual uint8 GetQuestState(uint32 userSerial, uint32 questHash)=0;
		virtual uint8 SetQuestState(uint32 userSerial, uint32 questHash, uint8 newState)=0; //����Ʈ ���� �ٲٱ�. ���ϰ��� �ٲ�� �ٷ� ���� ����.
		virtual uint32 GetQuestParameter(uint32 userSerial, uint32 questHash, uint8 paramIndex)=0; //�������� ����Ʈ�� �Ķ���� �� ������. paramIndex�� 0~2 ����.
		virtual uint32 SetQuestParameter(uint32 userSerial, uint32 questHash, uint8 paramIndex, uint32 newValue)=0; //�������� ����Ʈ�� �Ķ���� �� �ٲٱ�. ���ϰ��� �ٲ�� �ٷ� �� ��.

		/* object status - flexible */
		virtual void GiveExp(XRated::Constants::ExpAcquiredType type,uint32 userSerial, uint16 level, uint32 exp)=0;
		virtual void GiveExp(XRated::Constants::ExpAcquiredType type,uint16 level, uint32 exp)=0;
		virtual void GiveBattleEXPtoAllPlayer(XRated::Constants::ExpAcquiredType type, uint8 team, uint32 exp)=0;
		virtual void AddHp(uint32 id, int delta)=0;
		virtual void AddMp(uint32 id, int delta)=0;
		virtual void DealDamage(int location, uint8 team, float dmg, bool bRate)=0;
		virtual void DealDamage(int location, uint32 hash, float dmg, bool bRate)=0;

		virtual bool AddItemToPlayer(uint32 userSerial, uint32 itemId, int cnt, bool bToTeam)=0;
		virtual bool AddItemToPlayer(uint32 userSerial, int* arrayId, int* arrayCnt)=0;
		virtual bool RemoveItem(uint32 userSerial, uint32 itemId, int cnt, bool bFromTeam)=0;
		virtual bool RemoveItem(uint32 userSerial, int* arrayId, int* arrayCnt)=0;
		virtual bool ExamineItemSlotFromPlayer(uint32 userSerial, uint32 itemId, int cnt)=0;

		/* output */
		virtual void DisplayTextEvent(Constants::DisplayTextType displayTo, uint16 textId, float param)=0; //�ؽ�Ʈ ���÷����̺�Ʈ�� ������.
		virtual void NoticeTextEvent(uint32 userSerial, Constants::DisplayTextType displayTo, uint16 textId, float param)=0; //�ؽ�Ʈ ���÷��� �̺�Ʈ�� ������. Ư������
		virtual void DisplayText(Constants::DisplayTextType displayTo, const std::wstring& text)=0; // displayTo �� ���� �޽����� ���޵� ��ġ�� ��, ������ ����
		virtual void NoticeText(uint32 userSerial, Constants::DisplayTextType displayTo, const std::wstring& text)=0;
		virtual void DisplayTimer(int timer, Constants::DisplayTimerType type)=0; //Timer setting�ϰ� �ٷ� �ҷ���� �Ѵ�.
		virtual void BroadcastEvent(int eventId)=0; // contains cinematic
		virtual void NoticeEvent(uint32 userSerial, int eventId)=0; // personal event
		virtual void NoticeClientStageEvent( uint32 userSerial, int eventId, int eventParam ) = 0;
		virtual void AddMinimapPing(int pingId, int x, int y, Constants::MiniMapPingType type)=0;
		virtual void NoticeAddMinimapPing(int pingId, int x, int y, Constants::MiniMapPingType type, uint32 userSerial)=0;
		virtual void RemoveMinimapPing(int pingId)=0;

		virtual void GiveLicense(uint32 licenseGroup, uint32 accessLevel)=0; //����� ��� �÷��̾�� license �� �߱��Ѵ�.
		virtual void GiveLicenseToPlayer(uint32 userSerial, uint32 licenseGroup, uint32 accessLevel)=0; //Ư�� �÷��̾�� License�� �߱��Ѵ�.
		virtual void GiveCharacterLicense(uint32 userSerial, uint32 characterId)=0;//Ư�� �÷��̾�� Character License �� �߱��Ѵ�.
	
		virtual void NoticeStylePointState(XRated::StylePoint::State state)=0; //����� ��� �÷��̾�� StylePointState�� �����Ѵ�.

		/* Gamble */
		virtual void GambleGameEnd(std::vector<uint8> rankList)=0;
		virtual void GambleNextGame(uint32 userSerial, float seconds)=0;
		virtual int GetSlimeRaceIntervalInMin() const = 0;

		/* Holiday Event */
		virtual	bool IsHolidayEventTime(uint32 eventId)=0;
		virtual uint32 GetHolidayEventNpcHash(uint32 eventId, uint32 oldNpcHash)=0;

		virtual void ChangeWeatherToNone() = 0;
		virtual void ChangeWeatherToClear() = 0;
		virtual void ChangeWeatherToRain() = 0;
		virtual void ChangeWeatherToSnow() = 0;
		virtual void ChangeWeatherToAqua() = 0;
	};

}	
}	
}

