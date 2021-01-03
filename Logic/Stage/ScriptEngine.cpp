#include "ScriptEngine.h"
#include <Core/Utils/StringUtil/GenericConversion.h>
#include <Core/Resource/Resource.h>
#include "ScriptLoadThread.h"
#include <Core/DeltaTimer.h>
using namespace Lunia::StringUtil;
#include <iostream>
using namespace std;

#define PREPARE_CONTEXT(func) if (PrepareContext(asModule->GetFunctionByName(func), func, __FUNCTION__)==false) return

namespace Lunia 
{
	namespace XRated 
	{
		namespace Logic
		{

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// ScriptEngine
			ScriptEngine::ScriptEngine( IStageScript* initStageScript )//, const std::string& module, const std::string& filename)
				: stageScript( initStageScript ), elapsedTime(0), engine(0), context(0), bLoading(false), refCount(1)
			{
				
			}

			ScriptEngine::~ScriptEngine() 
			{
			}

			void ScriptEngine::Clear()
			{
				// �̷��� �ص� ������ �Ʒ� ticket�� vector�� ����Ǳⶫ�� �̹� create ticket�� �տ� �ִ°��
				// this�� �Ҹ�� ���Ŀ� create ticket�̳� release ticket�� ������ ȣ��� �� �ִ�.
				// caspian 07.12.27

				engine = NULL; // should not call release() because to make return to pool.
				context = NULL;
				ScriptLoaderInstance().ReleaseScriptEngine( ScriptLoader::Ticket(ScriptLoader::Ticket::TicketType::Release, this, moduleName, 0) );		
			}

			bool ScriptEngine::IsLoading() const
			{
				return bLoading;
			}

			int ScriptEngine::LoadScript(const std::string& module, uint16 uniqueId)
			{
				Logger::GetInstance().Info( "requesting to load({0}) script(stageid:{1}) to listener(uniqueId:{2})", bLoading, module.c_str(), uniqueId );

				bLoading = true;

				Clear(); //clear engine

				moduleName = module; //Set Module name

				ScriptLoaderInstance().LoadScript( ScriptLoader::Ticket(ScriptLoader::Ticket::TicketType::Create, this, module, uniqueId) );

				return 1;
			}

			int ScriptEngine::ExecuteScript(const string& script)
			{
				return 0;//engine->ExecuteString(0/*moduleName.c_str()*/, script.c_str());
			}


			////////////////////////////////////////////////////////////////////////////////////////
			// ScriptLoadThread::IThreadListener implement	////////////////////////////////////////
			void ScriptEngine::ScriptLoaded(uint16 uniqueId)
			{
				Logger::GetInstance().Info( "script loaded({0}). (stagename:{1}, hash:{2}, unique:{3})"
					, bLoading, moduleName.c_str(), stageScript->GetStageId(), uniqueId);

				//Regist stageScript instance
				auto* func = asModule->GetFunctionByName("SetInstance");
				if (PrepareContext(func, "SetInstance", __FUNCTION__)==false)
					throw;

				//PrepareContext(engine->GetFunctionIDByName(0/*moduleName.c_str()*/, "SetInstance"));
				context->SetArgObject(0, (void*)this);
				int r = context->Execute();
				if (r<0) throw;

				stageScript->Initialized(uniqueId);
				bLoading = false;
				return;
			}


			////////////////////////////////////////////////////////////////////////////////////////
			// To Stage
			bool ScriptEngine::IsPlayer(uint32 userSerial)
			{
				return stageScript->IsPlayer( userSerial );
			}

			int ScriptEngine::GetPlayerCnt()
			{
				return stageScript->GetPlayerCnt();
			}

			int ScriptEngine::GetActivePlayerCnt()
			{
				return stageScript->GetActivePlayerCnt();
			}

			uint8 ScriptEngine::GetPlayerTeam(uint32 hash)
			{
				return stageScript->GetPlayerTeam(hash);
			}

			int ScriptEngine::GetPlayerClass(uint32 hash)
			{
				return stageScript->GetPlayerClass(hash);
			}

			int	ScriptEngine::GetRandomInt(int min, int max)
			{
				return stageScript->GetRandomInt(min, max);
			}

			void ScriptEngine::GetNPCPosition(Serial serial, float& x, float& y)
			{
				stageScript->GetNPCPosition(serial, x, y);
			}

			int ScriptEngine::GetPlayerLv(uint32 userSerial)
			{
				return stageScript->GetPlayerLv(userSerial);
			}

			void ScriptEngine::GetServerTime(int& month, int& day, int& hour, int& minute)
			{
				SYSTEMTIME time;
				GetLocalTime(&time);

				month = time.wMonth;
				day = time.wDay;
				hour = time.wHour;
				minute = time.wMinute;
			}

			void ScriptEngine::ShowScriptMsg(const std::string& msg)
			{
				Logger::GetInstance().Info( "{0}.", msg.c_str() );
			}

			void ScriptEngine::Pause(float time)
			{
				stageScript->Pause(time);
			}

			void ScriptEngine::DropPvpItem()
			{
				stageScript->DropPvpItem();
			}

			void ScriptEngine::SetPvpItemInfo(uint32 hash)
			{
				stageScript->SetPvpItemInfo(hash);
			}

			void ScriptEngine::AddInterestLocation(int location, bool bAdd)
			{
				stageScript->AddInterestLocation(location, bAdd);
			}

			void ScriptEngine::AddInterestObjectLocation(uint32 npcSerial, float range, bool bAdd)
			{
				stageScript->AddInterestObjectLocation(npcSerial, range, bAdd);
			}
			bool ScriptEngine::IsFulfilledInterestLocation(int location)
			{
				return stageScript->IsFulfilledInterestLocation( location );
			}

			void ScriptEngine::AddInterestObjects(uint32 objectId, bool bAdd)
			{
				stageScript->AddInterestObjects( objectId, bAdd);
			}

			void ScriptEngine::AddInterestNPCPath(uint32 objectId, bool bAdd)
			{
				stageScript->AddInterestNPCPath( objectId, bAdd);
			}

			void ScriptEngine::AddSpecifiedObjects(uint32 serial)
			{
				stageScript->AddInterestObjects(serial);
			}

			void ScriptEngine::LocPlayers(bool lock)
			{
				stageScript->LocPlayers(lock);
			}

			void ScriptEngine::SetFloor( int floor )
			{
				stageScript->SetFloor( floor );
			}

			void ScriptEngine::TimerRun(int id, float time)
			{
				stageScript->TimerRun(id, time);
			}

			void ScriptEngine::TimerPause(int id)
			{
				stageScript->TimerPause(id);
			}

			void ScriptEngine::MissionClear(bool success, uint8 team)
			{
				stageScript->MissionClear(success, team);
			}

			void ScriptEngine::ActivateFastMatch()
			{
				stageScript->ActivateFastMatch();
			}

			void ScriptEngine::SecretFound()
			{
				stageScript->SecretFound();
			}

			///* object */
			void ScriptEngine::ClearItem()
			{
				stageScript->ClearItem();
			}

			void ScriptEngine::CreateItemInLocation(uint32 id, int location, int cnt)
			{
				stageScript->CreateItem( id, location, cnt );
			}

			void ScriptEngine::CreateItemInPosition(uint32 id, float x, float z)
			{
				stageScript->CreateItem( id, float3(x, 0, z));
			}

			void ScriptEngine::createItemInLocationById(uint32 id, uint32 itemId, int location, int cnt)
			{
				stageScript->CreateItem( itemId, location, cnt, id);
			}

			void ScriptEngine::CreateItemInPositionById(uint32 id, uint32 itemId, float x, float z)
			{
				stageScript->CreateItem( itemId, float3(x, 0, z), id);
			}

			void ScriptEngine::CreatePrivateItemInPosition(uint32 id, float x, float z)
			{
				stageScript->CreateItem( id, float3(x, 0, z), 0, true);
			}

			bool ScriptEngine::RemoveItemFromPlayer(uint32 id, uint32 itemId, int cnt)
			{
				return stageScript->RemoveItem(id, itemId, cnt, false);
			}

			bool ScriptEngine::RemoveItemFromPlayerByTeam(uint32 id, uint32 itemId, int cnt)
			{
				return stageScript->RemoveItem(id, itemId, cnt, true);
			}

			bool ScriptEngine::AddItemToPlayer(uint32 id, uint32 itemId, int cnt)
			{
				return stageScript->AddItemToPlayer(id, itemId, cnt, false);
			}

			bool ScriptEngine::AddItemToPlayerByTeam(uint32 id, uint32 itemId, int cnt)
			{
				return stageScript->AddItemToPlayer(id, itemId, cnt, true);
			}

			bool ScriptEngine::AddItemsToPlayer(uint32 id, std::vector<int>* arrayId, std::vector<int>* arrayCnt)
			{
				int ids[5] = {0,};
				int cnts[5] = {0,};

				unsigned int cnt = arrayId->size();
				if ( cnt > 5) {
					Logger::GetInstance().Info( "Too many parameters. [{0}]", cnt);
					return false;
				}
				if ( cnt != arrayCnt->size() ) {
					Logger::GetInstance().Info( "parameter count mismatch [{0}/{1}]", cnt, arrayCnt->size());
					return false;
				}
				while ( cnt-- )  {
					ids[cnt] = arrayId->at(cnt);
					cnts[cnt] = arrayCnt->at(cnt);
				}

				return stageScript->AddItemToPlayer(id, ids, cnts);
			}

			bool ScriptEngine::RemoveItemsFromPlayer(uint32 id, std::vector<int>* arrayId, std::vector<int>* arrayCnt)
			{
				int ids[5] = {0,};
				int cnts[5] = {0,};

				unsigned int cnt = arrayId->size();
				if ( cnt > 5) {
					Logger::GetInstance().Info( "Too many parameters. [{0}]", cnt);
					return false;
				}
				if ( cnt != arrayCnt->size() ) {
					Logger::GetInstance().Info( "parameter count mismatch [{0}/{1}]", cnt, arrayCnt->size());
					return false;
				}
				while ( cnt-- )  {
					ids[cnt] = arrayId->at(cnt);
					cnts[cnt] = arrayCnt->at(cnt);
				}

				return stageScript->RemoveItem(id, ids, cnts);
			}

			bool ScriptEngine::ExamineItemSlotFromPlayer(uint32 serial, uint32 itemId, int cnt)
			{
				return stageScript->ExamineItemSlotFromPlayer(serial, itemId, cnt);
			}

			//Create object functions
			uint32 ScriptEngine::CreateObjectByPosition(uint32 id, float pos_x, float pos_y, uint8 team)
			{
				return stageScript->CreateObject(id, float2(pos_x, pos_y), team);
			}

			void ScriptEngine::CreateObjectsByPosition(uint32 hash, std::vector<float>* arrayX, std::vector<float>* arrayY, uint8 team)
			{
				unsigned int cnt = arrayX->size();
				float x, y;
				while ( cnt )  {
					--cnt;
					x = arrayX->at(cnt);
					y = arrayY->at(cnt);
					stageScript->CreateObject(hash, float2(x, y), team);
				}
			}

			uint32 ScriptEngine::CreateObjectByPositionD(uint32 id, float pos_x, float pos_y, float dirX, float dirY, uint8 team)
			{
				return stageScript->CreateObject(id, float2(pos_x, pos_y), float3(dirX, 0, dirY), team);
			}

			uint32 ScriptEngine::CreateNPCByPos(uint32 id, float x, float y,
				uint8 team, int idleType, int pathGroupIndex, int defenceLocation, int transferType)
			{
				return stageScript->CreateNPC(id, float3(x, 0, y), team, (Database::Info::StageInfo::Npc::IdleStateType)idleType,
					pathGroupIndex, defenceLocation, (Database::Info::StageInfo::Npc::TransferType)transferType);
			}

			void ScriptEngine::CreateNPCsByPos(uint32 hash, std::vector<float>* array1, std::vector<float>* array2, uint8 team, int idleType, int pathGroupIndex, int defenceLocation, int transferType)
			{
				unsigned int cnt = array1->size();
				float x, y;
				while ( cnt )  {
					--cnt;
					x = array1->at(cnt);
					y = array2->at(cnt);
					stageScript->CreateNPC(hash, float3(x, 0, y), team, (Database::Info::StageInfo::Npc::IdleStateType)idleType,
						pathGroupIndex, defenceLocation, (Database::Info::StageInfo::Npc::TransferType)transferType);
				}
			}

			uint32 ScriptEngine::CreateNPCByPosD(uint32 id, float x, float y, float dirX, float dirY,
				uint8 team, int idleType, int pathGroupIndex, int defenceLocation, int transferType)
			{
				return stageScript->CreateNPC(id, float3(x, 0, y), float3(dirX, 0, dirY), team, (Database::Info::StageInfo::Npc::IdleStateType)idleType,
					pathGroupIndex, defenceLocation, (Database::Info::StageInfo::Npc::TransferType)transferType);
			}

			void ScriptEngine::ChangePlayerStatusLimit(std::vector<uint32>* statuses, const std::vector<uint32>& limits)
			{
				stageScript->ChangePlayerStatusLimit( statuses, limits );
			}

			void ScriptEngine::ChangePlayerStatusRate(std::vector<uint32>* statusId, const std::vector<float>& rate)
			{
				stageScript->ChangePlayerStatusRate( statusId, rate );
			}

			void ScriptEngine::DestroyObject(uint32 id, bool silent)
			{
				stageScript->DestroyObject( id, silent );
			}

			void ScriptEngine::DestroyObjectFromLocationById(uint32 id, int location, bool silent)
			{
				stageScript->DestroyObject( location, id, silent);
			}

			void ScriptEngine::DestroyObjectFromLocationByTeam(uint8 team, int location, bool silent)
			{
				stageScript->DestroyObject( location, team, silent);
			}

			void ScriptEngine::DestroyNPCFromLocationById(uint32 id, int location, bool silent)
			{
				stageScript->DestroyNPC( location, id, silent);
			}

			void ScriptEngine::DestroyNPCFromLocationByTeam(uint8 team, int location, bool silent)
			{
				stageScript->DestroyNPC( location, team, silent);
			}

			void ScriptEngine::ChangePlayerTeam(uint32 userSerial, uint8 team)
			{
				stageScript->ChangePlayerTeam( userSerial, team);
			}

			void ScriptEngine::ChangeNPCTeam(uint32 serial, uint8 team)
			{
				stageScript->ChangeNpcTeam( serial, team);
			}

			//void ScriptEngine::EnableAI(bool enable); // pause or play NPC
			void ScriptEngine::ChangeNPCAI(int location, uint32 id, int idleType, int value)
			{
				stageScript->ChangeNpcAI(location, id, idleType, value);
			}

			void ScriptEngine::ChangeNPCAIBySerial(uint32 serial, int idleType, int value)
			{
				stageScript->ChangeNpcAI(serial, idleType, value);
			}

			void ScriptEngine::MoveObjectFromLocation(int locationFrom, int locationTo, uint16 objectType)
			{
				stageScript->MoveObject(locationFrom, locationTo, objectType);
			}

			void ScriptEngine::MoveObjectFromLocationById(int locationFrom, int locationTo, uint32 id, uint16 objectType)
			{
				stageScript->MoveObject(locationFrom, locationTo, id, objectType);
			}

			void ScriptEngine::MovePlayer(uint32 userSerial, int location)
			{
				stageScript->MovePlayer( userSerial, location );
			}

			void ScriptEngine::MoveAllPlayer(int location)
			{
				stageScript->MovePlayer( location );
			}

			void ScriptEngine::ChangeNPCSight(uint32 serial, float range)
			{
				stageScript->ChangeNpcSight(serial, range);
			}

			///* object status - flexible */
			void ScriptEngine::GiveExp(uint32 userSerial, uint16 level, uint32 exp)
			{
				stageScript->GiveExp( XRated::Constants::ExpAcquiredType::ExpScript, userSerial, level, exp);
			}

			void ScriptEngine::GiveExpToAllPlayer(uint16 level, uint32 exp)
			{
				stageScript->GiveExp( XRated::Constants::ExpAcquiredType::ExpScript, level, exp);
			}

			void ScriptEngine::GiveBattleEXPtoAllPlayer(uint8 team, uint32 warExp)
			{
				stageScript->GiveBattleEXPtoAllPlayer( XRated::Constants::ExpAcquiredType::ExpScript, team, warExp );
			}

			void ScriptEngine::AddHp(uint32 id, int delta)
			{
				stageScript->AddHp( id, delta);
			}

			void ScriptEngine::AddMp(uint32 id, int delta)
			{
				stageScript->AddMp( id, delta);
			}

			void ScriptEngine::DealDamageFromLocationByTeam(int location, uint8 team, float dmg, bool bRate)
			{
				stageScript->DealDamage(location, team, dmg, bRate);
			}

			void ScriptEngine::DealDamageFromLocationById(int location, uint32 hash, float dmg, bool bRate)
			{
				stageScript->DealDamage(location, hash, dmg, bRate);
			}

			void ScriptEngine::ChangeNPCAction(uint32 serial, uint32 action)
			{
				stageScript->ChangeNPCAction(serial, action);
			}

			void ScriptEngine::ChangeNPCActionInLocationById(int location, uint32 hash, uint32 action)
			{
				stageScript->ChangeNPCAction(location, hash, action);
			}

			void ScriptEngine::ChangeMISCStateById(uint32 id, uint32 type, bool bAdd)
			{
				stageScript->ChangeMISCState(id, type, bAdd);
			}

			void ScriptEngine::ChangeMISCStateInLocationById(uint32 id, int location, uint32 type, bool bAdd)
			{
				stageScript->ChangeMISCState(id, location, type, bAdd);
			}

			void ScriptEngine::ChangeNPCStateById(uint32 id, uint32 type, bool bAdd)
			{
				stageScript->ChangeNPCState(id, type, bAdd);
			}

			void ScriptEngine::ChangeNPCStateInLocationById(uint32 id, int location, uint32 type, bool bAdd)
			{
				stageScript->ChangeNPCState(id, location, type, bAdd);
			}

			void ScriptEngine::CheckStateBundleForAchievement( uint32 stateBundleHash )
			{
				stageScript->CheckStateBundleForAchievement( stateBundleHash );
			}

			uint32 ScriptEngine::CheckMinimumPlayerCount( )
			{
				return stageScript->CheckMinimumPlayerCount();
			}

			void ScriptEngine::GiveStateBundleToObject( uint32 objectSerial, uint32 id )
			{
				stageScript->GiveStateBundleToObject( objectSerial, id );
			}

			///* Quest */
			uint8 ScriptEngine::GetQuestState(uint32 userSerial, uint32 questHash)
			{
				return stageScript->GetQuestState(userSerial, questHash);
			}

			uint8 ScriptEngine::SetQuestState(uint32 userSerial, uint32 questHash, uint8 newState)
			{
				return stageScript->SetQuestState(userSerial, questHash, newState);
			}

			uint32 ScriptEngine::GetQuestParameter(uint32 userSerial, uint32 questHash, uint8 paramIndex)
			{
				return stageScript->GetQuestParameter(userSerial, questHash, paramIndex);
			}

			uint32 ScriptEngine::SetQuestParameter(uint32 userSerial, uint32 questHash, uint8 paramIndex, uint32 newValue)
			{
				return stageScript->SetQuestParameter(userSerial, questHash, paramIndex, newValue);
			}


			///* output */
			void ScriptEngine::DisplayTextEvent(uint8 displayTo, uint16 textId, float param)
			{
				stageScript->DisplayTextEvent((Constants::DisplayTextType)displayTo, textId, param);
			}

			void ScriptEngine::NoticeTextEvent(uint32 userSerial, uint8 displayTo, uint16 textId, float param)
			{
				stageScript->NoticeTextEvent(userSerial, (Constants::DisplayTextType)displayTo, textId, param);
			}

			void ScriptEngine::DisplayText(uint8 displayTo, const std::string& text)
			{
				stageScript->DisplayText((Constants::DisplayTextType)displayTo, ToUnicode<string>(text));
			}

			void ScriptEngine::NoticeText(uint32 userSerial, uint8 displayTo, const std::string& text)
			{
				stageScript->NoticeText(userSerial, (Constants::DisplayTextType)displayTo, ToUnicode<string>(text));
			}

			void ScriptEngine::DisplayTimer(int timer, uint32 type)
			{
				stageScript->DisplayTimer(timer, (Constants::DisplayTimerType)type);
			}

			void ScriptEngine::BroadcastEvent(int eventId)
			{
				stageScript->BroadcastEvent(eventId);
			}

			void ScriptEngine::NoticeEvent(uint32 userSerial, int eventId)
			{
				stageScript->NoticeEvent(userSerial, eventId);
			}

			void ScriptEngine::NoticeClientStageEvent( uint32 userSerial, int eventId, int eventParam )
			{
				stageScript->NoticeClientStageEvent( userSerial, eventId, eventParam );
			}

			void ScriptEngine::AddMinimapPing(int pingId, int x, int y, int type)
			{
				stageScript->AddMinimapPing(pingId, x, y, (Constants::MiniMapPingType)type);
			}

			void ScriptEngine::NoticeAddMinimapPing(int pingId, int x, int y, int type, uint32 userSerial)
			{
				stageScript->NoticeAddMinimapPing(pingId, x, y, (Constants::MiniMapPingType)type, userSerial);
			}

			void ScriptEngine::RemoveMinimapPing(int pingId)
			{
				stageScript->RemoveMinimapPing(pingId);
			}

			void ScriptEngine::GiveLicense(uint32 licenseGroup, uint32 accessLevel)
			{
				stageScript->GiveLicense(licenseGroup, accessLevel);
			}

			void ScriptEngine::GiveLicenseToPlayer(uint32 userSerial, uint32 licenseGroup, uint32 accessLevel)
			{
				stageScript->GiveLicenseToPlayer(userSerial, licenseGroup, accessLevel);
			}

			void ScriptEngine::GiveCharacterLicense(uint32 userSerial, uint32 characterId)
			{
				stageScript->GiveCharacterLicense(userSerial, characterId);
			}

			void ScriptEngine::NoticeStylePointState(XRated::StylePoint::State state)
			{
				stageScript->NoticeStylePointState(state);
			}

			void ScriptEngine::GambleGameEnd(std::vector<int>* rankList)
			{
				std::vector<uint8> ranks;

				unsigned int cnt = rankList->size();
				if ( cnt > XRated::Gamble::SlimeRace::MaxSlimeNumber) {
					Logger::GetInstance().Error( "Too many parameters. [{0}]", cnt);
					return;
				}

				while ( cnt-- )  {
					ranks.push_back( (uint8)rankList->at(cnt) );
				}

				stageScript->GambleGameEnd(ranks);
			}

			int ScriptEngine::GetSlimeRaceIntervalInMin() const
			{
				return stageScript->GetSlimeRaceIntervalInMin();
			}

			void ScriptEngine::GambleNextGame(uint32 userSerial, float seconds)
			{
				stageScript->GambleNextGame(userSerial, seconds);
			}

			bool ScriptEngine::IsHolidayEventTime(uint32 eventId)
			{
				return stageScript->IsHolidayEventTime(eventId);
			}

			uint32 ScriptEngine::GetHolidayEventNpcHash(uint32 eventId, uint32 oldNpcHash)
			{
				return stageScript->GetHolidayEventNpcHash(eventId, oldNpcHash);
			}

			void ScriptEngine::ChangeWeatherToNone()
			{
				stageScript->ChangeWeatherToNone();
			}

			void ScriptEngine::ChangeWeatherToClear()
			{
				stageScript->ChangeWeatherToClear();
			}

			void ScriptEngine::ChangeWeatherToRain()
			{
				stageScript->ChangeWeatherToRain();
			}

			void ScriptEngine::ChangeWeatherToSnow()
			{
				stageScript->ChangeWeatherToSnow();
			}

			void ScriptEngine::ChangeWeatherToAqua()
			{
				stageScript->ChangeWeatherToAqua();
			}

			bool ScriptEngine::PrepareContext(asIScriptFunction* function, const char* funcName, const char* callerName)
			{
				int result = context->Prepare(function);
				if( result < 0 )
				{
					switch( result )
					{
					case -6: // method not found in the script
					case -2: // already active context that means, you cannot call a script function in the c++ function which is called by a script function
						// skip known issues
						break;
					default:
						Logger::GetInstance().Error("Invalid Script Context(id:{0}, result:{1}, script_func:{2}, caller:{3}", function->GetId(), result, funcName, callerName);
						break;
					}

					return false;
				}
				return true;
			}

			////////////////////////////////////////////////////////////////////////////////////////
			//IStageScript::IStageEventListener implementation
			// To script
			void ScriptEngine::InitializeScript()
			{
				PREPARE_CONTEXT("Initialize");
				context->Execute();
			}

			/* general events */
#pragma warning(push)
#pragma warning(disable:4189)
			void ScriptEngine::TimerAlarmed(int timer)
			{
				volatile uint32 stageHash=stageScript->GetStageId(); // debugging buffer
				PREPARE_CONTEXT("TimerAlarmed");
				context->SetArgDWord(0, timer);
				context->Execute();
			}
#pragma warning(pop)

			void ScriptEngine::PvpGameStart()
			{
				PREPARE_CONTEXT("PvpGameStart");
				context->Execute();
			}

			void ScriptEngine::BattleGroundTimeover()
			{
				PREPARE_CONTEXT("BattleGroundTimeover");
				context->Execute();
			}

			void ScriptEngine::MissionCleared(bool success, uint8 team)
			{
				PREPARE_CONTEXT("MissionCleared");
				context->SetArgDWord(0, success?1:0);
				context->SetArgDWord(1, (int)team);
				context->Execute();
			}

			void ScriptEngine::NoticeHolidayEvent(uint32 eventId, bool start /*true = start, false = end*/)
			{
				PREPARE_CONTEXT("NoticeHolidayEvent");
				context->SetArgDWord(0, eventId);
				context->SetArgDWord(1, start?1:0);
				context->Execute();
			}

			/* object related */
			void ScriptEngine::ObjectEntered(Constants::ObjectType type, uint32 serial, int location, uint8 team)
			{
				volatile uint32 stageHash=stageScript->GetStageId(); // debugging buffer
				PREPARE_CONTEXT("ObjectEntered");
				context->SetArgDWord(0, (int)type);
				context->SetArgDWord(1, (int)serial);
				context->SetArgDWord(2, location);
				context->SetArgDWord(3, (int)team);
				context->Execute();
			}

			void ScriptEngine::NpcObjectEntered(Constants::ObjectType type, uint32 serial, int location, uint8 team)
			{
				volatile uint32 stageHash=stageScript->GetStageId(); // debugging buffer
				PREPARE_CONTEXT("NpcObjectEntered");
				context->SetArgDWord(0, (int)type);
				context->SetArgDWord(1, (int)serial);
				context->SetArgDWord(2, location);
				context->SetArgDWord(3, (int)team);
				context->Execute();
			}

			void ScriptEngine::ObjectLeft(Constants::ObjectType type, uint32 serial, int location, uint8 team)
			{
				PREPARE_CONTEXT("ObjectLeft");
				context->SetArgDWord(0, (int)type);
				context->SetArgDWord(1, (int)serial);
				context->SetArgDWord(2, location);
				context->SetArgDWord(3, (int)team);
				context->Execute();
			}

			void ScriptEngine::NpcObjectLeft(Constants::ObjectType type, uint32 serial, int location, uint8 team)
			{
				PREPARE_CONTEXT("NpcObjectLeft");
				context->SetArgDWord(0, (int)type);
				context->SetArgDWord(1, (int)serial);
				context->SetArgDWord(2, location);
				context->SetArgDWord(3, (int)team);
				context->Execute();
			}

			void ScriptEngine::ObjectEnteredObjectLocation(Constants::ObjectType type, uint32 serial, int location, uint8 team)
			{
				PREPARE_CONTEXT("ObjectEnteredObjectLocation");
				context->SetArgDWord(0, (int)type);
				context->SetArgDWord(1, (int)serial);
				context->SetArgDWord(2, location);
				context->SetArgDWord(3, (int)team);
				context->Execute();
			}

			void ScriptEngine::ObjectLeftFromObjectLocation(Constants::ObjectType type, uint32 serial, int location, uint8 team)
			{
				PREPARE_CONTEXT("ObjectLeftFromObjectLocation");
				context->SetArgDWord(0, (int)type);
				context->SetArgDWord(1, (int)serial);
				context->SetArgDWord(2, location);
				context->SetArgDWord(3, (int)team);
				context->Execute();
			}

			void ScriptEngine::ObjectDestroyed(uint32 hash, Constants::ObjectType type)
			{
				PREPARE_CONTEXT("ObjectDestroyed");
				context->SetArgDWord(0, hash);
				context->SetArgDWord(1, (int)type);
				context->Execute();
			}

			void ScriptEngine::NPCArrived(uint32 hash, uint32 serial, int pathCnt, int posCnt)
			{
				PREPARE_CONTEXT("NPCArrived");
				context->SetArgDWord(0, hash);
				context->SetArgDWord(1, serial);
				context->SetArgDWord(2, pathCnt);
				context->SetArgDWord(3, posCnt);
				context->Execute();
			}

			void ScriptEngine::InterestObjectDestroyed(Serial serial)
			{
				PREPARE_CONTEXT("InterestObjectDestroyed");
				context->SetArgDWord(0, serial);
				context->Execute();
			}

			void ScriptEngine::PlayerDied(uint32 serial, uint16 life)
			{
				PREPARE_CONTEXT("PlayerDied");
				context->SetArgDWord(0, serial);
				context->SetArgDWord(1, life);
				context->Execute();
			}

			void ScriptEngine::PlayerEntered(uint32 serial, uint8 team)
			{
				PREPARE_CONTEXT("PlayerEntered");
				context->SetArgDWord(0, serial);
				context->SetArgDWord(1, team);
				context->Execute();
			}
			void ScriptEngine::PlayerLeft(uint32 serial, uint8 team)
			{
				PREPARE_CONTEXT("PlayerLeft");
				context->SetArgDWord(0, serial);
				context->SetArgDWord(1, team);
				context->Execute();
			}
			void ScriptEngine::CompletedQuest( uint32 serial, uint32 questHash )
			{
				PREPARE_CONTEXT("CompletedQuest");
				context->SetArgDWord(0, serial);
				context->SetArgDWord(1, questHash);
				context->Execute();
			}
			void ScriptEngine::AcceptedQuest( uint32 serial, uint32 questHash )
			{
				PREPARE_CONTEXT("AcceptedQuest");
				context->SetArgDWord(0, serial);
				context->SetArgDWord(1, questHash);
				context->Execute();
			}
		}
	}
}
