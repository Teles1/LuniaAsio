#pragma once
#include "./Database.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				template <typename Template>
				struct PositionData : public Serializer::ISerializable
				{
					Template dataPosition;
					virtual void Serialize(Serializer::IStreamWriter& out) const
					{
						out.Write(L"dataPosition", dataPosition);
					}
					virtual void Deserialize(Serializer::IStreamReader& in)
					{
						in.Read(L"dataPosition", dataPosition);
					}
				};
				struct StateInfo
				{
					enum Sort {
						ORDINARYSTATE = 0,
						ATTACKSTATE,
						DEFENCESTATE,
						NORMAL,
						CONDITION,
					};

					enum Type {
						NONE = -1,
						ATTACK = 0,
						STUN,
						MAKESTUN,
						DOWN,
						FLIED,
						KNOCKBACK,
						MOVE,
						STEP,
						CHARGE,
						INVINCIBLE,
						DESTROY,
						LESSMP,
						INCSTAT,
						GHOST,
						DAMAGE,
						MAGICAMR,
						RESIST,
						IMMUNE,
						DOT,
						DOTMP,
						CHOKE,
						PASSABLE,
						HATE,
						INCDMG,
						INCHEALAMOUNT,
						ATTACKSPEED,
						SELFHEAL,
						AREAHEAL,
						CLEAR,
						REGENHP,
						REGENMP,
						RESURRECTION,
						WEAKEN,
						ROOT,
						SLEEP,
						SLOW,
						FREEZE,
						BLIND,
						ZOMBIE,
						CRIALLINC,
						CRIATK,
						CRIATKINC,
						ELEMENTALATK,
						CRIMAGIC,
						CRIMAGICINC,
						CRIRUSH,
						TAUNT,
						BLOCKUSINGSKILL,
						MINIMIZE,
						IGNOREDMG,
						REFLECTION,
						DEVINESHD,
						DEFENCE,
						CAMPFIRE,
						IGNORESTUN,
						IGNORECRITICALHIT,
						DESTROYENEMY,
						CREATENPC,
						CREATEOBJECT,
						CREATEPROJECTILE,
						SPREADHATE,
						INCLIFE,
						CHANGEACTION,
						MORPH,
						REVERSECMD,
						INVISIBLE,
						RESETCOOLDOWN,
						ADDBUFFWHENHIT,
						ADDBUFFWHENHITTED,
						INCSKILLLEVEL,
						REVIVE,
						DETECTINVISIBLE,
						IGNORESTATE,
						RESETSKILLGROUPCOOLDOWN,
						DECREASESKILLCOOLDOWN,
						//CONVERTDMGTOHPMP,
						ABSORBTOHPMP,
						DMGTOHPMP,
						GOLDGAIN,
						IFHIT,
						IFHITBY,
						IFHEAL,
						IFCRIATK,
						IFCRIMAGIC,
						IFCASTSKILL,
						INVENTFAME,
						AUTOACTION,
						STATECOUNT,
						NOLOGIC
					};

					enum RangeType { Circle = 0, Rectangle, FanShape, Segment, Self };

					static Type StringToType(const wchar_t* str);
					static void TypeToString(Type, std::wstring&);

					class StateTypeNotFoundException : public Exception
					{
					public:
						inline StateTypeNotFoundException(Type t)
							: Exception(L"##StateTypeNotFoundException## Wrong state type : %d", t)
							, type(t)
							, typeStr(L"")
						{}
						inline StateTypeNotFoundException(const wchar_t* str)
							: Exception(L"##StateTypeNotFoundException## Wrong state type : %s", str)
							, type(Type::NONE)
							, typeStr(str)
						{}

						inline Type GetInvalidType() const { return type; }
						inline const std::wstring& GetInvalidTypeStr() const { return typeStr; }
					private:
						Type type;
						std::wstring typeStr;
					};
				};
			}
		}
	}
}