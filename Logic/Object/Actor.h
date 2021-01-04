#pragma once
#include "Object.h"
#include "../Action/Action.h"
#include "../State/StateBundle/StateBundle.h"
#include "StatCalculator.h"

namespace Lunia 
{ 
	namespace XRated 
	{	
		namespace Logic 
		{

			class Actor : public Object 
			{

			public :

				class IgnoreState
				{
					struct IgnoreCategoryInfo
					{
						uint32 Category;
						int	Count;
					};

					typedef std::vector<IgnoreCategoryInfo> Categorys;
					Categorys	categorys;
				public:
					void AddCategory(uint32 category);
					void RemoveCategory(uint32 category);
					const bool IsIgnoreCategory(uint32 category);
					void Clear();			
				};
				struct HeightControl 
				{
					float speed;
					float height;
					float maxHeight;
					float currentTime, maxTime;
					HeightControl() : speed(0), height(0), maxHeight(0), currentTime(0), maxTime(0) {}
					void Init() { speed=0;height=0;maxHeight=0;currentTime=0;maxTime=0; }
				};

				enum AttackType 
				{
					CIRCLE = 0, RECTANGLE, FANSHAPE, SEGMENT
				};

			protected :
				CharacterData characterData;

				//Status	////////////////////////////////////////////////////////
				struct Status 
				{
					uint16 &Lv;
					std::vector<int> StatusLimit;
					int Str, Dex, Int, Vit , MaxStatus;
					int LimitStr, LimitDex, LimitInt, LimitVit;

					float StrFactor , DexFactor , IntFactor , VitFactor;
					int stackedVit, stackedInt;
					float basicHp, basicMp, basicAtkSpeed;
					float &MaxHp, &Hp, &MaxMp, &Mp, AtkSpeed;

					float hpPlus, hpMul, mpPlus, mpMul, atkSpeedPlus, atkSpeedMul;

					float dodgeRate;

					struct Guard 
					{
						float probability[Stat::StatConstants::AttackEffectType::CNT];
						uint32 flag;

						inline void Init() { flag = 0; std::fill(probability, probability+(sizeof(probability)/sizeof(float)), .0f); }
						void AddGuardProbability(uint32 f, float prob);
						void RemoveGuardProbability(uint32 f, float prob);
						inline bool IsActivated(uint32 f, float prob) const { if (f<sizeof(probability)/sizeof(float)) return probability[f] >= prob; else throw; }
					} guard;

					struct Def 
					{
						float acPlus = 0;
						float acMultiply = 0;
						float reduceDmg = 0; //std::vector<float> reduceDmg;	//�̳���.. �ۼ�Ʈ�� ���δ�.. 10,10 �̶�� 9/10�� ���ϰ� �ٽ� 9/10�� ��..
						float ac = 0;

						inline virtual void Recalculate(bool /*bIgnoreLimit*/)
						{
							ac=0;
							ac = acPlus * (1.0f + acMultiply);
							ac /= 10;
						}

						void AddReduceDmg(float dmg) { reduceDmg += dmg; }
						inline void RemoveReduceDmg(float dmg) {
							reduceDmg -= dmg;
						}

						inline void CalculateReducedDmg(float& dmg) {	//�������� �ָ� %���� ���̰��� ����� �����ش�.
							dmg *= (1-reduceDmg);
						}

						inline Def() : ac(0) {}
						void Init() { ac=0; acPlus=0; acMultiply=0; reduceDmg=0; }
					} Ac;

					struct Dmg 
					{
						int str, dex;

						int basicStr,basicDex;
						float atkMinPlus;
						float atkMaxPlus;
						float atkMultiply;

						float AtkMin, AtkMax;

						float MaxAtk;		//�ش� ������������ Max�� ������ �ִ� ATk���̴�.

						inline virtual void Recalculate(bool /*bIgnoreLimit*/)
						{
							float basicAtkMin = static_cast<float>(basicStr) / 3.0f;
							float basicAtkMax = (static_cast<float>(basicDex) / 8.0f) + (static_cast<float>(basicStr) / 3.0f);

							int addedStr = str - basicStr;
							int addedDex = dex - basicDex;
							if( addedStr < 0 ) { addedStr =0; }
							if( addedDex < 0 ) { addedDex =0; }

							AtkMin = basicAtkMin + StatusFunctions::GetMinDmg(addedStr);
							AtkMax = basicAtkMax + StatusFunctions::GetMaxDmg(addedStr, addedDex);

							AtkMin += atkMinPlus;
							AtkMax += atkMaxPlus;
							AtkMin += AtkMin*atkMultiply;
							AtkMax += AtkMax*atkMultiply;
				
							if (AtkMin > MaxAtk) AtkMin = MaxAtk;
							if (AtkMax > MaxAtk) AtkMax = MaxAtk;
						}
						//void AddStr(int s) { str += s; }
						//void AddDex(int d) { dex += d; }
						inline Dmg() : str(0), dex(0) {}
						inline void Init() {
							str=dex=0; 
							atkMinPlus = atkMaxPlus = atkMultiply = 0; 
							AtkMin=AtkMax=0; 
							basicStr=basicDex=0; 
							MaxAtk = XRated::Constants::MaxStat;}
					} dmg;

					float resist[Constants::DamageType::CNT];
					Constants::DamageType element;
					struct Immune {
						const static uint16 Fire		= 0x0001;
						const static uint16 Water		= 0x0002;
						const static uint16 Ice			= 0x0004;
						const static uint16 Lightning	= 0x0008;
						const static uint16 Land		= 0x0010;
						const static uint16 Wind		= 0x0020;
						const static uint16 Poison		= 0x0040;
						const static uint16 Light		= 0x0080;
						const static uint16 Curse		= 0x0100;
						const static uint16 Physical	= 0x0200;
						const static uint16 Independence= 0x0400;
						int immune[Constants::DamageType::CNT];

						Immune() { Init(); }
						void SetImmune(uint16 i);
						void RemoveImmune(uint16 i);
						bool IsImmune(Constants::DamageType type);
						inline void Init() { for(int a=0;a<Constants::DamageType::CNT;++a) immune[a]=0; }
					} immune;

					struct Probability
					{
						int criticalRushCount;
						float criAtkProbByState, criAtkPlusProbByState, criMagicProbByState, criMagicProbByStatePlus;
						float ignoreCriHit, ignoreCriHitPlus;

						float lessMp, lessMpPlus;
						float minimize, minimizePlus;

						Probability();

						void Init();

						void SetProbLimit(float& value, float& plusValue, float max = 1.0f);

						virtual void Recalculate(bool /*bIgnoreLimit*/);
						virtual void RecalculateCriAtkProb(bool /*bIgnoreLimit*/);
						virtual void RecalculateCriMagicProb(bool /*bIgnoreLimit*/);
						virtual void RecalculateLessMp(bool /*bIgnoreLimit*/);
						virtual void RecalculateMinimize(bool /*bIgnoreLimit*/);
						virtual void RecalculateIgnoreCriHit(bool /*bIgnoreLimit*/);

						void CriticalRushActivated(bool activate);

						inline void AddCriAtk(float probability) { criAtkPlusProbByState += probability; RecalculateCriAtkProb(false); }
						inline void AddCriMagic(float probability) { criMagicProbByStatePlus += probability; RecalculateCriMagicProb(false); }
						inline void AddLessMp(float probability) { lessMpPlus += probability; RecalculateLessMp(false); }
						inline void AddMinimize(float probability) { minimizePlus += probability; RecalculateMinimize(false); }
						inline void AddIgnoreCriHit(float probability) { ignoreCriHitPlus += probability; RecalculateIgnoreCriHit(false); }
					}prob;

					struct ConvertDamage
					{
						//float absorbToHpRate, absorbToMpRate;
						//float damageToHpRate, damageToMpRate;



						ConvertDamage();

						void Init();				

						inline void AddAbsorbToHpRate(float rate, float probability) {
							RateToProbabilityTable::iterator iter = std::find_if(absorbToHpRates.begin(), absorbToHpRates.end(), FindByProbability(probability));
							if( iter != absorbToHpRates.end() ) {
								iter->second += rate;
								if( IsZero(iter->second) == true ) {
									absorbToHpRates.erase( iter );
								}						
							}else {
								if( IsZero(rate) == false ) {
									absorbToHpRates.push_back( std::pair<int, float>(static_cast<int>(probability*ProbabilityPermissionValue), rate) );
								}
							}
						}
						inline void AddAbsorbToMpRate(float rate, float probability) {
							RateToProbabilityTable::iterator iter = std::find_if(absorbToMpRates.begin(), absorbToMpRates.end(), FindByProbability(probability));
							if( iter != absorbToMpRates.end() ) {
								iter->second += rate;
								if( IsZero(iter->second) == true ) {
									absorbToMpRates.erase( iter );
								}
							}else {
								if( IsZero(rate) == false ) {
									absorbToMpRates.push_back( std::pair<int, float>(static_cast<int>(probability*ProbabilityPermissionValue), rate) );
								}
							}
						}

						inline void AddDamageToHpRate(float rate, float probability) {
							RateToProbabilityTable::iterator iter = std::find_if(damageToHpRates.begin(), damageToHpRates.end(), FindByProbability(probability));
							if( iter != damageToHpRates.end() ) {
								iter->second += rate;
								if( IsZero(iter->second) == true ) {
									damageToHpRates.erase( iter );
								}
							}else {
								if( IsZero(rate) == false ) {
									damageToHpRates.push_back( std::pair<int, float>(static_cast<int>(probability*ProbabilityPermissionValue), rate) );
								}
							}
						}
						inline void AddDamageToMpRate(float rate, float probability) {
							RateToProbabilityTable::iterator iter = std::find_if(damageToMpRates.begin(), damageToMpRates.end(), FindByProbability(probability));
							if( iter != damageToMpRates.end() ) {
								iter->second += rate;
								if( IsZero(iter->second) == true ) {
									damageToMpRates.erase( iter );
								}
							}else {
								if( IsZero(rate) == false ) {						
									damageToMpRates.push_back( std::pair<int, float>(static_cast<int>(probability*ProbabilityPermissionValue), rate) );
								}
							}
						}

						float GetAbsorbToHp(float currentProbability, float damage) const;
						float GetAbsorbToMp(float currentProbability, float damage) const;
						float GetDamageToHp(float currentProbability, float damage) const;
						float GetDamageToMp(float currentProbability, float damage) const;
					private:
						void RevisionRate(float& value);
						bool IsZero(float value) const;
					private:
						const static int ProbabilityPermissionValue = 10000;
						typedef std::vector< std::pair<int/*probability persent*/, float /*rate*/> > RateToProbabilityTable;

						struct FindByProbability
						{
							int probability;

							inline FindByProbability(const float inProbability) : probability(static_cast<int>(inProbability*ProbabilityPermissionValue)){}

							inline bool operator()(const std::pair<int, float >& in) const { return (in.first==probability); }
						};

						RateToProbabilityTable	absorbToHpRates;
						RateToProbabilityTable	absorbToMpRates;
						RateToProbabilityTable	damageToHpRates;
						RateToProbabilityTable	damageToMpRates;				
					}convertDamage;

					Actor::HeightControl heightControl;
				public :
					inline Status(CharacterData* data)
						: basicAtkSpeed( 1.0f )
						, MaxHp(data->MaxHp)
						, Hp(data->Hp)
						, MaxMp(data->MaxMp)
						, Mp(data->Mp)
						, AtkSpeed( 1.0f )
						, Lv(data->Level)
						, hpPlus(0)
						, hpMul(1)
						, mpPlus(0)
						, mpMul(1)
						, atkSpeedPlus( 0.0f )
						, atkSpeedMul( 1.0f )
						, dodgeRate(0)
					{ 
						for ( int a = 0; a < Constants::DamageType::CNT; ++a )
						{
							resist[ a ] = 0; 
						}
					}

					inline void operator = ( Status& )
					{
					}

					inline void Init()
					{
						MaxStatus = XRated::Constants::MaxStat;
						LimitStr = XRated::Constants::MaxStat;
						LimitDex = XRated::Constants::MaxStat;
						LimitInt = XRated::Constants::MaxStat;
						LimitVit = XRated::Constants::MaxStat;
						StrFactor = 1.0f;
						IntFactor = 1.0f;
						DexFactor = 1.0f;
						VitFactor = 1.0f;

						hpPlus=0;
						hpMul=1;
						mpPlus=0;
						mpMul=1;

						basicAtkSpeed = 1.0f;
						AtkSpeed = 1.0f;
						atkSpeedPlus = 0.0f;
						atkSpeedMul = 1.0f;

						dodgeRate = 0;
						std::fill(resist, resist+(sizeof(resist)/sizeof(float)), .0f);
						element = Constants::DamageType::INDEPENDENCE;
						Ac.Init();
						dmg.Init();
						immune.Init();
						heightControl.Init();
						guard.Init();
						prob.Init();
						convertDamage.Init();
					}

					/**
					@brief calculate all status members from level and character information
					*/
					virtual void Recalculate(bool bIgnoreLimit);

					virtual void RecalculateHP(bool bFill, bool bIgnoreLimit);

					virtual void RecalculateMP(bool bFill, bool bIgnoreLimit);

					void RecalculateAtkSpeed();

					inline void RecalculateAc(bool bIgnoreLimit)
					{ 
						Ac.Recalculate(bIgnoreLimit); 
						dodgeRate = StatusFunctions::GetDodgeProb(GetDex()); 
					}

					inline void RecalculateDmg(bool bIgnoreLimit)
					{ 
						dmg.Recalculate(bIgnoreLimit); 
					}

					inline void RecalculateProb(bool bIgnoreLimit=false)
					{ 
						prob.Recalculate(bIgnoreLimit); 
					}

					BasicStat GetBasicStat() const;

					int GetStr()const;
					int GetInt()const;
					int GetDex()const;
					int GetVit()const;

					void AddStr( int str );
					void AddInt( int i );
					void AddDex( int dex );
					void AddVit( int vit );

					void AddStrFactor( float factor );
					void AddIntFactor( float factor );
					void AddDexFactor( float factor );
					void AddVitFactor( float factor );

					int SmartCast( float value , int validFigure ) const;
				} status;

				float3 bufferedDirection;

				float pauseTime;

				bool bCollide;
				bool bOverlapped;

				Action::ActionManager actions;

				Stat::StateBundleManager stateBundleManager;
				IgnoreState ignoreState;

				uint32 stunFlag;
				std::list<uint32> stunFlags;

				int airHitCnt;
				float airHitFactor;

				Database::Info::StateBundleInfo* morphStateBundleInfo;

				uint32 GetMaxAirComboCount();
			public :
				Actor(Constants::ObjectType type);
				virtual ~Actor() {}
				void operator= (Actor&) {}

				inline Lunia::XRated::CharacterData& GetCharacterData()
				{ 
					return characterData; 
				}
				inline Action::Action::ActionData& GetActionData()
				{
					return actions.GetActionData(); 
				}
				inline IgnoreState& GetIgnoreState()
				{
					return ignoreState; 
				}


				virtual void Initialize(IGameStateDatabase* db);

				inline virtual int GetClass() const
				{
					return -1; 
				}

				inline virtual uint8 GetTeam() const
				{
					return characterData.Team; 
				}
				inline void SetTeam(uint8 teamNumber)
				{
					characterData.Team = teamNumber; 
				}
				inline virtual uint8 GetAlliance() const
				{ 
					return characterData.Alliance; 
				}
				inline void SetAlliance(uint8 alliNum)
				{
					characterData.Alliance = alliNum; 
				}

				inline virtual const float& GetHP() const
				{
					return status.Hp; 
				}
				inline virtual const float& GetHP(float& max) const
				{
					max = status.MaxHp; return status.Hp;
				}
				virtual void SetHP(float hp, float max=0); //overrided by nonplayer class.
				virtual void ChangeHpFromState(float hp, float max,Lunia::XRated::Serial donor);
				inline virtual const float& GetMP() const
				{ 
					return status.Mp; 
				}
				inline virtual const float& GetMP(float& max) const
				{
					max = status.MaxMp; return status.Mp;
				}
				virtual void SetMP(float mp, float max=0);

				inline virtual uint16 GetLevel() const
				{ 
					return status.Lv; 
				}

				inline virtual void GetBaseStat(int& Str, int& Dex, int& Int, int& Vit) const
				{
					Str=status.GetStr();Dex=status.GetDex();Int=status.GetInt();Vit=status.GetVit();
				}

				inline virtual HeightControl& GetHeightInfo()
				{
					return status.heightControl; 
				}

				inline virtual void SetHeight(float height)
				{
					objectData.Position.y = height; status.heightControl.height = height;
				}
				inline virtual float GetHeight() const
				{
					return objectData.Position.y; 
				}
				void InitHeightInfo();

				inline virtual const float3& GetBufferedDirection() const
				{
					return bufferedDirection;
				}
				inline virtual void SetBufferedDirection(const float3& dir)
				{
					bufferedDirection = dir;
				}

				inline virtual float GetResist(int type) const
				{
					return (type<Constants::DamageType::CNT)?status.resist[type]:0.0f; 
				}
				inline virtual float GetResist(Constants::DamageType type) const
				{
					return (type<Constants::DamageType::CNT)?status.resist[type]:0.0f; 
				}

				inline uint32 SetIgnoreStun(uint32 flag, bool bcopy = false)
				{
                    uint32 old = stunFlag; 
					
					stunFlags.push_back( flag );
					stunFlag |= flag;

					return old;
				}
				inline uint32 RemoveIgnoreStun(uint32 flag, bool bcopy = false)
				{
					uint32 old = stunFlag; 
					for( std::list<uint32>::iterator it = stunFlags.begin(), endIt=stunFlags.end(); it!=endIt; ++it )
					{
						if( *it == flag )
						{
							stunFlags.erase( it );
							break;
						}
					}

					stunFlag = 0;
					for( std::list<uint32>::const_iterator it = stunFlags.begin(), endIt=stunFlags.end(); it!=endIt; ++it )
					{
						stunFlag |= *it;
					}

					return old;
				}
				inline uint32 IsIgnoreStun(uint32 flag) const
				{
					return flag & stunFlag; 
				}
				
				inline uint32 SetIgnoreFreeze(uint32 flag, bool bcopy = false)
				{
                    uint32 old = stunFlag; 
					
					stunFlags.push_back( flag );
					stunFlag |= flag;

					return old;
				}
				inline uint32 RemoveIgnoreFreeze(uint32 flag, bool bcopy = false)
				{
					uint32 old = stunFlag; 
					for( std::list<uint32>::iterator it = stunFlags.begin(), endIt=stunFlags.end(); it!=endIt; ++it )
					{
						if( *it == flag )
						{
							stunFlags.erase( it );
							break;
						}
					}

					stunFlag = 0;
					for( std::list<uint32>::const_iterator it = stunFlags.begin(), endIt=stunFlags.end(); it!=endIt; ++it )
					{
						stunFlag |= *it;
					}

					return old;
				}
				inline uint32 IsIgnoreFreeze(uint32 flag) const
				{
					return flag & stunFlag; 
				}



				inline float GetSpeedFactor() const
				{
					return characterData.SpeedVector; 
				}
				inline float AddSpeedFactor(float spd)
				{
					if ( characterData.SpeedVector+spd < 0 )
					{
						spd = -characterData.SpeedVector;
					}
					characterData.SpeedVector += spd;
					return spd;
				}
				void SetSizeRate(float rate=0);

				bool IsDodge();
				virtual float CalculatedDmg(float dmg);

				//Collision functions
				virtual void ChangeCollisionState();
				inline virtual bool IsCollide() const
				{
					return bCollide; 
				}
				inline virtual void SetOverlappable()
				{ 
					bOverlapped = true; 
				}
				inline virtual void SetOverlap( bool overlap )
				{
					bOverlapped = overlap; 
				}
				inline virtual bool IsOverlapped() const
				{
					return bOverlapped; 
				}
				
				inline virtual bool Command(Action::Action::CommandType state, uint32 param);

				inline virtual bool DoDamage(Object* who, uint32 creatorActionHash, float minDmg, float maxDmg, float damage, Constants::DamageType dmgType, uint32 stateID, Constants::AttackStateType attackType, unsigned int& sFlag);


				//Status
				inline virtual void AddStr( int str )
				{
					status.AddStr( str );
				}

				inline virtual void AddStrFactor( float factor )
				{
					status.AddStrFactor( factor );
				}

				inline virtual void AddInt( int i )
				{
					status.AddInt( i );
				}

				inline virtual void AddIntFactor( float factor )
				{
					status.AddIntFactor( factor );
				}

				inline virtual void AddDex( int dex )
				{
					status.AddDex( dex );
				}

				inline virtual void AddDexFactor( float factor )
				{
					status.AddDexFactor( factor );
				}

				inline virtual void AddVit( int vit )
				{
					status.AddVit( vit );
				}

				inline virtual void AddVitFactor( float factor )
				{
					status.AddVitFactor( factor );
				}

				virtual void GetPlusDamage(float&, float&);
				virtual float GetPlusMaxDamage();
				virtual float GetPlusMinDamage();
				virtual void GetDamage(float&, float&);
				virtual void AddDamage(float, float);
				virtual void AddDamage(float);
				virtual void RemoveDamage(float, float);
				virtual void RemoveDamage(float);
				inline virtual float GetAc() const
				{
					return status.Ac.ac; 
				}
				virtual void AddAc(float);
				virtual void RemoveAc(float);
				virtual void AddAcMul(float ac);
				virtual void RemoveAcMul(float ac);
				virtual void AddReduceDmg(float);
				virtual void RemoveReduceDmg(float);
				virtual void AddResist(float reg, int type=11);
				virtual void RemoveResist(float reg, int type=11);
				inline virtual void AddHP(float hp) 
				{ 
					status.hpPlus += hp; 
				}
				inline virtual void AddHPMul(float hp)
				{ 
					status.hpMul += hp; 
				}
				inline virtual void AddMP(float mp)
				{
					status.mpPlus += mp; 
				}
				inline virtual void AddMPMul(float mp)
				{
					status.mpMul += mp; 
				}

				// Probability
				inline virtual void AddCriAtk(float probability)
				{
					status.prob.AddCriAtk(probability); 
				}
				inline virtual void AddCriMagic(float probability)
				{
					status.prob.AddCriMagic(probability); 
				}

				inline virtual void AddLessMp(float probability)
				{
					status.prob.AddLessMp(probability); 
				}
				inline virtual void AddMinimize(float probability)
				{
					status.prob.AddMinimize(probability); 
				}

				inline virtual void AddIgnoreCriHit(float probability)
				{
					status.prob.AddIgnoreCriHit(probability); 
				}

				inline virtual void AddAbsorbToHpRate(float rate, float probability)
				{
					status.convertDamage.AddAbsorbToHpRate(rate, probability); 
				}
				inline virtual	void AddAbsorbToMpRate(float rate, float probability)
				{ 
					status.convertDamage.AddAbsorbToMpRate(rate, probability); 
				}

				inline virtual void AddDamageToHpRate(float rate, float probability)
				{
					status.convertDamage.AddDamageToHpRate(rate, probability); 
				}
				inline virtual void AddDamageToMpRate(float rate, float probability)
				{
					status.convertDamage.AddDamageToMpRate(rate, probability); 
				}


				void SetImmune(uint16 i);
				void RemoveImmune(uint16 i);
				bool IsImmune(Constants::DamageType type);
				//Recalculate second stat
				inline virtual void RecalculateStatus(bool bIgnoreLimit = false)
				{
					status.Recalculate(bIgnoreLimit); 
					RecalculateSecondStat();	/// by kpongky( 09.09.24 ) stat revolution.
				}

				inline virtual void RecalculateHP(bool bFill, bool bIgnoreLimit)
				{
					status.RecalculateHP(bFill, bIgnoreLimit); 
				}

				inline virtual void RecalculateMP(bool bFill, bool bIgnoreLimit)
				{
					status.RecalculateMP(bFill, bIgnoreLimit); 
				}

				inline void RecalculateAtkSpeed()
				{
					status.RecalculateAtkSpeed(); 
				}

				inline void RecalculateAc(bool bIgnoreLimit)
				{
					status.RecalculateAc(bIgnoreLimit); 
				}

				inline void RecalculateDmg(bool bIgnoreLimit)
				{
					status.RecalculateDmg(bIgnoreLimit); 
				}

				virtual void Debuff(float debuffRate, int slot, uint32 subCategoryHash);

				bool IsLessMp(IGameStateDatabase* db);
				bool IsMinimize(IGameStateDatabase* db);
				bool IsCriAtk(IGameStateDatabase* db);
				bool IsCriMagic(IGameStateDatabase* db);
				bool IsIgnoreCriHit(IGameStateDatabase* db);
				inline void CriticalRushActivated(bool activate)
				{
					status.prob.CriticalRushActivated(activate); 
				}

				void AbsorbToHpMp(float damage);
				void DamageToHpMp(float damage);

				inline virtual void AddEquipSetBonus()
				{
					return; 
				}

				bool RemoveAllStateBundle(Database::Info::StateInfo::Type type);
				bool RemoveStateBundle(Stat::IState* state);
				bool AddStateBundle(Database::Info::StateBundleInfo* info,Lunia::XRated::Serial creator,Lunia::XRated::Serial byWhom=0);
				void AddPassive(Database::Info::StateBundleInfo* info);
				void RemovePassive(Database::Info::StateBundleInfo* info);
				void ProcessStateBundle(const Database::Info::StateBundleInfo::Condition& condition);

				void ProcessStateWhenHit(Actor* target, float damage, uint32 sFlag);
				void ProcessStateWhenHitted(Actor* attacker , float damage, uint32 sFlag);

				//Action
				void ClearMorphStateBundleInfo(uint32 objectHash);

				inline virtual bool SetAction(uint32 name, Action::Action::IActionParam* param, bool force = false)
				{
					actions.RemoveReservedAction();
					return actions.SetAction(name, this, param, *stageData, force);
				}
				inline virtual void ReserveAction(uint32 name, Action::Action::IActionParam* param, bool force = false)
				{
					actions.ReserveAction(name, 0, param, force);
				}
				inline void RemoveReservedAction()
				{
					actions.RemoveReservedAction(); 
				}
				inline const std::wstring& GetActionName()
				{
					return actions.GetAction(L"")->GetName(); 
				}
				inline uint32 GetActionNameHash()
				{
					return actions.GetAction(uint32(0))->GetNameHash(); 
				}
				inline Action::Action* GetCurrentAction()
				{
					return actions.GetAction(L""); 
				}
				virtual void PauseAction(float dt);
				inline virtual float GetPauseTime() const
				{
					return pauseTime; 
				}
				inline virtual void SetActionDuration(float t)
				{
					actions.SetActionDuration(t); 
				}
				Action::Action::ActionPtrList* SetActionList(Action::Action::ActionPtrList* newActionList, uint32 defaultAction);
				inline void HitOthers()
				{
					actions.Hit(); 
				}

				// For monsters. In NPC class skill name directly matched to action name.
				inline virtual bool UseSkill(uint32 skill)
				{
					return actions.SetAction(skill, this, NULL, *stageData); 
				}

				//Guard action method
				inline void AddGuardProbability(uint32 f, float prob)
				{
					status.guard.AddGuardProbability(f, prob); 
				}
				inline void RemoveGuardProbability(uint32 f, float prob)
				{
					status.guard.RemoveGuardProbability(f, prob); 
				}
				inline bool IsGuarding(uint32 flag)
				{
					return ( status.guard.flag & 1<<flag ) ? true : false; 
				}
				bool IsGuardActivated(uint32 flag);

				virtual void LoadActions(const std::wstring& objName, const std::wstring& defaultAction, Action::Action::ActionPtrList* list);

				virtual bool Update(float dt, IGameStateDatabase* db);

				//help functions
				const Database::Info::ActionInfo* GetActionInfo(uint32 hash);

				///////////////////////////////////////////////////////////////////////////////////////////////////////
			protected:
				std::vector<Serial>		damagePlayerList;
				std::vector<Serial>		attackPlayerList;

				std::vector<Serial>		attackStateList;
				std::vector<uint32>		attackStateIDList;
				std::vector<uint32>		damageStateIDList;

				uint32  curAirAttackComboCnt;
				uint32	maxAirAttackComboCnt;

			public :
				void AddAttacker(Object* who, uint32 stateID, Constants::AttackStateType attackType);
				void AddAirAttackCombo(Serial damager);

				void DeleteDamager(Serial damager);

				uint32 GetCurAirAttackComboCnt();
				uint32 GetMaxAirAttackComboCnt();

				void UpdateAirAttackCombo();

				void ClearAirAttackData();
				void ClearAttackPlayerList();
				void InitAirAttackComboCnt();

				bool IsOnAirState();
				bool IsAddableAirComboCount(Serial objectSerial, uint32 stateID, Constants::AttackStateType attackType);

				void UpdateStylePoint(Object* who);
				///////////////////////////////////////////////////////////////////////////////////////////////////////

			public:
				bool ProcessCondition(Database::Info::StateInfo::Type conditionStateType, Object* object = NULL);

				void AddAttackSpeed( float amount );
				void AddAttackSpeedRate( float rate );
				float GetAttackSpeed();

				virtual void RecalculateSecondStat();

				virtual float GetSkillDamageIncreaseFactorByStat() const;
				virtual float GetCriticalProbabilityByStat() const;
				virtual float GetCoolDownTimeReduceAmount() const;
				virtual float GetPhysicalCriticalInc() const;
				virtual float GetMagicalCriticalInc() const;
				virtual float GetHitIgnoreHpRate() const;
				virtual float GetHealIncRateByStat() const;
				virtual bool IsHitEffectIgnored() const;
				virtual const float GetHitIgnoreProb( const float hitIgnoreValue ) const;
				virtual const float GetHitIgnoreValue() const;
			};
		}	
	}	
}
