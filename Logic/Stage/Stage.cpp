#include "Stage.h"
#include "../GameStateDatabase.h"
#include "../Object/Player.h"
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
#include "../Object/Misc/Misc.h"
#include <Core/Utils/StringUtil/GenericConversion.h>
#include "../LogicDB/LogicDB.h"
#include "../State/StateInvincible.h"
#include <iostream>
#include <iterator>
#include <algorithm>
using namespace std;

using namespace Lunia::StringUtil;
using namespace Lunia::XRated::Database;
using namespace Lunia::XRated::Database::Info;

namespace Lunia 
{
	namespace XRated 
	{
		namespace Logic 
		{

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//
			bool Stage::InterestObjects::IsInterestObject(Serial serial)
			{
				int cnt = (int)serials.size();
				while( cnt ) {
					--cnt;
					if ( serials[cnt] == serial ) {
						serials[cnt] = serials.back();
						serials.pop_back();
						return true;;
					}
				}
				return false;
			}

			bool Stage::InterestObjects::IsInterestObjectID(uint32 hash)
			{
				int cnt = (int)destroy.size();
				while( cnt ) {
					--cnt;
					if ( destroy[cnt] == hash )
						return true;;
				}
				return false;
			}

			void Stage::InterestObjects::SetInterestNPCPath(uint32 hash, bool bAdd)
			{
				if (bAdd) {
					pathFinding.push_back(hash);
				} else {
					int cnt = (int)pathFinding.size();
					while ( cnt ) {
						--cnt;
						if ( pathFinding[cnt] == hash ) {
							pathFinding[cnt] = pathFinding.back();
							pathFinding.pop_back();
							return;
						}
					}
				}
			}

			void Stage::InterestObjects::SetInterestObjectDestroy(uint32 hash, bool bAdd)
			{
				if (bAdd) {
					destroy.push_back(hash);
				} else {
					int cnt = (int)destroy.size();
					while ( cnt ) {
						--cnt;
						if ( destroy[cnt] == hash ) {
							destroy[cnt] = destroy.back();
							destroy.pop_back();
							return;
						}
					}
				}
			}


			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Stage class
#pragma warning( disable : 4355 )
			Stage::Stage()	: script(this)/*, bMissionEnded(false)*/, MissionPlayTime(0), secretCnt(0)/*, bInitialized(false)*/, pvpItemInfo(NULL), gameState(GameState::NotInitialized), pvpMissionClearDelay(-1)
			{
				continueTimer.ContinueTime = 0.0f;
				continueTimer.IsEnable = false;

				bFastMatchActivated = false;
				maxNpcCnt = DatabaseInstance().InfoCollections.GeneralInfos.Retrieve().MaxNPCCnt;
			}
#pragma warning( default : 4355 )

			Stage::~Stage()
			{
			}

			void Stage::GetStageStatistics(CriticalStatistics::StageStatistics& SS)
			{
				SS.ElapsedTime = MissionPlayTime;
				SS.FoundSecret = secretCnt;
				//SS.AllPlayerReviveCount = 0;
			}

			float Stage::GetElpasedTime()
			{
				return MissionPlayTime;
			}

			void Stage::CoinPlayerDied(uint32 serial)
			{
				std::map<uint32, float>::iterator i = coin.continueDelay.find(serial);
				if ( i != coin.continueDelay.end() ) {
					(*i).second = 10.0f;
				} else
					coin.continueDelay[serial] = 10.0f;

				switch( gameState ) {
		case GameState::MissionStarted :
			gameState = GameState::CoinPlayerDied;
			break;
				}
			}

			void Stage::CoinPlayerRevived(uint32 serial)
			{
				switch(gameState) {
		case GameState::MissionEnded :
		case GameState::CampfireWait :
		case GameState::Campfire :
			return;

				}
				std::map<uint32, float>::iterator i = coin.continueDelay.find(serial);
				if ( i != coin.continueDelay.end() )
					coin.continueDelay.erase(i);

				if ( coin.continueDelay.empty() ) //��Ȱ�߰� ī��Ʈ�ٿ����� �÷��̾ ����. �Ϲ� �÷��� ���·� ���ư���.
				{
					gameState = GameState::MissionStarted;
					continueTimer.IsEnable = false;
				}
				else //���� ī��Ʈ�ٿ����� �÷��̾ �ִ�.
					gameState = GameState::CoinPlayerDied;
			}

			void Stage::CoinPlayerGiveUp(uint32 serial)
			{
				std::map<uint32, float>::iterator i = coin.continueDelay.find(serial);
				if ( i != coin.continueDelay.end() )
					coin.continueDelay.erase(i);

				if ( coin.continueDelay.empty() ) {
					switch( gameState ) {
			case GameState::CoinPlayerDied : //���� �̼� ���а� �ƴϴ�. ī��Ʈ�ٿ� �� ������ ������ �� ������ ��������.
				gameState = GameState::MissionStarted;
				break;
			case GameState::ContinueWait : //ī��Ʈ�ٿ��� ������ �̼ǽ��� ������̾���. ���ӿ���
				script.MissionCleared(coin.success, coin.team);
				MissionClear(coin.success, coin.team);
				gameState = GameState::MissionEnded;
				break;
					}
				}
			}

			void Stage::LoadStage(StageGroup* sGroup, uint32 name, uint16 uniqueId)
			{
				stageGroup = sGroup;
				id=name;
				info = DatabaseInstance().InfoCollections.Stages.Retrieve(name);

				if ( !info ) {
					Logger::GetInstance().Exception( "[Stage::LoadStage] Cannot find stageinfo. [{}]", name );
				}

				if ( info->bUpdateScript == false )
					gameState = GameState::NotInitializedWithoutScript;
				else
					gameState = GameState::NotInitialized;

				{//Timer setting
					std::vector<StageInfo::Timer>::iterator i = info->Timers.begin(), end = info->Timers.end();
					for(; i != end ; ++i )
						timers.push_back( Stage::Timer((*i).Countdown) );
				}

				// Location ���� ������ ����س���.
				locationPosition = info->LocationPositions; //.resize(info->Locations.size());
				int n = (int)locationPosition.size();
				while(n) {
					interestLocations.push_back(InterestLocation());
					--n;
				}

				stageMap.LoadMap(info);

				gameType = stageGroup->GameType;
				ghostRule = stageGroup->GhostRule;

				pvpItemInfo = NULL;

				//script �ʱ�ȭ
				script.LoadScript(StringUtil::To<std::string>(info->Id.c_str()), uniqueId);
			}

			void Stage::Init()
			{
				if ( script.IsLoading() ) {
					Logger::GetInstance().Exception(L"[Stage::Init] Already loading script engine...");
				}

				locationPosition.clear();
				interestLocations.clear();
				interestObjectLocations.clear();
				timers.clear();

				interestObjects.Clear();

				coin.continueDelay.clear();

				campfireDelay = 0;
				//campFireDelayTotal = 0;
				pvpMissionClearDelay = -1;

				MissionPlayTime = 0;
				secretCnt = 0;

				continueTimer.ContinueTime = 0.0f;
				continueTimer.IsEnable = false;

				gameState = GameState::NotInitialized;

				bFastMatchActivated = false;
				pvpItemInfo = NULL;
			}

			bool Stage::IsAbletoBuy(const float3& pos, Constants::ShopType shopType)
			{
				float4 area;
				std::map<std::wstring, Lunia::XRated::StageEvent>::iterator i;
				for (i = info->Events.begin() ; i != info->Events.end() ; ++i) {
					if ( (*i).second.Value == shopType && (*i).second.Type == XRated::Constants::StageEventType::EnterShop ) {
						area = (*i).second.Area;
						if ( pos.x >= area.x && pos.z <= area.y && pos.x <= area.z && pos.z >= area.w )
							return true;
					}
				}
				return false;
			}

			void Stage::Update()
			{
				float dt = clock.GetTimeDelta();

				switch ( gameState ) {
		case GameState::NotInitializedWithoutScript:
			gameState = GameState::WithoutScript;
			script.InitializeScript();
			return;
		case GameState::WithoutScript :
			return;
		case GameState::NotInitialized :
			gameState = GameState::MissionStarted;
			script.InitializeScript();
			return;
		case GameState::MissionStarted :
			break;
		case GameState::CoinPlayerDied :
			if ( UpdateCoinPlayerCountdown(dt) )
				gameState = GameState::MissionStarted;
			break;
		case GameState::ContinueWait :
			if ( UpdateCoinPlayerCountdown(dt) && UpdateContinueTimer(dt) ) {
				script.MissionCleared(coin.success, coin.team);
				MissionClear(coin.success, coin.team);
				gameState = GameState::MissionEnded;
				return;
			}
			break;
		case GameState::MissionEnded :
			gameState = GameState::CampfireWait;
			return;
		case GameState::CampfireWait :
			{
				campfireDelay += dt;
				if ( stageGroup->GameType & Constants::CooperativeGameTypeMask ) { 
					if ( campfireDelay >= Constants::CampfireDelayCooperative ) {
						gameStateDatabase->CampfireBegin();
						gameState = GameState::Campfire;
						return;
					}
				} else if ( stageGroup->GameType & Constants::GameTypes::PvpGameTypeMask) {
					if (  (pvpMissionClearDelay > 0 && campfireDelay >= pvpMissionClearDelay )
						|| (campfireDelay >= Constants::CampfireDelayPVP)  )
					{
						gameState = GameState::Campfire;
						return;
					}
				}
				else
				{

				}
			}
			return;
		case GameState::Campfire :
			return;
				}

				UpdateObjectLocation();
				UpdateLayer();
				UpdateTimer(dt);
			}

			void Stage::UpdateObjectLocation()
			{
				std::list<InterestObjectLocation>::iterator i = interestObjectLocations.begin();
				for ( ; i != interestObjectLocations.end() ; ) { //�߰��� ���� �Ǵ� ��ƾ ������ for������ ������ ��ų �� ����.
					InterestObjectLocation& objectLocation = (*i);
					if ( objectLocation.bRemove ) {
						i = interestObjectLocations.erase(i);
						continue;
					}
					NonPlayer* npc = gameStateDatabase->RetrieveNPC(objectLocation.serial);
					if ( !npc ) {// �׾���ȳ�����. �����Ѵ�.
						++i;
						continue;
					}

					std::vector<Player*> l;
					gameStateDatabase->GetAreaObject(npc->GetPosition(), objectLocation.range, Constants::AllianceType::AnyObjects, 0/*team�� ���� �ƴϹǷ� ��� ����*/, npc, l);
					std::vector<Player*>::iterator ip = l.begin();

					//���� location �� ��ϵ� ����� ����Ʈ�� �����ؿ´�.
					//���� �����ϴ� ����� ���ϸ鼭 �̹��Ͽ� ���� �����ϴ� ����� üũ�ϱ� ����.
					std::list<Serial> exists = objectLocation.objects; //exists�� �����ؼ� ������ �Ѵ�.

					for (; ip != l.end() ; ++ip) {
						Player* player = (*ip);
						if ( player->CheckState(Info::StateInfo::Type::GHOST) ) continue;
						Serial serial = player->GetSerial();
						std::list<Serial>::iterator li = objectLocation.objects.begin(), lend = objectLocation.objects.end();
						bool bExist = false;
						for(; li != lend ; ++li) {
							if ( (*li) == serial ) {//layer�� �����ϴ� ���̴�.
								bExist = true;

								// ����Ʈ�� �����ϴ³��� exists vector���� �����Ѵ�.
								// exists vector�� ���� �ִ³���� �̹��Ͽ� layer���� �������.
								exists.remove(serial);
								break;
							}
						}
						if (!bExist) {
							script.ObjectEnteredObjectLocation(player->GetType(), player->GetSerial(), objectLocation.serial, player->GetTeam());

							// ObjectEntered �̺�Ʈ�� �߻��ϰ� �� �ڿ��� ���ɿ����� ���� �Ǿ����� �˻��Ѵ�.
							if ( objectLocation.bRemove ) {
								break;
							}

							objectLocation.objects.push_back(serial);
						}
					}

					// ���ɿ������� ���� �Ǿ��ٸ� ���̻� �˻�� ���ǹ� �ϴ�.
					if ( objectLocation.bRemove ) {
						i = interestObjectLocations.erase(i);
						continue;
					}

					/// ������� �ͼ� exists vector �� �����ϴ� ����� �̹��Ͽ� layer���� ��������̴�.
					for (std::list<Serial>::iterator a = exists.begin() ; a != exists.end() ; ++a) {
						objectLocation.objects.remove((*a));
						Object* obj = gameStateDatabase->Retrieve((*a));
						if (obj) {
							script.ObjectLeftFromObjectLocation(obj->GetType(), obj->GetSerial(), objectLocation.serial, obj->GetTeam());
							if ( objectLocation.bRemove )
								break;
						}
					}

					// ���ɿ������� ���� �Ǿ��ٸ� ���̻� �˻�� ���ǹ� �ϴ�.
					if ( objectLocation.bRemove ) {
						i = interestObjectLocations.erase(i);
						continue;
					}

					++i;
				}
			}

			void Stage::UpdateLayer()
			{
				//Layer control
				std::vector<float4>::iterator iterLoc = info->Locations.begin(), iterLocEnd = info->Locations.end();
				int cnt=0;
				for (; iterLoc != iterLocEnd ; ++iterLoc, ++cnt) {
					if ( !interestLocations.at(cnt).bActivated )
						continue;

					//std::vector<Player*> l;
					//gameStateDatabase->GetAreaObjectRect((*iterLoc), l);
					//std::vector<Player*>::iterator i = l.begin(), end = l.end();
					std::vector<Actor*> l;
					gameStateDatabase->GetAreaObjectRect((*iterLoc), l);
					std::vector<Actor*>::iterator i = l.begin(), end = l.end();

					std::list<Serial> exists = interestLocations[cnt].locationObjects;
					InterestLocation& interestLocation = interestLocations[cnt];

					for (; i != end ; ++i) {
						if ( (*i)->CheckState(Info::StateInfo::Type::GHOST) ) continue;
						Serial serial = (*i)->GetSerial();
						std::list<Serial>::iterator li = interestLocation.locationObjects.begin(), lend = interestLocation.locationObjects.end();
						bool bExist = false;
						for(; li != lend ; ++li) {
							if ( (*li) == serial ) {
								bExist = true;

								exists.remove(serial);
								break;
							}
						}
						if (!bExist) 
						{
							switch ((*i)->GetType())
							{
							case Constants::ObjectType::Player:
								script.ObjectEntered((*i)->GetType(), (*i)->GetSerial(), cnt, (*i)->GetTeam());
								break;
							case Constants::ObjectType::NonPlayer:
								script.NpcObjectEntered((*i)->GetType(), (*i)->GetSerial(), cnt, (*i)->GetTeam());
								break;
							default:
								break;
							}
							// ObjectEntered �̺�Ʈ�� �߻��ϰ� �� �ڿ��� ���ɿ����� ���� �Ǿ����� �˻��Ѵ�.
							if ( !interestLocation.bActivated )
								break;

							interestLocation.locationObjects.push_back(serial);
						}
					}

					if ( !interestLocation.bActivated )
						continue;

					for (std::list<Serial>::iterator a = exists.begin() ; a != exists.end() ; ++a) {
						interestLocation.locationObjects.remove((*a));
						Object* obj = gameStateDatabase->Retrieve((*a));
						if (obj) {
							switch (obj->GetType())
							{
							case Constants::ObjectType::Player:
								script.ObjectLeft(obj->GetType(), obj->GetSerial(), cnt, obj->GetTeam());
								break;
							case Constants::ObjectType::NonPlayer:
								script.NpcObjectLeft(obj->GetType(), obj->GetSerial(), cnt, obj->GetTeam());
								break;
							default:
								break;
							}

							if ( !interestLocation.bActivated )
								break;
						}
					}
				}
			}

			void Stage::UpdateTimer(float dt)
			{
				//Timer control
				std::vector<Timer>::iterator i = timers.begin(), end = timers.end();
				int cnt=0;
				for (; i != end ; ++i, ++cnt) {
					if ( (*i).bEnabled ) {
						if ( (*i).bCountdown ) {
							(*i).time -= dt;
							if ( (*i).time <= 0 ) {//�ð��� 0���ϰ� �Ǹ� �ڵ����� �˶��ϰ� Ÿ�̸Ӹ� ����.
								(*i).bEnabled = false;
								script.TimerAlarmed(cnt);
							}
						} else {
							(*i).time += dt;
							if ( (*i).time >= (*i).alarmAt ) {
								(*i).bEnabled = false;
								script.TimerAlarmed(cnt);
							}
						}
					}
				}

				//��� display�� ������ ����
				MissionPlayTime += dt;
			}

			bool Stage::UpdateContinueTimer(float dt)
			{
				if(continueTimer.IsEnable == false)
					return	true;

				continueTimer.ContinueTime -= dt;

				if(continueTimer.ContinueTime < 0.0f)
				{
					//continueTimer.ContinueTime = 6.0f;
					continueTimer.Reset();
					continueTimer.IsEnable = false;

					return	true;
				}

				return	false;
			}

			bool Stage::UpdateCoinPlayerCountdown(float dt)
			{
				if(continueTimer.IsEnable == true)
				{
					coin.continueDelay.clear();
					return true;
				}

				for( std::map<uint32, float>::iterator i = coin.continueDelay.begin() ; i != coin.continueDelay.end() ; ) {
					(*i).second -= dt;
					if ( (*i).second <= 0 )
						i = coin.continueDelay.erase(i);
					else
						++i;
				}
				return coin.continueDelay.empty();
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Event to Script	////////////////////////////////////////////////////////////////////////////////////////////
			void Stage::ObjectDestroyed(Serial serial, uint32 hash, Constants::ObjectType type)
			{
				if ( interestObjects.IsInterestObject(serial) ) {
					InterestObjectDestroyed(serial);
					return;
				}

				switch(type) {
			case Constants::ObjectType::Player : //�÷��̾�� hash�� �������� ���ǹ��ϴ�. �ϴ� ����.
				break;
			case Constants::ObjectType::NonPlayer :
			case Constants::ObjectType::Misc :
			case Constants::ObjectType::Item :
				if ( interestObjects.IsInterestObjectID(hash) )
					script.ObjectDestroyed(hash, type);
				}
			}

			void Stage::NPCArrived(uint32 hash, uint32 serial, int pathCnt, int posCnt)
			{
				for ( std::vector<uint32>::iterator i = interestObjects.pathFinding.begin() ; i != interestObjects.pathFinding.end() ; ++i) {
					if ((*i) == hash) {
						//���⼭ script�� �̺�Ʈ�� �����ش�.
						script.NPCArrived(hash, serial, pathCnt, posCnt);
						return;
					}
				}
			}

			void Stage::InterestObjectDestroyed(Serial serial)
			{
				script.InterestObjectDestroyed(serial);
			}

			void Stage::PvpGameStart()
			{
				script.PvpGameStart();
			}

			void Stage::PlayerDied(Serial serial, uint16 life)
			{
				script.PlayerDied(serial, life);
			}
			void Stage::CompletedQuest(Serial serial, uint32 questHash)
			{
				script.CompletedQuest( serial, questHash );
			}
			void Stage::AcceptedQuest(Serial serial, uint32 questHash)
			{
				script.AcceptedQuest( serial, questHash );
			}
			void Stage::NoticeHolidayEvent(uint32 eventId, bool start)
			{
				script.NoticeHolidayEvent( eventId, start );
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// IStageScript impl	////////////////////////////////////////////////////////////////////////////////////////
			ILockable& Stage::GetSyncRoom()
			{
				return gameStateDatabase->GetSyncRoom();
			}

			void Stage::Initialized(uint16 uniqueId)
			{
				//script �� �ε������Ƿ� �ʱ�ȭ�� �����������.
				//script.InitializeScript();
				gameStateDatabase->Initialized(uniqueId);
				//Lunia_INFO(( L"[Stage::Initialized] Stage initialized." ));
			}

			bool Stage::IsPlayer(uint32 userSerial)
			{
				return gameStateDatabase->RetrievePlayer(userSerial) ? true : false;
			}

			int Stage::GetPlayerCnt() const
			{
				return gameStateDatabase->GetPlayerCnt();
			}

			int Stage::GetActivePlayerCnt() const
			{
				return gameStateDatabase->GetActivePlayerCnt();
			}

			uint8 Stage::GetPlayerTeam(uint32 userSerial) const
			{
				const Player* player = gameStateDatabase->RetrievePlayer(userSerial);
				if ( player ) {
					return player->GetTeam();
				}
				return 0;
			}

			int Stage::GetPlayerClass(uint32 userSerial) const
			{
				const Player* player = gameStateDatabase->RetrievePlayer(userSerial);
				if( player) {
					return (int)player->GetClassType();
				}
				return -1;
			}

			int	Stage::GetRandomInt(int min, int max)
			{
				return gameStateDatabase->GetRandomInt(min, max);
			}

			void Stage::GetNPCPosition(Serial serial, float& x, float& y)
			{
				float3 pos;
				NonPlayer* npc = gameStateDatabase->RetrieveNPC(serial);
				if (npc) {
					pos = npc->GetPosition();
					x = pos.x;
					y = pos.z;
				} else {
					x = 10.0f;
					y = 10.0f;
				}
				return;
			}

			int Stage::GetPlayerLv(uint32 userSerial) const
			{
				const Player* player = gameStateDatabase->RetrievePlayer(userSerial);
				if( player) {
					return (int)player->GetLevel();
				}
				return -1;
			}

			uint16 Stage::GetPlayerCnt(uint8 team)  const 
			{ 
				return gameStateDatabase->GetPlayerCnt( team ); 
			}
			int Stage::GetPlayerSerial( uint8 team, uint16 index ) const 
			{ 
				return gameStateDatabase->GetPlayerSerial( team, index ); 
			}
			uint16 Stage::GetPlayablePlayerCnt() const 
			{ 
				return gameStateDatabase->GetPlayablePlayerCnt(); 
			}		
			uint16 Stage::GetPlayablePlayerCnt(uint8 team) const 
			{ 
				return gameStateDatabase->GetPlayablePlayerCnt( team ); 
			}		
			bool Stage::IsPlayable( uint32 playerSerial ) const 
			{ 
				return gameStateDatabase->IsPlayable( playerSerial ); 
			} 

			void Stage::CreatePvpItemInPosition(std::vector<Place>& l)
			{
				int sz = (int)l.size();
				int probIndex = (sz-1)/2;
				if ( pvpItemInfo->Probability->Probability.size() <= static_cast<uint32>(probIndex) ) throw;
				const std::vector< std::vector<float> >& probTable = pvpItemInfo->Probability->Probability[probIndex];
				if ( probTable.size() < l.size() ) throw;

				std::vector<int> wait; //�����ġ�� ���ϱ� ���� ǥ�� ������. �� ��ġ���� �Ѱ��� �����۸��� �������� ������
				wait.reserve(pvpItemInfo->Info->DropPosition.size());
				for ( int a=0;a<(int)pvpItemInfo->Info->DropPosition.size(); ++a ) { wait.push_back(a); } //���ǥ�� ���� ����� ���߿� �������� ��� �� �� �������� ���� �̾�����.

				std::vector<Place>::iterator i = l.begin();
				std::vector<Place>::iterator last = l.end();
				int playerCnt=0;
				for( ; i != last ; ++i, ++playerCnt ) { //��� �÷��̾ ���鼭,
					float prob = gameStateDatabase->GetRandomFloat();
					float total=0;
					const std::vector<float>& placeProb = probTable.at(playerCnt); //�ش� ������ ��ũ�� ���� Ȯ�� ���̺��� ��� ����.
					int rankCnt=0;
					if ( pvpItemInfo->Table->Table.size() < placeProb.size() ) throw; // assert(pvpItemInfo->Table->Table.size() >= placeProb.size());
					for(std::vector<float>::const_iterator cIter = placeProb.begin() ; cIter != placeProb.end() ; ++cIter, ++rankCnt) { //���� ���� ���� �˸´� ��ũ�� �������� �ѷ�����.
						total += (*cIter);
						if ( prob <= total ) { //�ɷȴ� ������ ���!
							const std::vector<uint32>& itemList = pvpItemInfo->Table->Table.at(rankCnt);

							//�������� ������ ��ġ�� �����ϰ� ������
							int posIndex = gameStateDatabase->GetRandomInt(0, (int)wait.size());
							float3 pos = pvpItemInfo->Info->DropPosition.at(wait.at(posIndex));
							wait[posIndex] = wait.back();
							wait.pop_back();

							CreateItem(itemList.at(gameStateDatabase->GetRandomInt(0, (int)itemList.size())), pos, (*i).userSerial);
							break;
						}
					}
				}
			}

			void Stage::CreatePvpItem(std::vector<Place>& l)
			{
				int sz = (int)l.size();
				int probIndex = (sz-1)/2;
				if ( pvpItemInfo->Probability->Probability.size() <= static_cast<uint32>(probIndex) ) throw; // assert( (int)pvpItemInfo->Probability->Probability.size() > probIndex);

				const std::vector< std::vector<float> >& probTable = pvpItemInfo->Probability->Probability[probIndex];
				if ( probTable.size() < l.size() ) throw; // assert( probTable.size() >= l.size() );

				std::vector<Place>::const_iterator i = l.begin();
				std::vector<Place>::const_iterator last = l.end();
				int playerCnt=0;
				for( ; i != last ; ++i, ++playerCnt ) { //��� �÷��̾ ���鼭,
					Player* player = gameStateDatabase->RetrievePlayer(i->userSerial);
					float prob = gameStateDatabase->GetRandomFloat();
					float total=0;
					const std::vector<float>& placeProb = probTable[playerCnt]; //�ش� ������ ��ũ�� ���� Ȯ�� ���̺��� ��� ����.
					int rankCnt=0;
					if (pvpItemInfo->Table->Table.size() < placeProb.size()) throw; // assert(pvpItemInfo->Table->Table.size() >= placeProb.size());
					for(std::vector<float>::const_iterator cIter = placeProb.begin() ; cIter != placeProb.end() ; ++cIter, ++rankCnt) { //���� ���� ���� �˸´� ��ũ�� �������� �ѷ�����.
						total += (*cIter);
						if ( prob <= total ) { //�ɷȴ� ������ ���!
							const std::vector<uint32>& itemList = pvpItemInfo->Table->Table[rankCnt];

							//�÷��̾��� �ٷ� �ڿ� �������� ��������.
							float3 pos = player->GetPosition(); //�÷��̾��� �ڿ� �ڸ��� ���ٸ� �÷��̾� ��ġ�� ����߸���.
							if ( gameStateDatabase->IsAbletoLocateByGhost(pos + player->GetDirection()*-8, 6) ) //�÷��̾��� �ڿ� �����°� �����ϴٸ� ����߷��ش�.
								//-8 �̶�� ��ġ��.. �׳� �ϰų� ����. -_-; 6�� �������� �������ε�.. ���. ������
								//-3 �̶�� ��ġ�� �������� ����鼭 �⺻������ -5��ŭ �ڿ� ����� ������
								pos = pos + player->GetDirection()*-3; 

							CreateItem(itemList.at(gameStateDatabase->GetRandomInt(0, (int)itemList.size())), pos, (*i).userSerial, player->GetDirection());
							break;
						}
					}
				}
			}

			/* Player list�� ���� �ͼ� ������ �ű�� �˸��� �������� ������ ��ġ�� �����ش�. **/
			void Stage::DropPvpItem()
			{
				if (!info) return;

				const PlayerMap& players = gameStateDatabase->GetPlayers();
				int playerCnt = (int)players.size();
				if ( playerCnt > 8 ) {//������ 8���� �ִ�
					Logger::GetInstance().Warn( L"[Stage::DropPvpItem] Too many users. [{0}/{1}]", info->Id.c_str(), playerCnt );
					return;
				} else if ( playerCnt < 1 ) {
					return;
				}

				gameStateDatabase->ClearItem();

				std::vector<Place> placeList;
				PlayerMap::const_iterator last = players.end();
				int cnt=0;
				for ( PlayerMap::const_iterator i = players.begin(); i != last ; ++i, ++cnt ) {
					PlayerData& playerData = (*i).second->GetPlayerData();
					placeList.push_back(Place(playerData.BaseCharacter.BaseObject.GameObjectSerial, (float)playerData.Life + playerData.BaseCharacter.Hp/playerData.BaseCharacter.MaxHp));
					std::sort(placeList.begin(), placeList.end(), Place::Comp);
				}
				CreatePvpItem(placeList);
			}

			void Stage::SetPvpItemInfo(uint32 hash)
			{
				pvpItemInfo = Database::DatabaseInstance().InfoCollections.Stages.RetrieveItemStageInfo(hash);
				if ( !info ) {
					Logger::GetInstance().Warn( L"[Stage::SetPvpItemInfo] Unable to find ItemStageInfo  [{}]", hash );
					pvpItemInfo = NULL;
					return;
				}
			}

			void Stage::AddInterestLocation(int location, bool bAdd)
			{
				if ( location < 0 || location >= (int)interestLocations.size() ) {
					return;
					//throw( Lunia_EXCEPTION((L"[Stage::AddInterestLocation] Wrong location. [{}]", location)) );
				}

				if ( bAdd ) {
					interestLocations[location].locationObjects.clear();
					interestLocations[location].bActivated = true;
				} else {
					interestLocations[location].locationObjects.clear();
					interestLocations[location].bActivated = false;
				}
			}

			bool Stage::IsFulfilledInterestLocation(int location)
			{

				std::vector<float4>::iterator iterLoc = info->Locations.begin(), iterLocEnd = info->Locations.end();

				int numberOfInLocation=0;
				int cnt =0 ;

				for ( cnt=0  ; cnt<location ; ++cnt) 
				{
					++iterLoc;
				}

				InterestLocation& interestLocation = interestLocations[location];

				std::vector<Player*> playerList;
				gameStateDatabase->GetAreaObjectRect((*iterLoc), playerList);
				std::vector<Player*>::iterator iter = playerList.begin(), end=playerList.end();

				for (; iter != end ; ++iter) 
				{
					if ( (*iter)->CheckState(Info::StateInfo::Type::GHOST) ) continue;
					Serial playerSerial = (*iter)->GetSerial();
					std::list<Serial>::const_iterator iterSerial = interestLocation.locationObjects.begin();
					std::list<Serial>::const_iterator iterSerialIterEnd = interestLocation.locationObjects.end();

					for(; iterSerial != iterSerialIterEnd ; ++iterSerial)
					{
						if ( (*iterSerial) == playerSerial )
						{
							++numberOfInLocation;
							break;
						}
					}
				}


				// �÷��̾� ������ -1 ���� ������ Location�� ������ �� �Լ����� �ʰ� ������Ʈ �Ǳ� �����̴�.
				// ��, ���� �ֱٿ� �� ������ ���� ����� ����numberOfInLocation���� count���� �ʴ´�.
				// ��Ż�� ����� ���� �� üũ�ϴ� �� �ܿ��� ����� �۵����� ���ɼ��� ���� ���δ�-_-
				if( numberOfInLocation == GetActivePlayerCnt() -1 ) return true; 
				else return false;


			}

			void Stage::AddInterestObjectLocation(uint32 npcSerial, float range, bool bAdd)
			{
				if ( bAdd ) {
					std::list<InterestObjectLocation>::iterator i = interestObjectLocations.begin();
					std::list<InterestObjectLocation>::iterator end = interestObjectLocations.end();
					for ( ; i != end ; ++i ) {
						if ( (*i).serial == npcSerial ) //�̹� ���� �Ѵ�.
							return;
					}
					interestObjectLocations.push_back(InterestObjectLocation(npcSerial, range));
				} else {
					std::list<InterestObjectLocation>::iterator i = interestObjectLocations.begin();
					std::list<InterestObjectLocation>::iterator end = interestObjectLocations.end();
					for ( ; i != end ; ++i ) {
						if ( (*i).serial == npcSerial ) {//ã�Ҵ�.
							(*i).bRemove = true;
							//interestObjectLocations.erase(i); //������ ����� ���� Stage::Update�� �̷�� �� ��
							return;
						}
					}
				}
			}

			void Stage::AddInterestObjects(uint32 hash, bool bAdd)
			{
				interestObjects.SetInterestObjectDestroy(hash, bAdd);
			}

			void Stage::AddInterestNPCPath(uint32 hash, bool bAdd)
			{
				interestObjects.SetInterestNPCPath( hash, bAdd );
			}

			void Stage::AddInterestObjects(uint32 serial)
			{
				interestObjects.serials.push_back( serial );
			}

			void Stage::Pause(float time)
			{
				gameStateDatabase->Pause(time);
			}

			void Stage::LocPlayers(bool lock)
			{
				gameStateDatabase->LocPlayers(lock);
				MissionPlayTime = 0;
			}

			void Stage::TimerRun(int id, float time)
			{
				if ( id < 0 || id >= (int)timers.size() ) {
					return;
					//throw( Lunia_EXCEPTION((L"[Stage::TimerRun] Wrong timer id. [{}]", id)) );
				}

				timers[id].bEnabled = true;
				if (timers[id].bCountdown)
					timers[id].time = time;
				else {
					timers[id].time = 0;
					timers[id].alarmAt = time;
				}
			}

			void Stage::TimerPause(int id)
			{
				timers.at(id).bEnabled = false;
			}

			void Stage::MissionClear(bool success, uint8 team)
			{
				// �̼��� �������� ��ũ��Ʈ���� �˷��ذ�. ���нÿ� ������ �ִ��� ���̻� ����ϴ°� �Ұ����ϴ�.
				// �̹� �̼��� �����ٸ� �ٸ� �޼����� �����Ѵ�.
				switch ( gameState ) {
			case GameState::MissionEnded :
			case GameState::CampfireWait :
			case GameState::Campfire :
				return;
				}
				//if (bMissionEnded)
				//	return;

				gameStateDatabase->MissionClear(success, team);
				gameState = GameState::MissionEnded;
				//bMissionEnded = true;
				bFastMatchActivated = false;
			}

			void Stage::MissionClearFromLogic(bool success, uint8 team)
			{
				switch ( gameState ) {
		case GameState::WithoutScript :
			return;
		case GameState::CoinPlayerDied :
			gameState = GameState::ContinueWait;
			coin.success = success;
			coin.team = team;
			return;
		case GameState::ContinueWait : //�̷����� ��� ����� �Ѵ�. ���� �ȵ� -.-
			Logger::GetInstance().Warn( L"[Stage::MissionClearFromLogic] Something wrong in coin process. [{0}, {1}]", team, success ? L"true" : L"false");
			coin.success = success;
			coin.team = team;
			return;
		case GameState::MissionEnded :
		case GameState::CampfireWait :
		case GameState::Campfire :
			return;
				}

				if(continueTimer.IsEnable == true)
				{
					gameState = GameState::ContinueWait;
					coin.success = success;
					coin.team = team;
					return;
				}

				script.MissionCleared(success, team);
				MissionClear(success, team);
			}

			void Stage::MissionClearTimeOver()
			{
				if ( gameType == XRated::Constants::GameTypes::BattleGroundType )
				{
					script.BattleGroundTimeover();
				}
			}

			void Stage::ContinueCountDown()
			{
				//continueTimer.ContinueTime = 6.0f;
				continueTimer.Reset();
				continueTimer.IsEnable = true;
			}

			void Stage::SetFloor( int floor )
			{
				gameStateDatabase->SetFloor( floor );
			}

			void Stage::ClearItem()
			{
				gameStateDatabase->ClearItem();
			}

			void Stage::CreateItem(uint32 id, int location, int cnt, uint32 /*userSerial*/)
			{
				if ( location < 0 || location >= (int)locationPosition.size() )
					return;

				float diameter = 4.0f;
				float3 startPos = locationPosition[location];
				float3 pos = startPos;
				float3 direction[4] = {float3(0, 0, 1), float3(1, 0, 0), float3(0, 0, -1), float3(-1, 0, 0)};
				int directionTurn = 0, turn = 0, repeat=1, currentRepeat=0;
				while( cnt-- ) {
					gameStateDatabase->CreateItem(id, pos, float3(0,0,-1), uint16(1));

					pos += direction[directionTurn]*diameter;
					++turn;
					if (repeat == ++currentRepeat) { if (!(turn%2)) ++repeat; currentRepeat=0; ++directionTurn; }
					if (directionTurn > 3) directionTurn=0;
				}
			}

			void Stage::CreateItem(uint32 id, const float3& pos, uint32 userSerial, const float3& direction)
			{
				if ( !userSerial )
					gameStateDatabase->CreateItem(id, pos, direction, uint16(1));
				else
					gameStateDatabase->CreateItem(id, pos, direction, userSerial);
			}

			void Stage::CreateItem(uint32 id, const float3& pos, uint32 userSerial, bool isPrivateItem)
			{
				if ( !userSerial )
					gameStateDatabase->CreateItem(id, pos, float3(0,0,-1), uint16(1));
				else
					gameStateDatabase->CreateItem(id, pos, float3(0,0,-1), userSerial, uint16(1), 0, isPrivateItem);
			}

			bool Stage::AddItemToPlayer(uint32 userSerial, uint32 itemId, int cnt, bool /*bToTeam*/)
			{
				Player* player = gameStateDatabase->RetrievePlayer(userSerial);

				if ( player ) {
					return gameStateDatabase->AddItem( player, itemId, cnt );
				}
				return false;
			}

			bool Stage::AddItemToPlayer(uint32 userSerial, int* arrayId, int* arrayCnt)
			{
				Player* player = gameStateDatabase->RetrievePlayer(userSerial);
				if ( player ) {
					return gameStateDatabase->AddItem( player, arrayId, arrayCnt );
				}
				return false;
			}

			bool Stage::RemoveItem(uint32 userSerial, uint32 itemId, int cnt, bool /*bFromTeam*/)
			{
				Player* player = gameStateDatabase->RetrievePlayer(userSerial);

				if ( player ) {
					return gameStateDatabase->RemoveItem( player, itemId, cnt );
				}
				return false;
			}

			bool Stage::RemoveItem(uint32 userSerial, int* arrayId, int* arrayCnt)
			{
				Player* player = gameStateDatabase->RetrievePlayer(userSerial);
				if ( player ) {
					return gameStateDatabase->RemoveItem( player, arrayId, arrayCnt );
				}
				return false;
			}

			bool Stage::ExamineItemSlotFromPlayer(uint32 userSerial, uint32 itemId, int cnt)
			{
				return gameStateDatabase->ExamineItemSlotFromPlayer(userSerial, itemId, cnt);
			}

			uint32 Stage::CreateObject(uint32 id, float2 pos, uint8 team)
			{
				return CreateObject(id, pos, float3(0, 0, -1), team);
			}

			uint32 Stage::CreateObject(uint32 id, float2 pos, const float3& direction, uint8 team)
			{
				return LogicDBInstance().CreateMisc(gameStateDatabase, id, float3(pos.x,0,pos.y), direction, team, 0, 0);
			}

			uint32 Stage::CreateNPC(uint32 id, float3 pos, uint8 team, Database::Info::StageInfo::Npc::IdleStateType idleType,
				int pathGroupIndex, int defenceLocation, Database::Info::StageInfo::Npc::TransferType transferType)
			{
				return CreateNPC(id, pos, float3(0, 0, -1), team, idleType, pathGroupIndex, defenceLocation, transferType);
			}

			uint32 Stage::CreateNPC(uint32 id, float3 pos, const float3& direction, uint8 team, Database::Info::StageInfo::Npc::IdleStateType idleType,
				int pathGroupIndex, int defenceLocation, Database::Info::StageInfo::Npc::TransferType transferType)
			{
				if (gameStateDatabase->GetNPCCnt() >= maxNpcCnt) {
					Logger::GetInstance().Warn( L"Exception [Stage::CreateNPC] Too many NPC [{0}]. Create NPC Aborted.[{1}] ", gameStateDatabase->GetNPCCnt(), info->Id.c_str() );
					return 0;
				}

				return LogicDBInstance().CreateNPC(gameStateDatabase, id, pos, Constants::DirectionF::GetDirection(direction), team, idleType, pathGroupIndex, defenceLocation, 0, transferType, gameStateDatabase->GetMaxTeamPlayerCnt());
			}

			void Stage::DestroyObject(uint32 id, bool silent)
			{
				std::vector<Object*> l;
				gameStateDatabase->GetObjectbyName<Object>(id, l);
				std::vector<Object*>::iterator i = l.begin(), end = l.end();
				for (; i != end ; ++i) {
					//((Actor*)(*i))->SetHP(0);

					switch ( (*i)->GetType() ) {
			case Constants::ObjectType::Misc :
				if ( gameStateDatabase->IsUpdateMISC() ) {
					Logger::GetInstance().Warn( L"[Stage::DestroyObject1] Error!! object list is updating. unable to remove object {0}.", id );
					break;
				}
				gameStateDatabase->ObjectDestroyed((*i), (*i)->GetType(), silent);
				if ( gameStateDatabase->Remove((*i)->GetSerial()) )
					LogicDBInstance().ReleaseMisc( (Misc*)(*i) );
				break;
			case Constants::ObjectType::NonPlayer :
				if ( gameStateDatabase->IsUpdateNPC() ) {
					Logger::GetInstance().Warn( L"[Stage::DestroyObject1] Error!! npc list is updating. unable to remove npc {0}.", id );
					break;
				}
				gameStateDatabase->ObjectDestroyed((*i), (*i)->GetType(), silent);
				if ( gameStateDatabase->Remove((*i)->GetSerial()) )
					LogicDBInstance().ReleaseNPC( (NonPlayer*)(*i) );
				break;
			default :
				gameStateDatabase->ObjectDestroyed((*i), (*i)->GetType(), silent);
				if ( gameStateDatabase->Remove((*i)->GetSerial()) )
					delete (*i);
					}
				}
			}

			void Stage::DestroyObject(const int& location, uint32 id, bool silent)
			{
				if ( location < 0 || location >= (int)info->Locations.size() )
					return;

				std::vector<Object*> l;
				gameStateDatabase->GetAreaObjectRect<Object>(info->Locations[location], id, l);
				std::vector<Object*>::iterator i = l.begin(), end = l.end();
				for (; i != end ; ++i) {
					//((Actor*)(*i))->SetHP(0);

					switch ( (*i)->GetType() ) {
			case Constants::ObjectType::Misc :
				if ( gameStateDatabase->IsUpdateMISC() ) {
					Logger::GetInstance().Warn( L"[Stage::DestroyObject2] Error!! object list is updating. unable to remove object {0}.", id );
					break;
				}
				gameStateDatabase->ObjectDestroyed((*i), (*i)->GetType(), silent);
				if ( gameStateDatabase->Remove((*i)->GetSerial()) )
					LogicDBInstance().ReleaseMisc( (Misc*)(*i) );
				break;
			case Constants::ObjectType::NonPlayer :
				if ( gameStateDatabase->IsUpdateNPC() ) {
					Logger::GetInstance().Warn( L"[Stage::DestroyObject2] Error!! npc list is updating. unable to remove npc {0}.", id );
					break;
				}
				gameStateDatabase->ObjectDestroyed((*i), (*i)->GetType(), silent);
				if ( gameStateDatabase->Remove((*i)->GetSerial()) )
					LogicDBInstance().ReleaseNPC( (NonPlayer*)(*i) );
				break;
			default :
				gameStateDatabase->ObjectDestroyed((*i), (*i)->GetType(), silent);
				if ( gameStateDatabase->Remove((*i)->GetSerial()) )
					delete (*i);
					}
				}
			}

			void Stage::DestroyObject(const int& location, const uint8& team, bool silent)
			{
				if ( location < 0 || location >= (int)info->Locations.size() )
					return;

				std::vector<Object*> l;
				gameStateDatabase->GetAreaObjectRect<Object>(info->Locations[location], team, l);
				std::vector<Object*>::iterator i = l.begin(), end = l.end();
				for (; i != end ; ++i) {
					//((Actor*)(*i))->SetHP(0);

					switch ( (*i)->GetType() ) {
			case Constants::ObjectType::Misc :
				if ( gameStateDatabase->IsUpdateMISC() ) {
					Logger::GetInstance().Warn( L"[Stage::DestroyObject3] Error!! object list is updating. unable to remove object {0}.", id );
					break;
				}
				gameStateDatabase->ObjectDestroyed((*i), (*i)->GetType(), silent);
				if ( gameStateDatabase->Remove((*i)->GetSerial()) )
					LogicDBInstance().ReleaseMisc( (Misc*)(*i) );
				break;
			case Constants::ObjectType::NonPlayer :
				if ( gameStateDatabase->IsUpdateNPC() ) {
					Logger::GetInstance().Warn( L"[Stage::DestroyObject3] Error!! npc list is updating. unable to remove npc {0}.", (*i)->GetName().c_str() );
					break;
				}
				gameStateDatabase->ObjectDestroyed((*i), (*i)->GetType(), silent);
				if ( gameStateDatabase->Remove((*i)->GetSerial()) )
					LogicDBInstance().ReleaseNPC( (NonPlayer*)(*i) );
				break;
			default :
				gameStateDatabase->ObjectDestroyed((*i), (*i)->GetType(), silent);
				if ( gameStateDatabase->Remove((*i)->GetSerial()) )
					delete (*i);
					}
				}
			}

			void Stage::DestroyNPC(const int& location, uint32 id, bool silent)
			{
				if ( gameStateDatabase->IsUpdateNPC() ) {
					Logger::GetInstance().Warn(L"[Stage::DestroyNPC1] Error!! npc list is updating. unable to remove npc {0}.", id );
					return;
				}
				if ( location < 0 || location >= (int)info->Locations.size() )
					return;

				std::vector<NonPlayer*> l;
				gameStateDatabase->GetAreaObjectRect<NonPlayer>(info->Locations[location], id, l);
				std::vector<NonPlayer*>::iterator i = l.begin(), end = l.end();
				for (; i != end ; ++i) {
					//(*i)->SetHP(0);

					gameStateDatabase->ObjectDestroyed((*i), (*i)->GetType(), silent);
					if ( gameStateDatabase->RemoveNpc((*i)->GetSerial()) )
						LogicDBInstance().ReleaseNPC( *i );
				}
			}

			void Stage::DestroyNPC(const int& location, const uint8& team, bool silent)
			{
				if ( gameStateDatabase->IsUpdateNPC() ) {
					Logger::GetInstance().Info( L"[Stage::DestroyNPC2] Error!! npc list is updating. unable to remove npc in location {0}.", location );
					return;
				}
				if ( location < 0 || location >= (int)info->Locations.size() )
					return;

				std::vector<NonPlayer*> l;
				gameStateDatabase->GetAreaObjectRect<NonPlayer>(info->Locations[location], team, l);
				std::vector<NonPlayer*>::iterator i = l.begin(), end = l.end();
				for (; i != end ; ++i) {
					//(*i)->SetHP(0);

					gameStateDatabase->ObjectDestroyed((*i), (*i)->GetType(), silent);
					if ( gameStateDatabase->RemoveNpc((*i)->GetSerial()) )
						LogicDBInstance().ReleaseNPC( *i );
				}
			}

			template<> void Stage::MoveObject<Actor>(int locationFrom, int locationTo)
			{
				std::vector<Actor*> l;
				gameStateDatabase->GetAreaObjectRect(info->Locations.at(locationFrom), l);
				for (std::vector<Actor*>::iterator i = l.begin(), end=l.end(); i != end; ++i) {
					(*i)->SetPosition( gameStateDatabase->GetAvailableGhostPos( locationPosition.at(locationTo), (*i)->GetRadius()) );
					(*i)->SetOverlap(true);
					Lunia::XRated::CharacterData data = (*i)->GetCharacterData();
					gameStateDatabase->StatusChanged(data.BaseObject.GameObjectSerial, data.BaseObject.Position, data.BaseObject.Direction,
						data.Hp, data.Mp, 0, 0);
				}
			}

			template<> void Stage::MoveObject<NonPlayer>(int locationFrom, int locationTo)
			{
				std::vector<NonPlayer*> l;
				gameStateDatabase->GetAreaObjectRect(info->Locations.at(locationFrom), l);
				for (std::vector<NonPlayer*>::iterator i = l.begin(), end=l.end(); i != end; ++i) {
					float3 pos = gameStateDatabase->GetAvailableGhostPos(locationPosition.at(locationTo), (*i)->GetRadius());
					(*i)->SetPosition( pos );
					(*i)->SetOverlappable();
					Lunia::XRated::CharacterData data = (*i)->GetCharacterData();
					(*i)->SetInitialPos( data.BaseObject.Position);	
					(*i)->InitHeightInfo();
					(*i)->SetAction((*i)->GetNPCInfo()->StartActionHash, NULL, true);
					//gameStateDatabase->StatusChanged(data.BaseObject.GameObjectSerial, data.BaseObject.Position, data.BaseObject.Direction,
					//							data.Hp, data.Mp, 0, L"");
				}
			}

			template<> void Stage::MoveObject<Player>(int locationFrom, int locationTo)
			{
				std::vector<Player*> l;
				gameStateDatabase->GetAreaObjectRect(info->Locations.at(locationFrom), l);
				for (std::vector<Player*>::iterator i = l.begin(), end=l.end(); i != end; ++i) {
					Player* player = (*i);
					float3 pos = gameStateDatabase->GetAvailableGhostPos(locationPosition.at(locationTo), player->GetRadius());
					player->SetPosition( pos );
					player->SetOverlappable();
					Lunia::XRated::CharacterData data = player->GetCharacterData();
					player->InitHeightInfo();
					if ( player->CheckState(Database::Info::StateInfo::Type::GHOST) ) {
						if ( player->GetPlayerData().Life > 0 ) {
							player->ReduceLife();
							player->Revive( Player::ReviveStatus::Basic, false, 1.0f);
						} else {
							player->SetAction(Database::Info::HashTable::Actions::GhostStand, NULL, true);
						}
					} else {
						player->SetAction(Database::Info::HashTable::Actions::Stand, NULL, true);
					}
				}
			}

			template<> void Stage::MoveObject<NonPlayer>(int locationFrom, int locationTo, uint32 id)
			{
				std::vector<NonPlayer*> l;
				gameStateDatabase->GetAreaObjectRect<NonPlayer>(info->Locations.at(locationFrom), id, l);
				for (std::vector<NonPlayer*>::iterator i = l.begin(), end=l.end(); i != end; ++i) {
					float3 pos = gameStateDatabase->GetAvailableGhostPos(locationPosition.at(locationTo), (*i)->GetRadius());
					(*i)->SetPosition( pos );
					(*i)->SetOverlappable();
					Lunia::XRated::CharacterData data = (*i)->GetCharacterData();
					(*i)->SetInitialPos( data.BaseObject.Position);	// idle type �� stand�� npc�� ���� ó�� ��ġ�� ���ư����� �Ӽ�������.
					(*i)->InitHeightInfo();
					(*i)->SetAction((*i)->GetNPCInfo()->StartActionHash, NULL, true);
				}
			}

			void Stage::MoveObject(int locationFrom, int locationTo, uint16 objectType)
			{
				if ( locationFrom < 0 || locationFrom >= (int)locationPosition.size() ||
					locationTo < 0 || locationTo >= (int)locationPosition.size() )
					return;

				switch (objectType) {
		case 0 : // player
			MoveObject<Player>(locationFrom, locationTo);
			break;
		case 1 : // Nonplayer
			MoveObject<NonPlayer>(locationFrom, locationTo);
			break;
				}
			}

			void Stage::MoveObject(int locationFrom, int locationTo, uint32 id, uint16 objectType)
			{
				if ( locationFrom < 0 || locationFrom >= (int)locationPosition.size() ||
					locationTo < 0 || locationTo >= (int)locationPosition.size() )
					return;

				switch (objectType) {
		case 1 : // Nonplayer
			MoveObject<NonPlayer>(locationFrom, locationTo, id);
			break;
				}
			}

			void Stage::MovePlayer(uint32 serial, int location)
			{
				if ( location < 0 || location >= (int)locationPosition.size() )
					return;

				Player* player = gameStateDatabase->RetrievePlayer(serial);
				if (player) {
					float3 pos = gameStateDatabase->GetAvailableGhostPos(locationPosition[location], player->GetRadius());
					player->SetPosition( pos );
					player->SetOverlap(true);
					Lunia::XRated::CharacterData data = player->GetCharacterData();
					player->InitHeightInfo();
					if ( player->CheckState(Database::Info::StateInfo::Type::GHOST) ) {
						if ( player->GetPlayerData().Life > 0 ) {
							player->ReduceLife();
							player->Revive( Player::ReviveStatus::Basic, false, 1.0f);
							//player->SetAction(Database::Info::HashTable::Actions::Stand, NULL, true); //player->Revive���� �̹� �ϰ� �ִ�.
						} else {
							player->SetAction(Database::Info::HashTable::Actions::GhostStand, NULL, true);
						}
					} else {
						player->SetAction(Database::Info::HashTable::Actions::Stand, NULL, true);
					}
				}
			}

			void Stage::MovePlayer(int location)
			{
				if ( location < 0 || location >= (int)locationPosition.size() )
					return;

				const PlayerMap& players = gameStateDatabase->GetPlayers();
				for (PlayerMap::const_iterator i = players.begin() ; i != players.end() ; ++i) {
					Player* player = (*i).second;
					float3 pos = gameStateDatabase->GetAvailableGhostPos(locationPosition[location], player->GetRadius());
					//Lunia::XRated::CharacterData data = player->GetCharacterData();
					player->SetPosition( pos );
					player->SetOverlap(true);
					player->InitHeightInfo();
					if ( player->CheckState(Database::Info::StateInfo::Type::GHOST) ) {
						if ( player->GetPlayerData().Life > 0 ) {
							player->ReduceLife();
							player->Revive( Player::ReviveStatus::Basic, false, 1.0f);
							//player->SetAction( Database::Info::HashTable::Actions::Stand, NULL, true);//player->Revive���� �̹� �ϰ� �ִ�.
						} else {
							player->SetAction(Database::Info::HashTable::Actions::GhostStand, NULL, true);
						}
					} else {
						player->SetAction(Database::Info::HashTable::Actions::Stand, NULL, true);
					}
				}
			}

			void Stage::ChangePlayerTeam(uint32 userSerial, uint8 team)
			{
				Player* player = gameStateDatabase->RetrievePlayer(userSerial);
				if (player) {
					player->SetTeam(team);
					gameStateDatabase->TeamChange(player->GetSerial(), team);
				}
			}

			void Stage::ChangeNpcTeam(uint32 serial, uint8 team)
			{
				NonPlayer* npc = gameStateDatabase->RetrieveNPC(serial);
				if (npc) {
					npc->SetTeam(team);
					npc->ClearHateList();
					gameStateDatabase->TeamChange(serial, team);
				}
			}

			void Stage::ChangeNpcAI(int location, uint32 id, int idleType, int value)
			{
				std::vector<NonPlayer*> l;
				if ( location < 0 || location >= (int)info->Locations.size() )
					return;
				gameStateDatabase->GetAreaObjectRect(info->Locations[location], id, l);
				std::vector<NonPlayer*>::iterator i = l.begin(), end = l.end();
				for (; i != end ; ++i) {
					switch (idleType) {
			case StageInfo::Npc::IdleStateType::STAND :
				break;
			case StageInfo::Npc::IdleStateType::AREADEFENCE :
				if ( value < 0 || value >= (int)info->Locations.size() )
					return;
				(*i)->SetAIArea(info->Locations[value]);
				break;
			case StageInfo::Npc::IdleStateType::PATHFINDING :
				if ( value < 0 || value >= (int)info->PathGroups.size() )
					return;
				(*i)->SetAIPath(&info->PathGroups[value]);
				break;
			default :
				return;
					}
					(*i)->SetIdleState( (StageInfo::Npc::IdleStateType)idleType );
				}
			}

			void Stage::ChangeNpcAI(uint32 serial, int idleType, int value)
			{
				NonPlayer* npc = gameStateDatabase->RetrieveNPC(serial);
				if (!npc) return;

				switch (idleType) {
		case StageInfo::Npc::IdleStateType::STAND :
			break;
		case StageInfo::Npc::IdleStateType::AREADEFENCE :
			if ( value < 0 || value >= (int)info->Locations.size() )
				return;
			npc->SetAIArea(info->Locations[value]);
			break;
		case StageInfo::Npc::IdleStateType::PATHFINDING :
			if ( value < 0 || value >= (int)info->PathGroups.size() )
				return;
			npc->SetAIPath(&info->PathGroups[value]);
			break;
		default :
			return;
				}
				npc->SetIdleState( (StageInfo::Npc::IdleStateType)idleType );
			}

			void Stage::ChangeNpcSight(uint32 serial, float range)
			{
				NonPlayer* npc = gameStateDatabase->RetrieveNPC(serial);
				if ( npc)
					npc->SetSight(range);
			}

			void Stage::GiveExp(XRated::Constants::ExpAcquiredType type, uint32 userSerial, uint16 level, uint32 exp)
			{
				Player* player = gameStateDatabase->RetrievePlayer(userSerial);
				if ( !player ) {
					//Lunia_INFO(( L"[Stage::GiveExp] Unable to find target player.[{}]", id ));
					return;
				}
				uint16 playerLevel = player->GetLevel();

				uint64 xp = ExpFunctions::Stage::CalculateXpWithLevelGap((float)exp, (int)level-(int)playerLevel);

				player->XpGained( type, (uint64)xp );
			}

			void Stage::GiveExp(XRated::Constants::ExpAcquiredType type, uint16 level, uint32 exp)
			{
				const PlayerMap& players = gameStateDatabase->GetPlayers();
				uint16 lv=0;
				float avgLv;
				PlayerMap::const_iterator i;
				for (i = players.begin() ; i != players.end() ; ++i) {
					Player* player = (*i).second;
					lv = lv + player->GetLevel();
				}
				avgLv = (float)lv / (float)players.size();

				//���� xp���Ϸ� �ް� �Ǹ� ���� xp�� �����Ѵ�(50% ���Ϸδ� ���� �ʰ� ó��).
				float minXp = (static_cast<float>(exp)/2);
				for (i = players.begin() ; i != players.end() ; ++i) {
					Player* player = (*i).second;
					uint16 playerLevel = player->GetLevel();
					double xp = (double)(exp * playerLevel) / avgLv;
					if( xp < minXp ) {
						xp = minXp;
					}

					xp = ExpFunctions::Stage::CalculateXpWithLevelGap(xp, (int)level-(int)playerLevel);
					player->XpGained( type, (uint64)xp );
				}
			}

			void Stage::GiveBattleEXPtoAllPlayer(XRated::Constants::ExpAcquiredType type, uint8 team, uint32 exp)
			{
				const PlayerMap& players = gameStateDatabase->GetPlayers();

				PlayerMap::const_iterator i;

				for (i = players.begin() ; i != players.end() ; ++i) {
					Player* player = (*i).second;
					if ( player->GetTeam() == team )
					{
						uint32 xpWar = DatabaseInstance().InfoCollections.Statuses.GetExp(Info::StatusInfoManager::ExpType::War, player->GetWarLevel()-1);
						player->WarLevelUp(type,exp,xpWar);
					}
				}

			}

			void Stage::AddHp(uint32 id, int delta)
			{
				std::vector<NonPlayer*> l;
				gameStateDatabase->GetObjectbyName<NonPlayer>(id, l);
				std::vector<NonPlayer*>::iterator i = l.begin(), end = l.end();
				for (; i != end ; ++i) {
					CharacterData& data = (*i)->GetCharacterData();
					(*i)->SetHP(data.Hp+delta);
					gameStateDatabase->StatusChanged(data.BaseObject.GameObjectSerial, data.BaseObject.Position, data.BaseObject.Direction,
						data.Hp, data.Mp, 0, 0);
				}
			}

			void Stage::AddMp(uint32 id, int delta)
			{
				std::vector<NonPlayer*> l;
				gameStateDatabase->GetObjectbyName<NonPlayer>(id, l);
				std::vector<NonPlayer*>::iterator i = l.begin(), end = l.end();
				for (; i != end ; ++i) {
					CharacterData& data = (*i)->GetCharacterData();
					(*i)->SetMP(data.Mp+delta);
					gameStateDatabase->StatusChanged(data.BaseObject.GameObjectSerial, data.BaseObject.Position, data.BaseObject.Direction,
						data.Hp, data.Mp, 0, 0);
				}
			}

			void Stage::DealDamage(int location, uint8 team, float dmg, bool bRate)
			{
				if ( location < 0 || location >= (int)info->Locations.size() )
					return;

				std::vector<Actor*> l;
				std::vector<Actor*>::iterator i;
				gameStateDatabase->GetAreaObjectRect(info->Locations[location], l);
				i = l.begin();
				while ( i != l.end() ) {
					// ���� ������ ��󿡼� ���ܽ�ų����~
					if ( (*i)->GetType() == Constants::ObjectType::NonPlayer && static_cast<NonPlayer*>(*i)->IsPetType() == true )
					{
						++i;
						continue;
					}

					if ( (*i)->GetTeam() == team ) {
						float max, hp;
						hp = (*i)->GetHP(max);
						if ( bRate ) //��ü ü�¿� %��
							(*i)->SetHP(hp - max*dmg);
						else //���뷮����
							(*i)->SetHP(hp-dmg);
						CharacterData& data = (*i)->GetCharacterData();
						gameStateDatabase->StatusChanged(data.BaseObject.GameObjectSerial, data.BaseObject.Position, data.BaseObject.Direction,
							data.Hp, data.Mp, 0, 0);
					}
					++i;
				}
			}

			void Stage::DealDamage(int location, uint32 hash, float dmg, bool bRate)
			{
				if ( location < 0 || location >= (int)info->Locations.size() )
					return;

				std::vector<Actor*> l;
				std::vector<Actor*>::iterator i;
				gameStateDatabase->GetAreaObjectRect(info->Locations[location], l);
				i = l.begin();
				while ( i != l.end() ) {
					// ���� ������ ��󿡼� ���ܽ�ų����~
					if ( (*i)->GetType() == Constants::ObjectType::NonPlayer && static_cast<NonPlayer*>(*i)->IsPetType() == true )
					{
						++i;
						continue;
					}

					if ( (*i)->GetNameHash() == hash ) {
						float max, hp;
						hp = (*i)->GetHP(max);
						if ( bRate ) //��ü ü�¿� %��
							(*i)->SetHP(hp - max*dmg);
						else //���뷮����
							(*i)->SetHP(hp-dmg);
						CharacterData& data = (*i)->GetCharacterData();
						gameStateDatabase->StatusChanged(data.BaseObject.GameObjectSerial, data.BaseObject.Position, data.BaseObject.Direction,
							data.Hp, data.Mp, 0, 0);
					}
					++i;
				}
			}

			void Stage::ChangeNPCAction( uint32 serial, uint32 action)
			{
				NonPlayer* npc = gameStateDatabase->RetrieveNPC(serial);
				if ( npc && !npc->IsOnAirState() ) {		//Script���� NPC�� Action�� ������ �ٲ܋���.. ���߿� �� �ִ� NPC�� �����ϰ� �ٲ��ش�.(������� ����)	
					npc->SetAction(action, NULL, true);
				}
			}

			void Stage::ChangePlayerStatusRate( std::vector<uint32>* statusId, const std::vector<float>& rate )
			{
				const PlayerMap& players = gameStateDatabase->GetPlayers();
				for (PlayerMap::const_iterator i = players.begin() ; i != players.end() ; ++i) {
					Player* player = (*i).second;
					player->ChangePlayerStatusRate(statusId, rate);
					gameStateDatabase->ChangePlayerStatusRate(gameStateDatabase->RetrievePlayer(player->GetSerial()), rate);
				}	
				// i can call here kekw'
			}

			void Stage::ChangePlayerStatusLimit( std::vector<uint32>* statuses, const std::vector<uint32>& limits )
			{
				const PlayerMap& players = gameStateDatabase->GetPlayers();
				for (PlayerMap::const_iterator i = players.begin() ; i != players.end() ; ++i) {
					Player* player = (*i).second;
					player->ChangePlayerStatusLimit(statuses, limits);
					gameStateDatabase->ChangePlayerStatusLimit(gameStateDatabase->RetrievePlayer(player->GetSerial()), limits);
				}	
			}

			void Stage::ChangeNPCAction(int location, uint32 hash, uint32 action)
			{
				if ( location < 0 || location >= (int)info->Locations.size() )
					return;

				std::vector<NonPlayer*> l;
				gameStateDatabase->GetAreaObjectRect<NonPlayer>(info->Locations[location], hash, l);
				std::vector<NonPlayer*>::iterator i = l.begin(), end = l.end();
				for (; i != end ; ++i) {	
					if (!(*i)->IsOnAirState() )
						(*i)->SetAction(action, NULL, true);
				}
			}

			void Stage::ChangeMISCState(uint32 id, uint32 type, bool bAdd)
			{
				if ( gameStateDatabase->IsUpdateMISC() ) {
					Logger::GetInstance().Info( L"[Stage::ChangeMISCState] Error!! MISC list is updating. unable to change object state [{0}/{1}].", this->id, id );
					return;
				}
				if ( type != Database::Info::StateInfo::Type::INVINCIBLE )
					return;

				std::vector<Object*> l;
				gameStateDatabase->GetObjectbyName<Object>(id, l);
				std::vector<Object*>::iterator i = l.begin(), end = l.end();
				for (; i != end ; ++i) {
					switch ( (*i)->GetType() ) {
			case Constants::ObjectType::Misc :
				if ( bAdd ) {
					(*i)->AddState( new Stat::Invincible(*i) );
				} else {
					Stat::IState* state = (*i)->GetState(Database::Info::StateInfo::Type::INVINCIBLE, Database::Info::StateInfo::Sort::ORDINARYSTATE);
					if ( state )
						(*i)->RemoveState(Database::Info::StateInfo::Sort::ORDINARYSTATE, state->GetId());
				}
				break;
					}
				}
			}

			void Stage::ChangeMISCState(uint32 id, int location, uint32 type, bool bAdd)
			{
				if ( gameStateDatabase->IsUpdateMISC() ) {
					Logger::GetInstance().Info( L"[Stage::ChangeMISCState] Error!! MISC list is updating. unable to change object state [{0}/{1}].", this->id, id );
					return;
				}
				if ( type != Database::Info::StateInfo::Type::INVINCIBLE ) 
					return;
				if ( location < 0 || location >= (int)info->Locations.size() )
					return;

				std::vector<Object*> l;
				gameStateDatabase->GetAreaObjectRect<Object>(info->Locations[location], id, l);
				std::vector<Object*>::iterator i = l.begin(), end = l.end();
				for (; i != end ; ++i) {
					switch ( (*i)->GetType() ) {
			case Constants::ObjectType::Misc :
				if ( bAdd ) {
					(*i)->AddState( new Stat::Invincible(*i) );
				} else {
					Stat::IState* state = (*i)->GetState(Database::Info::StateInfo::Type::INVINCIBLE, Database::Info::StateInfo::Sort::ORDINARYSTATE);
					if ( state )
						(*i)->RemoveState(Database::Info::StateInfo::Sort::ORDINARYSTATE, state->GetId());
				}
				break;
					}
				}
			}

			void Stage::ChangeNPCState(uint32 id, uint32 type, bool bAdd)
			{
				if ( gameStateDatabase->IsUpdateNPC() ) {
					Logger::GetInstance().Info( L"[Stage::ChangeNPCState] Error!! NPC list is updating. unable to change object state [{0}/{1}].", this->id, id );
					return;
				}
				if ( type != Database::Info::StateInfo::Type::INVINCIBLE )
					return;

				std::vector<Object*> l;
				gameStateDatabase->GetObjectbyName<Object>(id, l);
				std::vector<Object*>::iterator i = l.begin(), end = l.end();
				for (; i != end ; ++i) {
					switch ( (*i)->GetType() ) {
			case Constants::ObjectType::NonPlayer :
				if ( bAdd ) {
					(*i)->AddState( new Stat::Invincible(*i) );
				} else {
					Stat::IState* state = (*i)->GetState(Database::Info::StateInfo::Type::INVINCIBLE, Database::Info::StateInfo::Sort::ORDINARYSTATE);
					if ( state )
						(*i)->RemoveState(Database::Info::StateInfo::Sort::ORDINARYSTATE, state->GetId());
				}
				break;
					}
				}
			}

			void Stage::ChangeNPCState(uint32 id, int location, uint32 type, bool bAdd)
			{
				if ( gameStateDatabase->IsUpdateNPC() ) {
					Logger::GetInstance().Info( L"[Stage::ChangeNPCState] Error!! NPC list is updating. unable to change object state [{0}/{1}].", this->id, id );
					return;
				}
				if ( type != Database::Info::StateInfo::Type::INVINCIBLE )
					return;
				if ( location < 0 || location >= (int)info->Locations.size() )
					return;

				std::vector<Object*> l;
				gameStateDatabase->GetAreaObjectRect<Object>(info->Locations[location], id, l);
				std::vector<Object*>::iterator i = l.begin(), end = l.end();
				for (; i != end ; ++i) {
					switch ( (*i)->GetType() ) {
			case Constants::ObjectType::NonPlayer :
				if ( bAdd )
					(*i)->AddStateFlag( (Database::Info::StateInfo::Type)type );
				else
					(*i)->RemoveStateFlag( (Database::Info::StateInfo::Type)type );
				break;
					}
				}
			}

			uint32 Stage::CheckMinimumPlayerCount()  // 3.1 things
			{
				return uint32(info->MinimumPlayerCount);
			}	

			void Stage::CheckStateBundleForAchievement( uint32 stateBundleHash)  // 3.1 things
			{
				Database::Info::StateBundleInfo* info = DatabaseInstance().InfoCollections.StateBundles.Retrieve( stateBundleHash );

				if( info != NULL )
				{
					gameStateDatabase->CheckStateBundleForAchievement( stateBundleHash );
				}
			}			

			void Stage::GiveStateBundleToObject( uint32 objectSerial, uint32 id )
			{
				Actor* actor = gameStateDatabase->RetrieveActor( objectSerial );

				if( actor != NULL )
				{
					Database::Info::StateBundleInfo* info = DatabaseInstance().InfoCollections.StateBundles.Retrieve( id );

					if( info != NULL )
					{
						actor->AddStateBundle( info, 0, 0 );
					}
				}
			}

			///* Quest */
			uint8 Stage::GetQuestState(uint32 userSerial, uint32 questHash)
			{
				return gameStateDatabase->GetQuestState(gameStateDatabase->RetrievePlayer(userSerial), questHash);
			}

			uint8 Stage::SetQuestState(uint32 userSerial, uint32 questHash, uint8 newState)
			{
				return gameStateDatabase->SetQuestState(gameStateDatabase->RetrievePlayer(userSerial), questHash, newState);
			}

			uint32 Stage::GetQuestParameter(uint32 userSerial, uint32 questHash, uint8 paramIndex)
			{
				return gameStateDatabase->GetQuestParameter(gameStateDatabase->RetrievePlayer(userSerial), questHash, paramIndex);
			}

			uint32 Stage::SetQuestParameter(uint32 userSerial, uint32 questHash, uint8 paramIndex, uint32 newValue)
			{
				return gameStateDatabase->SetQuestParameter(gameStateDatabase->RetrievePlayer(userSerial), questHash, paramIndex, newValue);
			}

			///* output */
			void Stage::DisplayTextEvent(Constants::DisplayTextType displayTo, uint16 textId, float param)
			{
				gameStateDatabase->DisplayTextEvent(displayTo, textId, param);
			}

			void Stage::NoticeTextEvent(uint32 userSerial, Constants::DisplayTextType displayTo, uint16 textId, float param)
			{
				gameStateDatabase->NoticeTextEvent(gameStateDatabase->RetrievePlayer(userSerial), displayTo, textId, param);
			}

			void Stage::DisplayText(Constants::DisplayTextType displayTo, const std::wstring& text)
			{
				if (displayTo == XRated::Constants::Debug)
				{
					Logger::GetInstance().Warn(L"stage({0}), debug message : {1}", info->Id.c_str(), text.c_str());
				}
				else
				{
					gameStateDatabase->DisplayText(displayTo, text);
				}
			}

			void Stage::NoticeText(uint32 userSerial, Constants::DisplayTextType displayTo, const std::wstring& text)
			{
				gameStateDatabase->NoticeText(gameStateDatabase->RetrievePlayer(userSerial), displayTo, text);
			}

			void Stage::DisplayTimer(int timer, Constants::DisplayTimerType type)
			{
				gameStateDatabase->DisplayTimer(type, timers.at(timer).time);
			}

			void Stage::BroadcastEvent(int eventId)
			{
				gameStateDatabase->BroadcastEvent(eventId, L"");
			}

			void Stage::NoticeEvent(uint32 userSerial, int eventId)
			{
				gameStateDatabase->NoticeEvent(gameStateDatabase->RetrievePlayer(userSerial), eventId, L"");
			}

			void Stage::NoticeClientStageEvent( uint32 userSerial, int eventId, int eventParam )
			{
				gameStateDatabase->NoticeClientStageEvent( gameStateDatabase->RetrievePlayer( userSerial ), eventId, eventParam );
			}

			void Stage::AddMinimapPing(int pingId, int x, int y, Constants::MiniMapPingType type)
			{
				gameStateDatabase->AddMinimapPing(pingId, float3((float)x, 0, (float)y), type);
			}

			void Stage::NoticeAddMinimapPing(int pingId, int x, int y, Constants::MiniMapPingType type, uint32 userSerial)
			{
				gameStateDatabase->NoticeAddMinimapPing(pingId, float3((float)x, 0, (float)y), type, gameStateDatabase->RetrievePlayer( userSerial ));
			}

			void Stage::RemoveMinimapPing(int pingId)
			{
				gameStateDatabase->RemoveMinimapPing(pingId);
			}

			void Stage::GiveLicense(uint32 licenseGroup, uint32 accessLevel)
			{
				gameStateDatabase->GiveLicense(NULL, licenseGroup, accessLevel);
			}

			void Stage::GiveLicenseToPlayer(uint32 userSerial, uint32 licenseGroup, uint32 accessLevel)
			{
				gameStateDatabase->GiveLicense( gameStateDatabase->RetrievePlayer(userSerial), licenseGroup, accessLevel );
			}

			void Stage::GiveCharacterLicense(uint32 userSerial, uint32 characterId)
			{
				gameStateDatabase->GiveCharacterLicense(gameStateDatabase->RetrievePlayer(userSerial), characterId);
			}

			void Stage::NoticeStylePointState(XRated::StylePoint::State state)
			{
				gameStateDatabase->NoticeStylePointState(state);
			}

			void Stage::GambleGameEnd(std::vector<uint8> rankList)
			{
				gameStateDatabase->GambleGameEnd(rankList);
			}

			int Stage::GetSlimeRaceIntervalInMin() const
			{
				return gameStateDatabase->GetSlimeRaceIntervalInMin();
			}

			void Stage::GambleNextGame(uint32 userSerial, float seconds)
			{
				if ( userSerial == 0 ) //Broadcasting
					gameStateDatabase->GambleNextGame(seconds);
				else
					gameStateDatabase->GambleNextGame(gameStateDatabase->RetrievePlayer(userSerial), seconds);
			}

			/* Holiday Event */
			bool Stage::IsHolidayEventTime(uint32 eventId)
			{
				return gameStateDatabase->IsHolidayEventTime(eventId);
			}

			uint32 Stage::GetHolidayEventNpcHash(uint32 eventId, uint32 oldNpcHash)
			{
				return gameStateDatabase->GetHolidayEventNpcHash(eventId, oldNpcHash);
			}

			void Stage::ChangeWeatherToNone()
			{
				gameStateDatabase->ChangeWeatherToNone();
			}

			void Stage::ChangeWeatherToClear()
			{
				gameStateDatabase->ChangeWeatherToClear();
			}

			void Stage::ChangeWeatherToRain()
			{
				gameStateDatabase->ChangeWeatherToRain();
			}

			void Stage::ChangeWeatherToSnow()
			{
				gameStateDatabase->ChangeWeatherToSnow();
			}

			void Stage::ChangeWeatherToAqua()
			{
				gameStateDatabase->ChangeWeatherToAqua();
			}
		}
	}
}


