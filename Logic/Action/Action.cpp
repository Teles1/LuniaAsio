#include <Core/Utils/StringUtil.h>
#include <Info/Info/InfoHelper.h>
#include "../State/States.h"
#include "_ActionBase.h"
#include "ActionAttack.h"
#include "ActionDown.h"
#include "ActionFaint.h"
#include "ActionGhostRise.h"
#include "ActionMove.h"
#include "ActionPhoenixDownAir.h"
#include "ActionPhoenixDown2Air.h"
#include "ActionCenterMove.h"
#include "ActionSleep.h"
#include "ActionStand.h"
#include "ActionStun.h"
#include "NPCs/ActionTurn_Boss.h"

#include "../GameState.h"
#include "../Object/Player.h"
#include "../LogicDB/LogicDB.h"

using namespace Lunia::XRated::Logic::Stat;
using namespace Lunia::XRated::Database;
using namespace Lunia::XRated::Database::Info;

namespace Lunia 
{ 
	namespace XRated 
	{ 
		namespace Logic 
		{ 
			namespace Action 
			{

				Action::Action( Lunia::XRated::Database::Info::ActionInfo* i ) 
					: info( i )
				{
					std::vector< ActionInfo::State >::iterator iter = i->states.begin();
					while ( iter != i->states.end() ) 
					{
						if ( iter->startTime != iter->endTime ) 
						{
							switch ( iter->type ) 
							{
							case StateInfo::Type::AREAHEAL :
							case StateInfo::Type::ATTACK :
							case StateInfo::Type::CHANGEACTION :
							case StateInfo::Type::CHARGE :
							case StateInfo::Type::CLEAR :
							case StateInfo::Type::CREATENPC :
							case StateInfo::Type::CREATEOBJECT :
							case StateInfo::Type::CREATEPROJECTILE :
							case StateInfo::Type::DAMAGE :
							case StateInfo::Type::DESTROYENEMY :
							case StateInfo::Type::INCLIFE :
							case StateInfo::Type::KNOCKBACK :
							case StateInfo::Type::MAKESTUN:
							case StateInfo::Type::RESETCOOLDOWN :
							case StateInfo::Type::RESURRECTION :
							case StateInfo::Type::SELFHEAL :
							case StateInfo::Type::SLEEP :
							case StateInfo::Type::TAUNT :
								Logger::GetInstance().Warn( L"[Action::Action] [{0}/{1}] This state type should have same start time and end time. [{2}]", i->actionName.c_str(), ( int )iter->type, i->animationName.c_str() );
								++iter;
								continue;
							}
						}
						Stat::IState* state = CreateState< Player >( iter->type, NULL, &iter->params, &iter->optionalStates );

						if ( !state )
						{
							throw; // should not be null value
						}

						stateList.push_back( state );
						++iter;
					}
				}

				Action::~Action()
				{
					std::vector< Stat::IState* >::iterator iter = stateList.begin();
					while ( iter != stateList.end() ) 
					{
						delete ( *iter );
						++iter;
					}
					// stageList.clear() is not necessary because Action class cannot copy-constructed by private modifier.
				}

				bool Action::IsPossible( uint32 name, float, Actor*, ActionData& ) const
				{
					Info::ActionInfo::ActionList::iterator iter;
					Info::ActionInfo::ActionList::iterator iterEnd = info->linkedActions.end();
					for ( iter = info->linkedActions.begin() ; iter != iterEnd; ++iter )
					{
						if ( name == iter->hash )
						{
							return true;
						}
					}

					if ( !info->linkedActionsWhenHit.empty() ) 
					{ 
						iterEnd = info->linkedActionsWhenHit.end();
						for ( iter = info->linkedActionsWhenHit.begin(); iter != iterEnd; ++iter )
						{
							if ( name == iter->hash )
							{
								return true;
							}
						}
					}
					return false;
				}

				const Database::Info::ActionInfo* Action::GetLinkedAction( Actor* actor, unsigned int key, float dt, bool &reserved, float& changeTime, bool bHit )
				{
					const Database::Info::ActionInfo* returnValue = nullptr;

					Info::ActionInfo::ActionList::iterator iter;
					Info::ActionInfo::ActionList::iterator iterEnd;

//					dt *= actor->GetAttackSpeed();

					if ( bHit && !info->linkedActionsWhenHit.empty() ) 
					{ 
						iterEnd = info->linkedActionsWhenHit.end();
						for ( iter = info->linkedActionsWhenHit.begin(); iter != iterEnd; ++iter ) 
						{
							if ( iter->in.key & key ) 
							{	
								if ( ( iter->in.startTime <= dt ) && ( ( iter->in.endTime < 0 ) || ( iter->in.endTime > dt ) ) ) 
								{
									returnValue = iter->info;
									if ( iter->changeTime > dt ) 
									{	
										reserved = true;
										changeTime = iter->changeTime;
									} 
									else 
									{
										reserved = false;
									}
									break;
								}
							}
						}
					}

					iterEnd = info->linkedActions.end();
					for ( iter = info->linkedActions.begin(); iter != iterEnd; ++iter ) 
					{
						if ( iter->in.key & key ) 
						{
							if ( ( iter->in.startTime <= dt ) && ( ( iter->in.endTime < 0 ) || ( iter->in.endTime > dt ) ) )
							{
								returnValue = iter->info;
								if ( iter->changeTime > dt ) 
								{
									reserved = true;
									changeTime = iter->changeTime;
								} 
								else 
								{
									reserved = false;
								}
								break;
							}
						}
					}
					return returnValue;
				}

				uint32 Action::GetNextAction( Actor*, IActionParam*&, bool bHit )
				{
					if ( bHit && !info->linkedActionsWhenHit.empty() && ( info->linkedActionsWhenHit[ 0 ].in.startTime == -1 ) )
					{
						return info->linkedActionsWhenHit[ 0 ].hash;
					}

					for ( std::vector< Lunia::XRated::Database::Info::ActionInfo::LinkedAction >::iterator iter = info->linkedActions.begin(); iter != info->linkedActions.end(); ++iter )
					{
						if ( iter->in.startTime < 0 )
						{
							return iter->hash;
						}
					}
					return 0;
				}

				void Action::Initialize( Actor* obj, IGameStateDatabase* /*db*/, IActionParam* /*param*/, ActionData& data )
				{
					data.passedTime = 0;
					data.bHit = false;
					int cnt=0;
					for ( Lunia::XRated::Database::Info::ActionInfo::StateList::const_iterator iter = info->states.begin(); iter != info->states.end(); ++iter, ++cnt ) 
					{
						if ( iter->startTime == 0 ) 
						{
							Logic::Stat::IState *state;
							if ( ( int )stateList.size() <= cnt ) 
							{
								Logger::GetInstance().Info( L"Error [Action::Initialize] Unable to create state instance.({0})", info->actionName.c_str() );
								continue;
							}
							state = Stat::CreateState( stateList[ cnt ], obj );
							if ( !state ) 
							{
								Logger::GetInstance().Info( L"Error [Action::Initialize] Unable to create state instance.({0})", info->actionName.c_str() );
								continue;
							}
							obj->AddState( state );
							if ( iter->startTime != iter->endTime )
							{
								data.bufferedStates.push_back( BufferedState( state, state->GetSort(), iter->endTime, state->GetId() ) );
							}
						}
					}
				}

				bool Action::Do( float dt, Actor* obj, IGameStateDatabase* db, ActionData& data )
				{
					{
						BufferedStateList::iterator iter;
						for (iter = data.bufferedStates.begin(); iter != data.bufferedStates.end(); )
						{
							if ((iter->endTime >= 0) && (iter->endTime < (data.passedTime + dt)))
							{
								obj->RemoveState(iter->sort, iter->id);
								iter = data.bufferedStates.erase(iter);
							}
							else
							{
								++iter;
							}
						}
					}

					{
						int cnt=0;
						for ( auto& iter : info->states)
						{
							if ( ( iter.startTime > data.passedTime ) && ( iter.startTime <= ( data.passedTime + dt ) ) )
							{
								Logic::Stat::IState *state;
								if ( ( int )stateList.size() <= cnt ) 
								{
									Logger::GetInstance().Error( L"out of range state in action.({0})", info->actionName.c_str() );
									continue;
								}
								state = Stat::CreateState( stateList[ cnt ], obj );
								//state = Stat::CreateState<Actor>((*iter).type, obj, &(*iter).params);
								obj->AddState( state );
								if ( iter.startTime != iter.endTime )
								{
									data.bufferedStates.push_back( BufferedState( state, state->GetSort(), iter.endTime, state->GetId() ) );
								}
							}
						}
					}

					{
						//Projectile
						for (auto& iter : info->projectiles)
						{
							if ( ( iter.startTime > data.passedTime ) && ( iter.startTime <= data.passedTime + dt ) ) 
							{
								float min, max;
								obj->GetDamage( min, max );
								db->CreateProjectile( iter.hash, iter.position, iter.fireAngle, iter.type, obj, min, max, iter.toTarget, iter.createTime, obj->GetSerial() );
							}
						}
					}

					{
						//StateBundle
						for ( auto& iter : info->stateBundleConditions)
						{
							if ( ( iter.startTime > data.passedTime ) && ( iter.startTime <= ( data.passedTime + dt ) ) ) 
							{
								obj->ProcessStateBundle( iter );
							}
						}
						////////////////////////////////////////////////////////////////////////////////////////
					}


					data.passedTime += dt;
					if ( ( info->playTime >= 0 ) && ( data.passedTime >= info->playTime ) )
					{
						return true;
					}
					return false;
				}

				void Action::Destroy( Actor* obj, ActionData& data )
				{
					for (auto& i : data.bufferedStates)
					{
						obj->RemoveState( i.sort, i.id );
					}
					data.bufferedStates.clear();

					int cnt=0;
					for ( auto& iter : info->states)
					{
						if ( ( iter.startTime == -1 ) && ( iter.endTime == -1 ) )
						{
							Stat::IState *state;
							if ( ( int )stateList.size() <= cnt ) 
							{
								Logger::GetInstance().Error( L"out of range state in action.({0})", info->actionName.c_str() );
								continue;
							}
							state = Stat::CreateState( stateList[ cnt ], obj );
							if ( !state ) 
							{
								Logger::GetInstance().Error( L"[Action::Destroy] unable to create state. [{0}]", info->hash );
								continue;
							}
							obj->AddState( state );
						}
					}
				}

				void ActionManager::Init( const std::wstring& objName,  const std::wstring& defaultAction, Action::ActionPtrList* list )
				{
					currentAction = NULL;
					objectName = objName;
					actionList = list;

					if ( !actionList ) 
					{
						Logger::GetInstance().Exception( L"[ActionManager::Init] actionList is Null. ({0})", objName.c_str() );
					}

					Action* action;
					if ( defaultAction.empty() )
					{
						action = GetAction( Database::Info::HashTable::Actions::Stand );
					}
					else 
					{
						action = GetAction( defaultAction.c_str() );
					}

					if ( !action ) 
					{
						Logger::GetInstance().Exception( L"[ActionManager::Init] unable to find initial action. ({0}/{1})", objName.c_str(), defaultAction.c_str() );
					}
					currentAction = action;
					actionStartTime = 0;
					actionData.Init();
					actionData.length = action->GetActionInfo()->playTime;
					pauseTime = 0;
				}


				Action::ActionPtrList* ActionManager::SetActionList( Actor* object, IGameStateDatabase& sd, Action::ActionPtrList* newActionList, uint32 defaultAction )
				{
					Action::ActionPtrList* returnValue = actionList;

					if ( currentAction )
					{
						currentAction->Destroy(object, actionData);
					}

					actionList = newActionList;

					Action* action = GetAction( defaultAction );
					if ( !action ) 
					{
						Logger::GetInstance().Exception( L"unable to find initial action. ({0}/{1})", objectName.c_str(), defaultAction );
					}
					currentAction = action;

					//currentAction->Initialize(object, &sd, actionData.actionSpecificParams, actionData);

					actionStartTime = 0;
					actionData.Init();
					actionData.length = action->GetActionInfo()->playTime;
					CharacterData& data = object->GetCharacterData();
					data.ActionName = defaultAction;

					object->GetHeightInfo().Init();
					object->SetHeight( 0 );
					data.Speed = 0;

					if ( object->IsCollide() ) 
					{
						object->ChangeCollisionState();
						sd.CollisionStateChanged( object->GetSerial(), object->IsCollide(), object->GetPosition() );
					}
					return returnValue;
				}

				Action* ActionManager::GetAction( const wchar_t* name )
				{
					return GetAction( StringUtil::Hash( name ) );
				}

				Action* ActionManager::GetAction( uint32 hash )
				{
					if ( !hash )
					{
						return currentAction;
					}

					Action::ActionPtrList::iterator i = actionList->begin();
					for ( ; i != actionList->end(); ++i )
					{
						if ( ( *i )->hashCode == hash )
						{
							return ( *i )->action;
						}
					}
					return NULL;
				}

				void ActionManager::ReserveAction( uint32 name, float startTime, Action::IActionParam* param, bool force )
				{
					if ( actionData.reserved.bExist ) 
					{
						if ( actionData.reserved.bForce && !force )
						{
							return;
						}
						RemoveReservedAction();
					}
					actionData.reserved.bExist = true;
					actionData.reserved.actionName = name;
					actionData.reserved.startTime = startTime;
					if ( param )
					{
						actionData.reserved.param = param->GetClonePtr();
					}
					else
					{
						actionData.reserved.param = NULL;
					}
					actionData.reserved.bForce = force;
				}

				void ActionManager::RemoveReservedAction()
				{
					actionData.reserved.Clear();
				}

#pragma warning(disable : 4189)
				bool ActionManager::SetAction( uint32 name, Actor* object, Action::IActionParam* param, IGameStateDatabase& sd, bool force )
				{
					Action* action = GetAction( name );
					if ( !action ) 
					{

						Logger::GetInstance().Info( L"[ActionManager::SetAction] [{0}] action [{1}] not found.", object->GetName().c_str(), name );

						return false;
					}

					if ( !currentAction ) 
					{
						Logger::GetInstance().Exception(L"[ActionManager::SetAction] current action is null");
					}

					if ( !force && !currentAction->IsPossible( name, actionStartTime, object, actionData ) )
					{
						return false;
					}
					if ( pauseTime == -1 )/*Frozen*/
					{
						return false;
					}

					if ( pauseTime > 0 /*puased*/) 
					{
						if ( param ) 
						{
							ReserveAction( name, 0, param, force );
						} 
						else
						{
							ReserveAction( name, 0, NULL, force );
						}

						return true;
					}

					//if (object && object->IsOnAirState() ) {
					//	Lunia_IMPORTANT((L"[ActionManager::SetAction  On AirState] [{}] action [{}]", object->GetName().c_str(), action->GetName().c_str() ));
					//}

					currentAction->Destroy( object, actionData );
					Action* oldAction = currentAction;
					currentAction = action;
					if ( actionData.actionSpecificParams )
					{
						delete actionData.actionSpecificParams; // actionData.actionSpecificParams is gonna set newly soon.
					}

					if ( param )
					{
						actionData.actionSpecificParams = param->GetClonePtr();
					}
					else
					{
						actionData.actionSpecificParams = NULL;
					}
					try {
						currentAction->Initialize( object, &sd, actionData.actionSpecificParams, actionData );
					}
					catch(...)
					{
						//stageName
						Database::Info::StageInfo* stageInfo = sd.GetStageInfo();
						if( stageInfo != NULL )
						{
							Logger::GetInstance().Exception( L"Stage - {0}({1})" , stageInfo->Id.c_str() , stageInfo->Title.c_str() );
						}
						//oldAction
						if( oldAction != NULL )
						{
							Logger::GetInstance().Exception( L"OldAction - {0}({1})" , oldAction->GetName().c_str() , oldAction->GetNameHash() );
						}
						//currentAction
						if( currentAction != NULL )
						{
							Logger::GetInstance().Exception( L"CurrentAction - {0}({1})" , currentAction->GetName().c_str() , currentAction->GetNameHash() );
						}
						//Object
						if( object != NULL )
						{
							float3 position = object->GetPosition();
							Logger::GetInstance().Exception( L"Object - {0}(Hash:{1},Type:{2}) , ({3},{4},{5})" , object->GetName().c_str() , object->GetNameHash() , static_cast<int>( object->GetType() ) , position.x , position.y , position.z );
						}

						throw;
					}

					CharacterData& data = object->GetCharacterData();
					sd.AnimationChanged( data.BaseObject.GameObjectSerial, name, data.BaseObject.Position, data.BaseObject.Direction, object->GetHeightInfo().speed );

					data.ActionName = name;
					data.Speed = object->GetHeightInfo().speed;


					if ( object->IsCollide() ) 
					{
						object->ChangeCollisionState();
						sd.CollisionStateChanged( object->GetSerial(), object->IsCollide(), object->GetPosition() );
					}
					actionStartTime = 0;

					return true;
				}
#pragma warning( default : 4189 )

				bool ActionManager::StopAction( Actor* object )
				{
					if( !currentAction )
					{
						return false;
					}
					currentAction->Destroy( object, actionData );
					currentAction = NULL;
					return true;
				}

				unsigned int ActionManager::GetRelativeDirection( Actor* obj, uint32 direction )
				{
					float3 dir = obj->GetDirection();
					if ( ( dir.x < 0 ) && ( dir.z < 0 ) )
					{
						switch ( direction ) 
						{
						case 1 : return 8;
						case 2 : return 7;
						case 3 : return 4;
						case 4 : return 9;
						case 6 : return 1;
						case 7 : return 6;
						case 8 : return 3;
						case 9 : return 2;
						}
					} 
					else if ( ( dir.z < 0 ) && ( dir.x == 0 ) )
					{
						switch ( direction ) 
						{
						case 1 : return 9;
						case 2 : return 8;
						case 3 : return 7;
						case 4 : return 6;
						case 6 : return 4;
						case 7 : return 3;
						case 8 : return 2;
						case 9 : return 1;
						}
					} 
					else if ( ( dir.x > 0 ) && ( dir.z < 0 ) )
					{
						switch ( direction ) 
						{
						case 1 : return 6;
						case 2 : return 9;
						case 3 : return 8;
						case 4 : return 3;
						case 6 : return 7;
						case 7 : return 2;
						case 8 : return 1;
						case 9 : return 4;
						}
					} 
					else if ( ( dir.x < 0 ) && ( dir.z == 0 ) )
					{
						switch ( direction ) 
						{
						case 1 : return 7;
						case 2 : return 4;
						case 3 : return 1;
						case 4 : return 8;
						case 6 : return 2;
						case 7 : return 9;
						case 8 : return 6;
						case 9 : return 3;
						}
					} 
					else if ( ( dir.x > 0 ) && ( dir.z == 0 ) )
					{
						switch ( direction ) 
						{
						case 1 : return 3;
						case 2 : return 6;
						case 3 : return 9;
						case 4 : return 2;
						case 6 : return 8;
						case 7 : return 1;
						case 8 : return 4;
						case 9 : return 7;
						}
					}
					else if ( ( dir.x < 0 ) && ( dir.z > 0 ) )
					{
						switch ( direction ) 
						{
						case 1 : return 4;
						case 2 : return 1;
						case 3 : return 2;
						case 4 : return 7;
						case 6 : return 3;
						case 7 : return 8;
						case 8 : return 9;
						case 9 : return 6;
						}
					} 
					else if ( ( dir.x == 0 ) && ( dir.z > 0 ) )
					{
						switch ( direction ) 
						{
						case 1 : return 1;
						case 2 : return 2;
						case 3 : return 3;
						case 4 : return 4;
						case 6 : return 6;
						case 7 : return 7;
						case 8 : return 8;
						case 9 : return 9;
						}
					} 
					else if ( ( dir.x > 0 ) && ( dir.z > 0 ) )
					{
						switch ( direction ) 
						{
						case 1 : return 2;
						case 2 : return 3;
						case 3 : return 6;
						case 4 : return 1;
						case 6 : return 9;
						case 7 : return 4;
						case 8 : return 7;
						case 9 : return 8;
						}
					}
					return 0;
				}

				bool ActionManager::IsAbleToCast( uint32 skillGroupHash, Constants::GameTypes gameType )
				{
					const Database::Info::ActionInfo* actionInfo = currentAction->GetActionInfo();

					if ( pauseTime == -1 ) 
					{			
						return false;
					}
					switch (actionInfo->useSkill)
					{
					case 0://stage x / PVP x
						{

						}
						break;
					case 1://stage o / PVP o
						{
							return true;
						}
						break;
					case 2://stage o / PVP x
						{
							if (!(gameType & XRated::Constants::GameTypes::PvpGameTypeMask))
								return true;
						}
						break;
					case 3://stage x / PVP o
						{
							if (gameType & XRated::Constants::GameTypes::PvpGameTypeMask)
								return true;
						}
						break;
					default:
						return false;
					}
					//if ( actionInfo->bUseSkill )
					//{
					//	return true;
					//}
					if ( actionInfo->linkedSkills.empty() )
					{
						return false;
					}

					Database::Info::ActionInfo::ActionList::const_iterator i = actionInfo->linkedSkills.begin();
					Database::Info::ActionInfo::ActionList::const_iterator iEnd = actionInfo->linkedSkills.end();
					for ( ; i != iEnd; ++i ) 
					{
						if ( i->hash == skillGroupHash ) 
						{
							if ( ( actionStartTime > i->in.startTime ) && ( ( i->in.endTime < 0 ) || ( actionStartTime <= i->in.endTime ) ) )
							{
								return true;
							}
							else
							{
								return false;
							}
						}
					}
					return false;
				}


				bool ActionManager::Command( Action::CommandType state, Actor* obj, IGameStateDatabase& sd, uint32 param )
				{
					if ( !currentAction ) 
					{
						Logger::GetInstance().Exception( L"[ActionManager::Command] current action not found." );
					}

					unsigned int key = 0;

					//command parsing
					unsigned int direction = GetRelativeDirection( obj, param );
					
					float atkRate = 1.0f;
					
					switch( state ) 
					{
					case Action::CommandType::MOVE :
						switch ( direction ) 
						{
						case 1 :
							key = ActionInfo::INPUT::LB; 
							break;
						case 2 :
							key = ActionInfo::INPUT::B; 
							break;
						case 3 :
							key = ActionInfo::INPUT::RB; 
							break;
						case 4 :
							key = ActionInfo::INPUT::L; 
							break;
						case 6 :
							key = ActionInfo::INPUT::R; 
							break;
						case 7 :
							key = ActionInfo::INPUT::LF; 
							break;
						case 8 :
							key = ActionInfo::INPUT::F; 
							break;
						case 9 :
							key = ActionInfo::INPUT::RF; 
							break;
						}
						switch ( param ) 
						{
						case 1 :
							obj->SetBufferedDirection( Lunia::XRated::Constants::DirectionF::LeftDown ); 
							break;
						case 2 :
							obj->SetBufferedDirection( Lunia::XRated::Constants::DirectionF::Down ); 
							break;
						case 3 :
							obj->SetBufferedDirection( Lunia::XRated::Constants::DirectionF::RightDown ); 
							break;
						case 4 :
							obj->SetBufferedDirection( Lunia::XRated::Constants::DirectionF::Left ); 
							break;
						case 6 :
							obj->SetBufferedDirection( Lunia::XRated::Constants::DirectionF::Right ); 
							break;
						case 7 :
							obj->SetBufferedDirection( Lunia::XRated::Constants::DirectionF::LeftUp ); 
							break;
						case 8 :
							obj->SetBufferedDirection( Lunia::XRated::Constants::DirectionF::Up ); 
							break;
						case 9 :
							obj->SetBufferedDirection( Lunia::XRated::Constants::DirectionF::RightUp ); 
							break;
						}

						if ( currentAction->GetActionClassType() == ActionInfo::ActionClassType::FAINT ) 
						{
							switch ( param ) 
							{
							case 4 :
								( ( ActionFaint* )currentAction )->ReduceTime( 0, actionData ); 
								break;
							case 6 :
								( ( ActionFaint* )currentAction )->ReduceTime(1, actionData); 
								break;
							}
							return true;
						}
						// edit by kds218
						else if( currentAction->GetActionClassType() == ActionInfo::ActionClassType::SLEEP ) 
						{
							switch ( param ) 
							{
							case 4 :
								( ( ActionSleep* )currentAction )->ReduceTime( 0, actionData ); 
								break;
							case 6 :
								( ( ActionSleep* )currentAction )->ReduceTime(1, actionData); 
								break;
							}
							return true;
						}
						break;
					case Action::CommandType::DASH :
						switch ( direction ) 
						{
						case 1 :
							key = ActionInfo::INPUT::DASH_LB; 
							break;
						case 2 :
							key = ActionInfo::INPUT::DASH_B; 
							break;
						case 3 :
							key = ActionInfo::INPUT::DASH_RB; 
							break;
						case 4 :
							key = ActionInfo::INPUT::DASH_L; 
							break;
						case 6 :
							key = ActionInfo::INPUT::DASH_R; 
							break;
						case 7 :
							key = ActionInfo::INPUT::DASH_LF; 
							break;
						case 8 :
							key = ActionInfo::INPUT::DASH_F; 
							break;
						case 9 :
							key = ActionInfo::INPUT::DASH_RF; 
							break;
						}

						switch ( param ) 
						{
						case 1 :
							obj->SetBufferedDirection( Lunia::XRated::Constants::DirectionF::LeftDown ); 
							break;
						case 2 :
							obj->SetBufferedDirection( Lunia::XRated::Constants::DirectionF::Down ); 
							break;
						case 3 :
							obj->SetBufferedDirection( Lunia::XRated::Constants::DirectionF::RightDown ); 
							break;
						case 4 :
							obj->SetBufferedDirection( Lunia::XRated::Constants::DirectionF::Left ); 
							break;
						case 6 :
							obj->SetBufferedDirection( Lunia::XRated::Constants::DirectionF::Right ); 
							break;
						case 7 :
							obj->SetBufferedDirection( Lunia::XRated::Constants::DirectionF::LeftUp ); 
							break;
						case 8 :
							obj->SetBufferedDirection( Lunia::XRated::Constants::DirectionF::Up ); 
							break;
						case 9 :
							obj->SetBufferedDirection( Lunia::XRated::Constants::DirectionF::RightUp ); 
							break;
						}

						if ( currentAction->GetActionClassType() == ActionInfo::ActionClassType::FAINT ) 
						{
							switch ( param ) 
							{
							case 4 :
								( ( ActionFaint* )currentAction )->ReduceTime( 0, actionData ); 
								break;
							case 6 :
								( ( ActionFaint* )currentAction )->ReduceTime( 1, actionData ); 
								break;
							}
							return true;
						}
						// edit by kds218
						else if ( currentAction->GetActionClassType() == ActionInfo::ActionClassType::SLEEP ) 
						{
							switch ( param ) 
							{
							case 4 :
								( ( ActionSleep* )currentAction )->ReduceTime( 0, actionData ); 
								break;
							case 6 :
								( ( ActionSleep* )currentAction )->ReduceTime( 1, actionData ); 
								break;
							}
							return true;
						}
						break;
					case Action::CommandType::STOP :
						key = ActionInfo::INPUT::STOP;
						obj->SetBufferedDirection( float3( 0, 0, 0 ) );
						break;
					case Action::CommandType::ATTACKA :
						key = ActionInfo::INPUT::ATTACKA;
						atkRate = obj->GetAttackSpeed();
						break;
					case Action::CommandType::ATTACKARELEASE :
						key = ActionInfo::INPUT::ATTACKARELEASE;
						break;
					case Action::CommandType::ATTACKB :
						key = ActionInfo::INPUT::ATTACKB;
						atkRate = obj->GetAttackSpeed();
						break;
					case Action::CommandType::ATTACKBRELEASE :
						key = ActionInfo::INPUT::ATTACKBRELEASE;
						break;
					case Action::CommandType::DOWNATTACK :
						key = ActionInfo::INPUT::DOWNATTACK;
						break;
					case Action::CommandType::JUMP :
						key = ActionInfo::INPUT::JUMP;
						break;
					}

					bool reserved;
					float changeTime;					
					const ActionInfo* actionInfo = currentAction->GetLinkedAction( obj, key, actionStartTime * atkRate, reserved, changeTime, actionData.bHit );

					uint32 actionName;

					if ( actionInfo ) 
					{
						//LoggerInstance().Info(L"ActionHash: {}", actionInfo->hash);
						actionName = actionInfo->hash;

						if ( atkRate > 1.0f )
						{
							Logger::GetInstance().Warn( L"actionName : {}", actionName );
						}

						float3 dir;
						ActionMove::Param data;
						switch( state ) 
						{
						case Action::CommandType::MOVE :
						case Action::CommandType::DASH :
							switch ( param ) 
							{
							case 1 :
								dir = Lunia::XRated::Constants::DirectionF::LeftDown; 
								break;
							case 2 :
								dir = Lunia::XRated::Constants::DirectionF::Down; 
								break;
							case 3 :
								dir = Lunia::XRated::Constants::DirectionF::RightDown; 
								break;
							case 4 :
								dir = Lunia::XRated::Constants::DirectionF::Left; 
								break;
							case 6 :
								dir = Lunia::XRated::Constants::DirectionF::Right; 
								break;
							case 7 :
								dir = Lunia::XRated::Constants::DirectionF::LeftUp; 
								break;
							case 8 :
								dir = Lunia::XRated::Constants::DirectionF::Up; 
								break;
							case 9 :
								dir = Lunia::XRated::Constants::DirectionF::RightUp; 
								break;
							}
						}

						data.direction = dir;
						if ( reserved ) 
						{
							ReserveAction( actionName, changeTime, &data );
						} 
						else 
						{
							RemoveReservedAction();
							SetAction( actionName, obj, &data, sd );
						}
					}
					return true;
				}

				void ActionManager::Do( float dt, Actor* obj, IGameStateDatabase* db )
				{
					if ( pauseTime == -1 ) 
					{
						return;
					} 
					else if ( pauseTime > 0 ) 
					{
						if ( pauseTime - dt <= 0 ) 
						{
							dt -= pauseTime;
							pauseTime = 0;

							if ( actionData.reserved.bExist && ( actionData.reserved.startTime <= actionStartTime ) )
							{
								Action::ReservedAction reserved = actionData.reserved;
								RemoveReservedAction();
								SetAction( reserved.actionName, obj, reserved.param, *db, reserved.bForce );
								return;
							}
						} 
						else 
						{
							pauseTime -= dt;
							return;
						}
					}

					actionStartTime += dt;

					if ( currentAction ) 
					{
						if ( currentAction->Do( dt, obj, db, actionData ) ) 
						{
							if ( actionData.reserved.bExist && ( actionData.reserved.startTime <= actionStartTime ) )
							{
								Action::ReservedAction reserved = actionData.reserved;
								RemoveReservedAction();
								SetAction( reserved.actionName, obj, reserved.param, *db, reserved.bForce );
								return;
							}

							Action::IActionParam* param = NULL; // used for output parameter of GetNextAction()
							uint32 nextAction = currentAction->GetNextAction( obj, param, actionData.bHit );
							if ( !nextAction ) 
							{
								//Lunia_INFO(( L"Error [ActionManager::Do] action {} cannot find next action.", currentAction->GetName().c_str() ));
								nextAction = Database::Info::HashTable::Actions::Stand;
							}
							bool ret = SetAction( nextAction, obj, param, *db );
							if ( !ret )
							{
								SetAction( Database::Info::HashTable::Actions::Stand, obj, NULL, *db );
							}

							if ( param )
							{
								delete param;
							}
						} 
						else 
						{
							if ( actionData.reserved.bExist && ( actionData.reserved.startTime <= actionStartTime ) )
							{
								Action::ReservedAction reserved = actionData.reserved;
								RemoveReservedAction();
								SetAction( reserved.actionName, obj, reserved.param, *db, reserved.bForce );
								return;
							}
						}
					}
				}

			}
		}	
	}	
}
