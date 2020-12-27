#include "Enchant.h"
#include "EnchantTable.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Enchant {
				Enchant::EnchantTableManager database;

				std::pair<uint8, uint8> EnchantBitfields::GetIdentifyFields(int pos) const
				{
					switch (pos)
					{
					case 0:
						return std::pair<uint8, uint8>((uint8)Option1, (uint8)Level1);
					case 1:
						return std::pair<uint8, uint8>((uint8)Option2, (uint8)Level2);
					case 2:
						return std::pair<uint8, uint8>((uint8)Option3, (uint8)Level3);
					}
					return std::pair<uint8, uint8>(0, 0);
				}

				std::pair<uint16 /*first*/, uint16 /*second*/> EnchantBitfields::GetMagicalAttach() const
				{
					return std::pair<uint16, uint16>((uint16)Magic1, (uint16)Magic2);
				}

				/* bool NormalBitfields::IsExpired(const DateTime &now) const
				{
					if (ExpiredYear == 0)
						return false; // no-expiration for '0000/00/00'
					DateTime expired(GetExpiredDate());
					if (expired > now)
						return false; // not expired
					return true;
				}

				DateTime NormalBitfields::GetExpiredDate() const
				{
					if (*this == 0)
						return DateTime::Infinite;
					return DateTime(ExpiredYear, ExpiredMonth, ExpiredDay, ExpiredHour, ExpiredMinute, ExpiredSecond);
				}

				void NormalBitfields::ForceExpiration()
				{
					DateTime nowExpiredDate = DateTime::Now();
					ExpiredYear = nowExpiredDate.GetDate().GetYear() - 1;
					ExpiredMonth = nowExpiredDate.GetDate().GetMonth();
					ExpiredDay = nowExpiredDate.GetDate().GetDay();
					ExpiredHour = nowExpiredDate.GetTime().GetHour();
					ExpiredMinute = 0;
					ExpiredSecond = 0;
				}

				void NormalBitfields::MakeUnlimitedPeriod()
				{
					ExpiredYear = 0;
					ExpiredMonth = 0;
					ExpiredDay = 0;
					ExpiredHour = 0;
					ExpiredMinute = 0;
					ExpiredSecond = 0;
				}

				DateTime NormalBitfields::ExtensionExpiredDay(uint32 day)
				{
					if (*this == 0)
						return DateTime::Infinite;

					DateTime nowExpiredDate = DateTime::Now(); //NormalBitfields::GetExpiredDate();

					nowExpiredDate = nowExpiredDate.Add(Lunia::DateTime::Unit::Day, day);

					ExpiredYear = nowExpiredDate.GetDate().GetYear();
					ExpiredMonth = nowExpiredDate.GetDate().GetMonth();
					ExpiredDay = nowExpiredDate.GetDate().GetDay();
					ExpiredHour = nowExpiredDate.GetTime().GetHour();
					ExpiredMinute = nowExpiredDate.GetTime().GetMinute();
					ExpiredSecond = nowExpiredDate.GetTime().GetSecond();

					return DateTime(ExpiredYear, ExpiredMonth, ExpiredDay, ExpiredHour, ExpiredMinute, ExpiredSecond);
				} */

				bool IsEnchantable(const Info::ItemInfo *info)
				{
					/*
							enchanting is only available for equipments
						*/
					if (info->MaxCnt == 1 && info->ItemType == Info::ItemInfo::Type::EQUIPMENT)
					{
						switch (info->EquipParts)
						{
						case XRated::Constants::EquipParts::WEAPON:
						case XRated::Constants::EquipParts::HEAD:
						case XRated::Constants::EquipParts::CHEST:
						case XRated::Constants::EquipParts::LEG:
						case XRated::Constants::EquipParts::HAND:
						case XRated::Constants::EquipParts::FOOT:
						case XRated::Constants::EquipParts::SUPPORT:
						case XRated::Constants::EquipParts::NECKLACE:
						case XRated::Constants::EquipParts::RING:
						case XRated::Constants::EquipParts::EARING:
							return true;
						}
					}

					return false;
				}

				bool IsCashEnchantable(const Info::ItemInfo &info)
				{
					if (info.MaxCnt == 1 && info.ItemType == Info::ItemInfo::Type::EQUIPMENT)
					{
						switch (info.EquipParts)
						{
						case XRated::Constants::EquipParts::CASH_WEAPON:
						case XRated::Constants::EquipParts::CASH_HEAD:
						case XRated::Constants::EquipParts::CASH_CHEST:
						case XRated::Constants::EquipParts::CASH_LEG:
						case XRated::Constants::EquipParts::CASH_HAND:
						case XRated::Constants::EquipParts::CASH_FOOT:
						case XRated::Constants::EquipParts::CASH_SUPPORT:
						case XRated::Constants::EquipParts::CASH_MASK:
						case XRated::Constants::EquipParts::CASH_BACK:
						case XRated::Constants::EquipParts::CASH_HIP:
						case XRated::Constants::EquipParts::CASH_ETC1:
						case XRated::Constants::EquipParts::CASH_ETC2:
						case XRated::Constants::EquipParts::FAMEADJ:
						case XRated::Constants::EquipParts::FAMENOUN:
						case XRated::Constants::EquipParts::PET:
							return true;
						}
					}
					else if (info.ItemType == Info::ItemInfo::Type::PETEQUIPMENT)
					{
						return true;
					}
					return false;
				}
				bool IsRestorableBelonging(const Info::ItemInfo *info, const int64 instance)
				{
					if (IsEnchantable(info))
					{
						//Enchant bit field¸¦ »ç¿ë
						EnchantBitfields bitfields(instance);
						if (bitfields.HasEquipped == 0)
							return false; // ±Í¼ÓµÇÁö ¾Ê¾Ò´Ù
						else if (bitfields.RestoreBelongingTimes == 3)
							return false; // ÇØÁ¦ È½¼ö ÃÊ°ú
					}
					else
					{
						//normal bit field¸¦ »ç¿ë
						NormalBitfields bitfields(instance);
						if (bitfields.HasEquipped == 0)
							return false; // ±Í¼ÓµÇÁö ¾Ê¾Ò´Ù
						else if (bitfields.RestoreBelongingTimes == 3)
							return false; // ÇØÁ¦ È½¼ö ÃÊ°ú
					}
					return true;
				}
				int GetItemRestoreBelongingTimes(const Database::Info::ItemInfo *info, const int64 instance)
				{
					if (IsEnchantable(info))
					{
						//Enchant bit field¸¦ »ç¿ë
						EnchantBitfields bitfields(instance);
						return bitfields.RestoreBelongingTimes;
					}
					else
					{
						//normal bit field¸¦ »ç¿ë
						NormalBitfields bitfields(instance);
						return bitfields.RestoreBelongingTimes;
					}
				}
				bool RestoreBelonging(const Info::ItemInfo *info, InstanceEx &instance)
				{
					if (info == NULL)
						return false;

					if (IsRestorableBelonging(info, instance) == false)
						return false;

					if (IsEnchantable(info) == true)
					{
						EnchantBitfields bitfields(instance);
						bitfields.HasEquipped = 0;
						bitfields.RestoreBelongingTimes++;
						instance = bitfields;
					}
					else
					{
						NormalBitfields bitfields(instance);
						bitfields.HasEquipped = 0;
						bitfields.RestoreBelongingTimes++;
						instance = bitfields;
					}
					return true;
				}

				void MakeLightReinforcementParam(const Info::ItemInfo *info, EnchantBitfields enchant, Info::ItemInfo &resultInfo, float basisRatioValue)
				{
					uint8 lightReinforcementLv = enchant.LightLevel;
					if (lightReinforcementLv == 0)
						return;

					float values[13] = {
						0.0f,
					};
					if (lightReinforcementLv)
					{ //ºû´Ü°è°¡ Á¸Àç ÇÑ´Ù¸é
						for (int stat = 0; stat < Enchant::EnchantTableManager::EnchantTargets::Type::MaxHp; ++stat)
						{
							if (info->Limits.RebirthCount == 0)
							{
								values[stat] += database.RetrieveLightReinforceValue((uint16)info->Limits.Level, lightReinforcementLv - 1, info->EquipParts, stat);
							}
							else
							{
								values[stat] += database.RetrieveLightReinforceValueForRebirth((uint16)info->Limits.StoredLevel, lightReinforcementLv - 1, info->EquipParts, stat);
							}
						}
					}

					std::vector<Info::ActionInfo::State>::const_iterator i = info->States.begin();
					std::vector<Info::ActionInfo::State>::const_iterator iEnd = info->States.end();
					for (; i != iEnd; ++i)
					{
						switch ((*i).type)
						{
						case Info::StateInfo::Type::INCSTAT:
						{
							std::map<std::wstring, std::wstring>::const_iterator iParam = (*i).params.find(L"category");
							if (iParam != (*i).params.end())
							{
								std::wstring strType = iParam->second;
								if (strType == L"MAXHP")
								{
									values[6] += info->Limits.RebirthCount == 0
													 ? database.RetrieveLightReinforceValue((uint16)info->Limits.Level, lightReinforcementLv - 1, info->EquipParts, 6)
													 : database.RetrieveLightReinforceValueForRebirth((uint16)info->Limits.StoredLevel, lightReinforcementLv - 1, info->EquipParts, 6);
								}
								else if (strType == L"MAXMP")
								{
									values[7] += info->Limits.RebirthCount == 0
													 ? database.RetrieveLightReinforceValue((uint16)info->Limits.Level, lightReinforcementLv - 1, info->EquipParts, 7)
													 : database.RetrieveLightReinforceValueForRebirth((uint16)info->Limits.StoredLevel, lightReinforcementLv - 1, info->EquipParts, 7);
								}
							}
						}
						break;
						}
					}

					// °ªµéÀÇ ÇÕÀ» ´Ù ±¸ÇßÀ¸¸é state·Î ¸¸µé¾îÁÖÀÚ.
					Info::ActionInfo::State state;
					state.startTime = 0;
					state.endTime = -1;					   // equipment must be (0, -1)
					state.type = Info::StateInfo::INCSTAT; // current enchant works for.
					state.params[L"type"] = L"AMOUNT";
					state.params[L"bFill"] = L"false";				 // by default, HP/MP should not be filled by equiping.
					state.params[L"marker"] = L"lightReinforcement"; // enchant marker.
					int a = 0;
					for (; a < Enchant::EnchantTableManager::EnchantTargets::Resist1; ++a)
					{
						if (values[a] != 0)
						{ //°ªÀÌ µé¾îÀÖ´Ù¸é.. ¸¸µé¾îÁà¾ßÁö
							//state.params.clear();
							state.params[L"value"] = StringUtil::ToUnicode(values[a]);
							switch ((Enchant::EnchantTableManager::EnchantTargets::Type)a)
							{
							case Enchant::EnchantTableManager::EnchantTargets::Dmg: // damageÀÇ °æ¿ì´Â max, min µÑ´Ù ´õÇØÁØ´Ù.
								state.params[L"category"] = L"ATKMAX";
								resultInfo.States.push_back(state);
								state.params[L"category"] = L"ATKMIN";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Str:
								state.params[L"category"] = L"STR";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Dex:
								state.params[L"category"] = L"DEX";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Int:
								state.params[L"category"] = L"INT";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Vit:
								state.params[L"category"] = L"VIT";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Def:
								state.params[L"category"] = L"DEF";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::MaxHp:
								state.params[L"category"] = L"MAXHP";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::MaxMp:
								state.params[L"category"] = L"MAXMP";
								break;
							}
							resultInfo.States.push_back(state);
						}
					}
					state.params.clear();

					//ÀúÇ×·ÂÀ» ÇØÁÖÀÚ.
					state.type = Info::StateInfo::RESIST;			 // current enchant works for.
					state.params[L"marker"] = L"lightReinforcement"; // enchant marker.
					for (; a < Enchant::EnchantTableManager::EnchantTargets::None; ++a)
					{
						if (values[a] != 0)
						{ //°ªÀÌ µé¾îÀÖ´Ù¸é.. ¸¸µé¾îÁà¾ßÁö
							state.params[L"amount"] = StringUtil::ToUnicode(values[a]);
							switch ((Enchant::EnchantTableManager::EnchantTargets::Type)a)
							{
							case Enchant::EnchantTableManager::EnchantTargets::Resist1:
								state.params[L"type"] = L"0";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Resist2:
								state.params[L"type"] = L"2";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Resist3:
								state.params[L"type"] = L"4";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Resist4:
								state.params[L"type"] = L"6";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Resist5:
								state.params[L"type"] = L"3";
								break;
							}
							resultInfo.States.push_back(state);
						}
					}
				}
				//°¨Á¤ ¿¹»ó °á°úµé ¸®ÅÏ
				void GetIdentifyResults(std::vector<std::vector<float> > &results, const Info::ItemInfo *info, EnchantBitfields enchant)
				{
					float basisRatioValue = 0.0f;
					if (info->Limits.RebirthCount > 0)
					{
						std::vector<Database::Info::ActionInfo::State>::const_iterator itr;
						for (itr = info->States.begin(); itr != info->States.end(); ++itr)
						{
							if (itr->type == Database::Info::StateInfo::INCSTAT)
							{
								std::map<std::wstring, std::wstring>::const_iterator paramItr = itr->params.find(L"category");
								if (paramItr != itr->params.end() &&
									(paramItr->second == L"STR" || paramItr->second == L"DEX" || paramItr->second == L"INT" || paramItr->second == L"VIT"))
								{
									paramItr = itr->params.find(L"value");

									if (paramItr == itr->params.end())
										continue;

									basisRatioValue += Lunia::StringUtil::ToFloat(paramItr->second);
								}
							}
						}
					}

					if (enchant.Identified == 0)
					{
						const std::vector<float> &typeProb = database.GetIdentifyStatusTypeProb();
						const std::vector<float> &gradeProb = database.GetIdentifyGradeProb();

						results.clear();
						results.resize(typeProb.size());

						for (size_t type = 0; type < typeProb.size(); ++type)
						{
							if (typeProb[type] != 0)
							{
								for (size_t grade = 0; grade < gradeProb.size(); ++grade)
								{
									if (gradeProb[grade] != 0)
									{
										/* preparing results */
										float value = (info->Limits.RebirthCount == 0) ? database.RetrieveIdentifyValue((uint16)info->Limits.Level, (Enchant::EnchantTableManager::EnchantTargets::Type)type, grade) : database.RetrieveIdentifyRebirthedItemValue(basisRatioValue, (Enchant::EnchantTableManager::EnchantTargets::Type)type, grade, (uint16)info->Limits.RebirthCount);

										if (info->Limits.RebirthCount == 0)
										{
											value *= info->Limits.Level; // ÀÏ¹Ý Àåºñµµ ½ÅÈ­ ÀåºñÃ³·³ ºñÀ²·Î ÀÛ¾÷ÇÒ ¼ö ÀÖ°Ô °³¼±
										}
										else
										{
											value *= basisRatioValue;
										}

										if (value > 0)
										{
											value = ((value == static_cast<int>(value)) ? value : (static_cast<int>(value) + 1));
										}
										else
										{
											value = ((value == static_cast<int>(value)) ? value : (static_cast<int>(value) - 1));
										}

										results[type].push_back(value);
									}
								}
							}
						}
					}
					else
					{
						const std::vector<float> &typeProb = database.GetAdvancedIdentifyStatusTypeProb();
						const std::vector<float> &gradeProb = database.GetAdvancedIdentifyGradeProb();
						for (size_t type = 0; type < typeProb.size(); ++type)
						{
							results.clear();
							results.resize(typeProb.size());

							for (size_t type = 0; type < typeProb.size(); ++type)
							{
								for (size_t grade = 0; grade < gradeProb.size(); ++grade)
								{
									if (gradeProb[grade] != 0)
									{
										/* preparing results */
										float value = (info->Limits.RebirthCount == 0) ? database.RetrieveIdentifyValue((uint16)info->Limits.Level, (Enchant::EnchantTableManager::EnchantTargets::Type)type, grade) : database.RetrieveIdentifyRebirthedItemValue(basisRatioValue, (Enchant::EnchantTableManager::EnchantTargets::Type)type, grade, (uint16)info->Limits.RebirthCount);

										if (info->Limits.RebirthCount == 0)
										{
											value *= info->Limits.Level; // ÀÏ¹Ý Àåºñµµ ½ÅÈ­ ÀåºñÃ³·³ ºñÀ²·Î ÀÛ¾÷ÇÒ ¼ö ÀÖ°Ô °³¼±
										}
										else
										{
											value *= basisRatioValue;
										}

										if (value > 0)
										{
											value = ((value == static_cast<int>(value)) ? value : (static_cast<int>(value) + 1));
										}
										else
										{
											value = ((value == static_cast<int>(value)) ? value : (static_cast<int>(value) - 1));
										}

										results[type].push_back(value);
									}
								}
							}
						}
					}
				}

				void MakeReinforcementParam(const Info::ItemInfo *info, EnchantBitfields enchant, Info::ItemInfo &resultInfo, float basisRatioValue)
				{
					static const int EquipPartsCount = static_cast<int>(Constants::EquipParts::EARING) + 1;
					// ±Í°ÉÀÌ°¡ ¸¶Áö¸·ÀÌ´Ù. enum¿¡ CNTÇ×¸ñÀÌ ÀÖÀ¸¸é ÁÁÀ»ÅÙµ¥, Ãß°¡Çß´Ù°¡´Â ¹«½¼ ÀÏÀÌ »ý±æÁö ¸ô¶ó ³Àµ×´Ù.

					static const int ReinforceValueCount = 4;
					// °­È­ ·¹º§¿¡ µû¶ó 1Â÷ 2Â÷ 3Â÷ 4Â÷ ´É·ÂÄ¡ ¼øÀ¸·Î Ãß°¡µÈ´Ù.
					// 4Â÷ ´É·ÂÄ¡´Â ¹«Á¶°Ç ÈûÀÌ°í, ¾Ç¼¼¿¡´Â ºÙÁö ¾Ê´Â´Ù.

					enum Ability
					{
						Dmg = 0,
						Str,
						Dex,
						Int,
						Vit,
						Def,
						Str2,
					};

					static const int ReinforcementParts[EquipPartsCount][ReinforceValueCount] = {
						{Dmg, Str, Dex, Str2},  //¹«±â
						{Def, Int, Vit, Str2},  //¸Ó¸®
						{Def, Int, Str, Str2},  //°¡½¿
						{Def, Vit, Dex, Str2},  //´Ù¸®
						{Def, Str, Int, Str2},  //¼Õ
						{Def, Dex, Vit, Str2},  //¹ß
						{Dmg, Dex, Str, Str2},  //º¸Á¶
						{Def, Dex, Str, Str2},  //¸ñ°ÉÀÌ
						{Dmg, Vit, Str, Str2},  //¹ÝÁö
						{Def, Int, Str, Str2}}; //±Í°ÉÀÌ

					float values[13] = {
						0.0f,
					};
					uint8 reinforcementLv = enchant.Reinforcement;

					if (reinforcementLv == 0)
						return;

					if (info->EquipParts > Lunia::XRated::Constants::EquipParts::EARING)
					{
						Logger::GetInstance().Warn(L"[Enchant::MakeReinforcementParam] wrong equipment type. [{}]", info->EquipParts);
						return;
					}

					float4 reinforcementValue = info->Limits.RebirthCount == 0 ? database.RetrieveReinforceValue((uint16)info->Limits.Level, reinforcementLv - 1, info->EquipParts)
																			   : database.RetrieveReinforceRebirthedItemValue(basisRatioValue, reinforcementLv - 1, info->EquipParts, info->Limits.RebirthCount);

					if (info->Limits.RebirthCount > 0)
					{
						reinforcementValue *= basisRatioValue;
						reinforcementValue.x += static_cast<float>(reinforcementLv);
						reinforcementValue.y += static_cast<float>(reinforcementLv);
						reinforcementValue.z += static_cast<float>(reinforcementLv);
						if (Constants::IsAccessory(info->EquipParts) == false)
						{
							reinforcementValue.w += static_cast<float>(reinforcementLv);
						}
					}

					if (reinforcementLv < 4)
					{
						values[ReinforcementParts[info->EquipParts][0]] = static_cast<int>(reinforcementValue.x);
					}
					else if (reinforcementLv < 7)
					{
						values[ReinforcementParts[info->EquipParts][0]] = static_cast<int>(reinforcementValue.x);
						values[ReinforcementParts[info->EquipParts][1]] = static_cast<int>(reinforcementValue.y);
					}
					else if (reinforcementLv < 12)
					{
						values[ReinforcementParts[info->EquipParts][0]] = static_cast<int>(reinforcementValue.x);
						values[ReinforcementParts[info->EquipParts][1]] = static_cast<int>(reinforcementValue.y);
						values[ReinforcementParts[info->EquipParts][2]] = static_cast<int>(reinforcementValue.z);
					}
					else if (reinforcementLv < 16)
					{
						values[ReinforcementParts[info->EquipParts][0]] = static_cast<int>(reinforcementValue.x);
						values[ReinforcementParts[info->EquipParts][1]] = static_cast<int>(reinforcementValue.y);
						values[ReinforcementParts[info->EquipParts][2]] = static_cast<int>(reinforcementValue.z);
						values[ReinforcementParts[info->EquipParts][3]] += static_cast<int>(reinforcementValue.w); //ÈûÀº Áßº¹µÉ ¼öÀÖ´Ù.
					}
					else
					{
						assert(0 && L"ReinforcementLv is must less than 16.");
					}

					// °ªµéÀÇ ÇÕÀ» ´Ù ±¸ÇßÀ¸¸é state·Î ¸¸µé¾îÁÖÀÚ.
					Info::ActionInfo::State state;
					state.startTime = 0;
					state.endTime = -1;					   // equipment must be (0, -1)
					state.type = Info::StateInfo::INCSTAT; // current enchant works for.
					state.params[L"type"] = L"AMOUNT";
					state.params[L"bFill"] = L"false";			// by default, HP/MP should not be filled by equiping.
					state.params[L"marker"] = L"reinforcement"; // enchant marker.
					int a = 0;
					for (; a < Enchant::EnchantTableManager::EnchantTargets::Resist1; ++a)
					{
						if (values[a] != 0)
						{ //°ªÀÌ µé¾îÀÖ´Ù¸é.. ¸¸µé¾îÁà¾ßÁö
							//state.params.clear();
							state.params[L"value"] = StringUtil::ToUnicode(values[a]);
							switch (static_cast<Ability>(a))
							{
							case Dmg: // damageÀÇ °æ¿ì´Â max, min µÑ´Ù ´õÇØÁØ´Ù.
								state.params[L"category"] = L"ATKMAX";
								resultInfo.States.push_back(state);
								state.params[L"category"] = L"ATKMIN";
								break;
							case Str:
								state.params[L"category"] = L"STR";
								break;
							case Dex:
								state.params[L"category"] = L"DEX";
								break;
							case Int:
								state.params[L"category"] = L"INT";
								break;
							case Vit:
								state.params[L"category"] = L"VIT";
								break;
							case Def:
								state.params[L"category"] = L"DEF";
								break;
							case Str2:
								state.params[L"category"] = L"STR";
								break;
							}
							resultInfo.States.push_back(state);
						}
					}
					state.params.clear();
				}

				/* std::wstring GetTimeStringFromInstance(InstanceEx instance, const Lunia::DateTime &currentTime)
				{
					Lunia::DateTime expiration(instance.ExpireDate);
	
					if (expiredDate.ExpiredYear == 0)
					{
						return L"$UI.Global.ToolTipLabel.Item_UnlimitedTerm_Label$";
					}

					//À¯È¿ÇÑ ³¯Â¥ÀÎÁö Ã¼Å©, ¿¹¸¦ µé¸é 0,0,0,0,0,0,0 ÀÌ¸é À¯È¿ÇÑ ³¯Â¥°¡ ¾Æ´Ï´Ù..
					if (expiration.IsValid() == false)
						return L"";

					if(expiration_new.IsValid() == false) // 3.1 by ultimate
						return L""; // 3.1 by ultimate

					TimeSpan expirationTimeSpan;
					TimeSpan expirationTimeSpan_new; // 3.1 by ultimate
					expirationTimeSpan.SetTimeSpan(expiration);
					expirationTimeSpan_new.SetTimeSpan(expiration_new); // 3.1 by ultimate

					// ¼­¹ö ½Ã°£À» timespan ¿¡´Ù°¡ ¼ÂÆÃÇÔ
					TimeSpan serverTimeSpan;

					TimeSpan serverTimeSpan_new;

					// ¾îÂ¶ °Å³ª ½Ã°£³¢¸® ´õÇÏ°Å³ª »¬¶§´Â ´Ù TIMESPAN À» ÀÌ¿ëÇÏµµ·Ï ÇÏÀÚ. ÃÊ¸¦ ÀÔ·ÂÇÑ´Ù.
					serverTimeSpan.SetTimeSpan(currentTime);
					serverTimeSpan_new.SetTimeSpan(currentTime); // 3.1 by ultimate
					serverTimeSpan = expirationTimeSpan - serverTimeSpan;
					serverTimeSpan_new = expirationTimeSpan_new - serverTimeSpan_new; // 3.1 by ultimate

					if (serverTimeSpan.GetTotalDays() > 0)
					{
						return Lunia::StringUtil::Format(L"%s%s", Lunia::StringUtil::ToUnicode(serverTimeSpan.GetTotalDays()).c_str(), L"$UI.Global.ToolTipLabel.Day_Label$");
					}
					else if (serverTimeSpan.GetHours() > 0)
					{
						return Lunia::StringUtil::Format(L"%s%s", Lunia::StringUtil::ToUnicode(serverTimeSpan.GetHours()).c_str(), L"$UI.Global.ToolTipLabel.Time_Label$");
					}
					else if (serverTimeSpan.GetMinutes() > 0)
					{
						return Lunia::StringUtil::Format(L"%s%s", Lunia::StringUtil::ToUnicode(serverTimeSpan.GetMinutes()).c_str(), L"$UI.Global.ToolTipLabel.Minute_Label$");
					}
	
					if (serverTimeSpan_new.GetTotalDays() > 0)
					{
						return Lunia::StringUtil::Format(L"%s%s", Lunia::StringUtil::ToUnicode(serverTimeSpan_new.GetTotalDays()).c_str(), L"$UI.Global.ToolTipLabel.Day_Label$");
					}
					else if (serverTimeSpan_new.GetHours() > 0)
					{
						return Lunia::StringUtil::Format(L"%s%s", Lunia::StringUtil::ToUnicode(serverTimeSpan_new.GetHours()).c_str(), L"$UI.Global.ToolTipLabel.Time_Label$");
					}
					else if (serverTimeSpan_new.GetMinutes() > 0)
					{
						return Lunia::StringUtil::Format(L"%s%s", Lunia::StringUtil::ToUnicode(serverTimeSpan_new.GetMinutes()).c_str(), L"$UI.Global.ToolTipLabel.Minute_Label$");
					}

					return Lunia::StringUtil::Format(L"1%s%s", L"$UI.Global.ToolTipLabel.Minute_Label$", L"$UI.Global.ToolTipLabel.Down$");
				}
				 */
				InstanceEx MakeInstance(const Database::Info::ItemInfo *itemInfo, uint16 cashEnchant1Index, uint16 cashEnchant2Index, uint32 expireMin, bool addExpireMin, InstanceEx pInstance, uint16 cashEnchant3Index, uint16 cashEnchant4Index)
				{
					/* calculate instance fields */
					InstanceEx instanceEx(0);

					if (IsEnchantable(itemInfo))
					{
						instanceEx = pInstance;
					}
					else if (itemInfo->MaxCnt == 1) // unenchantable
					{
						NormalBitfields field(pInstance);
						if (expireMin)
						{
							Lunia::DateTime expireDate(Lunia::DateTime::Now());

							if (addExpireMin)
							{
								if (instanceEx.ExpireDate.GetDate().GetYear() > 0)
								{
									if (instanceEx.ExpireDate > expireDate)
									{
										expireDate = instanceEx.ExpireDate;
									}
								}
							}
							int minSum = expireDate.Get(Lunia::DateTime::Unit::Minute) + expireMin;
							if (minSum / Lunia::DateTime::Constant::MinutesInAnHour != 0)
								expireDate.Add(Lunia::DateTime::Unit::Hour, minSum / Lunia::DateTime::Constant::MinutesInAnHour);
							expireDate.Set(Lunia::DateTime::Unit::Minute, minSum % Lunia::DateTime::Constant::MinutesInAnHour);
							instanceEx.ExpireDate = expireDate;
						}
						else if (cashEnchant1Index == 0 && cashEnchant2Index == 0)
						{
							instanceEx.ExpireDate = instanceEx.NoExpiration;
						}

						if (cashEnchant1Index != 0)
						{
							field.CashEnchant1 = cashEnchant1Index;
						}
						if (cashEnchant2Index != 0)
						{
							field.CashEnchant2 = cashEnchant2Index;
						}
						if (cashEnchant1Index != 3)
						{
							field.CashEnchant3 = cashEnchant3Index;
						}
						if (cashEnchant2Index != 4)
						{
							field.CashEnchant4 = cashEnchant4Index;
						}

						instanceEx = field;
					}
					return instanceEx;
				}

				Info::ItemInfo GetItemInstance(const Info::ItemInfo *info, EnchantBitfields enchant)
				{
				#ifdef _DEBUG
					if (IsEnchantable(info) == false)
						return *info;
				#endif

					Info::ItemInfo ret(*info);

					int optionCnt = static_cast<int>(enchant.LightLevel) / 2 + 1;
					/* preparing item state to increase status */
					Info::ActionInfo::State state;
					state.startTime = 0;
					state.endTime = -1; // equipment must be (0, -1)

					float basisRatioValue = 0.0f;

					//if(info->Limits.RebirthCount > 0)
					{
						std::vector<Database::Info::ActionInfo::State>::const_iterator itr;
						for (itr = info->States.begin(); itr != info->States.end(); ++itr)
						{
							if (itr->type == Database::Info::StateInfo::INCSTAT)
							{
								std::map<std::wstring, std::wstring>::const_iterator paramItr = itr->params.find(L"category");
								if (paramItr != itr->params.end() &&
									(paramItr->second == L"STR" || paramItr->second == L"DEX" || paramItr->second == L"INT" || paramItr->second == L"VIT"))
								{
									paramItr = itr->params.find(L"value");

									if (paramItr == itr->params.end())
										continue;

									basisRatioValue += Lunia::StringUtil::ToFloat(paramItr->second);
								}
							}
						}
					}

					for (int i = 0; i < optionCnt; ++i) // max 3 option values
					{
						if (enchant.Identified == 0)
							break;
						//if ((i==0 && enchant.Identified1==0) ||	(i==1 && enchant.Identified2==0) || (i==2 && enchant.Identified3==0))
						//	continue; // not identified

						state.params.clear();
						std::pair<uint8, uint8> option = enchant.GetIdentifyFields(i);

						/* preparing values */
						float value = (info->Limits.RebirthCount == 0) ? database.RetrieveIdentifyValue((uint16)info->Limits.Level, (Enchant::EnchantTableManager::EnchantTargets::Type)option.first, option.second) : database.RetrieveIdentifyRebirthedItemValue(basisRatioValue, (Enchant::EnchantTableManager::EnchantTargets::Type)option.first, option.second, (uint16)info->Limits.RebirthCount);

						if (info->Limits.RebirthCount == 0)
						{
							value *= info->Limits.Level; // ÀÏ¹Ý Àåºñµµ ½ÅÈ­ ÀåºñÃ³·³ ºñÀ²·Î ÀÛ¾÷ÇÒ ¼ö ÀÖ°Ô °³¼±
						}
						else
						{
							value *= basisRatioValue;
						}

						if (value > 0)
						{
							value = ((value == static_cast<int>(value)) ? value : (static_cast<int>(value) + 1));
						}
						else
						{
							value = ((value == static_cast<int>(value)) ? value : (static_cast<int>(value) - 1));
						}

						/* make item state for the enchant option */
						//MaxDmg, MinDmg, Str, Dex, Int, Vit, Def, MaxHp, MaxMp,
						//Resist1, Resist2, Resist3, Resist4, Resist5,
						switch (option.first)
						{
						case Enchant::EnchantTableManager::EnchantTargets::Dmg:
							state.type = Info::StateInfo::INCSTAT; // current enchant works for.
							state.params[L"category"] = L"ATKMAX";
							state.params[L"type"] = L"AMOUNT";
							state.params[L"value"] = StringUtil::ToUnicode(value);
							// ´ëºÎºÐ ¾Æ·¡¿¡¼­ marker¸¦ ³ÖÁö¸¸ ÀÌ°ÍÀº max ÀÇ °æ¿ì ¹Ù·Î pushÇØ¼­ ret¿¡ ³Ö±â ‹š¹®¿¡ ¿©±â¼­ ¸¶Ä¿¸¦ ¼ÂÆÃÇÑ´Ù. by kwind
							state.params[L"marker"] = L"identify";
							//state.params[L"marker"]=StringUtil::Format(L"identify%d", i+1); // enchant marker.
							state.params[L"bFill"] = L"false"; // by default, HP/MP should not be filled by equiping.
							ret.States.push_back(state);
							state.params.clear();

							state.type = Info::StateInfo::INCSTAT; // current enchant works for.
							state.params[L"category"] = L"ATKMIN";
							state.params[L"type"] = L"AMOUNT";
							state.params[L"value"] = StringUtil::ToUnicode(value);
							break;
						case Enchant::EnchantTableManager::EnchantTargets::Str:
							state.type = Info::StateInfo::INCSTAT; // current enchant works for.
							state.params[L"category"] = L"STR";
							state.params[L"type"] = L"AMOUNT";
							state.params[L"value"] = StringUtil::ToUnicode(value);
							break;
						case Enchant::EnchantTableManager::EnchantTargets::Dex:
							state.type = Info::StateInfo::INCSTAT; // current enchant works for.
							state.params[L"category"] = L"DEX";
							state.params[L"type"] = L"AMOUNT";
							state.params[L"value"] = StringUtil::ToUnicode(value);
							break;
						case Enchant::EnchantTableManager::EnchantTargets::Int:
							state.type = Info::StateInfo::INCSTAT; // current enchant works for.
							state.params[L"category"] = L"INT";
							state.params[L"type"] = L"AMOUNT";
							state.params[L"value"] = StringUtil::ToUnicode(value);
							break;
						case Enchant::EnchantTableManager::EnchantTargets::Vit:
							state.type = Info::StateInfo::INCSTAT; // current enchant works for.
							state.params[L"category"] = L"VIT";
							state.params[L"type"] = L"AMOUNT";
							state.params[L"value"] = StringUtil::ToUnicode(value);
							break;
						case Enchant::EnchantTableManager::EnchantTargets::Def:
							state.type = Info::StateInfo::INCSTAT; // current enchant works for.
							state.params[L"category"] = L"DEF";
							state.params[L"type"] = L"AMOUNT";
							state.params[L"value"] = StringUtil::ToUnicode(value);
							break;
						case Enchant::EnchantTableManager::EnchantTargets::MaxHp:
							state.type = Info::StateInfo::INCSTAT; // current enchant works for.
							state.params[L"category"] = L"MAXHP";
							state.params[L"type"] = L"AMOUNT";
							state.params[L"value"] = StringUtil::ToUnicode(value);
							break;
						case Enchant::EnchantTableManager::EnchantTargets::MaxMp:
							state.type = Info::StateInfo::INCSTAT; // current enchant works for.
							state.params[L"category"] = L"MAXMP";
							state.params[L"type"] = L"AMOUNT";
							state.params[L"value"] = StringUtil::ToUnicode(value);
							break;
						case Enchant::EnchantTableManager::EnchantTargets::Resist1:
							state.type = Info::StateInfo::RESIST; // current enchant works for.
							state.params[L"type"] = L"0";
							state.params[L"amount"] = StringUtil::ToUnicode(value);
							break;
						case Enchant::EnchantTableManager::EnchantTargets::Resist2:
							state.type = Info::StateInfo::RESIST; // current enchant works for.
							state.params[L"type"] = L"2";
							state.params[L"amount"] = StringUtil::ToUnicode(value);
							break;
						case Enchant::EnchantTableManager::EnchantTargets::Resist3:
							state.type = Info::StateInfo::RESIST; // current enchant works for.
							state.params[L"type"] = L"4";
							state.params[L"amount"] = StringUtil::ToUnicode(value);
							break;
						case Enchant::EnchantTableManager::EnchantTargets::Resist4:
							state.type = Info::StateInfo::RESIST; // current enchant works for.
							state.params[L"type"] = L"6";
							state.params[L"amount"] = StringUtil::ToUnicode(value);
							break;
						case Enchant::EnchantTableManager::EnchantTargets::Resist5:
							state.type = Info::StateInfo::RESIST; // current enchant works for.
							state.params[L"type"] = L"3";		  //LIGHTNING
							state.params[L"amount"] = StringUtil::ToUnicode(value);
							break;
						}

						if (state.params.empty() == false)
						{
							state.params[L"marker"] = L"identify"; //StringUtil::Format(L"identify%d", i+1); // enchant marker.
							state.params[L"bFill"] = L"false";	 // by default, HP/MP should not be filled by equiping.
							ret.States.push_back(state);
						}
					}

					MakeReinforcementParam(info, enchant, ret, basisRatioValue);
					MakeLightReinforcementParam(info, enchant, ret, basisRatioValue);

					return ret;
				}

				bool GetItemInstanceForServer(const Info::ItemInfo *info, EnchantBitfields enchant, std::vector<Info::ActionInfo::State> &states)
				{
					static const int EquipPartsCount = static_cast<int>(Constants::EquipParts::EARING) + 1;
					// ±Í°ÉÀÌ°¡ ¸¶Áö¸·ÀÌ´Ù. enum¿¡ CNTÇ×¸ñÀÌ ÀÖÀ¸¸é ÁÁÀ»ÅÙµ¥, Ãß°¡Çß´Ù°¡´Â ¹«½¼ ÀÏÀÌ »ý±æÁö ¸ô¶ó ³Àµ×´Ù.

					static const int ReinforceValueCount = 4;
					// °­È­ ·¹º§¿¡ µû¶ó 1Â÷ 2Â÷ 3Â÷ 4Â÷ ´É·ÂÄ¡ ¼øÀ¸·Î Ãß°¡µÈ´Ù.
					// 4Â÷ ´É·ÂÄ¡´Â ¹«Á¶°Ç ÈûÀÌ°í, ¾Ç¼¼¿¡´Â ºÙÁö ¾Ê´Â´Ù.

					enum Ability
					{
						Dmg = 0,
						Str,
						Dex,
						Int,
						Vit,
						Def,
					};

					static const int ReinforcementParts[EquipPartsCount][ReinforceValueCount] = {
						{Dmg, Str, Dex, Str},
						{Def, Int, Vit, Str},
						{Def, Int, Str, Str},
						{Def, Vit, Dex, Str}, 
						{Def, Str, Int, Str},
						{Def, Dex, Vit, Str},  
						{Dmg, Dex, Str, Str},  
						{Def, Dex, Str, Str},  
						{Dmg, Vit, Str, Str},  
						{Def, Int, Str, Str}}; 

				#ifdef _DEBUG
					if (IsEnchantable(info) == false)
						return false;
				#endif
					//Info::ItemInfo ret(*info);
					float values[13] = {
						0.0f,
					}; //°¢ ¼öÄ¡µéÀÇ ÇÕÀÌ µé¾î°¥ ¹öÆÛ. ¼­¹ö¿¡¼± ¸ð¾Æ¼­ ÇÑ¹æ¿¡ ¸¸µéÀÚ.
					float basisRatioValue = 0.0f;

					//if(info->Limits.RebirthCount > 0)
					{
						std::vector<Database::Info::ActionInfo::State>::const_iterator itr;
						for (itr = info->States.begin(); itr != info->States.end(); ++itr)
						{
							if (itr->type == Database::Info::StateInfo::INCSTAT)
							{
								std::map<std::wstring, std::wstring>::const_iterator paramItr = itr->params.find(L"category");
								if (paramItr != itr->params.end() &&
									(paramItr->second == L"STR" || paramItr->second == L"DEX" || paramItr->second == L"INT" || paramItr->second == L"VIT"))
								{
									paramItr = itr->params.find(L"value");

									if (paramItr == itr->params.end())
										continue;

									basisRatioValue += Lunia::StringUtil::ToFloat(paramItr->second);
								}
							}
						}
					}

					// ¸ÕÀú Áõ°¡µÇ´Â ¼öÄ¡µéÀÇ ÇÕÀ» °è»êÇÑ´Ù.
					// °¨Á¤
					int optionCnt = static_cast<int>(enchant.LightLevel) / 2 + 1; //ºÙÀ» ¿É¼ÇÀÇ °¹¼ö.

					for (int i = 0; i < optionCnt; ++i)
					{
						if (enchant.Identified == 0)
							break;
						//if ((i==0 && enchant.Identified1==0) ||	(i==1 && enchant.Identified2==0) || (i==2 && enchant.Identified3==0))
						//	continue; // not identified

						std::pair<uint8, uint8> option = enchant.GetIdentifyFields(i);
						/* preparing values */
						float value = (info->Limits.RebirthCount == 0) ? database.RetrieveIdentifyValue((uint16)info->Limits.Level, (Enchant::EnchantTableManager::EnchantTargets::Type)option.first, option.second) : database.RetrieveIdentifyRebirthedItemValue(basisRatioValue, (Enchant::EnchantTableManager::EnchantTargets::Type)option.first, option.second, (uint16)info->Limits.RebirthCount);

						if (info->Limits.RebirthCount == 0)
						{
							value *= info->Limits.Level; // ÀÏ¹Ý Àåºñµµ ½ÅÈ­ ÀåºñÃ³·³ ºñÀ²·Î ÀÛ¾÷ÇÒ ¼ö ÀÖ°Ô °³¼±
						}
						else
						{
							value *= basisRatioValue;
						}

						if (value > 0)
						{
							value = ((value == static_cast<int>(value)) ? value : (static_cast<int>(value) + 1));
						}
						else
						{
							value = ((value == static_cast<int>(value)) ? value : (static_cast<int>(value) - 1));
						}

						values[option.first] += value;
					}

					// °­È­
					uint8 reinforcementLv = enchant.Reinforcement;
					if (reinforcementLv)
					{
						if (info->EquipParts > Lunia::XRated::Constants::EquipParts::EARING)
						{
							Logger::GetInstance().Warn(L"[Enchant::GetItemInstanceForServer] wrong equipment type. [{0}]", info->EquipParts);
							//return false;
						}
						else
						{
							float4 reinforcementValue = info->Limits.RebirthCount == 0 ? database.RetrieveReinforceValue((uint16)info->Limits.Level, reinforcementLv - 1, info->EquipParts)
																					   : database.RetrieveReinforceRebirthedItemValue(basisRatioValue, reinforcementLv - 1, info->EquipParts, info->Limits.RebirthCount);

							if (info->Limits.RebirthCount > 0)
							{
								reinforcementValue *= basisRatioValue;
								reinforcementValue.x += static_cast<float>(reinforcementLv);
								reinforcementValue.y += static_cast<float>(reinforcementLv);
								reinforcementValue.z += static_cast<float>(reinforcementLv);
								if (Constants::IsAccessory(info->EquipParts) == false)
								{
									reinforcementValue.w += static_cast<float>(reinforcementLv);
								}
							}

							switch (reinforcementLv)
							{
							case 1:
							case 2:
							case 3:
								values[ReinforcementParts[info->EquipParts][0]] += static_cast<int>(reinforcementValue.x);
								break;
							case 4:
							case 5:
							case 6:
								values[ReinforcementParts[info->EquipParts][0]] += static_cast<int>(reinforcementValue.x);
								values[ReinforcementParts[info->EquipParts][1]] += static_cast<int>(reinforcementValue.y);
								break;
							case 7:
							case 8:
							case 9:
							case 10:
							case 11:
								values[ReinforcementParts[info->EquipParts][0]] += static_cast<int>(reinforcementValue.x);
								values[ReinforcementParts[info->EquipParts][1]] += static_cast<int>(reinforcementValue.y);
								values[ReinforcementParts[info->EquipParts][2]] += static_cast<int>(reinforcementValue.z);
								break;
							case 12:
							case 13:
							case 14:
							case 15:
								values[ReinforcementParts[info->EquipParts][0]] += static_cast<int>(reinforcementValue.x);
								values[ReinforcementParts[info->EquipParts][1]] += static_cast<int>(reinforcementValue.y);
								values[ReinforcementParts[info->EquipParts][2]] += static_cast<int>(reinforcementValue.z);
								values[ReinforcementParts[info->EquipParts][3]] += static_cast<int>(reinforcementValue.w);
								break;
							}
						}
					}

					// ºû °­È­
					reinforcementLv = enchant.LightLevel;
					if (reinforcementLv)
					{ //ºû´Ü°è°¡ Á¸Àç ÇÑ´Ù¸é
						for (int stat = 0; stat < 13; ++stat)
						{
							values[stat] += info->Limits.RebirthCount == 0
												? database.RetrieveLightReinforceValue((uint16)info->Limits.Level, reinforcementLv - 1, info->EquipParts, stat)
												: database.RetrieveLightReinforceValueForRebirth((uint16)info->Limits.StoredLevel, reinforcementLv - 1, info->EquipParts, stat);
						}
					}

					// °ªµéÀÇ ÇÕÀ» ´Ù ±¸ÇßÀ¸¸é state·Î ¸¸µé¾îÁÖÀÚ.
					Info::ActionInfo::State state;
					state.startTime = 0;
					state.endTime = -1;					   // equipment must be (0, -1)
					state.type = Info::StateInfo::INCSTAT; // current enchant works for.
					state.params[L"type"] = L"AMOUNT";
					state.params[L"bFill"] = L"false"; // by default, HP/MP should not be filled by equiping.
					int a = 0;
					for (; a < Enchant::EnchantTableManager::EnchantTargets::Resist1; ++a)
					{
						if (values[a] != 0)
						{ //°ªÀÌ µé¾îÀÖ´Ù¸é.. ¸¸µé¾îÁà¾ßÁö
							//state.params.clear();
							state.params[L"value"] = StringUtil::ToUnicode(values[a]);
							switch (static_cast<Enchant::EnchantTableManager::EnchantTargets::Type>(a))
							{
							case Enchant::EnchantTableManager::EnchantTargets::Dmg: // damageÀÇ °æ¿ì´Â max, min µÑ´Ù ´õÇØÁØ´Ù.
								state.params[L"category"] = L"ATKMAX";
								states.push_back(state);
								state.params[L"category"] = L"ATKMIN";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Str:
								state.params[L"category"] = L"STR";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Dex:
								state.params[L"category"] = L"DEX";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Int:
								state.params[L"category"] = L"INT";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Vit:
								state.params[L"category"] = L"VIT";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Def:
								state.params[L"category"] = L"DEF";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::MaxHp:
								state.params[L"category"] = L"MAXHP";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::MaxMp:
								state.params[L"category"] = L"MAXMP";
								break;
							}
							states.push_back(state);
						}
					}
					state.params.clear();

					state.type = Info::StateInfo::RESIST; // current enchant works for.
					for (; a < Enchant::EnchantTableManager::EnchantTargets::None; ++a)
					{
						if (values[a] != 0)
						{ 
							state.params[L"amount"] = StringUtil::ToUnicode(values[a]);
							switch ((Enchant::EnchantTableManager::EnchantTargets::Type)a)
							{
							case Enchant::EnchantTableManager::EnchantTargets::Resist1:
								state.params[L"type"] = L"0";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Resist2:
								state.params[L"type"] = L"2";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Resist3:
								state.params[L"type"] = L"4";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Resist4:
								state.params[L"type"] = L"6";
								break;
							case Enchant::EnchantTableManager::EnchantTargets::Resist5:
								state.params[L"type"] = L"3";
								break;
							}
							states.push_back(state);
						}
					}

					return true;
				}

				void RequiredItem::Serialize(Serializer::IStreamWriter &out) const
				{
					out.Begin(L"Lunia::XRated::Database::Enchant::RequiredItem");
					out.Write(L"ItemHash", ItemHash);
					out.Write(L"RequireCount", RequireCount);
				}

				void RequiredItem::Deserialize(Serializer::IStreamReader &in)
				{
					in.Begin(L"Lunia::XRated::Database::Enchant::RequiredItem");
					in.Read(L"ItemHash", ItemHash);
					in.Read(L"RequireCount", RequireCount);
				}

				uint32 GetIdentifyRequirement(const Info::ItemInfo *info, const EnchantBitfields &enchant)
				{
					uint16 level = std::max<int>(info->Limits.Level, info->Limits.StoredLevel);
					return level * (static_cast<uint32>(enchant.LightLevel) / 2 + 1) * 100;

					//old
					//uint16 level = std::max<int>( info->Limits.Level, info->Limits.StoredLevel );
					//return level * (static_cast<uint32>(enchant.LightLevel) + 1) * database.GetRequirementFactorPerGrade((uint8)info->EquipLv);
				}

				Condition GetReinforcementRequirement(const Info::ItemInfo *itemInfo, const EnchantBitfields &enchant)
				{
					Condition condition;
					uint16 level = std::max<int>(itemInfo->Limits.Level, itemInfo->Limits.StoredLevel);

					condition.Fee = level * database.GetRequirementFactorPerGrade((uint8)itemInfo->EquipLv) * database.GetRequirementFactorPerReinforcement(enchant.Reinforcement);
					if (itemInfo->EquipParts > Constants::EquipParts::FOOT)
					{
						condition.reqiredItems.push_back(RequiredItem(database.GetConstants().HashMineralForAccessory, database.GetMineralCntPerLevel(level)));
					}
					else
					{
						condition.reqiredItems.push_back(RequiredItem(database.GetConstants().HashMineral, database.GetMineralCntPerLevel(level)));
					}
					return condition;
				}

				Condition GetLightReinforcementRequirement(const Info::ItemInfo *info, const EnchantBitfields &enchant)
				{
					Condition condition;
					uint16 level = std::max<int>(info->Limits.Level, info->Limits.StoredLevel);
					condition.Fee = level * database.GetRequirementFactorPerGrade((uint8)info->EquipLv) * database.GetRequirementFactorPerLightReinforcement(enchant.LightLevel);
					if (info->EquipParts > Constants::EquipParts::FOOT)
					{
						condition.reqiredItems.push_back(RequiredItem(database.GetConstants().HashJewelForAccessory[enchant.LightLevel], database.GetJewelCntPerLevel(level)));
					}
					else
					{
						condition.reqiredItems.push_back(RequiredItem(database.GetConstants().HashJewel[enchant.LightLevel], database.GetJewelCntPerLevel(level)));
					}
					return condition;
				}

				Condition GetMagicalAttachRequirement(const Info::ItemInfo *info, const EnchantBitfields &enchant)
				{
					Condition condition;
					std::pair<uint16, uint16> magicalAttach = enchant.GetMagicalAttach();
					if (magicalAttach.first)
						condition.Fee = 20 * info->SellPrice;
					else
						condition.Fee = 10 * info->SellPrice;
					//database.Get
					return condition;
				}

				Condition GetAlchemyRequirement(const Info::ItemInfo *info, const EnchantBitfields & /*enchant*/)
				{
					Condition condition;
					const Enchant::AlchemyTable *data = database.GetAlchemyTable(info);
					if (data != NULL)
					{
						condition.Fee = data->Fee;
						//std::vector<Enchant::RequiredItem>::const_iterator iter = data->Requirements.begin();
						//for (; iter != data->Requirements.end(); ++iter)
						//{
						//	condition.reqiredItems.push_back(*iter);
						//}
					}
					return condition;
				}

				Condition GetRestoreStrengthenRequirement(const Info::ItemInfo *info, const EnchantBitfields & /*enchant*/)
				{
					Condition condition;

					std::vector<Database::Info::ActionInfo::State>::const_iterator iter = info->States.begin();
					std::vector<Database::Info::ActionInfo::State>::const_iterator end = info->States.end();
					while (iter != end)
					{
						std::map<std::wstring, std::wstring>::const_iterator finditer = (*iter).params.find(L"Fee");
						if (finditer != (*iter).params.end())
						{
							condition.Fee = Lunia::StringUtil::To<uint32>((*finditer).second);
							condition.reqiredItems.clear();
							break;
						}
						++iter;
					}
					return condition;
				}
				Condition GetRestoreAppraisalRequirement(const Info::ItemInfo *info, const EnchantBitfields & /*enchant*/)
				{
					Condition condition;

					std::vector<Database::Info::ActionInfo::State>::const_iterator iter = info->States.begin();
					std::vector<Database::Info::ActionInfo::State>::const_iterator end = info->States.end();
					while (iter != end)
					{
						std::map<std::wstring, std::wstring>::const_iterator finditer = (*iter).params.find(L"Fee");
						if (finditer != (*iter).params.end())
						{
							condition.Fee = Lunia::StringUtil::To<uint32>((*finditer).second);
							condition.reqiredItems.clear();
						}
						++iter;
					}
					return condition;
				}

				Condition GetRestoreBelongingRequirement(const Info::ItemInfo *info, const EnchantBitfields &enchant)
				{
					Condition condition;
					condition.Fee = info->Limits.Level * 5.7;

					return condition;
				}

				uint32 GetExtractionRequirement(const Info::ItemInfo *info, const EnchantBitfields &enchant)
				{
					return info->Limits.Level * (static_cast<uint32>(enchant.LightLevel) + 1) * database.GetRequirementFactorPerGrade((uint8)info->EquipLv);
				}

				bool IsHaveTodayDailyItem(const Lunia::DateTime::Date &connectDate, InstanceEx instance)
				{
					if (instance.IsExpired(connectDate) == false)
					{
						NormalBitfields field(instance);
						DailyItemLastAcquiredDate date;
						date.Year = field.CashEnchant1;
						date.Month = (field.CashEnchant2 & 480) >> 5;
						date.Day = field.CashEnchant2 & 31;

						if (((static_cast<uint16>(date.Year) == connectDate.GetYear()) && (static_cast<uint16>(date.Month) == connectDate.GetMonth()) && (static_cast<uint16>(date.Day) < connectDate.GetDay())) || ((static_cast<uint16>(date.Year) == connectDate.GetYear()) && (static_cast<uint16>(date.Month) < connectDate.GetMonth())) || (static_cast<uint16>(date.Year) < connectDate.GetYear()))
						{
							return true;
						}
					}
					return false;
				}
				bool IsExpired(const Database::Info::ItemInfo *info, InstanceEx instance)
				{
					if (IsEnchantable(info))
						return false; /* no expiration for enchantable items */
					return instance.IsExpired();
				}

			} 
		}
	}
}
