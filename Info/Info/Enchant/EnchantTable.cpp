#pragma once
#include "EnchantTable.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				namespace Enchant {
					void AlchemyTable::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(L"AlchemyTable");
						out.Write(L"Fee", Fee);
						out.Write(L"SuccessRate", SuccessRate);
						out.Write(L"Requirements", Requirements);
						out.Write(L"Results", Results);
						out.Write(L"Portions", Portions);
					}

					void AlchemyTable::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(L"AlchemyTable");
						in.Read(L"Fee", Fee);
						in.Read(L"SuccessRate", SuccessRate);
						in.Read(L"Requirements", Requirements);
						in.Read(L"Results", Results);
						in.Read(L"Portions", Portions);
					}

					void EnchantTableManager::ReinforcementTable::Clear()
					{
						Table.clear();
					}

					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// EnchantTableManager::ReinforcementTable

					void EnchantTableManager::ProbabilityInfo::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(L"ProbabilityInfo");  
						out.Write(L"Hash", Hash);
						out.Write(L"DestroyTable", DestroyTable);
						out.Write(L"SuccessTable", SuccessTable);
						out.Write(L"GradeDownTable", GradeDownTable);
					}
					void EnchantTableManager::ProbabilityInfo::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(L"ProbabilityInfo");
						in.Read(L"Hash", Hash);
						in.Read(L"DestroyTable", DestroyTable);
						in.Read(L"SuccessTable", SuccessTable);
						in.Read(L"GradeDownTable", GradeDownTable);
					}
	
					void EnchantTableManager::ReinforcementTable::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::ReinforcementTable");
						out.Write(L"Probability", Probabilities);
						out.Write(L"Table", Table);
						out.Write(L"RebirthTable", RebirthTable);
						out.Write(L"RecoverFee", RecoverFee);
						out.Write(L"RecoverTable", RecoverTable);
					}

					void EnchantTableManager::ReinforcementTable::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::ReinforcementTable");
						in.Read(L"Probability", Probabilities);
						in.Read(L"Table", Table);
						in.Read(L"RebirthTable", RebirthTable);
						in.Read(L"RecoverFee", RecoverFee);
						in.Read(L"RecoverTable", RecoverTable);
					}


					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// EnchantTableManager::IdentifyTable
					void EnchantTableManager::IdentifyTable::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::IdentifyTable");
						out.Write(L"StatusTypeProbability", StatusTypeProbability);
						out.Write(L"GradeProbability", GradeProbability);
						out.Write(L"Table", Table);
						out.Write(L"RebirthTable", RebirthTable);
						out.Write(L"ResetFee", ResetFee);
						out.Write(L"ResetTable", ResetTable);
					}

					void EnchantTableManager::IdentifyTable::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::IdentifyTable");
						in.Read(L"StatusTypeProbability", StatusTypeProbability);
						in.Read(L"GradeProbability", GradeProbability);
						in.Read(L"Table", Table);
						in.Read(L"RebirthTable", RebirthTable);
						in.Read(L"ResetFee", ResetFee, 1000);
						in.Read(L"ResetTable", ResetTable, std::map< uint16, std::map< uint32, uint8 > >());
					}


					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// EnchantTableManager::LightReinforcementTable
					void EnchantTableManager::LightReinforcementTable::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::LightReinforcementTable");
						out.Write(L"Probability", Probabilities);
						out.Write(L"Table", Table);
						out.Write(L"RebirthTable", RebirthTable);
						out.Write(L"RecoverFee", RecoverFee);
						out.Write(L"RecoverTable", RecoverTable);
					}

					void EnchantTableManager::LightReinforcementTable::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::LightReinforcementTable");
						in.Read(L"Probability", Probabilities);
						in.Read(L"Table", Table);
						in.Read(L"RebirthTable", RebirthTable);
						in.Read(L"RecoverFee", RecoverFee);
						in.Read(L"RecoverTable", RecoverTable);
					}


					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// EnchantTableManager::MagicalAttachInfo
					void EnchantTableManager::MagicalAttachInfo::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::MagicalAttachInfo");
						out.Write(L"Hash", Hash);
						out.Write(L"Requirements", Requirements);
						out.Write(L"StateBundles", StateBundles);
					}

					void EnchantTableManager::MagicalAttachInfo::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::MagicalAttachInfo");
						in.Read(L"Hash", Hash);
						in.Read(L"Requirements", Requirements);
						in.Read(L"StateBundles", StateBundles);
					}


					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// EnchantTableManager::MagicalAttachTable
					void EnchantTableManager::MagicalAttachTable::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::MagicalAttachTable");
						out.Write(L"ScrollList", ScrollList);
					}

					void EnchantTableManager::MagicalAttachTable::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::MagicalAttachTable");
						in.Read(L"ScrollList", ScrollList);
					}


					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// EnchantTableManager::ExtractionTable
					void EnchantTableManager::ExtractionTable::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::ExtractionTable");
						out.Write(L"Probability", Probability);
						out.Write(L"CountPerLevel", CountPerLevel);
						out.Write(L"CountPerReinforcement", CountPerReinforcement);
					}

					void EnchantTableManager::ExtractionTable::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::ExtractionTable");
						in.Read(L"Probability", Probability);
						in.Read(L"CountPerLevel", CountPerLevel);
						in.Read(L"CountPerReinforcement", CountPerReinforcement);
					}


					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// EnchantTableManager::CatalystTable
					void EnchantTableManager::CatalystTable::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::CatalystTable");
						out.Write(L"ItemHash", ItemHash);
						out.Write(L"ProbabilityTableHash", ProbabilityTableHash);
						out.Write(L"ProbabilityPerCnt", ProbabilityPerCnt);
						out.Write(L"ProbabilityPerLevel", ProbabilityPerLevel);
						out.Write(L"ReinforceMinCounts", ReinforceMinCounts);
						out.Write(L"LightReinforceMinCounts", LightReinforceMinCounts);
					}

					void EnchantTableManager::CatalystTable::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::CatalystTable");
						in.Read(L"ItemHash", ItemHash);
						in.Read(L"ProbabilityTableHash", ProbabilityTableHash);
						in.Read(L"ProbabilityPerCnt", ProbabilityPerCnt);
						in.Read(L"ProbabilityPerLevel", ProbabilityPerLevel);
						in.Read(L"ReinforceMinCounts", ReinforceMinCounts, std::vector<uint8>());
						in.Read(L"LightReinforceMinCounts", LightReinforceMinCounts, std::vector<uint8>());
					}

	
					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// RequirementTable::RequirementTable
					void EnchantTableManager::RequirementTable::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::RequirementTable");
						out.Write(L"GradeFactor", GradeFactor);
						out.Write(L"ReinforcementFactor", ReinforcementFactor);
						out.Write(L"LightReinforcementFactor", LightReinforcementFactor);
						out.Write(L"MineralCntPerLevel", MineralCntPerLevel);
						out.Write(L"JewelCntPerLevel", JewelCntPerLevel);
					}

					void EnchantTableManager::RequirementTable::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::RequirementTable");
						in.Read(L"GradeFactor", GradeFactor);
						in.Read(L"ReinforcementFactor", ReinforcementFactor);
						in.Read(L"LightReinforcementFactor", LightReinforcementFactor);
						in.Read(L"MineralCntPerLevel", MineralCntPerLevel);
						in.Read(L"JewelCntPerLevel", JewelCntPerLevel);
					}


					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// RequirementTable::RequirementTable
					void CashEnchantInfo::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(L"Lunia::XRated::Database::Enchant::CashEnchantInfo");
						out.Write(L"Level", Level);
						out.Write(L"StoredLevel", StoredLevel);
						out.Write(L"RebirthCount", RebirthCount);
						out.Write(L"States", States);
						out.Write(L"StateBundleConditions", StateBundleConditions);
					}

					void CashEnchantInfo::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(L"Lunia::XRated::Database::Enchant::CashEnchantInfo");
						in.Read(L"Level", Level, uint16(1));
						in.Read(L"StoredLevel", StoredLevel, uint16(0));
						in.Read(L"RebirthCount", RebirthCount, uint16(0));
						in.Read(L"States", States, Info::ActionInfo::StateList());
						in.Read(L"StateBundleConditions", StateBundleConditions, Info::StateBundleInfo::ConditionList());
					}
					/* void CashEnchant3Info::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(L"Lunia::XRated::Database::Enchant::CashEnchant3Info");
						out.Write(L"Level", Level);
						out.Write(L"RebirthCount", RebirthCount); 
						out.Write(L"States", States);
						out.Write(L"StateBundleConditions", StateBundleConditions);
					}

					void CashEnchant3Info::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(L"Lunia::XRated::Database::Enchant::CashEnchant3Info");
						//std::cout << "Level = ";
						in.Read(L"Level", Level, uint16(1));
						//std::cout << Level << std::endl;
						in.Read(L"RebirthCount", RebirthCount, uint32(0));
						//std::cout << "Level = " << RebirthCount << std::endl;
						in.Read(L"States", States, Info::ActionInfo::StateList());
						in.Read(L"StateBundleConditions", StateBundleConditions, Info::StateBundleInfo::ConditionList());
					} */

					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// Constants::Constants

					void EnchantTableManager::Constants::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::Constants");
						out.Write(L"HashJewel", HashJewel);
						out.Write(L"HashJewelForAccessory", HashJewelForAccessory);
						out.Write(L"HashRecoverStone", HashRecoverStone );
						out.Write(L"HashResetScroll", HashResetScroll );
						out.Write(L"MaxSkillPlus", MaxSkillPlus );
						out.Write(L"HashMineral", HashMineral );
						out.Write(L"HashMineralForAccessory", HashMineralForAccessory );
						out.Write(L"HashCoin", HashCoin );
						out.Write(L"HashCatalyst", HashCatalyst );
					}

					void EnchantTableManager::Constants::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::Constants");
						in.Read(L"HashJewel", HashJewel, std::vector<uint32>(4,0));
						in.Read(L"HashJewelForAccessory", HashJewelForAccessory, std::vector<uint32>(4,0));
						in.Read(L"HashRecoverStone", HashRecoverStone, uint32(0));
						in.Read(L"HashResetScroll", HashResetScroll, uint32(0));
						in.Read(L"MaxSkillPlus", MaxSkillPlus, uint8(0));
						in.Read(L"HashMineral", HashMineral, uint32(0));
						in.Read(L"HashMineralForAccessory", HashMineralForAccessory, uint32(0));
						in.Read(L"HashCoin", HashCoin, uint32(0));
						in.Read(L"HashCatalyst", HashCatalyst, uint32(0));
					}

					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// EnchantTableManager
					void EnchantTableManager::Init(bool bForceXmlData)
					{
						Reinforcement.Clear();

						if (bForceXmlData) {//Xml loading
							LoadXmlData();
						} else { //Binary loading
							LoadBinaryData();
						}
					}

					float EnchantTableManager::RetrieveIdentifyValue(uint16 level, EnchantTargets::Type type, uint8  value) const
					{
						if ( level >= XRated::Constants::MaxLevel || value > 8 || type >= EnchantTargets::None ) {
							Logger::GetInstance().Info( L"[EnchantTableManager::RetrieveIdentifyValue] Wrong range. [{0}/{1}/{2}]", level, type, value );
							return 0.0f;
						}

						return Identify.Table[level][type][value];
					}

					float EnchantTableManager::RetrieveIdentifyRebirthedItemValue(uint16 sumOfValue, EnchantTargets::Type type, uint8 value, uint16 rebirthCount) const
					{
						if ( sumOfValue >= 10000 || value > 8 || type >= EnchantTargets::None || rebirthCount == 0 ) {
							Logger::GetInstance().Info(L"[EnchantTableManager::RetrieveIdentifyRebirthedItemValue] Wrong range. [{0}/{1}/{2}/{3}]", sumOfValue/100, type, value, rebirthCount);
							return 0.0f;
						}

						if(Identify.RebirthTable.size() < sumOfValue / 100 + 1)
							return Identify.RebirthTable[Identify.RebirthTable.size()-1][type][value];

						return Identify.RebirthTable[sumOfValue/100][type][value];
					}

					const float4& EnchantTableManager::RetrieveReinforceValue(uint16 level, uint8 reinforcementLv, XRated::Constants::EquipParts parts) const
					{
						static const float4 nullValue(0,0,0,0);
						//TODO : �Ź��� ��� ���̺��� ������ ���� �ʰ� �Ǿ� �־ ������ by kim.
						if ( level > XRated::Constants::MaxLevel || reinforcementLv > XRated::Constants::LocaleSpecificSetting::REINFORCEMENT_LEVEL /*��ȭ �ְ����� �ʰ�*/ ) 
						{
							Logger::GetInstance().Info( L"[EnchantTableManager::RetrieveReinforceValue] Wrong range. [{0}/{1}/{2}]", level, reinforcementLv, parts );
							return nullValue;
						}
						if ( level >= (uint16)Reinforcement.Table.size() || reinforcementLv >= (uint16)Reinforcement.Table[level].size() || (int)parts >= (int)Reinforcement.Table[level][reinforcementLv].size() ) {
							Logger::GetInstance().Info( L"[EnchantTableManager::RetrieveReinforceValue] Not enough table values. [{0}/{1}/{2}]tin", level, reinforcementLv, parts );
							return nullValue;
						}

						return Reinforcement.Table[level][reinforcementLv][parts];
					}

					const float4& EnchantTableManager::RetrieveReinforceRebirthedItemValue(uint16 sumOfValue, uint8 reinforcementLv, XRated::Constants::EquipParts parts, uint16 itemRebirthLimit) const
					{
						static const float4 nullValue(0,0,0,0);

						if(sumOfValue > 10000 || reinforcementLv > XRated::Constants::LocaleSpecificSetting::REINFORCEMENT_LEVEL /*��ȭ �ְ����� �ʰ�*/ )
						{
							Logger::GetInstance().Info(L"[EnchantTableManager::RetrieveReinforceRebirthedItemValue] Wrong range. [{0}/{1}/{2}/{3}]", sumOfValue, reinforcementLv, parts, itemRebirthLimit);
							return nullValue;
						}
						if ( sumOfValue/100 >= (uint16)Reinforcement.Table.size() || reinforcementLv >= (uint16)Reinforcement.RebirthTable[sumOfValue/100].size() || (int)parts >= (int)Reinforcement.Table[sumOfValue/100][reinforcementLv].size() ) {
							Logger::GetInstance().Info(L"[EnchantTableManager::RetrieveReinforceRebirthedItemValue] Not enough table values. [{0}/{1}/{2}/{3}]", sumOfValue, reinforcementLv, parts, itemRebirthLimit);
							return nullValue;
						}

						return Reinforcement.RebirthTable[sumOfValue/100][reinforcementLv][parts];
					}

					float EnchantTableManager::RetrieveLightReinforceValue(uint16 level, uint8 lightReinforcementLv, XRated::Constants::EquipParts parts, int stat) const
					{
						if ( level > XRated::Constants::MaxLevel || 
							lightReinforcementLv > XRated::Constants::LocaleSpecificSetting::LIGHTREINFORCEMENT_LEVEL /*�� ��ȭ �ְ����� �ʰ�*/ || 
							parts > XRated::Constants::EquipParts::EARING ) 
						{
							Logger::GetInstance().Info( L"[EnchantTableManager::RetrieveLightReinforceValue] Wrong range. [{0}/{1}/{2}]", level, lightReinforcementLv, parts );
							return 0.0f;
						}
		
						std::map< uint16, std::vector< std::vector< std::vector<float> > > >::const_iterator levelFindIt = LightReinforcement.Table.end();
						{
							std::map< uint16, std::vector< std::vector< std::vector<float> > > >::const_iterator levelIt = LightReinforcement.Table.begin();
							std::map< uint16, std::vector< std::vector< std::vector<float> > > >::const_iterator levelEndIt = LightReinforcement.Table.end();
							for( ; levelIt != levelEndIt; ++levelIt )
							{
								levelFindIt = levelIt;
								if( level == levelIt->first )
								{
									break;
								}
								else if( level < levelIt->first )
								{
									if( levelIt != LightReinforcement.Table.begin() )
									{
										levelFindIt = --levelIt;
									}
									break;
								}
							}
						}
						if ( levelFindIt == LightReinforcement.Table.end() || 
							lightReinforcementLv >= levelFindIt->second.size() || 
							(int)parts >= (int)levelFindIt->second.at(lightReinforcementLv).size() ||
							stat >= levelFindIt->second.at(lightReinforcementLv)[parts].size() ) 
						{
							//ALLM_IMPORTANT(( L"[EnchantTableManager::RetrieveLightReinforceValue] Not enough table values. [{0}/{1}/{2}/{3}]", level, lightReinforcementLv, parts, stat));
							return 0.0f;
						}

						return levelFindIt->second.at(lightReinforcementLv)[parts][stat];
					}
	
					float EnchantTableManager::RetrieveLightReinforceValueForRebirth(uint16 level, uint8 lightReinforcementLv, XRated::Constants::EquipParts parts, int stat) const
					{
						if ( level > XRated::Constants::StoredMaxLevel || 
							lightReinforcementLv > XRated::Constants::LocaleSpecificSetting::LIGHTREINFORCEMENT_LEVEL /*�� ��ȭ �ְ����� �ʰ�*/ || 
							parts > XRated::Constants::EquipParts::EARING ) 
						{
							Logger::GetInstance().Info( L"[EnchantTableManager::RetrieveLightReinforceValue] Wrong range. [{0}/{1}/{2}]", level, lightReinforcementLv, parts );
							return 0.0f;
						}
		
						std::map< uint16, std::vector< std::vector< std::vector<float> > > >::const_iterator levelFindIt = LightReinforcement.RebirthTable.end();
						{
							std::map< uint16, std::vector< std::vector< std::vector<float> > > >::const_iterator levelIt = LightReinforcement.RebirthTable.begin();
							std::map< uint16, std::vector< std::vector< std::vector<float> > > >::const_iterator levelEndIt = LightReinforcement.RebirthTable.end();
							for( ; levelIt != levelEndIt; ++levelIt )
							{
								levelFindIt = levelIt;
								if( level == levelIt->first )
								{
									break;
								}
								else if( level < levelIt->first )
								{
									if( levelIt != LightReinforcement.RebirthTable.begin() )
									{
										levelFindIt = --levelIt;
									}
									break;
								}
							}
						}
						if ( levelFindIt == LightReinforcement.RebirthTable.end() || 
							lightReinforcementLv >= levelFindIt->second.size() || 
							(int)parts >= (int)levelFindIt->second.at(lightReinforcementLv).size() ||
							stat >= levelFindIt->second.at(lightReinforcementLv)[parts].size() ) 
						{
							//ALLM_IMPORTANT(( L"[EnchantTableManager::RetrieveLightReinforceValue] Not enough table values. [{0}/{1}/{2}/{3}]", level, lightReinforcementLv, parts, stat));
							return 0.0f;
						}

						return levelFindIt->second.at(lightReinforcementLv)[parts][stat];
					}

					const EnchantTableManager::MagicalAttachInfo* EnchantTableManager::RetrieveMagicalAttachValue(uint16 index) const
					{
						if ( index >= MagicalAttach.ScrollList.size() ) {
							Logger::GetInstance().Info( L"[EnchantTableManager::RetrieveMagicalAttachValue] Wrong index. [{0}]", index );
							return NULL;
						}
						return &MagicalAttach.ScrollList[index];
					}


					//���꽺ų( �˻��� �±� )
					const bool EnchantTableManager::IsProduceSkill( const std::wstring& skillGroupName ) const
					{
						MakingSkillTable::MakingSkillGroups::const_iterator iter = MakingSkillTable.makingSkillGroups.find( skillGroupName );

						if ( iter != MakingSkillTable.makingSkillGroups.end() )
						{
							return true;
						}

						return false;		
					}

					//���꽺ų( �˻��� �±� )
					const bool EnchantTableManager::RetrieveMakingSkillMadeItems( const std::wstring& skillGroupName, const uint32 skillLevel, ItemInfosForProduce& result ) const
					{
						MakingSkillTable::MakingSkillGroups::const_iterator iter = MakingSkillTable.makingSkillGroups.find( skillGroupName );

						if ( iter != MakingSkillTable.makingSkillGroups.end() )
						{
							if ( iter->second.size() >= skillLevel )
							{
								result = iter->second.at( skillLevel );

								return true;
							}
							else
							{
								return false;		
							}
						}

						return false;
					}

					//����ȭ( �˻��� �±� )
					uint16 EnchantTableManager::RetrieveJewelCntPerLevel(uint16 lv) const
					{
						if ( (int)Extraction.CountPerLevel.size() < 4 )
							return 0;
						if ( lv >= (int)Extraction.CountPerLevel[0].size() )
							return static_cast<uint16>(Extraction.CountPerLevel[0].back());

						return Extraction.CountPerLevel[0][lv];
					}

					uint16 EnchantTableManager::RetrieveNextJewelCntPerLevel(uint16 lv) const
					{
						if ( (int)Extraction.CountPerLevel.size() < 4 )
							return 0;
						if ( lv >= (int)Extraction.CountPerLevel[1].size() )
							return static_cast<uint16>(Extraction.CountPerLevel[1].back());
						return Extraction.CountPerLevel[1][lv];
					}

					uint16 EnchantTableManager::RetrieveMineralCntPerLevel(uint16 lv) const
					{
						if ( (int)Extraction.CountPerLevel.size() < 4 )
							return 0;
						if ( lv >= (int)Extraction.CountPerLevel[2].size() )
							return static_cast<uint16>(Extraction.CountPerLevel[2].back());
						return Extraction.CountPerLevel[2][lv];
					}

					uint16 EnchantTableManager::RetrieveCatalystCntPerLevel(uint16 lv) const
					{
						if ( (int)Extraction.CountPerLevel.size() < 4 )
							return 0;
						if ( lv >= (int)Extraction.CountPerLevel[3].size() )
							return static_cast<uint16>(Extraction.CountPerLevel[3].back());
						return Extraction.CountPerLevel[3][lv];
					}

					uint16 EnchantTableManager::RetrieveJewelCntPerReinforcement(uint8 lv) const
					{
						if ( (int)Extraction.CountPerReinforcement.size() < 4 )
							return 0;
						if ( lv >= (int)Extraction.CountPerReinforcement[0].size() )
							return 0;
						return Extraction.CountPerReinforcement[0][lv];
					}

					uint16 EnchantTableManager::RetrieveNextJewelCntPerReinforcement(uint8 lv) const
					{
						if ( (int)Extraction.CountPerReinforcement.size() < 4 )
							return 0;
						if ( lv >= (int)Extraction.CountPerReinforcement[1].size() )
							return 0;
						return Extraction.CountPerReinforcement[1][lv];
					}


					//��ȭ��( �˻��� �±� )
					uint16 EnchantTableManager::RetrieveMineralCntPerReinforcement(uint8 lv) const
					{
						if ( (int)Extraction.CountPerReinforcement.size() < 4 )
							return 0;
						if ( lv >= (int)Extraction.CountPerReinforcement[2].size() )
							return 0;
						return Extraction.CountPerReinforcement[2][lv];
					}

					uint16 EnchantTableManager::RetrieveCatalystCntPerReinforcement(uint8 lv) const
					{
						if ( (int)Extraction.CountPerReinforcement.size() < 4 )
							return 0;
						if ( lv >= (int)Extraction.CountPerReinforcement[3].size() )
							return 0;
						return Extraction.CountPerReinforcement[3][lv];
					}


					//�˸�( �˻��� �±� )
					uint32 EnchantTableManager::GetCatalystMinCountForReinforce( uint32 hash, uint16 lv ) const
					{
						std::vector<CatalystTable>::const_iterator iter = Catalysts.begin();
						std::vector<CatalystTable>::const_iterator end = Catalysts.end();
						for( ; iter != end; ++iter ) 
						{
							if( (*iter).ItemHash == hash ) 
							{					
								if( iter->ReinforceMinCounts.size() > lv )
								{
									return iter->ReinforceMinCounts[ lv ];
								}
							}
						}
						return 0;
					}

					uint32 EnchantTableManager::GetCatalystMinCountForLightReinforce( uint32 hash, uint16 lv ) const
					{
						std::vector<CatalystTable>::const_iterator iter = Catalysts.begin();
						std::vector<CatalystTable>::const_iterator end = Catalysts.end();
						for( ; iter != end; ++iter ) 
						{
							if( (*iter).ItemHash == hash ) 
							{					
								if( iter->LightReinforceMinCounts.size() > lv )
								{
									return iter->LightReinforceMinCounts[ lv ];
								}
							}
						}
						return 0;
					}

					uint32 EnchantTableManager::GetCatalystMinCountForReinforceOfAccessory( uint32 hash, uint16 lv ) const
					{
						std::vector<CatalystTable>::const_iterator iter = CatalystsForAccessory.begin();
						std::vector<CatalystTable>::const_iterator end = CatalystsForAccessory.end();
						for( ; iter != end; ++iter ) 
						{
							if( (*iter).ItemHash == hash ) 
							{					
								if( iter->ReinforceMinCounts.size() > lv )
								{
									return iter->ReinforceMinCounts[ lv ];
								}
							}
						}
						return 0;
					}

					uint32 EnchantTableManager::GetCatalystMinCountForLightReinforceOfAccessory( uint32 hash, uint16 lv ) const
					{
						std::vector<CatalystTable>::const_iterator iter = CatalystsForAccessory.begin();
						std::vector<CatalystTable>::const_iterator end = CatalystsForAccessory.end();
						for( ; iter != end; ++iter ) 
						{
							if( (*iter).ItemHash == hash ) 
							{					
								if( iter->LightReinforceMinCounts.size() > lv )
								{
									return iter->LightReinforceMinCounts[ lv ];
								}
							}
						}
						return 0;
					}
	
					int EnchantTableManager::GetReinforceRecoverFee() const
					{
						return Reinforcement.RecoverFee;
					}
	
					int EnchantTableManager::GetLightReinforceRecoverFee() const
					{
						return LightReinforcement.RecoverFee;
					}
	
					int EnchantTableManager::GetResetIdentificationFee() const
					{
						return Identify.ResetFee;
					}
	
					int EnchantTableManager::GetReinforceRecoverStoneCnt( uint16 lv, uint32 stoneHash ) const// -1 : �� ������ �ش��ϴ� ������ �ؽð� �ƴϴ�.
					{
						std::map< uint16, std::map< uint32, uint8 > >::const_iterator levelIt = Reinforcement.RecoverTable.begin();
						std::map< uint16, std::map< uint32, uint8 > >::const_iterator endLevelIt = Reinforcement.RecoverTable.end();
						std::map< uint16, std::map< uint32, uint8 > >::const_iterator findLevelIt = Reinforcement.RecoverTable.end();
						for( ; levelIt != endLevelIt; ++levelIt )
						{
							findLevelIt = levelIt;
							if( lv == levelIt->first )
							{
								break;
							}
							else if( lv < levelIt->first )
							{
								if( levelIt != Reinforcement.RecoverTable.begin() )
								{
									findLevelIt = --levelIt;
								}
								break;
							}
						}

						if( findLevelIt != Reinforcement.RecoverTable.end() )
						{
							std::map< uint32, uint8 >::const_iterator stoneIt = findLevelIt->second.find( stoneHash );
							if( stoneIt != findLevelIt->second.end() )
							{
								return stoneIt->second;
							}
						}
						return -1;
					}
	
					int EnchantTableManager::GetLightReinforceRecoverStoneCnt( uint16 lv, uint32 stoneHash ) const // -1 : �� ������ �ش��ϴ� ������ �ؽð� �ƴϴ�.
					{
						std::map< uint16, std::map< uint32, uint8 > >::const_iterator levelIt = LightReinforcement.RecoverTable.begin();
						std::map< uint16, std::map< uint32, uint8 > >::const_iterator endLevelIt = LightReinforcement.RecoverTable.end();
						std::map< uint16, std::map< uint32, uint8 > >::const_iterator findLevelIt = LightReinforcement.RecoverTable.end();
						for( ; levelIt != endLevelIt; ++levelIt )
						{
							findLevelIt = levelIt;
							if( lv == levelIt->first )
							{
								break;
							}
							else if( lv < levelIt->first )
							{
								if( levelIt != LightReinforcement.RecoverTable.begin() )
								{
									findLevelIt = --levelIt;
								}
								break;
							}
						}

						if( findLevelIt != LightReinforcement.RecoverTable.end() )
						{
							std::map< uint32, uint8 >::const_iterator stoneIt = findLevelIt->second.find( stoneHash );
							if( stoneIt != findLevelIt->second.end() )
							{
								return stoneIt->second;
							}
						}
						return -1;
					}
	
					int EnchantTableManager::GetResetIdentificationScrollCnt( uint16 lv, uint32 scrollHash ) const // -1 : �� ������ �ش��ϴ� �簨���� �ؽð� �ƴϴ�.
					{
						std::map< uint16, std::map< uint32, uint8 > >::const_iterator levelIt = Identify.ResetTable.begin();
						std::map< uint16, std::map< uint32, uint8 > >::const_iterator endLevelIt = Identify.ResetTable.end();
						std::map< uint16, std::map< uint32, uint8 > >::const_iterator findLevelIt = Identify.ResetTable.end();
						for( ; levelIt != endLevelIt; ++levelIt )
						{
							findLevelIt = levelIt;
							if( lv == levelIt->first )
							{
								break;
							}
							else if( lv < levelIt->first )
							{
								if( levelIt != AdvancedIdentify.ResetTable.begin() )
								{
									findLevelIt = --levelIt;
								}
								break;
							}
						}

						if( findLevelIt != AdvancedIdentify.ResetTable.end() )
						{
							std::map< uint32, uint8 >::const_iterator scrollIt = findLevelIt->second.find( scrollHash );
							if( scrollIt != findLevelIt->second.end() )
							{
								return scrollIt->second;
							}
						}
						return -1;

					}

					uint32 EnchantTableManager::GetRequirementFactorPerGrade(uint8 grade) const
					{
						if ( grade >= (int)Requirement.GradeFactor.size() ) {
							Logger::GetInstance().Info(L"[EnchantTableManager::GetRequirementFactorPerGrade] wrong grade range. [{0}]", grade);
							return 0xFFFFFFFF;
						}
						return Requirement.GradeFactor[grade];
					}

					uint32 EnchantTableManager::GetRequirementFactorPerReinforcement(uint8 reinforceLv) const
					{
						if ( reinforceLv >= (int)Requirement.ReinforcementFactor.size() ) {
							Logger::GetInstance().Info(L"[EnchantTableManager::GetRequirementFactorPerGrade] wrong grade range. [{0}]", reinforceLv);
							return 0xFFFFFFFF;
						}
						return Requirement.ReinforcementFactor[reinforceLv];
					}

					uint32 EnchantTableManager::GetRequirementFactorPerLightReinforcement(uint8 lightLv) const
					{
						if ( lightLv >= (int)Requirement.LightReinforcementFactor.size() ) {
							Logger::GetInstance().Info(L"[EnchantTableManager::GetRequirementFactorPerLightReinforcement] wrong light level range. [{0}]", lightLv);
							return 0xFFFFFFFF;
						}
						return Requirement.LightReinforcementFactor[lightLv];
					}

					//��ȭ�ÿ� �ʿ��� ������ ����.
					uint16 EnchantTableManager::GetMineralCntPerLevel(uint16 lv) const
					{ 
						if ( lv > XRated::Constants::StoredMaxLevel ) 
						{
							return 0xFFFF;
						}
		
						return Requirement.MineralCntPerLevel[lv]; 
					} 

					//��ȭ�ÿ� �ʿ��� ������ ����.
					uint16 EnchantTableManager::GetJewelCntPerLevel(uint16 lv) const
					{ 
						if ( lv > XRated::Constants::StoredMaxLevel ) 
						{
							return 0xFFFF;
						}
		
						return Requirement.JewelCntPerLevel[lv]; 
					} 



					const AlchemyTable* EnchantTableManager::GetAlchemyTable(const Info::ItemInfo* info/* source */) const
					{
						std::map< uint32, AlchemyTable >::const_iterator i=alchemy.find(info->Hash);
						if (i==alchemy.end()) return NULL;
						return &(i->second);
					}

					bool EnchantTableManager::IsCashEnchantAvailable( const CashEnchantInfo* info, uint16 level, uint16 storedLevel, uint16 rebirthCount, uint16 rebirthCountEquipLimit )
					{
						if( info != NULL )
						{
							if( info->Level <= level &&
								info->StoredLevel <= storedLevel &&
								info->RebirthCount <= rebirthCount )
							{
								if( rebirthCountEquipLimit != 0 && rebirthCountEquipLimit >= info->RebirthCount )
								{
									return true;
								}
								else if( rebirthCountEquipLimit == 0 && RebirthCountEquipLimit >= info->RebirthCount )
								{
									return true;
								}
							}
						}
						return false;
					}
					std::pair< CashEnchantInfo*, CashEnchantInfo* > EnchantTableManager::GetCashEnchantInfo( uint32 id1, uint32 id2 )
					{
						std::pair< CashEnchantInfo*, CashEnchantInfo* > returnValue;
						std::map< uint32, CashEnchantInfo >::iterator i = CashEnchant[0].find(id1);
						if (i==CashEnchant[0].end())
						{
							returnValue.first = NULL;
						}
						else 
						{
							returnValue.first = &(*i).second;
						}

						i = CashEnchant[1].find(id2);
						if (i==CashEnchant[1].end() )
						{
							returnValue.second = NULL;
						}
						else 
						{
							returnValue.second = &(*i).second;
						}

						return returnValue;
					}
					//CashEnchantInfo* EnchantTableManager::GetCashEnchantInfo(uint32 id)
					//{
					//	std::map< uint32, CashEnchantInfo >::iterator i = CashEnchant.find(id);
					//	if (i==CashEnchant.end()) return NULL;
					//	return &(i->second);
					//}

					void EnchantTableManager::GetCashEnchantList(std::vector<uint32>& l) const
					{
						std::map< uint32, CashEnchantInfo >::const_iterator i = CashEnchant[0].begin();
						for( ; i != CashEnchant[0].end() ; ++i)
							l.push_back( (*i).first );
					}

					void EnchantTableManager::MakingSkillTable::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::MakingSkillTable");
						out.Write(L"MakingSkillGroups", makingSkillGroups);
					}

					void EnchantTableManager::MakingSkillTable::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::MakingSkillTable");
						in.Read(L"MakingSkillGroups", makingSkillGroups);
					}

					void EnchantTableManager::LoadXmlData()
					{
						if(Lunia::FileIO::File::Exists(L"Database/EnchantTable.xml")){
							Resource::SerializerStreamReader reader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/EnchantTable.xml");
							reader->Read(L"EnchantTableManager", *this);
						}else{
							Lunia::Resource::SerializerStreamReader reinforcementReader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Enchant/Reinforcement.xml");
							reinforcementReader->Read(L"Reinforcement", Reinforcement);

							Lunia::Resource::SerializerStreamReader identifyReader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Enchant/Identify.xml");
							identifyReader->Read(L"Identify", Identify);

							Lunia::Resource::SerializerStreamReader AdvancedIdentifyReader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Enchant/AdvancedIdentify.xml");
							AdvancedIdentifyReader->Read(L"AdvancedIdentify", AdvancedIdentify);

							Lunia::Resource::SerializerStreamReader CashIdentifyReader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Enchant/CashIdentify.xml");
							CashIdentifyReader->Read(L"CashIdentify", CashIdentify);

							Lunia::Resource::SerializerStreamReader lightReader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Enchant/LightReinforcement.xml");
							lightReader->Read(L"LightReinforcement", LightReinforcement);

							Lunia::Resource::SerializerStreamReader magicalReader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Enchant/MagicalAttach.xml");
							magicalReader->Read(L"MagicalAttach", MagicalAttach);

							Lunia::Resource::SerializerStreamReader extractionReader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Enchant/Extraction.xml");
							extractionReader->Read(L"Extraction", Extraction);

							Lunia::Resource::SerializerStreamReader catalystReader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Enchant/Catalyst.xml");
							catalystReader->Read(L"Catalysts", Catalysts);

							Lunia::Resource::SerializerStreamReader catalystForAccessoryReader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Enchant/CatalystsForAccessory.xml");
							catalystForAccessoryReader->Read(L"CatalystsForAccessory", CatalystsForAccessory);

							Lunia::Resource::SerializerStreamReader requirementReader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Enchant/Requirement.xml");
							requirementReader->Read(L"Requirement", Requirement);

							Lunia::Resource::SerializerStreamReader alchemyReader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Enchant/Alchemy.xml");
							alchemyReader->Read(L"alchemy", alchemy);

							Lunia::Resource::SerializerStreamReader cashEnchantReader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Enchant/CashEnchant.xml");
							cashEnchantReader->Read(L"CashEnchant", CashEnchant);

							Lunia::Resource::SerializerStreamReader constantsReader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Enchant/Constants.xml");
							constantsReader->Read(L"Constant", Constant);

							Lunia::Resource::SerializerStreamReader makingSkillTableReader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Enchant/MakingSkillTable.xml");
							makingSkillTableReader->Read(L"MakingSkillTable", MakingSkillTable);

							Lunia::Resource::SerializerStreamReader composeItemTypesReader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Enchant/ComposeItemTypeTable.xml");
							composeItemTypesReader->Read(L"ComposeItemTypes", ComposeItemTypes);
			
							Lunia::Resource::SerializerStreamReader composeGradePointsReader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Enchant/ComposeGradePointTable.xml");
							composeGradePointsReader->Read(L"ComposeGradePoints", ComposeGradePoints);
						}
					}

					void EnchantTableManager::LoadBinaryData()
					{
						Resource::SerializerStreamReader reader = Lunia::Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/EnchantTable.b");
						reader->Read(L"EnchantTableManager", *this);
					}

					void EnchantTableManager::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager");
						out.Write(L"Reinforcement", Reinforcement);
						out.Write(L"Identify", Identify);
						out.Write(L"AdvancedIdentify", AdvancedIdentify);
						out.Write(L"CashIdentify", CashIdentify);
						out.Write(L"LightReinforcement", LightReinforcement);
						out.Write(L"MagicalAttach", MagicalAttach);
						out.Write(L"Extraction", Extraction);
						out.Write(L"Catalysts", Catalysts);
						out.Write(L"CatalystsForAccessory", CatalystsForAccessory);
						out.Write(L"Requirement", Requirement);
						out.Write(L"alchemy", alchemy);
						out.Write(L"CashEnchant", CashEnchant);
						out.Write(L"CashEnchant3", CashEnchant3);
						out.Write(L"CashEnchant4", CashEnchant4);
						out.Write(L"Constant",	Constant );
						out.Write(L"MakingSkillTable",	MakingSkillTable );
						out.Write(L"ComposeItemTypes",	ComposeItemTypes );
						out.Write(L"ComposeGradePoints",	ComposeGradePoints );
					}

					void EnchantTableManager::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager");
						in.Read(L"Reinforcement", Reinforcement);
						printf("Done Reinforcement\n");
						in.Read(L"Identify", Identify);
						printf("Done Identify\n");
						in.Read(L"AdvancedIdentify", AdvancedIdentify);
						printf("Done AdvancedIdentify\n");
						in.Read(L"CashIdentify", CashIdentify);
						printf("Done CashIdentify\n");
						in.Read(L"LightReinforcement", LightReinforcement);
						printf("Done LightReinforcement\n");
						in.Read(L"MagicalAttach", MagicalAttach);
						printf("Done MagicalAttach\n");
						in.Read(L"Extraction", Extraction);
						printf("Done Extraction\n");
						in.Read(L"Catalysts", Catalysts);
						printf("Done Catalysts\n");
						in.Read(L"CatalystsForAccessory", CatalystsForAccessory);
						printf("Done CatalystsForAccessory\n");
						in.Read(L"Requirement", Requirement);
						printf("Done Requirement\n");
						in.Read(L"alchemy", alchemy);
						printf("Done alchemy\n");
						in.Read(L"CashEnchant", CashEnchant);
						printf("Done CashEnchant\n");
						in.Read(L"CashEnchant3", CashEnchant3);
						printf("Done CashEnchant3\n");
						in.Read(L"CashEnchant4", CashEnchant4);
						printf("Done CashEnchant4\n");
						in.Read(L"Constant",	Constant );
						printf("Done Constant\n");
						in.Read(L"MakingSkillTable",	MakingSkillTable );
						printf("Done MakingSkillTable\n");
						in.Read(L"ComposeItemTypes", ComposeItemTypes );
						printf("Done ComposeItemTypeTable\n");
						in.Read(L"ComposeGradePoints", ComposeGradePoints );
						printf("Done ComposeGradePoints\n");

						EnchantTableManager::TempConstants::HashRecoverStone = Constant.HashRecoverStone;
						EnchantTableManager::TempConstants::HashResetScroll = Constant.HashResetScroll;
					}

					void EnchantTableManager::SetRebirthCountEquipLimit( uint16 pRebirthCountEquipLimit )
					{
						RebirthCountEquipLimit = pRebirthCountEquipLimit;
					}
					EnchantTableManager::EnchantTableManager()
						: RebirthCountEquipLimit(0)
					{
					}
					const EnchantTableManager::Constants& EnchantTableManager::GetConstants() const
					{
						return Constant;
					}

					uint32 EnchantTableManager::TempConstants::HashRecoverStone;
					uint32 EnchantTableManager::TempConstants::HashResetScroll;

					void EnchantTableManager::ComposeItemTypes::Deserialize(Serializer::IStreamReader& in){
						in.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::ComposeItemTypeTable");
						in.Read(L"ItemTypePoint1Table", ItemTypePoint1Table);
						in.Read(L"ItemTypePoint2Table", ItemTypePoint2Table);
						in.Read(L"ItemTypePoint3Table", ItemTypePoint3Table);
					}
					void EnchantTableManager::ComposeItemTypes::Serialize(Serializer::IStreamWriter& out) const{
						out.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::ComposeItemTypeTable");
						out.Write(L"ItemTypePoint1Table", ItemTypePoint1Table);
						out.Write(L"ItemTypePoint2Table", ItemTypePoint2Table);
						out.Write(L"ItemTypePoint3Table", ItemTypePoint3Table);
					}
	
					void EnchantTableManager::ComposeGradePointTable::Deserialize(Serializer::IStreamReader& in){
						in.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::ComposeGradePointTable");
						in.Read(L"GradePointTables", GradePointTables);
					}
					void EnchantTableManager::ComposeGradePointTable::Serialize(Serializer::IStreamWriter& out) const{
						out.Begin(L"Lunia::XRated::Database::Enchant::EnchantTableManager::ComposeGradePointTable");
						out.Write(L"GradePointTables", GradePointTables);
					}
					Lunia::XRated::Database::Info::ItemInfo* EnchantTableManager::GetResult(std::pair<Lunia::XRated::Database::Info::ItemInfo*,Lunia::XRated::Database::Info::ItemInfo*> input){
		
						/* if(input.first->ComposeData.Type != input.second->ComposeData.Type)
							return NULL;
						for ( auto it = ComposeGradePoints.begin(); it != ComposeGradePoints.end(); it++){
							if( == it->first)
								for( auto itt = it->second.GradePointTables.begin(); itt != it->second.GradePointTables.end(); it++){
									input.first->ComposeData.
								}			
						} */
						return NULL;
					}
				}		
			}	
		}	
	}
}
