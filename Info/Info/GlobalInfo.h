#pragma once
#include "./Database.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct DifficultyInfo : public Serializer::ISerializable
				{
					std::wstring Name;
					uint8 Life;
					float HpFactor;
					float XpFactor;
					float IntFactor;
					float DropFactor;
					float DmgFactor;
					float AtkFactor;
					float UpdateTick;
					float UpdateFightTick;

				public:/* lSerializable implements */
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
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
						BOUNDEDMOVESPEED,
						SELFHEAL,
						AREAHEAL,
						CLEAR,
						REGENHP,
						REGENMP,
						BOUNDEDREGENHP,
						BOUNDEDREGENMP,
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
						ADDDEFENCE,
						ENABLEADDDEFENCE,
						CAMPFIRE,
						IGNORESTUN,
						IGNORECRITICALHIT,
						IGNOREFREEZE,
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
						INCREASESKILLDAMAGE,
						//CONVERTDMGTOHPMP,
						ABSORBTOHPMP,
						DMGTOHPMP,
						GOLDGAIN,
						IFHIT,
						IFHITBY,
						IFDAMAGED,
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
				struct BasicStatInfo : public Serializer::ISerializable {
					//const static int SIZE=6;
					struct Stat : public Serializer::ISerializable {
						int Str;
						int Dex;
						int Vit;
						int Int;

					public:
						Stat() {}
						Stat(int s, int d, int v, int i) : Str(s), Dex(d), Vit(v), Int(i) { }
						void Set(int s, int d, int v, int i) { Str = s; Dex = d; Vit = v; Int = i; }

						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
					std::wstring name;
					std::vector<Stat> stats;
					Constants::CharacterMajorStatType majorStat;
					//Stat stats[SIZE];

				public:
					BasicStatInfo() {}

					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}