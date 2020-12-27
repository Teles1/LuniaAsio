/*
Copyright (c) 2000-, Lunia Corp. All rights reserved.
http://www.Lunia.co.kr
tel:82-2-888-1555
fax:82-2-888-4414
mailTo:webmaster@Lunia.co.kr
*/
#pragma once
#include "IStageScript.h"
#include "ScriptLoadThread.h"
#include "AngelScript/stdstring.h"

namespace Lunia 
{ 
	namespace XRated 
	{
		namespace Logic 
		{
			class ScriptEngine : ScriptLoader::IThreadListener 
			{
			protected :
				float elapsedTime;
				IStageScript* stageScript;

				asIScriptEngine* engine;
				asIScriptContext* context;
				std::string moduleName;

				bool bLoading;
				int refCount;

			protected :
				void Clear();
				int ExecuteScript( const std::string& script );

			private :
				ScriptEngine();
				bool PrepareContext( int funcID, const char* funcName, const char* callerName ); ///< @return false if the preparing is failed

			public :
				ScriptEngine( IStageScript* initStageScript );//, const std::string& module, const std::string& filename);
				virtual ~ScriptEngine();

				int AddRef() 
				{
					return ++refCount;
				}

				int Release() 
				{
					return --refCount;
				} //�����ϴ³��� ���̻� ���ٰ� �ؼ� �������� �ʴ´�.

				int LoadScript( const std::string& module, uint16 uniqueId );

				void SetEngine( asIScriptEngine* e ) 
				{ 
					engine = e; 
				}

				void SetContext( asIScriptContext* c ) 
				{ 
					context = c; 
				}

				bool IsLoading() const;

				void ReleaseEngine() 
				{ 
					Clear(); 
				}
			public : //ScriptLoadThread::IThreadListener implement
				void ScriptLoaded( uint16 uniqueId );
				inline ILockable& GetSyncRoom() 
				{ 
					return stageScript->GetSyncRoom(); 
				}

			public :
				// script�� ���� �Ѿ���� �̺�Ʈ�� �˸°� �����ؼ� stage �� �ѱ��.
				bool IsPlayer( uint32 userSerial );
				int GetPlayerCnt();
				int GetActivePlayerCnt();
				uint8 GetPlayerTeam( uint32 hash );
				int GetPlayerClass( uint32 hash );
				int	GetRandomInt( int min, int max );
				void GetNPCPosition( Serial serial, float& x, float& y );
				int GetPlayerLv(uint32 userSerial);
				void GetServerTime( int& month, int& day, int& hour, int& minute ); //���� �ð��� �˾ƿ´�.
				int GetLocaleCode() const 
				{ 
					return Lunia::XRated::Constants::LocaleSpecificSetting::GetLocaleCode();
				}

				// Addby Caspian 20071115
				uint16 GetPlayerCntByTeam( uint8 team ) const
				{
					return stageScript->GetPlayerCnt( team );
				}

				int GetPlayerSerial( uint8 team, uint16 index ) const
				{
					return stageScript->GetPlayerSerial( team, index );
				}

				bool IsPlayable( uint32 playerSerial ) const
				{
					return stageScript->IsPlayable( playerSerial );
				}

				uint16 GetPlayablePlayerCnt() const
				{
					return stageScript->GetPlayablePlayerCnt();
				}

				uint16 GetPlayablePlayerCntByTeam( uint8 team ) const
				{ 
					return stageScript->GetPlayablePlayerCnt( team ); 
				}   

				// AddBy Caspian 20071120
				void SetPvpMissionClearDelay( float delay )
				{ 
					stageScript->SetPvpMissionClearDelay( delay ); 
				}

				void ShowScriptMsg( const asCScriptString& msg );
				///* stageScript */
				void SetPvpItemInfo( uint32 hash );
				void DropPvpItem();
				void AddInterestLocation( int location, bool bAdd );
				bool IsFulfilledInterestLocation( int location );
				void AddInterestObjectLocation( uint32 npcSerial, float range, bool bAdd );
				void AddInterestObjects( uint32 objectId, bool bAdd );
				void AddInterestNPCPath( uint32 objectId, bool bAdd );

				void AddSpecifiedObjects( uint32 serial );
				void Pause( float time );
				void LocPlayers( bool lock );// ���̻� ���ο� �÷��̾� ������ ���ϵ���
				void TimerRun( int id, float time );
				void TimerPause( int id );
				void MissionClear( bool success, uint8 team );
				void ActivateFastMatch();
				void SecretFound();
				void SetFloor( int floor );

				///* object */
				void ClearItem();
				void CreateItemInLocation( uint32 userSerial, int location, int cnt );
				void CreateItemInPosition( uint32 userSerial, float x, float z );
				void createItemInLocationById( uint32 userSerial, uint32 itemId, int location, int cnt ); //�ȵ�
				void CreateItemInPositionById( uint32 userSerial, uint32 itemId, float x, float z );
				void CreatePrivateItemInPosition( uint32 userSerial, float x, float z );

				bool RemoveItemFromPlayer( uint32 userSerial, uint32 itemId, int cnt );
				bool RemoveItemFromPlayerByTeam( uint32 userSerial, uint32 itemId, int cnt ); //�ȵ�
				bool AddItemToPlayer( uint32 userSerial, uint32 itemId, int cnt );
				bool AddItemToPlayerByTeam( uint32 userSerial, uint32 itemId, int cnt ); //�ȵ�
				bool AddItemsToPlayer( uint32 userSerial, std::vector <int >*, std::vector< int >* );
				bool RemoveItemsFromPlayer( uint32 userSerial, std::vector< int >*, std::vector< int >* );
				bool ExamineItemSlotFromPlayer( uint32 userSerial, uint32 itemId, int cnt );

				uint32 CreateObjectByPosition( uint32 id, float pos_x, float pos_y, uint8 team );
				void CreateObjectsByPosition( uint32 hash, std::vector< float >*, std::vector< float >*, uint8 team );
				uint32 CreateObjectByPositionD( uint32 id, float pos_x, float pos_y, float dirX, float dirY, uint8 team );
				uint32 CreateNPCByPos( uint32 id, float x, float y, uint8 team, int idleType, int pathGroupIndex, int defenceLocation, int transferType );
				void CreateNPCsByPos( uint32 hash, std::vector< float >*, std::vector< float >*, uint8 team, int idleType, int pathGroupIndex, int defenceLocation, int transferType );
				uint32 CreateNPCByPosD( uint32 id, float x, float y, float dirX, float dirY, uint8 team, int idleType, int pathGroupIndex, int defenceLocation, int transferType );
				void ChangePlayerStatusLimit( std::vector< uint32 >*, const std::vector< uint32 > &);
				void ChangePlayerStatusRate( std::vector< uint32 >*, const std::vector< float > & );

				void DestroyObject( uint32 id, bool silent); // silent==true�̸�, ������ ����Ʈ ���� ������ �������. player�� ���ټ� ����.
				void DestroyObjectFromLocationById( uint32 id, int location, bool silent );
				void DestroyObjectFromLocationByTeam( uint8 team, int location, bool silent );
				void DestroyNPCFromLocationById( uint32 id, int location, bool silent );
				void DestroyNPCFromLocationByTeam( uint8 team, int location, bool silent );
				void ChangePlayerTeam( uint32 userSerial, uint8 team );
				void ChangeNPCTeam( uint32 serial, uint8 team );
				void ChangeNPCAI( int location, uint32 id, int idleType, int value );
				void ChangeNPCAIBySerial( uint32 serial, int idleType, int value );
				void MoveObjectFromLocation( int locationFrom, int locationTo, uint16 objectType );
				void MoveObjectFromLocationById( int locationFrom, int locationTo, uint32 id, uint16 objectType );
				void MovePlayer( uint32 userSerial, int location );
				void MoveAllPlayer( int location );
				void ChangeNPCSight( uint32 serial, float range );

				///* object status - flexible */
				void GiveExp( uint32 userSerial, uint16 level, uint32 exp );
				void GiveExpToAllPlayer( uint16 level, uint32 exp );
				void GiveBattleEXPtoAllPlayer( uint8 team, uint32 warExp );
				void AddHp( uint32 id, int delta );
				void AddMp( uint32 id, int delta );
				void DealDamageFromLocationByTeam( int location, uint8 team, float dmg, bool bRate );
				void DealDamageFromLocationById( int location, uint32 hash, float dmg, bool bRate );
				void ChangeNPCAction( uint32 serial, uint32 action );
				void ChangeNPCActionInLocationById( int location, uint32 hash, uint32 action );
				void ChangeMISCStateById( uint32 id, uint32 type, bool bAdd );
				void ChangeMISCStateInLocationById( uint32 id, int location, uint32 type, bool bAdd );
				void ChangeNPCStateById( uint32 id, uint32 type, bool bAdd );
				void ChangeNPCStateInLocationById( uint32 id, int location, uint32 type, bool bAdd );

				// �°� ������ StateBundle�� �����ݴϴ�.
				uint32 CheckMinimumPlayerCount(  );
				void CheckStateBundleForAchievement( uint32 stateBundleHash );
				void GiveStateBundleToObject( uint32 objectSerial, uint32 id );

				///* Quest */
				/**�̹� �Ϸ�� ����Ʈ�� ���, ���� ������������ ���� �� �ִ� ����Ʈ �Ǵ� ���罺���������� ���� �� �ִ� ����Ʈ�� �ٷ����� ������ �Ϸ� ���θ� Ȯ���� �� ����.
				(��, ���� ������������ ����Ʈ�� ���� �� �ִ��� �˻��ؾߵ� �ʿ䰡 �ִ� ����Ʈ�鸸 �˻�)
				*/
				uint8 GetQuestState( uint32 userSerial, uint32 questHash );
				uint8 SetQuestState( uint32 userSerial, uint32 questHash, uint8 newState ); //����Ʈ ���� �ٲٱ�. ���ϰ��� �ٲ�� �ٷ� ���� ����.
				uint32 GetQuestParameter( uint32 userSerial, uint32 questHash, uint8 paramIndex ); //�������� ����Ʈ�� �Ķ���� �� ������. paramIndex�� 0~2 ����.
				uint32 SetQuestParameter( uint32 userSerial, uint32 questHash, uint8 paramIndex, uint32 newValue ); //�������� ����Ʈ�� �Ķ���� �� �ٲٱ�. ���ϰ��� �ٲ�� �ٷ� �� ��.


				///* output */
				void DisplayTextEvent( uint8 displayTo, uint16 textId, float param ); //�ؽ�Ʈ ���÷����̺�Ʈ�� ������.
				void NoticeTextEvent( uint32 userSerial, uint8 displayTo, uint16 textId, float param ); //�ؽ�Ʈ ���÷��� �̺�Ʈ�� ������. Ư������
				void DisplayText( uint8 displayTo, const asCScriptString& text ); // displayTo �� ���� �޽����� ���޵� ��ġ�� ��, ������ ���� ( enum type���� �����ʿ� )
				void NoticeText( uint32 userSerial, uint8 displayTo, const asCScriptString& text );
				void DisplayTimer (int timer, uint32 type ); // color==0 �̸� hiding
				void BroadcastEvent( int eventId ); // contains cinematic
				void NoticeEvent( uint32 userSerial, int eventId ); // personal event
				void NoticeClientStageEvent( uint32 userSerial, int eventId, int eventParam ); 
				void AddMinimapPing( int pingId, int x, int y, int type );
				void NoticeAddMinimapPing(int pingId, int x, int y, int type, uint32 userSerial);
				void RemoveMinimapPing( int pingId );

				void GiveLicense( uint32 licenseGroup, uint32 accessLevel ); //����� ��� �÷��̾�� license �� �߱��Ѵ�.
				void GiveLicenseToPlayer( uint32 userSerial, uint32 licenseGroup, uint32 accessLevel ); //Ư�� �÷��̾�� License�� �߱��Ѵ�.
				void GiveCharacterLicense( uint32 userSerial, uint32 characterId );//Ư�� �÷��̾�� Character License �� �߱��Ѵ�.

				void NoticeStylePointState( XRated::StylePoint::State state ); //����� ��� �÷��̾�� StylePointState�� �����Ѵ�.

				/* Gamble */
				void GambleGameEnd( std::vector< int >* rankList );
				void GambleNextGame( uint32 userSerial, float seconds );
				int GetSlimeRaceIntervalInMin() const;

				/* Holiday Event */
				bool IsHolidayEventTime( uint32 eventId );
				uint32 GetHolidayEventNpcHash( uint32 eventId, uint32 oldNpcHash );

				// Weather
				void ChangeWeatherToNone();//���� ��������
				void ChangeWeatherToClear();//�� �⺻���� << �̰� ���� ��������� �̸��� �̷���.
				void ChangeWeatherToRain();
				void ChangeWeatherToSnow();
				void ChangeWeatherToAqua();

			public :
				void InitializeScript();

				/* general events */
				void TimerAlarmed( int timer );
				void PvpGameStart();
				void BattleGroundTimeover();
				void MissionCleared( bool success, uint8 team );

				/* Holiday Event */
				void NoticeHolidayEvent( uint32 eventId, bool start /*true = start, false = end*/);

				/* object related */
				void ObjectEntered( Constants::ObjectType type, uint32 userSerial, int location, uint8 team );
				void NpcObjectEntered( Constants::ObjectType type, uint32 userSerial, int location, uint8 team );
				void ObjectLeft( Constants::ObjectType type, uint32 userSerial, int location, uint8 team );
				void NpcObjectLeft( Constants::ObjectType type, uint32 userSerial, int location, uint8 team );
				void ObjectEnteredObjectLocation( Constants::ObjectType type, uint32 userSerial, int location, uint8 team );
				void ObjectLeftFromObjectLocation( Constants::ObjectType type, uint32 userSerial, int location, uint8 team );
				void ObjectDestroyed( uint32 hash, Constants::ObjectType type );
				void NPCArrived( uint32 npcId, uint32 serial, int pathCnt, int posCnt );
				void InterestObjectDestroyed( Serial serial );
				void PlayerDied( uint32 serial, uint16 life );
				void PlayerEntered( uint32 serial, uint8 team );
				void PlayerLeft( uint32 serial, uint8 team );
				void CompletedQuest( uint32 serial, uint32 questHash );
				void AcceptedQuest( uint32 serial, uint32 questHash );
			};
		}
	}	
}
