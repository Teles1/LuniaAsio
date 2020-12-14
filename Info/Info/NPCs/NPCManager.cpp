#pragma once
#include "NPCManager.h"

namespace Lunia{
	namespace XRated {
		namespace Database {
			namespace Info {
				void NPCInfoManager::BasicResist::LoadBasicResist()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/Status/BasicResist.xml");
					reader->Read(L"MONSTER", resist[0]);
					reader->Read(L"ANIMAL", resist[1]);
					reader->Read(L"HUMAN", resist[2]);
					reader->Read(L"DEMON", resist[3]);
					reader->Read(L"UNDEAD", resist[4]);
					reader->Read(L"PLANT", resist[5]);
					reader->Read(L"GIANT", resist[6]);
					reader->Read(L"FAIRY", resist[7]);
					reader->Read(L"DRAGON", resist[8]);
					reader->Read(L"INSTRUMENT", resist[9]);
					reader->Read(L"SUMMONED", resist[10]);
				}

				void NPCInfoManager::BasicResist::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"BinaryBufferXRated::Database::Info::NPCInfoManager::BasicResist");
					out.Write(L"MONSTER", resist[0]);
					out.Write(L"ANIMAL", resist[1]);
					out.Write(L"HUMAN", resist[2]);
					out.Write(L"DEMON", resist[3]);
					out.Write(L"UNDEAD", resist[4]);
					out.Write(L"PLANT", resist[5]);
					out.Write(L"GIANT", resist[6]);
					out.Write(L"FAIRY", resist[7]);
					out.Write(L"DRAGON", resist[8]);
					out.Write(L"INSTRUMENT", resist[9]);
					out.Write(L"SUMMONED", resist[10]);
				}

				void NPCInfoManager::BasicResist::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"BinaryBufferXRated::Database::Info::NPCInfoManager::BasicResist");
					in.Read(L"MONSTER", resist[0]);
					in.Read(L"ANIMAL", resist[1]);
					in.Read(L"HUMAN", resist[2]);
					in.Read(L"UNDEAD", resist[3]);
					in.Read(L"DEMON", resist[4]);
					in.Read(L"PLANT", resist[5]);
					in.Read(L"GIANT", resist[6]);
					in.Read(L"FAIRY", resist[7]);
					in.Read(L"DRAGON", resist[8]);
					in.Read(L"INSTRUMENT", resist[9]);
					in.Read(L"SUMMONED", resist[10]);
				}

				NPCInfoManager::NPCInfoManager()
				{
				}

				void NPCInfoManager::LoadNPCInfo(const wchar_t* xml)
				{

					NPCInfoMap temp;

					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(xml);
					reader->Read(L"Npcs", temp);

					for (NPCInfoMap::const_iterator i = temp.begin(); i != temp.end(); ++i)
						i->second.CheckDropItemDuplication();

					Npcs.insert(temp.begin(), temp.end());
				}

				void NPCInfoManager::Init(bool bForceXmlData)
				{
					Npcs.clear();

					if (bForceXmlData) {//Xml loading
						LoadXmlData();
					}
					else { //Binary loading
						std::wstring fullLoading;
						NPCInfoManager::LoadBinaryData();
					}
				}

				void NPCInfoManager::Add(const NonPlayerInfo& info)
				{
					Npcs[StringUtil::Hash(info.Name.c_str())] = info;
				}

				void NPCInfoManager::LoadXmlData()
				{
					//Load basic resist table
					basicResist.LoadBasicResist();

					std::vector<std::wstring> NPCListXML;

					Resource::SerializerStreamReader reader =Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/NPC/NPCList.xml");
					reader->Read(L"NPCList", NPCListXML);

					std::vector<std::wstring>::const_iterator itr, end;

					for (itr = NPCListXML.begin(), end = NPCListXML.end(); itr != end; ++itr)
					{
						LoadNPCInfo((*itr).c_str());
					}
				}

				void NPCInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/NPCInfos.b");
					reader->Read(L"NPCInfoManager", *this);
				}

				NonPlayerInfo* NPCInfoManager::Retrieve(uint32 hash)
				{
					NPCInfoMap::iterator i = Npcs.find(hash);
					if (i == Npcs.end()) return NULL;
					return &i->second;
				}

				NonPlayerInfo* NPCInfoManager::Retrieve(const wchar_t* id)
				{
					return Retrieve(StringUtil::Hash(id));
				}

				NPCInfoManager::BasicResist::Resist* NPCInfoManager::Retrieve(NonPlayerInfo::Races race)
				{
					if (race < 0 || race > 10) return NULL;
					return &basicResist.resist[race];
				}

				void NPCInfoManager::GetNPCList(std::vector<std::wstring>& l) const
				{
					NPCInfoMap::const_iterator i = Npcs.begin(), end = Npcs.end();
					while (i != end) {
						l.push_back((*i).second.Name);
						++i;
					}
				}

				void NPCInfoManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"BinaryBufferXRated::Database::Info::NPCInfoManager");
					out.Write(L"Npcs", Npcs);
					out.Write(L"basicResist", basicResist);
				}

				void NPCInfoManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"BinaryBufferXRated::Database::Info::NPCInfoManager");
					in.Read(L"Npcs", Npcs);
					in.Read(L"basicResist", basicResist);
				}
			}
		}
	}
}