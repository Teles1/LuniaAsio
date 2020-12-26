#pragma once
#include "./GlobalInfo.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				void DifficultyInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::DifficultyInfo");
					out.Write(L"Name", Name);
					out.Write(L"Life", Life);
					out.Write(L"HpFactor", HpFactor);
					out.Write(L"XpFactor", XpFactor);
					out.Write(L"DropFactor", DropFactor);
					out.Write(L"IntFactor", IntFactor);
					out.Write(L"DmgFactor", DmgFactor);
					out.Write(L"AtkFactor", AtkFactor);
					out.Write(L"UpdateTick", UpdateTick);
					out.Write(L"UpdateFightTick", UpdateFightTick);
				}

				void DifficultyInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::DifficultyInfo");
					in.Read(L"Name", Name);
					in.Read(L"Life", Life);
					in.Read(L"HpFactor", HpFactor);
					in.Read(L"XpFactor", XpFactor);
					in.Read(L"DropFactor", DropFactor);
					in.Read(L"IntFactor", IntFactor);
					in.Read(L"DmgFactor", DmgFactor);
					in.Read(L"AtkFactor", AtkFactor);
					in.Read(L"UpdateTick", UpdateTick);
					in.Read(L"UpdateFightTick", UpdateFightTick);
				}

				StateInfo::Type StateInfo::StringToType(const wchar_t* str)
				{
					static std::map<std::wstring, Type > string_type;
					static bool loaded = false;

					if (!loaded)
					{
						for (int i = 0; i < Type::STATECOUNT; ++i)
						{
							Type type = static_cast<Type>(i);
							std::wstring typeStr;
							TypeToString(type, typeStr);
							string_type[typeStr] = type;
							//ALLM_INFO((L"%s | %d " , typeStr.c_str(), type ));
						}

						std::wstring tmp;
						TypeToString(StateInfo::Type::NOLOGIC, tmp);
						string_type[tmp] = StateInfo::Type::NOLOGIC;

						loaded = true;
						Logger::GetInstance().Info(L"String vs STATE Type index created [Count: {0}]", string_type.size());
					}

					std::map<std::wstring, Type >::const_iterator it = string_type.find(str);
					if (it == string_type.end())
						throw StateTypeNotFoundException(str);

					return it->second;
				}

				void StateInfo::TypeToString(StateInfo::Type type, std::wstring& returnValue)
				{
					switch (type) {
					case StateInfo::Type::ATTACK:
						returnValue = L"ATTACK";
						break;
					case StateInfo::Type::STUN:
						returnValue = L"STUN";
						break;
					case StateInfo::Type::MAKESTUN:
						returnValue = L"MAKESTUN";
						break;
					case StateInfo::Type::DOWN:
						returnValue = L"DOWN";
						break;
					case StateInfo::Type::FLIED:
						returnValue = L"FLIED";
						break;
					case StateInfo::Type::KNOCKBACK:
						returnValue = L"KNOCKBACK";
						break;
					case StateInfo::Type::MOVE:
						returnValue = L"MOVE";
						break;
					case StateInfo::Type::STEP:
						returnValue = L"STEP";
						break;
					case StateInfo::Type::CHARGE:
						returnValue = L"CHARGE";
						break;
					case StateInfo::Type::INVINCIBLE:
						returnValue = L"INVINCIBLE";
						break;
					case StateInfo::Type::DESTROY:
						returnValue = L"DESTROY";
						break;
					case StateInfo::Type::LESSMP:
						returnValue = L"LESSMP";
						break;
					case StateInfo::Type::INCSTAT:
						returnValue = L"INCSTAT";
						break;
					case StateInfo::Type::GHOST:
						returnValue = L"GHOST";
						break;
					case StateInfo::Type::DAMAGE:
						returnValue = L"DAMAGE";
						break;
					case StateInfo::Type::MAGICAMR:
						returnValue = L"MAGICAMR";
						break;
					case StateInfo::Type::RESIST:
						returnValue = L"RESIST";
						break;
					case StateInfo::Type::IMMUNE:
						returnValue = L"IMMUNE";
						break;
					case StateInfo::Type::DOT:
						returnValue = L"DOT";
						break;
					case StateInfo::Type::DOTMP:
						returnValue = L"DOTMP";
						break;
					case StateInfo::Type::CHOKE:
						returnValue = L"CHOKE";
						break;
					case StateInfo::Type::PASSABLE:
						returnValue = L"PASSABLE";
						break;
					case StateInfo::Type::HATE:
						returnValue = L"HATE";
						break;
					case StateInfo::Type::INCDMG:
						returnValue = L"INCDMG";
						break;
					case StateInfo::Type::INCHEALAMOUNT:
						returnValue = L"INCHEALAMOUNT";
						break;
					case StateInfo::Type::ATTACKSPEED:
						returnValue = L"ATTACKSPEED";
						break;
					case StateInfo::Type::BOUNDEDMOVESPEED:
						returnValue = L"BOUNDEDMOVESPEED";
						break;
					case StateInfo::Type::SELFHEAL:
						returnValue = L"SELFHEAL";
						break;
					case StateInfo::Type::AREAHEAL:
						returnValue = L"AREAHEAL";
						break;
					case StateInfo::Type::CLEAR:
						returnValue = L"CLEAR";
						break;
					case StateInfo::Type::REGENHP:
						returnValue = L"REGENHP";
						break;
					case StateInfo::Type::REGENMP:
						returnValue = L"REGENMP";
						break;
					case StateInfo::Type::BOUNDEDREGENHP:
						returnValue = L"BOUNDEDREGENHP";
						break;
					case StateInfo::Type::BOUNDEDREGENMP:
						returnValue = L"BOUNDEDREGENMP";
						break;
					case StateInfo::Type::RESURRECTION:
						returnValue = L"RESURRECTION";
						break;
					case StateInfo::Type::WEAKEN:
						returnValue = L"WEAKEN";
						break;
					case StateInfo::Type::ROOT:
						returnValue = L"ROOT";
						break;
					case StateInfo::Type::SLEEP:
						returnValue = L"SLEEP";
						break;
					case StateInfo::Type::SLOW:
						returnValue = L"SLOW";
						break;
					case StateInfo::Type::FREEZE:
						returnValue = L"FREEZE";
						break;
					case StateInfo::Type::BLIND:
						returnValue = L"BLIND";
						break;
					case StateInfo::Type::ZOMBIE:
						returnValue = L"ZOMBIE";
						break;
					case StateInfo::Type::CRIALLINC:
						returnValue = L"CRIALLINC";
						break;
					case StateInfo::Type::CRIATK:
						returnValue = L"CRIATK";
						break;
					case StateInfo::Type::CRIATKINC:
						returnValue = L"CRIATKINC";
						break;
					case StateInfo::Type::ELEMENTALATK:
						returnValue = L"ELEMENTALATK";
						break;
					case StateInfo::Type::CRIMAGIC:
						returnValue = L"CRIMAGIC";
						break;
					case StateInfo::Type::CRIMAGICINC:
						returnValue = L"CRIMAGICINC";
						break;
					case StateInfo::Type::CRIRUSH:
						returnValue = L"CRIRUSH";
						break;
					case StateInfo::Type::TAUNT:
						returnValue = L"TAUNT";
						break;
					case StateInfo::Type::BLOCKUSINGSKILL:
						returnValue = L"BLOCKUSINGSKILL";
						break;
					case StateInfo::Type::MINIMIZE:
						returnValue = L"MINIMIZE";
						break;
					case StateInfo::Type::IGNOREDMG:
						returnValue = L"IGNOREDMG";
						break;
					case StateInfo::Type::REFLECTION:
						returnValue = L"REFLECTION";
						break;
					case StateInfo::Type::DEVINESHD:
						returnValue = L"DEVINESHD";
						break;
					case StateInfo::Type::DEFENCE:
						returnValue = L"DEFENCE";
						break;
					case StateInfo::Type::ADDDEFENCE:
						returnValue = L"ADDDEFENCE";
						break;
					case StateInfo::Type::ENABLEADDDEFENCE:
						returnValue = L"ENABLEADDDEFENCE";
						break;
					case StateInfo::Type::CAMPFIRE:
						returnValue = L"CAMPFIRE";
						break;
					case StateInfo::Type::IGNORESTUN:
						returnValue = L"IGNORESTUN";
						break;
					case StateInfo::Type::IGNORECRITICALHIT:
						returnValue = L"IGNORECRITICALHIT";
						break;
					case StateInfo::Type::IGNOREFREEZE:
						returnValue = L"IGNOREFREEZE";
						break;
					case StateInfo::Type::DESTROYENEMY:
						returnValue = L"DESTROYENEMY";
						break;
					case StateInfo::Type::CREATENPC:
						returnValue = L"CREATENPC";
						break;
					case StateInfo::Type::CREATEOBJECT:
						returnValue = L"CREATEOBJECT";
						break;
					case StateInfo::Type::CREATEPROJECTILE:
						returnValue = L"CREATEPROJECTILE";
						break;
					case StateInfo::Type::SPREADHATE:
						returnValue = L"SPREADHATE";
						break;
					case StateInfo::Type::INCLIFE:
						returnValue = L"INCLIFE";
						break;
					case StateInfo::Type::CHANGEACTION:
						returnValue = L"CHANGEACTION";
						break;
					case StateInfo::Type::MORPH:
						returnValue = L"MORPH";
						break;
					case StateInfo::Type::REVERSECMD:
						returnValue = L"REVERSECMD";
						break;
					case StateInfo::Type::INVISIBLE:
						returnValue = L"INVISIBLE";
						break;
					case StateInfo::Type::RESETCOOLDOWN:
						returnValue = L"RESETCOOLDOWN";
						break;
					case StateInfo::Type::ADDBUFFWHENHIT:
						returnValue = L"ADDBUFFWHENHIT";
						break;
					case StateInfo::Type::ADDBUFFWHENHITTED:
						returnValue = L"ADDBUFFWHENHITTED";
						break;
					case StateInfo::Type::INCSKILLLEVEL:
						returnValue = L"INCSKILLLEVEL";
						break;
					case StateInfo::Type::REVIVE:
						returnValue = L"REVIVE";
						break;
					case StateInfo::Type::DETECTINVISIBLE:
						returnValue = L"DETECTINVISIBLE";
						break;
					case StateInfo::Type::IGNORESTATE:
						returnValue = L"IGNORESTATE";
						break;
					case StateInfo::Type::RESETSKILLGROUPCOOLDOWN:
						returnValue = L"RESETSKILLGROUPCOOLDOWN";
						break;
					case StateInfo::Type::DECREASESKILLCOOLDOWN:
						returnValue = L"DECREASESKILLCOOLDOWN";
						break;
					case StateInfo::Type::INCREASESKILLDAMAGE:
						returnValue = L"INCREASESKILLDAMAGE";
						break;
					case StateInfo::Type::ABSORBTOHPMP:
						returnValue = L"ABSORBTOHPMP";
						break;
					case StateInfo::Type::DMGTOHPMP:
						returnValue = L"DMGTOHPMP";
						break;
					case StateInfo::Type::GOLDGAIN:
						returnValue = L"GOLDGAIN";
						break;
					case StateInfo::Type::IFHIT:
						returnValue = L"IFHIT";
						break;
					case StateInfo::Type::IFHITBY:
						returnValue = L"IFHITBY";
						break;
					case StateInfo::Type::IFDAMAGED:
						returnValue = L"IFDAMAGED";
						break;
					case StateInfo::Type::IFHEAL:
						returnValue = L"IFHEAL";
						break;
					case StateInfo::Type::IFCRIATK:
						returnValue = L"IFCRIATK";
						break;
					case StateInfo::Type::IFCRIMAGIC:
						returnValue = L"IFCRIMAGIC";
						break;
					case StateInfo::Type::IFCASTSKILL:
						returnValue = L"IFCASTSKILL";
						break;
					case StateInfo::Type::INVENTFAME:
						returnValue = L"INVENTFAME";
						break;
					case StateInfo::Type::AUTOACTION:
						returnValue = L"AUTOACTION";
						break;
					case StateInfo::Type::NOLOGIC:
						returnValue = L"NOLOGIC";
						break;
					default:
						throw StateTypeNotFoundException(type);
					}
				}
				void BasicStatInfo::Stat::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::BasicStatInfo::Stat");
					out.Write(L"Str", Str);
					out.Write(L"Dex", Dex);
					out.Write(L"Vit", Vit);
					out.Write(L"Int", Int);
				}

				void BasicStatInfo::Stat::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::BasicStatInfo::Stat");
					in.Read(L"Str", Str);
					in.Read(L"Dex", Dex);
					in.Read(L"Vit", Vit);
					in.Read(L"Int", Int);
				}

				void BasicStatInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::BasicStatInfo");
					out.Write(L"Name", name);
					out.Write(L"Stats", stats);
					out.WriteEnum(L"MajorStat", majorStat);
				}

				void BasicStatInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::BasicStatInfo");
					in.Read(L"Name", name);
					in.Read(L"Stats", stats);
					in.ReadEnum(L"MajorStat", majorStat, Constants::CharacterMajorStatType::NoMajorStat);
				}
			}
		}
	}
}
