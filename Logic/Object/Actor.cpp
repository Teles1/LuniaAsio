#include "Actor.h"
#include "Projectile.h"

#include "../State/StateIgnoreDmg.h"
#include "../State/StateMinimize.h"
#include "../State/StateCriMagic.h"
#include "../State/StateIncrease.h"
#include "../State/StateElementalAtk.h"

#include "../GameState.h"
#include <Info/Info/Database.h>

//#include "../Etc/Balance.h"
#include "Player.h"
#include "NonPlayer.h"
#include <Logic/Object/StatCalculator.h>

namespace Lunia 
{ 
	namespace XRated 
	{	
		namespace Logic 
		{

			void Actor::IgnoreState::AddCategory( uint32 category )
			{
				Categorys::iterator iter = categorys.begin();
				Categorys::iterator end = categorys.end();
				while ( iter != end ) 
				{
					if( iter->Category == category ) 
					{
						++( iter->Count );
						return;
					}
					++iter;
				}
				IgnoreCategoryInfo info;
				info.Category = category;
				info.Count  = 1;
				categorys.push_back( info );
			}

			void Actor::IgnoreState::RemoveCategory( uint32 category )
			{
				Categorys::iterator iter = categorys.begin();
				Categorys::iterator end = categorys.end();
				while ( iter != end ) 
				{
					if ( iter->Category == category ) 
					{
						--( iter->Count );
					}
					++iter;
				}		
			}

			const bool Actor::IgnoreState::IsIgnoreCategory( uint32 category )
			{
				Categorys::const_iterator iter = categorys.begin();
				Categorys::const_iterator end = categorys.end();
				while ( iter != end ) 
				{
					if( iter->Category == category ) 
					{
						if( iter->Count > 0 ) 
						{
							return true;
						}				
					}
					++iter;
				}
				return false;
			}

			void Actor::IgnoreState::Clear()
			{
				categorys.clear();
			}

			void Actor::Status::Guard::AddGuardProbability( uint32 f, float prob )
			{
				if ( f >= Stat::StatConstants::AttackEffectType::CNT ) 
				{
					Logger::GetInstance().Info( L"[Actor::Status::Guard::AddGuardProbability] wrong index.({0})", f );
					return;
				}

				flag |= ( 1 << f );
				probability[ f ] += prob;
			}

			void Actor::Status::Guard::RemoveGuardProbability( uint32 f, float prob )
			{
				if ( f >= Stat::StatConstants::AttackEffectType::CNT ) 
				{
					Logger::GetInstance().Info( L"[Actor::Status::Guard::RemoveGuardProbability] wrong index.({0})", f);
					return;
				}

				probability[ f ] -= prob;
				if ( probability[ f ] <= 0.000001f )
				{
					flag &= ~( 1 << f );
				}
			}

			Actor::Status::Probability::Probability()
				: criAtkProbByState( 0.0f )
				, criAtkPlusProbByState( 0.0f )
				, criMagicProbByState( 0.0f )
				, criMagicProbByStatePlus( 0.0f )
				, criticalRushCount( 0 )
				, lessMp( 0.0f )
				, lessMpPlus( 0.0f )
				, minimize( 0.0f )
				, minimizePlus( 0.0f )
				, ignoreCriHit( 0.0f )
				, ignoreCriHitPlus( 0.0f )
			{		
			}

			void Actor::Status::Probability::Init()
			{
				criticalRushCount = 0;

				criAtkProbByState = criAtkPlusProbByState = 0.0f;
				criMagicProbByState = criMagicProbByStatePlus = 0.0f;
				lessMp = lessMpPlus = 0.0f;
				minimize = minimizePlus = 0.0f;
				ignoreCriHit = ignoreCriHitPlus = 0.0f;
			}

			void Actor::Status::Probability::SetProbLimit( float& value, float& plusValue, float max )
			{
				if ( plusValue < 0.0001f )
				{
					plusValue = 0.0f;
				}

				if ( plusValue > max )
				{
					value = max;
				}
				else
				{
					value = plusValue;
				}
			}

			void Actor::Status::Probability::Recalculate( bool /*bIgnoreLimit*/ )
			{
				RecalculateCriAtkProb( false );
				RecalculateCriMagicProb( false );
				RecalculateLessMp( false );
				RecalculateMinimize( false );
				RecalculateIgnoreCriHit( false );
			}

			void Actor::Status::Probability::RecalculateCriAtkProb( bool /*bIgnoreLimit*/ )
			{
				SetProbLimit( criAtkProbByState, criAtkPlusProbByState );
			}

			void Actor::Status::Probability::RecalculateCriMagicProb( bool /*bIgnoreLimit*/ )
			{
				SetProbLimit( criMagicProbByState, criMagicProbByStatePlus );
			}

			void Actor::Status::Probability::RecalculateLessMp( bool /*bIgnoreLimit*/ )
			{
				SetProbLimit( lessMp, lessMpPlus );
			}

			void Actor::Status::Probability::RecalculateMinimize( bool /*bIgnoreLimit*/ )
			{
				SetProbLimit( minimize, minimizePlus );
			}

			void Actor::Status::Probability::RecalculateIgnoreCriHit( bool /*bIgnoreLimit*/ )
			{
				SetProbLimit( ignoreCriHit, ignoreCriHitPlus );
			}

			void Actor::Status::Probability::CriticalRushActivated( bool activate )
			{
				if ( true == activate )
				{
					++criticalRushCount;
				}
				else
				{
					--criticalRushCount;
				}
			}

			Actor::Status::ConvertDamage::ConvertDamage()
			{
				absorbToHpRates.reserve( 5 );
				absorbToMpRates.reserve( 5 );
				damageToHpRates.reserve( 5 );
				damageToMpRates.reserve( 5 );
			}

			void Actor::Status::ConvertDamage::Init()
			{
				absorbToHpRates.clear();
				absorbToMpRates.clear();
				damageToHpRates.clear();
				damageToMpRates.clear();

				absorbToHpRates.reserve( 5 );
				absorbToMpRates.reserve( 5 );
				damageToHpRates.reserve( 5 );
				damageToMpRates.reserve( 5 );
			}

			void Actor::Status::ConvertDamage::RevisionRate( float& value )
			{
				if( IsZero( value ) == true )
				{
					value = 0.0f;
				}
			}

			bool Actor::Status::ConvertDamage::IsZero(float value) const
			{
				if ( (value < 0.0001f) && ( value > -0.0001f ) )
				{
					return true;
				}
				return false;
			}

			float Actor::Status::ConvertDamage::GetAbsorbToHp( float currentProbability, float damage ) const
			{
				if ( absorbToHpRates.empty() == false ) 
				{
					int persent = static_cast< int >( currentProbability * ProbabilityPermissionValue );
					float value =0.0f;
					RateToProbabilityTable::const_iterator iter = absorbToHpRates.begin();
					RateToProbabilityTable::const_iterator end = absorbToHpRates.end();
					while ( iter != end ) 
					{
						if ( persent <= iter->first ) 
						{
							value += ( iter->second * damage );
						}
						++iter;
					}
					return value;
				}		
				return 0.0f;
			}

			float Actor::Status::ConvertDamage::GetAbsorbToMp( float currentProbability, float damage ) const
			{
				if ( absorbToMpRates.empty() == false ) 
				{
					int persent = static_cast< int >( currentProbability * ProbabilityPermissionValue );
					float value =0.0f;
					RateToProbabilityTable::const_iterator iter = absorbToMpRates.begin();
					RateToProbabilityTable::const_iterator end = absorbToMpRates.end();
					while ( iter != end ) 
					{
						if ( persent <= iter->first ) 
						{
							value += ( iter->second * damage );
						}
						++iter;
					}
					return value;
				}		
				return 0.0f;
			}
			float Actor::Status::ConvertDamage::GetDamageToHp( float currentProbability, float damage ) const
			{
				if ( damageToHpRates.empty() == false ) 
				{
					int persent = static_cast< int >( currentProbability * ProbabilityPermissionValue );
					float value =0.0f;
					RateToProbabilityTable::const_iterator iter = damageToHpRates.begin();
					RateToProbabilityTable::const_iterator end = damageToHpRates.end();
					while ( iter != end ) 
					{
						if ( persent <= iter->first ) 
						{
							value += ( iter->second * damage );
						}
						++iter;
					}
					return value;
				}		
				return 0.0f;
			}
			float Actor::Status::ConvertDamage::GetDamageToMp( float currentProbability, float damage ) const
			{
				if ( damageToMpRates.empty() == false ) 
				{
					int persent = static_cast< int >( currentProbability * ProbabilityPermissionValue );
					float value =0.0f;
					RateToProbabilityTable::const_iterator iter = damageToMpRates.begin();
					RateToProbabilityTable::const_iterator end = damageToMpRates.end();
					while ( iter != end ) 
					{
						if ( persent <= iter->first ) 
						{
							value += ( iter->second * damage );
						}
						++iter;
					}
					return value;
				}		
				return 0.0f;
			}

			void Actor::Status::Immune::SetImmune( uint16 i )
			{
				for ( int a = 0; a < Constants::DamageType::CNT; ++a ) 
				{
					if ( i & ( 1 << a ) )
					{
						++immune[ a ];
					}
				}
			}

			void Actor::Status::Immune::RemoveImmune( uint16 i )
			{
				for ( int a = 0; a < Constants::DamageType::CNT; ++a ) 
				{
					if ( i & ( 1 << a ) ) 
					{
						--immune[ a ];
					}
				}
			}

			bool Actor::Status::Immune::IsImmune( Constants::DamageType type )
			{
				if ( ( int )type >= Constants::DamageType::CNT ) 
				{
					Logger::GetInstance().Info( L"[Actor::Status::Immune::IsImmune] wrong index.({})", ( int )type );
					return false;
				}

				if ( immune[ ( int )type ] )
				{
					return true;
				}
				return false;
			}

			void Actor::Status::Recalculate( bool bIgnoreLimit )
			{
				int additionalVit, additionalInt;

				additionalVit = GetVit() - stackedVit;
				additionalVit = additionalVit < 0 ? 0 : additionalVit;

				additionalInt = GetVit() - stackedInt;
				additionalInt = additionalInt < 0 ? 0 : additionalInt;

				MaxHp = ( basicHp + ( float )additionalVit * StatusFunctions::GetHPperVit( Lv ) + hpPlus ) * hpMul;

				MaxMp = ( basicMp + ( float )additionalInt * StatusFunctions::GetMPperInt( Lv ) + mpPlus ) * mpMul;

				Hp = Hp>MaxHp ? MaxHp : Hp;
				Mp = Mp>MaxMp ? MaxMp : Mp;

				dmg.Recalculate( bIgnoreLimit );
				Ac.Recalculate( bIgnoreLimit );
				prob.Recalculate( bIgnoreLimit );
				RecalculateAtkSpeed();
			}

			void Actor::Status::RecalculateHP( bool bFill, bool /*bIgnoreLimit*/)
			{
				float maxOld = MaxHp;
				int additionalHp;
				additionalHp = GetVit() - stackedVit;
				additionalHp = additionalHp < 0 ? 0 : additionalHp;
				MaxHp = ( basicHp + ( float )additionalHp * StatusFunctions::GetHPperVit(Lv) + hpPlus ) * hpMul;

				if ( bFill )
				{
					Hp += ( MaxHp - maxOld );
				}
				if ( Hp > MaxHp )
				{
					Hp = MaxHp;
				}
				else if ( Hp < 0 )
				{
					Hp = 0;
				}
			}

			void Actor::Status::RecalculateMP( bool bFill, bool /*bIgnoreLimit*/)
			{
				float maxOld = MaxMp;
				int additionalMp;
				additionalMp = GetInt() - stackedInt;
				additionalMp = additionalMp < 0 ? 0 : additionalMp;
				MaxMp = ( basicMp + ( float )additionalMp * StatusFunctions::GetMPperInt( Lv ) + mpPlus ) * mpMul;
				if ( bFill )
				{
					Mp += ( MaxMp - maxOld );
				}
				if ( Mp > MaxMp )
				{
					Mp = MaxMp;
				}
				else if ( Mp < 0 )
				{
					Mp = 0;
				}
			}

			void Actor::Status::RecalculateAtkSpeed()
			{
				AtkSpeed = ( basicAtkSpeed + atkSpeedPlus ) * atkSpeedMul;
			}
#pragma warning( disable : 4355 )
			Actor::Actor( Constants::ObjectType type )
				: Object( type )
				, bufferedDirection( float3( 0, 0, 0 ) )
				, characterData( objectData )
				, status( &characterData )
				, bCollide( false )
				, bOverlapped( false )
				, stateBundleManager( *this )
				, airHitFactor( 1 )
				, airHitCnt( 0 )
				, curAirAttackComboCnt( 0 )
				, maxAirAttackComboCnt( 0 )		
				, morphStateBundleInfo(NULL)
			{
				characterData.Team = 0;
			}
#pragma warning( default : 4355 )

			void Actor::Initialize( IGameStateDatabase* db )
			{
				Object::Initialize( db );

				status.Init();		

				stateBundleManager.Init( db );
				bufferedDirection = float3( 0, 0, 0 );
				characterData.SpeedVector = 1;
				characterData.States.clear();
				characterData.ActionName = 0;
				characterData.sizeRate = 1;
				stunFlag = 0;
				stunFlags.clear();

				airHitCnt = 0;
				airHitFactor = 1;
				ignoreState.Clear();
				morphStateBundleInfo = NULL;
			}

			void Actor::InitHeightInfo()
			{
				GetHeightInfo().Init();
				SetHeight( 0 );
				GetCharacterData().Speed = 0;
			}

			void Actor::LoadActions( const std::wstring& objName, const std::wstring& defaultAction, Action::Action::ActionPtrList* list )
			{
				actions.Init( objName, defaultAction, list );
			}

			void Actor::ClearMorphStateBundleInfo(uint32 objectHash)
			{
				if (morphStateBundleInfo)
				{
					Database::Info::StateBundleInfo::StateList& iList = morphStateBundleInfo->states;

					//StateList를 돌면서 만들어 주자.
					Database::Info::StateBundleInfo::StateList::iterator i = iList.begin();
					while( i != iList.end() ) 
					{
						switch ( (*i).type ) 
						{
						case Database::Info::StateInfo::Type::MORPH :
							if (objectHash == StringUtil::Hash( (*i).params[L"objectName"].c_str() ) )
							{
								morphStateBundleInfo = NULL;
								return;
							}
						}
						++i;
					}
				}
			}

			void Actor::PauseAction( float dt )
			{
				actions.Pause( dt );
				pauseTime = dt;

				//현재 상태 stack 에 들어 있는 상태들을 처리 허자고.
				std::list< Stat::IState* >::iterator iter;
				for ( iter = stateBuffers.state_ordinary.begin(); iter != stateBuffers.state_ordinary.end(); ++iter )
				{
					( *iter )->Pause( dt );
				}
			}

			Action::Action::ActionPtrList* Actor::SetActionList( Action::Action::ActionPtrList* newActionList, uint32 defaultAction )
			{
				return actions.SetActionList( this, *stageData, newActionList, defaultAction );
			}

			bool Actor::Command( Action::Action::CommandType state, uint32 param )
			{
				switch ( state ) 
				{
				case Action::Action::CommandType::MOVE :
				case Action::Action::CommandType::DASH :
					objectData.Move = true;
					break;
				case Action::Action::CommandType::STOP :
					objectData.Move = false;
					break;
				}
				return actions.Command( state, this, *stageData, param );
			}

			uint32 Actor::GetMaxAirComboCount()
			{
				if (stageData->GetGameType() & XRated::Constants::GameTypes::PvpGameTypeMask)
				{
					//천만번을 공중콤보 할 수 있는 애들은 없겠지 - _-;; ㅋㅋ
					return 10000000;
				}
				
				return 10;
			}

			bool Actor::DoDamage( Object* who, uint32 creatorActionHash, float minDmg, float, float damage, Constants::DamageType dmgType, uint32 stateID, Constants::AttackStateType attackType, unsigned int& sFlag)
			{
				if ( IsImmune( dmgType ) ) 
				{
					stageData->StatusChanged( objectData.GameObjectSerial, objectData.Position, objectData.Direction, status.Hp,
						status.Mp, 0, 0, Constants::StatusFlag::Immune );
					sFlag |= Constants::StatusFlag::HitIgnore;
					return false;
				}

				const Stat::IState* state = NULL;

				if ( dmgType == Constants::DamageType::PHYSICAL ) 
				{
					if ( who != NULL )
					{
						if ( who->CheckState( Database::Info::StateInfo::Type::ELEMENTALATK ) )
						{
							state = who->GetState( Database::Info::StateInfo::Type::ELEMENTALATK, Database::Info::StateInfo::Sort::ORDINARYSTATE );
							if ( state != NULL)
							{
								dmgType = ( ( Logic::Stat::ElementalAtk* )state )->GetElement();
							}

						}
					}
				}

				bool bLoop = false;
				do 
				{
					if ( CheckState( Database::Info::StateInfo::Type::CAMPFIRE ) ){
						damage = 0;
						break;
					}

					switch ( dmgType ) 
					{
					case Constants::DamageType::PHYSICAL :
					case Constants::DamageType::INDEPENDENCE :
						if ( CheckState ( Database::Info::StateInfo::Type::IGNOREDMG ) ) 
						{
							state = GetState( Database::Info::StateInfo::Type::IGNOREDMG, Database::Info::StateInfo::Sort::DEFENCESTATE );
							if ( state && ( ( Stat::IgnoreDmg* )state )->IsActivated() ) 
							{
								sFlag |= Constants::StatusFlag::IgnoreDmg;
								return true;
							}
						}

						if( CheckState( Database::Info::StateInfo::Type::MINIMIZE ) && IsMinimize( stageData ) )
						{
							damage = minDmg;

							sFlag |= Constants::StatusFlag::Minimize;
						}

						if (!(stageData->GetGameType() & XRated::Constants::GameTypes::PvpGameTypeMask) )
							damage = CalculatedDmg( damage );
						
						if ( CheckState( Database::Info::StateInfo::Type::FLIED ) ) 
						{
							++airHitCnt;
							if ( airHitCnt > 2 && airHitCnt < GetMaxAirComboCount()) 
							{
								if( who != NULL )
								{
									if ( who->GetType() == Lunia::XRated::Constants::ObjectType::Projectile ) 
									{
										Player* creator = stageData->RetrievePlayer( ( ( Projectile* )who )->GetCreatorSerial() );
										if ( creator ) 
										{
											airHitFactor = airHitFactor * stageData->GetAirDamage();
										}
										else 
										{
											airHitFactor = airHitFactor * stageData->GetAirDamageByNpc();
										}
									} 
									else if ( who->GetType() == Lunia::XRated::Constants::ObjectType::Player )
									{
										airHitFactor = airHitFactor * stageData->GetAirDamage();
									} 
									else if ( ( who->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer ) ||
										( who->GetType() == Lunia::XRated::Constants::ObjectType::Vehicle ) ||
										( who->GetType() == Lunia::XRated::Constants::ObjectType::Structure ) )
									{
										airHitFactor = airHitFactor * stageData->GetAirDamageByNpc();
									}
									else
									{
										airHitFactor = 0.0f;

										Logger::GetInstance().Info( L"[Actor::DoDamage] 데미지를 준 대상이 없습니다. Type=[{0}] Serial=[{1}]", (int)who->GetType(), (int)who->GetSerial() );
									}
								}
							}
							damage *= airHitFactor;

							AddAttacker( who, stateID, attackType ); /* air attack combo */
						} 
						else 
						{
							airHitCnt = 0;
							airHitFactor = 1;
						}
					break;
					default:
						{
							double damageRate = Database::DatabaseInstance().InfoCollections.Statuses.GetDamagePercent( dmgType, status.element );
							damage = damageRate * damage;

							float reducedDmg =  damage - GetResist(dmgType);

							damage = ( reducedDmg < damage/2 ? damage/2 : reducedDmg );

							if ( CheckState( Database::Info::StateInfo::Type::FLIED ) ) 
							{
								++airHitCnt;
								if ( airHitCnt > 2 && airHitCnt < GetMaxAirComboCount() ) 
								{
									if( who != NULL )
									{
										if ( who->GetType() == Lunia::XRated::Constants::ObjectType::Projectile ) 
										{
											Player* creator = stageData->RetrievePlayer( ( ( Projectile* )who )->GetCreatorSerial() );
											if ( creator ) 
											{
												airHitFactor = airHitFactor * stageData->GetAirDamage();
											}
											else 
											{
												airHitFactor = airHitFactor * stageData->GetAirDamageByNpc();
											}
										} 
										else if ( who->GetType() == Lunia::XRated::Constants::ObjectType::Player )
										{
											airHitFactor = airHitFactor * stageData->GetAirDamage();
										} 
										else if ( ( who->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer ) ||
											( who->GetType() == Lunia::XRated::Constants::ObjectType::Vehicle ) ||
											( who->GetType() == Lunia::XRated::Constants::ObjectType::Structure ) )
										{
											airHitFactor = airHitFactor * stageData->GetAirDamageByNpc();
										}
										else
										{
											airHitFactor = 0.0f;

											Logger::GetInstance().Info( L"[Actor::DoDamage] Chinese logger that doesnt make any sense. Type=[{0}] Serial=[{1}]", (int)who->GetType(), (int)who->GetSerial() );
										}
									}
								}
								damage *= airHitFactor;

								AddAttacker( who, stateID, attackType ); /* air attack combo */
							}
							else 
							{
								airHitCnt = 0;
								airHitFactor = 1;
							}

							if ( ( damage >= 0 ) && ( damage < 1 ) )
							{
								damage = 1;
							}
						}
					}
				} while ( bLoop );

				if ( damage <= 1.0f )
				{
					if ( !(stageData->GetStageGroupInfo()->GameType & XRated::Constants::GameTypes::PvpGameTypeMask) )
					{
						sFlag |= Constants::StatusFlag::HitIgnore;
					}
					else
					{
						if ( CheckState( Database::Info::StateInfo::Type::FLIED ) ) 
						{
							sFlag |= Constants::StatusFlag::HitIgnore;
						}
					}
				}

				SetHP( status.Hp - damage );

				if (GetHP() <= 0.f && who && CheckState( Database::Info::StateInfo::Type::FLIED ))
				{
					if ( who->GetType() == Lunia::XRated::Constants::ObjectType::Projectile )
					{
						NonPlayer* npcPlayer = stageData->RetrieveNPC( ( ( Projectile* )who )->GetCreatorSerial() );
						if ( npcPlayer ) 
						{
							sFlag |= Constants::StatusFlag::HitIgnore;
						}
					}
					else if ( ( who->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer ) ||
						( who->GetType() == Lunia::XRated::Constants::ObjectType::Vehicle ) ||
						( who->GetType() == Lunia::XRated::Constants::ObjectType::Structure ) )
					{
						sFlag |= Constants::StatusFlag::HitIgnore;
					} 
				}

				sFlag |= ( Constants::StatusFlag::Fire << ( int )dmgType );

				if ( !who ) 
				{
					stageData->StatusChanged( objectData.GameObjectSerial, objectData.Position, objectData.Direction, status.Hp,
						status.Mp, 0, 0, ( Constants::StatusFlag )0x0000);
				} 
				else if ( who->GetType() == Lunia::XRated::Constants::ObjectType::Projectile ) 
				{
					Player* creator = stageData->RetrievePlayer( ( ( Projectile* )who )->GetCreatorSerial() );
					if ( creator ) 
					{
						stageData->StatusChanged(objectData.GameObjectSerial, objectData.Position, objectData.Direction, status.Hp,
							status.Mp, ( ( Projectile* )who )->GetCreatorSerial(), who->GetNameHash(), sFlag, ( ( Actor* )creator )->GetCurAirAttackComboCnt() );
					}
					else 
					{
						stageData->StatusChanged(objectData.GameObjectSerial, objectData.Position, objectData.Direction, status.Hp,
							status.Mp, ( ( Projectile* )who )->GetCreatorSerial(), who->GetNameHash(), sFlag );
					}
				} 
				else if ( who->GetType() == Lunia::XRated::Constants::ObjectType::Player )
				{
					uint32 actionNameHash = creatorActionHash;

					if( actionNameHash == 0 )
					{
						actionNameHash = ( ( Actor* )who )->GetActionNameHash();
					}

					stageData->StatusChanged(objectData.GameObjectSerial, objectData.Position, objectData.Direction, status.Hp,
						status.Mp, who->GetSerial(), actionNameHash, sFlag, ( ( Actor* )who )->GetCurAirAttackComboCnt() );

				} 
				else if ( ( who->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer ) ||
					( who->GetType() == Lunia::XRated::Constants::ObjectType::Vehicle ) ||
					( who->GetType() == Lunia::XRated::Constants::ObjectType::Structure ) )
				{
					stageData->StatusChanged( objectData.GameObjectSerial, objectData.Position, objectData.Direction, status.Hp,
						status.Mp, who->GetSerial(), ( ( Actor* )who )->GetActionNameHash(), sFlag );
				} 
				else 
				{
					//Lunia_INFO(( L"[Actor::DoDamage] 데미지를 준 대상이 없습니다. Type=[{}] Serial=[{}]", (int)who->GetType(), (int)who->GetSerial() ));
				}

				ProcessCondition( Database::Info::StateInfo::IFHITBY, who );

				if ( ( CheckState( Database::Info::StateInfo::Type::ABSORBTOHPMP ) == true ) && ( GetHP() > 0.0f ) )
				{
					AbsorbToHpMp( damage );
				}

				if ( who != NULL )
				{
					Actor* actor = who->IsCharacter() == true ? ( Actor* )who : NULL;

					if ( actor == NULL )
					{
						if ( who->GetType() == XRated::Constants::ObjectType::Projectile )
						{
							actor = stageData->RetrieveActor( ( ( Projectile* )who )->GetCreatorSerial() );
						}
					}

					if ( actor != NULL )
					{
						if ( sFlag & Constants::StatusFlag::Critical )
						{
							actor->ProcessCondition( Database::Info::StateInfo::IFCRIATK, this );
						}

						if ( sFlag & Constants::StatusFlag::CriticalMagic )
						{
							actor->ProcessCondition( Database::Info::StateInfo::IFCRIMAGIC, this );
						}

						actor->ProcessCondition( Database::Info::StateInfo::IFHIT, this );

						if ( ( actor->CheckState( Database::Info::StateInfo::Type::DMGTOHPMP ) == true ) && ( actor->GetHP() > 0.0f ) )
						{
							actor->DamageToHpMp( damage );
						}
					}
				}

				//damage처리후에 계산해야 할 것들.
				if ( damage < 0 ) //흡수했다.
				{
					return false;
				}

				UpdateStylePoint( who );

				return true;
			}

			void Actor::SetSizeRate(float /*rate*/)
			{

			}

			bool Actor::IsDodge()
			{
				if ( status.dodgeRate >= stageData->GetRandomFloat() ) //피했다.
				{
					return true;
				}
				return false;
			}

			float Actor::CalculatedDmg( float damage )
			{
				if ( damage/2.f > status.Ac.ac)
				{
					return damage - status.Ac.ac;
				}
				else
				{
					float dmg = (damage/2.f) - ((status.Ac.ac - (damage/2.f) )/4.f);
					if (dmg < 1.0f) dmg = 1.0f;
					
					return dmg;
				}

				//if ( damage < status.Ac.ac + 1.0f )
				//{
				//	return 1.0f;
				//}
				//else
				//{
				//	return damage - ( int )status.Ac.ac;
				//}				
			}
			
			float Actor::GetPlusMinDamage()
			{
				if (status.dmg.atkMinPlus > status.dmg.MaxAtk) return status.dmg.MaxAtk;
				return status.dmg.atkMinPlus;
			}

			float Actor::GetPlusMaxDamage()
			{
				if (status.dmg.atkMaxPlus > status.dmg.MaxAtk) return status.dmg.MaxAtk;
				return status.dmg.atkMaxPlus;
			}

			void Actor::GetPlusDamage( float& min, float& max )
			{
				min = GetPlusMinDamage();
				max = GetPlusMaxDamage();
			}

			void Actor::GetDamage( float& min, float& max )
			{
				min = status.dmg.AtkMin;
				max = status.dmg.AtkMax;
			}

			void Actor::AddDamage( float min, float max )
			{
				status.dmg.atkMinPlus += min; //.push_back(min);
				status.dmg.atkMaxPlus += max; //.push_back(max);
				status.dmg.Recalculate( GetType() == Constants::ObjectType::NonPlayer ? true : false );
			}

			void Actor::AddDamage( float mul )
			{
				status.dmg.atkMultiply += mul; //.push_back(mul);
				status.dmg.Recalculate( GetType() == Constants::ObjectType::NonPlayer ? true : false );
			}

			void Actor::RemoveDamage( float min, float max )
			{
				status.dmg.atkMinPlus -= min;
				status.dmg.atkMaxPlus -= max;
				status.dmg.Recalculate( GetType() == Constants::ObjectType::NonPlayer ? true : false );
			}

			void Actor::RemoveDamage( float mul )
			{
				status.dmg.atkMultiply -= mul;
				status.dmg.Recalculate( GetType() == Constants::ObjectType::NonPlayer ? true : false );
			}

			void Actor::AddAc( float ac )
			{
				status.Ac.acPlus += ac;
				status.Ac.Recalculate( GetType()==Constants::ObjectType::NonPlayer ? true : false );
			}

			void Actor::RemoveAc( float ac )
			{
				status.Ac.acPlus -= ac;
				status.Ac.Recalculate( GetType() == Constants::ObjectType::NonPlayer ? true : false );
			}

			void Actor::AddAcMul( float ac )
			{
				status.Ac.acMultiply += ac;
				status.Ac.Recalculate(GetType() == Constants::ObjectType::NonPlayer ? true : false );
			}

			void Actor::RemoveAcMul( float ac )
			{
				status.Ac.acMultiply -= ac;
				status.Ac.Recalculate( GetType() == Constants::ObjectType::NonPlayer ? true : false );
			}

			void Actor::AddReduceDmg( float dmg )
			{
				status.Ac.AddReduceDmg( dmg ); //.reduceDmg.push_back(dmg);
			}

			void Actor::RemoveReduceDmg( float dmg )
			{
				status.Ac.RemoveReduceDmg( dmg );
			}

			void Actor::AddResist( float reg, int type )
			{
				if ( type < 0 )
				{
					return;
				}
				if ( type >= Constants::DamageType::CNT ) 
				{
					for ( int a = 0; a < Constants::DamageType::CNT; ++a ) 
					{
						status.resist[ a ] += reg;
					}
					return;
				}
				status.resist[ type ] += reg;
			}

			void Actor::RemoveResist( float reg, int type )
			{
				if ( type < 0 )
				{
					return;
				}
				if ( type >= Constants::DamageType::CNT ) 
				{
					for ( int a = 0; a < Constants::DamageType::CNT; ++a ) 
					{
						status.resist[ a ] -= reg;
					}
					return;
				}
				status.resist[ type ] -= reg;
			}

			void Actor::SetImmune( uint16 i )
			{
				status.immune.SetImmune( i );
			}

			void Actor::RemoveImmune( uint16 i )
			{
				status.immune.RemoveImmune( i );
			}

			bool Actor::IsImmune( Constants::DamageType type )
			{
				return status.immune.IsImmune( type );
			}


			//void Actor::RecalculateStatus( bool bIgnoreLimit = false)
			//{
			//	status.Recalculate(bIgnoreLimit);
			//}

			//void Actor::RecalculateHP( bool bFill, bool bIgnoreLimit)
			//{
			//	status.RecalculateHP(bFill, bIgnoreLimit);
			//}

			//void Actor::RecalculateMP( bool bFill, bool bIgnoreLimit)
			//{
			//	status.RecalculateMP(bFill, bIgnoreLimit);
			//}

			//void Actor::RecalculateAc( bool bIgnoreLimit)
			//{ 
			//	status.RecalculateAc(bIgnoreLimit);
			//}
			//void Actor::RecalculateDmg( bool bIgnoreLimit)
			//{
			//	status.RecalculateDmg(bIgnoreLimit);
			//}


			void Actor::SetHP( float hp, float max )
			{
				if ( max != 0 )
				{
					status.MaxHp = max;
				}
				if ( hp < 0 ) 
				{ 
					status.Hp = 0; 
					return; 
				}

				status.Hp = ( hp > status.MaxHp ? status.MaxHp : hp );
			}
			void Actor::ChangeHpFromState( float hp, float max,Lunia::XRated::Serial /*donor*/)
			{
				if ( max != 0 )
				{
					status.MaxHp = max;
				}
				if ( hp < 0 ) 
				{ 
					status.Hp = 0; 
					return; 
				}

				status.Hp = ( hp > status.MaxHp ? status.MaxHp : hp );
			}
			void Actor::SetMP( float mp, float max )
			{
				if ( max != 0 )
				{
					status.MaxMp = max;
				}
				if ( mp < 0 ) 
				{ 
					status.Mp = 0; 
					return; 
				}

				status.Mp = ( mp > status.MaxMp ? status.MaxMp : mp );
			}

			void Actor::ChangeCollisionState()
			{
				bCollide = !bCollide;
			}

			bool Actor::IsGuardActivated( uint32 flag )
			{
				return status.guard.IsActivated( flag, stageData->GetRandomFloat() );
			}



			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//state bundles & passive
			void Actor::Debuff( float debuffRate, int sort, uint32 subCategoryHash )
			{
				stateBundleManager.Debuff( debuffRate, sort, subCategoryHash );
			}

			bool Actor::RemoveAllStateBundle( Database::Info::StateInfo::Type type )
			{
				return stateBundleManager.RemoveAll( type );
			}

			bool Actor::RemoveStateBundle( Stat::IState* state )
			{
				return stateBundleManager.Remove( state );
			}
			bool Actor::AddStateBundle( Database::Info::StateBundleInfo* info, Lunia::XRated::Serial creator, Lunia::XRated::Serial byWhom )
			{
				if (info->morphBundle) 
				{
					morphStateBundleInfo = info;
				}

				return stateBundleManager.Add( info, creator, byWhom );
			}
			void Actor::AddPassive( Database::Info::StateBundleInfo* info )
			{
				if (info->morphBundle) 
				{
					morphStateBundleInfo = info;
				}

				stateBundleManager.Add( info, 0 );
			}

			void Actor::RemovePassive( Database::Info::StateBundleInfo* info )
			{
				stateBundleManager.Remove( info );
			}

			void Actor::ProcessStateBundle( const Database::Info::StateBundleInfo::Condition& condition )
			{
				stateBundleManager.ProcessStateBundle( condition );
			}
			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			bool Actor::IsLessMp( IGameStateDatabase* db )
			{
				if ( db->GetRandomClass()->Next() <= status.prob.lessMp )
				{
					return	true;
				}

				return false;
			}

			bool Actor::IsMinimize( IGameStateDatabase* db )
			{
				if ( db->GetRandomClass()->Next() <= status.prob.minimize )
				{
					return	true;
				}

				return false;
			}


			bool Actor::IsCriAtk( IGameStateDatabase* db )
			{
				if ( ( status.prob.criticalRushCount > 0 ) || ( db->GetRandomClass()->Next() <= status.prob.criAtkProbByState + GetCriticalProbabilityByStat() ) )
				{
					return	true;
				}
				return false;
			}

			bool Actor::IsCriMagic( IGameStateDatabase* db )
			{
				if ( db->GetRandomClass()->Next() <= ( status.prob.criMagicProbByState + GetCriticalProbabilityByStat() ) )
				{
					return	true;
				}
				return false;
			}

			bool Actor::IsIgnoreCriHit( IGameStateDatabase* db )
			{
				if( db->GetRandomClass()->Next() <= status.prob.ignoreCriHit )
				{
					return	true;
				}
				return false;
			}

			void Actor::AbsorbToHpMp( float damage )
			{
				float maxValue;
				float currentValue;

				currentValue = GetHP( maxValue );

				SetHP( currentValue + status.convertDamage.GetAbsorbToHp( stageData->GetRandomFloat(), damage ), maxValue );

				currentValue = GetMP( maxValue );
				SetMP( currentValue + status.convertDamage.GetAbsorbToMp( stageData->GetRandomFloat(), damage ), maxValue );

				stageData->StatusChanged( objectData.GameObjectSerial, objectData.Position, objectData.Direction, status.Hp, status.Mp, 0, 0, 0 );
			}

			void Actor::DamageToHpMp( float damage )
			{
				float maxValue;
				float currentValue;

				currentValue = GetHP( maxValue );
				SetHP( currentValue + status.convertDamage.GetDamageToHp( stageData->GetRandomFloat(), damage ), maxValue );

				currentValue = GetMP( maxValue );
				SetMP( currentValue + status.convertDamage.GetDamageToMp( stageData->GetRandomFloat(), damage ), maxValue );

				stageData->StatusChanged( objectData.GameObjectSerial, objectData.Position, objectData.Direction, status.Hp, status.Mp, 0, 0, 0 );
			}

			void Actor::ProcessStateWhenHit( Actor* target, float /*damage*/, uint32 /*sFlag*/ )
			{
				for ( StateList::iterator iter = stateBuffers.state_attack.begin(); iter != stateBuffers.state_attack.end(); ++iter ) 
				{
					Stat::IState* state = (*iter);
					switch ( state->GetType() ) 
					{
					case Database::Info::StateInfo::Type::ADDBUFFWHENHIT :
						state->Do( target, stageData );
						break;
					}
				}
			}

			void Actor::ProcessStateWhenHitted( Actor* attacker /*날 때린넘*/, float /*damage*//*받은 데미지*/, uint32 /*sFlag*//*Status flag*/ )
			{
				for ( StateList::iterator iter = stateBuffers.state_attack.begin(); iter != stateBuffers.state_attack.end(); ++iter ) 
				{
					Stat::IState* state = (*iter);
					switch ( state->GetType() ) 
					{
					case Database::Info::StateInfo::Type::ADDBUFFWHENHITTED :
						state->Do( attacker, stageData );
						break;
					}
				}
			}

			bool Actor::Update( float dt, IGameStateDatabase* db )
			{
				stateBundleManager.Update( dt );
				actions.Do( dt, this, db );	//Action manager 에게 현재 진행중인 액션을 진행하게 시킨다.

				UpdateAirAttackCombo(); /* air attack combo */

				return Object::Update( dt, db );
			}

			const Database::Info::ActionInfo* Actor::GetActionInfo( uint32 hash )
			{
				Action::Action* action = actions.GetAction( hash );
				if ( action )
				{
					return action->GetActionInfo();
				}
				return NULL;
			}



			///////////////////////////////////////////////////////////////////////////////////////////////////////
			bool Actor::IsAddableAirComboCount( Serial objectSerial, uint32 stateID, Constants::AttackStateType attackType )
			{
				//Attack Type에 따른.. 중복 검사..
				if ( attackType == Constants::AttackStateType::ATTACK )
				{
					std::vector< Serial >::iterator iter = std::find( attackStateList.begin(), attackStateList.end(), objectSerial );
					if ( iter != attackStateList.end() )
					{
						std::vector< uint32 >::iterator iter = std::find( attackStateIDList.begin(), attackStateIDList.end(), stateID );
						if ( iter != attackStateIDList.end() ) 
						{
							return false;
						}
					}

					//새로운 AirComboData를 넣기전에 그전꺼를 지워주자.. 만약 지워진 다음에 그전께 다시 온다면 OTL..
					attackStateList.clear();
					attackStateIDList.clear();

					attackStateList.push_back( objectSerial );
					attackStateIDList.push_back( stateID );
				}
				else if ( attackType == Constants::AttackStateType::DAMAGE )
				{
					std::vector< uint32 >::iterator iter = std::find( damageStateIDList.begin(), damageStateIDList.end(), stateID );
					if ( iter != damageStateIDList.end() ) 
					{
						return false;
					}

					//새로운 AirComboData를 넣기전에 그전꺼를 지워주자.. 만약 지워진 다음에 그전께 다시 온다면 OTL..
					damageStateIDList.clear();

					damageStateIDList.push_back( stateID );
				}
				else 
				{
					return false;
				}

				return true;
			}

			void Actor::AddAttacker( Object* who, uint32 stateID, Constants::AttackStateType attackType )
			{
				if ( !who )
				{
					return;
				}

				//Get Serial
				Serial objectSerial = 0;
				if ( who->GetType() == Lunia::XRated::Constants::ObjectType::Projectile ) 
				{
					objectSerial = ( ( Projectile* )who )->GetCreatorSerial();
				}
				else if ( who->GetType() == Lunia::XRated::Constants::ObjectType::Player )
				{
					objectSerial = who->GetSerial();
				}
				else 
				{
					return;
				}

				if ( ( this->GetType() != Lunia::XRated::Constants::ObjectType::Player ) && ( this->GetType() != Lunia::XRated::Constants::ObjectType::NonPlayer ) )
				{
					return;
				}
				if ( ( this->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer ) && ( ( NonPlayer* )this )->IsDead() ) 
				{
					return;
				}

				//Get Player
				Player* creator = stageData->RetrievePlayer( objectSerial );
				if ( !creator ) 
				{
					return;
				}

				if ( ( ( Actor* )creator )->IsAddableAirComboCount( who->GetSerial(), stateID, attackType ) )
				{
					std::vector< Serial >::iterator iter = std::find( attackPlayerList.begin(), attackPlayerList.end(), objectSerial );
					if ( iter == attackPlayerList.end() ) 
					{
						attackPlayerList.push_back( objectSerial );
					}
					//Attacker에게 Damager를 알려준다..
					( ( Actor* )creator )->AddAirAttackCombo( objectData.GameObjectSerial );

					//Air Combo StylePoint
					creator->AddAirComboStylePoint( ( ( Actor* )creator )->GetCurAirAttackComboCnt() );
				}
			}

			BasicStat Actor::Status::GetBasicStat() const
			{
				return BasicStat( GetStr(), GetInt(), GetDex(), GetVit() );
			}

			int Actor::Status::GetStr() const
			{
				int LimitMin = (MaxStatus < LimitStr) ? MaxStatus : LimitStr;
				float strength = Str * StrFactor;
				if (strength < 0) strength = 0;
				return ( strength > LimitMin )? LimitMin : SmartCast( strength , 4 );
			}

			void Actor::Status::AddStrFactor( float factor )
			{
				StrFactor += factor;
				dmg.str = GetStr();
			}

			void Actor::Status::AddStr(int str)
			{
				Str += str;
				dmg.str = GetStr();
			}

			int Actor::Status::GetInt() const
			{
				int LimitMin = (MaxStatus < LimitInt) ? MaxStatus : LimitInt;
				float intelligence = Int * IntFactor;
				if (intelligence < 0) intelligence = 0;
				return ( intelligence > LimitMin )? LimitMin : SmartCast( intelligence , 4 );
			}

			void Actor::Status::AddIntFactor( float factor )
			{
				IntFactor += factor;
			}

			void Actor::Status::AddInt(int i) 
			{ 
				Int += i;
			}

			int Actor::Status::GetDex() const
			{
				int LimitMin = (MaxStatus < LimitDex) ? MaxStatus : LimitDex;
				float dexterity = Dex * DexFactor;
				if (dexterity < 0) dexterity = 0;
				return ( dexterity > LimitMin )? LimitMin : SmartCast( dexterity , 4 );
			}

			void Actor::Status::AddDexFactor( float factor )
			{
				DexFactor += factor;
				dmg.dex = GetDex();
			}

			void Actor::Status::AddDex(int dex) 
			{
				Dex += dex;
				dmg.dex = GetDex();
			}

			int Actor::Status::GetVit() const
			{
				int LimitMin = (MaxStatus < LimitVit) ? MaxStatus : LimitVit;
				float vitality = Vit * VitFactor;
				if (vitality < 0) vitality = 0;
				return ( vitality > LimitMin )? LimitMin : SmartCast( vitality , 4 );
			}

			void Actor::Status::AddVitFactor( float factor )
			{
				VitFactor += factor;
			}

			void Actor::Status::AddVit(int vit) 
			{ 
				Vit += vit;
			}

			int Actor::Status::SmartCast( float value , int validFigure ) const
            {
				int temp = pow( 10.0f , validFigure );
				int result = static_cast< int >( ( value * temp ) + 0.5f );
				return static_cast< int >( result / temp );
			}

			void Actor::RecalculateSecondStat()
			{				
			}

			void Actor::AddAirAttackCombo( Serial damager )
			{
				std::vector< Serial >::iterator iter = std::find( damagePlayerList.begin(), damagePlayerList.end(), damager );
				if ( iter == damagePlayerList.end() )
				{
					damagePlayerList.push_back( damager );
				}

				++curAirAttackComboCnt;
				if ( maxAirAttackComboCnt < curAirAttackComboCnt )
				{
					maxAirAttackComboCnt = curAirAttackComboCnt;
				}

				//Lunia_IMPORTANT((L"[Actor::AddAirAttackCombo] objectSerial {}, CurCount {}, MaxCount {}", damager, curAirAttackComboCnt, maxAirAttackComboCnt));
			}

			void Actor::DeleteDamager( Serial damager )
			{
				std::vector< Serial >::iterator iter = std::find( damagePlayerList.begin(), damagePlayerList.end(), damager );
				if ( iter != damagePlayerList.end() ) 
				{
					damagePlayerList.erase( iter );
				}

				if ( damagePlayerList.empty() )
				{
					curAirAttackComboCnt = 0;
				}

				//Lunia_IMPORTANT((L"[Actor::DeleteDamager] objectSerial {}, CurCount {}, MaxCount {}", damager, curAirAttackComboCnt, maxAirAttackComboCnt));
			}

			/**
			죽었을 경우 불린다.
			*/
			void Actor::ClearAirAttackData()
			{
				ClearAttackPlayerList();
				damagePlayerList.clear();
				attackStateList.clear();
				attackStateIDList.clear();
				damageStateIDList.clear();
				InitAirAttackComboCnt();
			}

			void Actor::ClearAttackPlayerList()
			{
				std::vector< Serial >::iterator iter = attackPlayerList.begin();
				while ( iter != attackPlayerList.end() )
				{
					Player* creator = stageData->RetrievePlayer( *iter );
					if ( creator )
					{
						( ( Actor* )creator )->DeleteDamager( objectData.GameObjectSerial );
					}
					++iter;
				}

				attackPlayerList.clear();
			}

			void Actor::InitAirAttackComboCnt()
			{
				curAirAttackComboCnt = 0;
				maxAirAttackComboCnt = 0;
			}

			uint32 Actor::GetCurAirAttackComboCnt()
			{
				return curAirAttackComboCnt;
			}

			uint32 Actor::GetMaxAirAttackComboCnt()
			{
				return maxAirAttackComboCnt;
			}

			void Actor::UpdateAirAttackCombo()
			{
				if ( attackPlayerList.empty() )
				{
					return;
				}
				if ( !IsOnAirState() )
				{
					ClearAttackPlayerList();
				}
			}

			bool Actor::IsOnAirState()
			{
				return CheckState( Database::Info::StateInfo::Type::FLIED );
			}

			void Actor::UpdateStylePoint( Object* who )
			{
				if ( !who ) 
				{
					return;
				}

				//Get Serial
				Serial objectSerial = 0;
				if ( who->GetType() == Lunia::XRated::Constants::ObjectType::Projectile ) 
				{
					objectSerial = ( ( Projectile* )who )->GetCreatorSerial();
				}
				else if ( who->GetType() == Lunia::XRated::Constants::ObjectType::Player )
				{
					objectSerial = who->GetSerial();
				}
				else 
				{
					return;
				}

				//Get Player
				Player* p = stageData->RetrievePlayer( objectSerial );
				if ( p )
				{
					p->UpdateStylePoint();
				}
			}

			bool Actor::ProcessCondition( Database::Info::StateInfo::Type conditionStateType, Object* object )
			{
				if ( !object ) 
				{
					return false;
				}

				for ( StateList::iterator itr = stateBuffers.state_condition.begin(); itr != stateBuffers.state_condition.end(); ++itr )
				{
					Stat::IState* conditionState = ( *itr );

					if ( conditionState == NULL )
					{
						continue;
					}

					if ( conditionState->GetType() == conditionStateType ) 
					{
						if ( object->GetType() == Lunia::XRated::Constants::ObjectType::Projectile ) 
						{

							Actor* actor = stageData->RetrieveActor( ( ( Projectile* )object )->GetCreatorSerial() );
							if ( actor )
							{
								conditionState->Do( actor, stageData );
							}
						}
						else 
						{
							conditionState->Do( object, stageData );
						}
					}
				}		

				return true;
			}

			void Actor::AddAttackSpeed( float amount )
			{
				status.atkSpeedPlus += amount;
			}

			void Actor::AddAttackSpeedRate( float rate )
			{
				status.atkSpeedMul += rate;
			}

			float Actor::GetAttackSpeed()
			{
				return status.AtkSpeed;
			}

			float Actor::GetSkillDamageIncreaseFactorByStat() const
			{
				if (stageData->GetGameType() & XRated::Constants::GameTypes::PvpGameTypeMask)
					return StatusFunctions::GetIncreasedDamageRateInPvp( status.GetStr() );
				else
					return StatusFunctions::GetIncreasedDamageRate( status.GetStr() );
			}

			float Actor::GetCriticalProbabilityByStat() const
			{
				return 0.0f;
			}

			float Actor::GetCoolDownTimeReduceAmount() const
			{
				return 0.0f;
			}

			float Actor::GetPhysicalCriticalInc() const
			{
				return Database::DatabaseInstance().InfoCollections.GeneralInfos.Retrieve().PhysicalCri + increase.criAtkInc;
			}

			float Actor::GetMagicalCriticalInc() const
			{
				return Database::DatabaseInstance().InfoCollections.GeneralInfos.Retrieve().MagicCri + increase.criMagicInc;
			}

			float Actor::GetHitIgnoreHpRate() const
			{
				return -1.0f;
			}

			float Actor::GetHealIncRateByStat() const
			{
				return 1.0f;
			}

			bool Actor::IsHitEffectIgnored() const
			{
				return false;
			}

			const float Actor::GetHitIgnoreProb( const float hitIgnoreValue ) const
			{
				return 0.0f;
			}
			
			const float Actor::GetHitIgnoreValue() const
			{
				return 0.0f;
			}
		}	
	}	
}

