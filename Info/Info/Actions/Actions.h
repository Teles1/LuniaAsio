#pragma once
#include "../Database.h"
#include "../StateBundles/StateBundles.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct ActionInfo : public Serializer::ISerializable
				{
					struct INPUT {
						const static unsigned int NONE = 0x0000;
						const static unsigned int ATTACKA = 0x0001;
						const static unsigned int ATTACKB = 0x0002;
						const static unsigned int DOWNATTACK = 0x0004;
						const static unsigned int STOP = 0x0008;
						const static unsigned int LB = 0x0010;
						const static unsigned int B = 0x0020;
						const static unsigned int RB = 0x0040;
						const static unsigned int L = 0x0080;
						const static unsigned int R = 0x0100;
						const static unsigned int LF = 0x0200;
						const static unsigned int F = 0x0400;
						const static unsigned int RF = 0x0800;
						const static unsigned int DASH_LB = 0x1000;
						const static unsigned int DASH_B = 0x2000;
						const static unsigned int DASH_RB = 0x4000;
						const static unsigned int DASH_L = 0x8000;
						const static unsigned int DASH_R = 0x10000;
						const static unsigned int DASH_LF = 0x20000;
						const static unsigned int DASH_F = 0x40000;
						const static unsigned int DASH_RF = 0x80000;
						const static unsigned int ATTACKARELEASE = 0x100000;
						const static unsigned int ATTACKBRELEASE = 0x200000;
						const static unsigned int JUMP = 0x400000;

						const static unsigned int ALLDIRECTION = LB | B | RB | L | R | LF | F | RF;
						const static unsigned int DASH_ALLDIRECTION = DASH_LB | DASH_B | DASH_RB | DASH_L | DASH_R | DASH_LF | DASH_F | DASH_RF;
					};

					struct State : public Serializer::ISerializable
					{
						float startTime;
						float endTime;
						StateInfo::Type	type;
						std::map<std::wstring, std::wstring> params;
						std::vector<State> optionalStates;
						//#ifdef ALLM_COMPILE_EDITOR
						//				std::wstring stateTypeName;
						//#endif
					public:
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
					typedef std::vector<State> StateList;

					/* functor to find state by stl generics
					@code
						Database::Info::ActionInfo::StateList::const_iterator i
							= std::find_if(info->States.begin(), info->States.end(), Database::Info::ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());
					@endcode
					*/
					template<enum StateInfo::Type T> struct StateFinder
					{
						bool operator()(const ActionInfo::State& state) const { return (state.type == T) ? true : false; }
					};

					struct LinkedProjectile : public Serializer::ISerializable
					{
						std::wstring projectileName;
						uint32 hash;
						float startTime;
						float createTime;
						float2 position;
						uint16 fireAngle;
						uint16 type;	
						uint16 toTarget;

						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
					typedef std::vector<LinkedProjectile> ProjectileList;
					struct LinkedAction : public Serializer::ISerializable
					{
						struct Input : public Serializer::ISerializable
						{
							float startTime;
							float endTime;
							unsigned int key; 
							Input() : key(0) {}

							virtual void Serialize(Serializer::IStreamWriter& out) const;
							virtual void Deserialize(Serializer::IStreamReader& in);
						} in;
						float changeTime;
						std::wstring actionName;
						uint32 hash;
						ActionInfo* info;

						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& i);
					};
					typedef std::vector<LinkedAction> ActionList;

					enum ActionClassType {
						NORMAL = 0, STAND = 1, MOVE = 2, DASH = 3, STUN = 4, ATTACK = 5, DOWNSTART = 6, DOWNAIR = 7, DOWNEND = 8,
						DOWNFINISH = 9, PHOENIXDOWNAIR = 10, HITAIR = 11, FAINT = 12, GHOSTRISE = 13, SLEEP = 14, STRAFE = 15,
						MOVINGKNOCKBACK = 16, HEIGHTZERO = 17, PHOENIXDOWN2AIR = 18, CENTERMOVE = 19
					}; // 3.1 by ultimate @Oralce - 01JAN2020 20:03 EST added centermove. maybe be unnecessary

					static const std::wstring& GetActionClassTypeStr(ActionClassType type);
					static ActionClassType GetActionClassType(const std::wstring& str);
				public:
					bool enableColorEffect;
					std::wstring actionName;
					uint32 hash;
					float playTime;
					ActionClassType classType;
					uint32 useSkill;

					ActionList linkedActions;
					ActionList linkedActionsWhenHit;
					ActionList linkedSkills;
					StateList states;
					ProjectileList projectiles;
					StateBundleInfo::ConditionList stateBundleConditions;

					//clientspecific
					std::wstring animationName;
					std::wstring EffectCategory;
					Locator EffectPath;
					float EffectCancelTime; 
					bool Trail;	
					float force;

					~ActionInfo() {
						
					}

					const wchar_t* GetName() const { return actionName.c_str(); }

					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}