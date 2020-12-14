#pragma once
#include "Actions.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				const std::wstring& ActionInfo::GetActionClassTypeStr(ActionClassType type)
				{
					static std::wstring classStr[20] = { L"NORMAL", L"STAND", L"MOVE", L"DASH", L"STUN", L"ATTACK", L"DOWNSTART", L"DOWNAIR", L"DOWNEND",
					L"DOWNFINISH", L"PHOENIXDOWNAIR", L"PHOENIXDOWN2AIR", L"HITAIR", L"FAINT", L"GHOSTRISE", L"SLEEP", L"STRAFE", L"MOVINGKNOCKBACK", L"HEIGHTZERO", L"CENTERMOVE" }; // 3.1 by ultimate @Oracle 01JAN2020 20:04 EST added centermove
					return classStr[(int)type];
				}

				ActionInfo::ActionClassType ActionInfo::GetActionClassType(const std::wstring& str)
				{
					if (str == L"STAND")
						return ActionInfo::ActionClassType::STAND;
					else if (str == L"MOVE")
						return ActionInfo::ActionClassType::MOVE;
					else if (str == L"DASH")
						return ActionInfo::ActionClassType::DASH;
					else if (str == L"STUN")
						return ActionInfo::ActionClassType::STUN;
					else if (str == L"ATTACK")
						return ActionInfo::ActionClassType::ATTACK;
					else if (str == L"DOWNSTART")
						return ActionInfo::ActionClassType::DOWNSTART;
					else if (str == L"DOWNAIR")
						return ActionInfo::ActionClassType::DOWNAIR;
					else if (str == L"DOWNEND")
						return ActionInfo::ActionClassType::DOWNEND;
					else if (str == L"DOWNFINISH")
						return ActionInfo::ActionClassType::DOWNFINISH;
					else if (str == L"PHOENIXDOWNAIR")
						return ActionInfo::ActionClassType::PHOENIXDOWNAIR;
					else if (str == L"PHOENIXDOWN2AIR")
						return ActionInfo::ActionClassType::PHOENIXDOWN2AIR;
					else if (str == L"CENTERMOVE")
						return ActionInfo::ActionClassType::CENTERMOVE;
					else if (str == L"HITAIR")
						return ActionInfo::ActionClassType::HITAIR;
					else if (str == L"FAINT")
						return ActionInfo::ActionClassType::FAINT;
					else if (str == L"GHOSTRISE")
						return ActionInfo::ActionClassType::GHOSTRISE;
					else if (str == L"SLEEP")
						return ActionInfo::ActionClassType::SLEEP;
					else if (str == L"STRAFE")
						return ActionInfo::ActionClassType::STRAFE;
					else if (str == L"MOVINGKNOCKBACK")
						return ActionInfo::ActionClassType::MOVINGKNOCKBACK;
					else if (str == L"HEIGHTZERO")
						return ActionInfo::ActionClassType::HEIGHTZERO;
					else return ActionInfo::ActionClassType::NORMAL;
				}

				void ActionInfo::State::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::ActionInfo::State");
					out.Write(L"startTime", startTime);
					out.Write(L"endTime", endTime);
					std::wstring typeStr;
					StateInfo::TypeToString(type, typeStr);
					out.Write(L"type", typeStr);
					out.Write(L"params", params);
					out.Write(L"optionalStates", optionalStates);
				}

				void ActionInfo::State::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::ActionInfo::State");
					in.Read(L"startTime", startTime);
					in.Read(L"endTime", endTime);
					std::wstring typeStr;
					in.Read(L"type", typeStr);
					type = StateInfo::StringToType(typeStr.c_str());
					in.Read(L"params", params, std::map<std::wstring, std::wstring>());
					in.Read(L"optionalStates", optionalStates, std::vector<ActionInfo::State>());


					if (optionalStates.size() > 1)
						throw Exception(L"Not Implimented for multi optionalStates. 2008.03.03 Kanak.");

					//#ifdef ALLM_COMPILE_EDITOR
					//		this->stateTypeName = typeStr;
					//#endif
				}

				void ActionInfo::LinkedProjectile::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::ActionInfo::LinkedProjectile");
					out.Write(L"projectileName", projectileName);
					out.Write(L"startTime", startTime);
					out.Write(L"createTime", createTime);
					out.Write(L"position", position);
					out.Write(L"fireAngle", fireAngle);
					out.Write(L"type", type);
					out.Write(L"toTarget", toTarget);
				}

				void ActionInfo::LinkedProjectile::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::ActionInfo::LinkedProjectile");
					in.Read(L"projectileName", projectileName);
					hash = StringUtil::Hash(projectileName.c_str());
					in.Read(L"startTime", startTime);
					in.Read(L"createTime", createTime, 0.0f);
					in.Read(L"position", position);
					in.Read(L"fireAngle", fireAngle);
					in.Read(L"type", type);
					uint16 defaultValue = 0;
					in.Read(L"toTarget", toTarget, defaultValue);
				}

				void ActionInfo::LinkedAction::Input::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::ActionInfo::LinkedAction::Input");
					out.Write(L"startTime", startTime);
					out.Write(L"endTime", endTime);

					std::wstring input;
					if (key & ActionInfo::INPUT::ATTACKA)
						input += L"ATTACKA ";
					if (key & ActionInfo::INPUT::ATTACKARELEASE)
						input += L"ATTACKARELEASE ";
					if (key & ActionInfo::INPUT::ATTACKB)
						input += L"ATTACKB ";
					if (key & ActionInfo::INPUT::ATTACKBRELEASE)
						input += L"ATTACKBRELEASE ";
					if (key & ActionInfo::INPUT::DOWNATTACK)
						input += L"DOWNATTACK ";
					if (key & ActionInfo::INPUT::JUMP)
						input += L"JUMP";

					if ((key & ActionInfo::INPUT::ALLDIRECTION) == ActionInfo::INPUT::ALLDIRECTION)
						input += L"ALLDIRECTION ";
					else {
						if (key & ActionInfo::INPUT::LB)
							input += L"LB ";
						if (key & ActionInfo::INPUT::B)
							input += L"B ";
						if (key & ActionInfo::INPUT::RB)
							input += L"RB ";
						if (key & ActionInfo::INPUT::L)
							input += L"L ";
						if (key & ActionInfo::INPUT::R)
							input += L"R ";
						if (key & ActionInfo::INPUT::LF)
							input += L"LF ";
						if (key & ActionInfo::INPUT::F)
							input += L"F ";
						if (key & ActionInfo::INPUT::RF)
							input += L"RF ";
					}

					if ((key & ActionInfo::INPUT::DASH_ALLDIRECTION) == ActionInfo::INPUT::DASH_ALLDIRECTION)
						input += L"DASH_ALLDIRECTION ";
					else {
						if (key & ActionInfo::INPUT::DASH_LB)
							input += L"DASH_LB ";
						if (key & ActionInfo::INPUT::DASH_B)
							input += L"DASH_B ";
						if (key & ActionInfo::INPUT::DASH_RB)
							input += L"DASH_RB ";
						if (key & ActionInfo::INPUT::DASH_L)
							input += L"DASH_L ";
						if (key & ActionInfo::INPUT::DASH_R)
							input += L"DASH_R ";
						if (key & ActionInfo::INPUT::DASH_LF)
							input += L"DASH_LF ";
						if (key & ActionInfo::INPUT::DASH_F)
							input += L"DASH_F ";
						if (key & ActionInfo::INPUT::DASH_RF)
							input += L"DASH_RF ";
					}

					if (key & ActionInfo::INPUT::STOP)
						input += L"STOP";
					StringUtil::Trim<std::wstring>(input);

					out.Write(L"input", input);
				}

				void ActionInfo::LinkedAction::Input::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::ActionInfo::LinkedAction::Input");
					in.Read(L"startTime", startTime);
					in.Read(L"endTime", endTime);

					key = 0;
					std::wstring input;
					in.Read(L"input", input);
					std::pair<const wchar_t*, const wchar_t*> nextToken(input.c_str(), input.c_str());
					do {
						nextToken = StringUtil::Split(nextToken.second, input.c_str() + input.size(), L' ');
						if (wcslen(nextToken.first) == 0)
							key = ActionInfo::INPUT::NONE;
						else if (wcsncmp(nextToken.first, L"ATTACKARELEASE", 14) == 0)
							key |= ActionInfo::INPUT::ATTACKARELEASE;
						else if (wcsncmp(nextToken.first, L"ATTACKA", 7) == 0)
							key |= ActionInfo::INPUT::ATTACKA;
						else if (wcsncmp(nextToken.first, L"ATTACKBRELEASE", 14) == 0)
							key |= ActionInfo::INPUT::ATTACKBRELEASE;
						else if (wcsncmp(nextToken.first, L"ATTACKB", 7) == 0)
							key |= ActionInfo::INPUT::ATTACKB;
						else if (wcsncmp(nextToken.first, L"DOWNATTACK", 10) == 0)
							key |= ActionInfo::INPUT::DOWNATTACK;
						else if (wcsncmp(nextToken.first, L"JUMP", 4) == 0)
							key |= ActionInfo::INPUT::JUMP;
						else if (wcsncmp(nextToken.first, L"LB", 2) == 0)
							key |= (ActionInfo::INPUT::LB);
						else if (wcsncmp(nextToken.first, L"LF", 2) == 0)
							key |= (ActionInfo::INPUT::LF);
						else if (wcsncmp(nextToken.first, L"RB", 2) == 0)
							key |= (ActionInfo::INPUT::RB);
						else if (wcsncmp(nextToken.first, L"RF", 2) == 0)
							key |= (ActionInfo::INPUT::RF);
						else if (wcsncmp(nextToken.first, L"B", 1) == 0)
							key |= (ActionInfo::INPUT::B);
						else if (wcsncmp(nextToken.first, L"L", 1) == 0)
							key |= (ActionInfo::INPUT::L);
						else if (wcsncmp(nextToken.first, L"R", 1) == 0)
							key |= (ActionInfo::INPUT::R);
						else if (wcsncmp(nextToken.first, L"F", 1) == 0)
							key |= (ActionInfo::INPUT::F);
						else if (wcsncmp(nextToken.first, L"ALLDIRECTION", 12) == 0)
							key |= ActionInfo::INPUT::ALLDIRECTION;
						else if (wcsncmp(nextToken.first, L"DASH_LB", 7) == 0)
							key |= (ActionInfo::INPUT::DASH_LB);
						else if (wcsncmp(nextToken.first, L"DASH_RB", 7) == 0)
							key |= (ActionInfo::INPUT::DASH_RB);
						else if (wcsncmp(nextToken.first, L"DASH_LF", 7) == 0)
							key |= (ActionInfo::INPUT::DASH_LF);
						else if (wcsncmp(nextToken.first, L"DASH_RF", 7) == 0)
							key |= (ActionInfo::INPUT::DASH_RF);
						else if (wcsncmp(nextToken.first, L"DASH_B", 6) == 0)
							key |= (ActionInfo::INPUT::DASH_B);
						else if (wcsncmp(nextToken.first, L"DASH_L", 6) == 0)
							key |= (ActionInfo::INPUT::DASH_L);
						else if (wcsncmp(nextToken.first, L"DASH_R", 6) == 0)
							key |= (ActionInfo::INPUT::DASH_R);
						else if (wcsncmp(nextToken.first, L"DASH_F", 6) == 0)
							key |= (ActionInfo::INPUT::DASH_F);
						else if (wcsncmp(nextToken.first, L"DASH_ALLDIRECTION", 17) == 0)
							key |= ActionInfo::INPUT::DASH_ALLDIRECTION;
						else if (wcsncmp(nextToken.first, L"STOP", 4) == 0)
							key |= (ActionInfo::INPUT::STOP);
					} while (wcslen(nextToken.second) > 0);
				}

				void ActionInfo::LinkedAction::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::ActionInfo::LinkedAction");
					out.Write(L"actionName", actionName);
					out.Write(L"changeTime", changeTime);
					out.Write(L"in", in);
				}

				void ActionInfo::LinkedAction::Deserialize(Serializer::IStreamReader& i)
				{
					i.Begin(L"XRated::Database::Info::ActionInfo::LinkedAction");
					i.Read(L"actionName", actionName);
					hash = StringUtil::Hash(actionName.c_str());
					i.Read(L"changeTime", changeTime);
					i.Read(L"in", in);
				}

				void ActionInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::ActionInfo");
					out.Write(L"actionName", actionName);
					out.Write(L"animationName", animationName);
					out.Write(L"EffectCategory", EffectCategory);
					out.Write(L"EffectPath", EffectPath);
					out.Write(L"EffectCancelTime", EffectCancelTime);
					out.Write(L"Trail", (int)Trail);
					out.Write(L"force", force);
					out.Write(L"playTime", playTime);
					out.Write(L"classType", GetActionClassTypeStr(classType));
					out.Write(L"bUseSkill", useSkill);
					out.Write(L"linkedActions", linkedActions);
					out.Write(L"linkedActionsWhenHit", linkedActionsWhenHit);
					out.Write(L"linkedSkills", linkedSkills);
					out.Write(L"States", states);
					out.Write(L"projectiles", projectiles);
					out.Write(L"stateBundleConditions", stateBundleConditions);
					out.Write(L"enableColorEffect", (int)enableColorEffect);
				}

				void ActionInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::ActionInfo");
					in.Read(L"actionName", actionName);
					hash = StringUtil::Hash(actionName.c_str());
					in.Read(L"animationName", animationName);
					in.Read(L"EffectCategory", EffectCategory, std::wstring());
					in.Read(L"EffectPath", EffectPath, Locator(L""));
					in.Read(L"EffectCancelTime", EffectCancelTime, -1.0f);
					int bTrail;
					in.Read(L"Trail", bTrail, 0);
					Trail = bTrail ? true : false;
					in.Read(L"force", force, 90.0f);
					in.Read(L"playTime", playTime);
					std::wstring classStr;
					in.Read(L"classType", classStr, std::wstring());
					classType = GetActionClassType(classStr);
					in.Read(L"bUseSkill", useSkill, uint32(0));
					in.Read(L"linkedActions", linkedActions, ActionList());
					in.Read(L"linkedActionsWhenHit", linkedActionsWhenHit, ActionList());
					in.Read(L"linkedSkills", linkedSkills, ActionList());
					in.Read(L"States", states, StateList());
					in.Read(L"projectiles", projectiles, ProjectileList());
					in.Read(L"stateBundleConditions", stateBundleConditions, StateBundleInfo::ConditionList());
					int tmp;
					in.Read(L"enableColorEffect", tmp, 0);
					enableColorEffect = tmp ? true : false;
				}
			}
		}
	}
}