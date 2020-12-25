#pragma once
#include <Info/Info/ItemStageInfo/ItemStageInfo.h>
#include "StageInfoManager.h"
#include <Core/Resource/Xml/XmlParser.h>
#include <Core/Utils/ConfigReader.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				// StageInfoManager ///////////////////////////////////////////////////////////////////////////
				StageInfoManager::StageInfoManager() // load stages
				{
				}

				StageInfoManager::~StageInfoManager() // unload stages
				{
				}

				void StageInfoManager::Init(bool bForceXmlData)
				{
					stages.clear();

					if (bForceXmlData) {//Xml loading
						LoadXmlData();
					}
					else { //Binary loading
						LoadBinaryData();
					}
				}

				void StageInfoManager::LoadXmlData()
				{
					com_ptr<Resource::IStream> stream = Resource::ResourceSystemInstance().OpenStream(L"Database/Stages/StageList.xml");
					int len = stream->GetSize();
					wchar_t* buffer = (wchar_t*)new char[len + 2];
					ZeroMemory(buffer, len + 2);
					stream->Read((unsigned char*)buffer, len);

					Lunia::Xml::Parser parser;
					try {
						parser.Parse(buffer);
					}
					catch (Lunia::Xml::XmlException ex) {
						Logger::GetInstance().Error(L"[StageInfoManager::Init] XML Parsing Error.[%s]", ex.GetErrorString());
						return;
					}

					Lunia::Xml::ElementCollection col;

					parser.FindElements(L"StageList", &col);

					Lunia::Xml::ElementCollection::iterator iter = col.begin();
					//if (col.size() == 0)
					//assert(col.size());

					Lunia::Xml::Element* elem = *col.begin();

					std::wstring configValue;
					bool bPreloadMovemap = (Config::GetInstance().m_PreloadMovemap == true) ? true : false;
					bool bPreloadScript = (Config::GetInstance().m_PreloadScripts == true) ? true : false;

					for (iter = elem->SubElements.begin(); iter != elem->SubElements.end(); ++iter)
						LoadStage((*iter)->Attributes[L"path"].c_str(), bPreloadMovemap, bPreloadScript);

					delete[]buffer;

					LoadPvpItemStageInfo();
				}

				void StageInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/StageInfos.b");
					reader->Read(L"StageInfoManager", *this);

					StageInfoMap::iterator i = stages.begin();
					for (; i != stages.end(); ++i) {
						StageInfo& info = (*i).second;
						StageInfo::MoveMap& movemap = moveMaps[StringUtil::Hash(info.MoveMapName.c_str())];
						movemap.MoveMapName = info.MoveMapName;
						info.MoveMapInfo = &movemap;
					}
				}

				void StageInfoManager::SaveXml() {
					StageInfoMap::iterator i = stages.begin();
					int index;
					for (; i != stages.end(); ++i) {
						StageInfo& info = (*i).second;
						String tempChar = info.ScriptName;
						// Changing .move to .xml
						std::wstring StringMove = L".as";
						std::size_t position = tempChar.find(StringMove);
						if (position < 100) {
							tempChar.replace(position, StringMove.length(), L".xml");
						} 

						Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(tempChar.c_str());
						writer->Write(L"Stage", info);
					}
					std::wcout << L"Stage Done...\n";
				}

				void StageInfoManager::LoadPvpItemStageInfo()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/Stages/PVP/PvpItemStageInfo.xml");
					reader->Read(L"itemStageInfoContainer", itemStageInfoContainer);
				}

				void StageInfoManager::LoadStage(const wchar_t* filename, bool bPreloadMovemap, bool bPreloadScript)
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(filename);
					StageInfo stage;
					reader->Read(L"Stage", stage);
					uint32 hash = StringUtil::Hash(stage.Id.c_str());

					StageInfo& info = stages[hash] = stage;

					StageInfo::MoveMap& movemap = moveMaps[StringUtil::Hash(info.MoveMapName.c_str())];
					movemap.MoveMapName = info.MoveMapName;
					info.MoveMapInfo = &movemap;


					if (bPreloadMovemap)
						info.GetMoveMapInfo();
					if (bPreloadScript)
						info.LoadScript();
				}

				void StageInfoManager::GetStageList(std::vector<std::wstring>& l)
				{
					StageInfoMap::iterator i = stages.begin();
					for (; i != stages.end(); ++i)
						l.push_back((*i).second.Id);
				}

				StageInfo* StageInfoManager::Retrieve(uint32 hash)
				{
					StageInfoMap::iterator i = stages.find(hash);
					if (i == stages.end())
						return NULL;
					return &(*i).second;
				}

				StageInfo* StageInfoManager::Retrieve(const wchar_t* id)
				{
					return Retrieve(StringUtil::Hash(id));
				}

				ItemStageInfo::ItemStageInfoData* StageInfoManager::RetrieveItemStageInfo(uint32 hash)
				{
					return itemStageInfoContainer.Retrieve(hash);
				}

				void StageInfoManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::StageInfoManager");
					out.Write(L"stages", stages);
					out.Write(L"itemStageInfoContainer", itemStageInfoContainer);
				}

				void StageInfoManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::StageInfoManager");
					in.Read(L"stages", stages);
					in.Read(L"itemStageInfoContainer", itemStageInfoContainer, ItemStageInfoContainer());

					StageInfoMap::iterator i = stages.begin();
					for (; i != stages.end(); ++i) {
						StageInfo& info = (*i).second;
						StageInfo::MoveMap& movemap = moveMaps[StringUtil::Hash(info.MoveMapName.c_str())];
						movemap.MoveMapName = info.MoveMapName;
						info.MoveMapInfo = &movemap;
					}
					std::wstring configValue;
					if (Config::GetInstance().m_PreloadMovemap == true) {
						StageInfoMap::iterator i = stages.begin();
						for (; i != stages.end(); ++i)
							(*i).second.GetMoveMapInfo(); //LoadMoveMap( (*i).second );
					}
					if (Config::GetInstance().m_PreloadScripts == true) {
						StageInfoMap::iterator i = stages.begin();
						for (; i != stages.end(); ++i)
							(*i).second.LoadScript();
					}
				}
			}
		}
	}
}