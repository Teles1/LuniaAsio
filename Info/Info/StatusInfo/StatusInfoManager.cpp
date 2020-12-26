#pragma once
#include "StatusInfoManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				StatusInfoManager::StatusInfoManager()
				{
				}
				void StatusInfoManager::Init(bool bForceXmlData)
				{
					for (StatusTable::iterator it = statusTable.begin(); it != statusTable.end(); ++it) {
						for (std::vector<StatusInfo*>::iterator info = it->second.statusList.begin(); info != it->second.statusList.end(); ++info)
							delete (*info);
					}
					statusTable.clear();
					rebirthAddtionalFactors.clear();
					rebirthAddtionalMajorFactors.clear();
					dollStatApplyRate.clear();

					if (bForceXmlData) {//Xml loading
						LoadXmlData();
					}
					else { //Binary loading
						std::wstring fullLoading;
						LoadBinaryData();
					}
				}

				void StatusInfoManager::LoadXmlData()
				{
					Resource::SerializerStreamReader expReader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/Status/ExpTable.xml");
					expReader->Read(L"ExpTable", expTable);

					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/Status/BasicStat.xml");

					reader->Read(L"BasicStatMap", basicStatMap);

					Resource::SerializerStreamReader elementReader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/Status/ElementTable.xml");
					elementReader->Read(L"ElementTable", elementTable);

					for (BasicStatMap::iterator i = basicStatMap.begin(); i != basicStatMap.end(); ++i) {
						LoadStatusTable(i->second.name, i->second);
					}

					reader->Read(L"RebirthAddtionalFactors", rebirthAddtionalFactors);
					reader->Read(L"RebirthAddtionalMajorFactors", rebirthAddtionalMajorFactors);
					reader->Read(L"DollStatApplyRate", dollStatApplyRate);
				}

				void StatusInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/StatusInfos.b");
					reader->Read(L"StatusInfoManager", *this);
				}

				void StatusInfoManager::LoadStatusTable(const std::wstring& name, BasicStatInfo& info)
				{
					int v = info.stats[0].Vit, i = info.stats[0].Int, d = info.stats[0].Dex, s = info.stats[0].Str;
					//float rh=1, rm=1; //regenHP, regenMP
					float hp = v * StatusFunctions::GetHPperVit(0);
					float mp = i * StatusFunctions::GetMPperInt(0);

					uint32 hash = StringUtil::Hash(name.c_str());
					//statusTable[ hash ] = StatusGroup(); //std::vector<StatusInfo*>();
					StatusGroup& statusGroup = statusTable[hash];
					std::vector<StatusInfo*>& bases = statusGroup.statusList;
					statusGroup.majorStat = info.majorStat;

					int size = (int)info.stats.size() - 1;
					for (uint16 lvl = 1; lvl <= Constants::MaxLevel; ++lvl)
					{
						bases.push_back(new StatusInfo());
						StatusInfo& status = *bases.back();
						status.Level = lvl;
						int index = (lvl - 1) % size + 1;
						v += info.stats[index].Vit;
						status.Vitality = v;
						i += info.stats[index].Int;
						status.Intelligence = i;
						d += info.stats[index].Dex;
						status.Dextrity = d;
						s += info.stats[index].Str;
						status.Strength = s;
						//if (!(lvl%10)) rh++;
						//if (!(lvl%10)) rm++;
						//status.regenHP = rh;
						//status.regenMP = rm;
						hp += info.stats[index].Vit * StatusFunctions::GetHPperVit(lvl);
						mp += info.stats[index].Int * StatusFunctions::GetMPperInt(lvl);
						status.Hp = hp;
						status.Mp = mp;

						//status.CalculateSecondStat();
					}
					//statusGroup.StackedStr[ 0 ] = bases[ 19 ]->Strength;
					//statusGroup.StackedStr[ 1 ] = bases[ 39 ]->Strength;
					//statusGroup.StackedStr[ 2 ] = bases[ 59 ]->Strength;

					//statusGroup.StackedDex[ 0 ] = bases[ 9 ]->Dextrity;
					//statusGroup.StackedDex[ 1 ] = bases[ 19 ]->Dextrity;
					//statusGroup.StackedDex[ 2 ] = bases[ 29 ]->Dextrity;
					//statusGroup.StackedDex[ 3 ] = bases[ 39 ]->Dextrity;
					//statusGroup.StackedDex[ 4 ] = bases[ 49 ]->Dextrity;
					//statusGroup.StackedDex[ 5 ] = bases[ 59 ]->Dextrity;

					//statusGroup.StackedVit[ 0 ] = bases[ 18 ]->Vitality;
					//statusGroup.StackedVit[ 1 ] = bases[ 58 ]->Vitality;
					//statusGroup.StackedVit[ 2 ] = bases[ 59 ]->Vitality;

					//statusGroup.StackedInt[ 0 ] = bases[ 18 ]->Intelligence;
					//statusGroup.StackedInt[ 1 ] = bases[ 58 ]->Intelligence;
					//statusGroup.StackedInt[ 2 ] = bases[ 59 ]->Intelligence;
				}

				StatusInfoManager::~StatusInfoManager()
				{
					for (StatusTable::iterator it = statusTable.begin(); it != statusTable.end(); ++it)
					{
						for (std::vector< StatusInfo* >::iterator info = it->second.statusList.begin(); info != it->second.statusList.end(); ++info)
						{
							delete (*info);
						}
					}
				}

				StatusInfoManager::StatusGroup* StatusInfoManager::Retrieve(uint32 hash)
				{
					StatusTable::iterator table = statusTable.find(hash);
					if (table != statusTable.end()) {
						return &(table->second);
					}
					return NULL;
				}

				StatusInfoManager::StatusGroup* StatusInfoManager::Retrieve(const std::wstring& type)
				{
					return Retrieve(StringUtil::Hash(type.c_str()));
				}

				StatusInfo* StatusInfoManager::Retrieve(int level, uint32 hash)
				{
					StatusTable::iterator table = statusTable.find(hash);
					if (table != statusTable.end())
						return ((*table).second).statusList[level - 1];
					return NULL;
				}

				StatusInfo* StatusInfoManager::Retrieve(int level, const std::wstring& type)
				{
					return Retrieve(level, StringUtil::Hash(type.c_str()));
				}

				bool StatusInfoManager::IsExist(uint32 hash)
				{
					return statusTable.find(hash) == statusTable.end() ? false : true;
				}

				bool StatusInfoManager::IsExist(const std::wstring& type)
				{
					return IsExist(StringUtil::Hash(type.c_str()));
				}

				void StatusInfoManager::GetBasicStatList(std::vector<std::wstring>& l)
				{
					BasicStatMap::iterator i = basicStatMap.begin();
					while (i != basicStatMap.end()) {
						l.push_back((*i).second.name);
						++i;
					}
				}

				Constants::CharacterMajorStatType StatusInfoManager::GetMajorStatType(const std::wstring& type)
				{
					StatusTable::iterator table = statusTable.find(StringUtil::Hash(type.c_str()));
					if (table != statusTable.end())
					{
						return table->second.majorStat;
					}

					return Constants::CharacterMajorStatType::NoMajorStat;
				}

				uint32 StatusInfoManager::GetRebirthAdditionalFactor(uint16 currentLevel, uint16 rebirthCount, bool needStoredFactor)
				{
					if (rebirthCount == 0)
						return 0;

					if (needStoredFactor == true)
					{
						int multiplyCount = 1;

						if (currentLevel > 89)
							multiplyCount = 7;
						else if (currentLevel > 79)
							multiplyCount = 6;
						else if (currentLevel > 69)
							multiplyCount = 5;
						else if (currentLevel > 59)
							multiplyCount = 4;
						else if (currentLevel > 39)
							multiplyCount = 3;
						else if (currentLevel > 19)
							multiplyCount = 2;

						return rebirthAddtionalFactors[rebirthCount - 1] * multiplyCount;
					}

					switch (currentLevel)
					{
					case 1:
					case 20:
					case 40:
					case 60:
					case 70:
					case 80:
					case 90:
						return rebirthAddtionalFactors[rebirthCount - 1];
					}

					return 0;
				}

				uint32 StatusInfoManager::GetRebirthAdditionalMajorFactor(uint16 currentLevel, uint16 rebirthCount, bool needStoredFactor)
				{
					if (rebirthCount == 0)
						return 0;

					if (needStoredFactor == true)
					{
						int multiplyCount = 1;

						if (currentLevel > uint16(89))
							multiplyCount = 7;
						else if (currentLevel > uint16(79))
							multiplyCount = 6;
						else if (currentLevel > uint16(69))
							multiplyCount = 5;
						else if (currentLevel > uint16(59))
							multiplyCount = 4;
						else if (currentLevel > uint16(39))
							multiplyCount = 3;
						else if (currentLevel > uint16(19))
							multiplyCount = 2;

						return rebirthAddtionalMajorFactors[rebirthCount - 1] * multiplyCount;
					}

					switch (currentLevel)
					{
					case 1:
					case 20:
					case 40:
					case 60:
					case 70:
					case 80:
					case 90:
						return rebirthAddtionalMajorFactors[rebirthCount - 1];
					}

					return 0;
				}

				uint32 StatusInfoManager::GetRebirthSkillPoint(uint16 oldLevel)
				{
					if (oldLevel >= 99)
						return 5;
					else if (oldLevel >= 90)
						return 4;
					else if (oldLevel >= 80)
						return 3;
					else if (oldLevel >= 70)
						return 2;

					return 0;
				}
				double StatusInfoManager::GetDamagePercent(Constants::DamageType attackerType, Constants::DamageType targetType)
				{
					if (attackerType >= elementTable.size())
						return 1;
					else if (targetType >= elementTable.at(attackerType).size())
						return 1; 

					return (elementTable.at(attackerType)).at(targetType);
				}
				void StatusInfoManager::GetDollStatApplyRate(uint8 rebirthCount, float& Str, float& Dex, float& Vit, float& Int)
				{
					if (rebirthCount == 0)
					{
						Str = 1.0f;
						Dex = 1.0f;
						Vit = 0.5f;
						Int = 1.0f;
					}
					else
					{
						Str = 1.0f;
						Dex = 1.0f;
						Vit = 1.3f;
						Int = 1.0f;
					}

					//Str = 1.0f;
					//Dex = 1.0f;
					//Vit = 1.0f;
					//Int = 1.0f;

					//std::map<uint8/*RebirthCount*/, std::vector<float>/*StatRate*/>::const_iterator iter = dollStatApplyRate.find(rebirthCount);
					//if (iter != dollStatApplyRate.end() )
					//{
					//	Str = iter->second[0];
					//	Dex = iter->second[1];
					//	Vit = iter->second[2];
					//	Int = iter->second[3];
					//}
				}

				void StatusInfoManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::StatusInfoManager");
					out.Write(L"BasicStatMap", basicStatMap);
					out.Write(L"ExpTable", expTable);
					out.Write(L"RebirthAddtionalFactors", rebirthAddtionalFactors);
					out.Write(L"RebirthAddtionalMajorFactors", rebirthAddtionalMajorFactors);
					out.Write(L"ElementTable", elementTable);
					out.Write(L"DollStatApplyRate", dollStatApplyRate);
				}

				void StatusInfoManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::StatusInfoManager");
					in.Read(L"BasicStatMap", basicStatMap);
					in.Read(L"ExpTable", expTable);
					in.Read(L"RebirthAddtionalFactors", rebirthAddtionalFactors);
					in.Read(L"RebirthAddtionalMajorFactors", rebirthAddtionalMajorFactors);
					in.Read(L"ElementTable", elementTable);
					if (elementTable.size() != elementTable.at(elementTable.size() - 1).size())
						Logger::GetInstance().Exception(L"[ElementTable] Row size is not equals to cloumn size");

					in.Read(L"DollStatApplyRate", dollStatApplyRate);

					for (BasicStatMap::iterator i = basicStatMap.begin(); i != basicStatMap.end(); ++i)
						LoadStatusTable(i->second.name, i->second);
				}
				////////////////////////////////////////////////////////////////////////// StatusInfoManager //

			}
		}
	}
}
