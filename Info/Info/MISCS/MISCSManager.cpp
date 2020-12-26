#pragma once
#include "MISCSManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				MISCInfoManager::MISCInfoManager()
				{
				}

				void MISCInfoManager::Init(bool bForceXmlData)
				{
					MISCs.clear();

					if (bForceXmlData) {//Xml loading
						LoadXmlData();
					}
					else { //Binary loading
						std::wstring fullLoading;
						LoadBinaryData();
					}
				}

				void MISCInfoManager::LoadXmlData()
				{
					com_ptr<Resource::IStream> stream = Resource::ResourceSystemInstance().OpenStream(L"Database/MISC/MISCList.xml");
					int len = stream->GetSize();
					wchar_t* buffer = (wchar_t*)new char[len + 2];
					ZeroMemory(buffer, len + 2);
					stream->Read((unsigned char*)buffer, len);

					Lunia::Xml::Parser parser;
					try {
						parser.Parse(buffer);
					}
					catch (Lunia::Xml::XmlException ex) {
						Logger::GetInstance().Warn(L"[MISCInfoManager::Init] XML Parsing Error.[{0}]", ex.GetErrorString());
						return;
					}

					Lunia::Xml::ElementCollection col;
					parser.FindElements(L"MISCs", &col);
					Lunia::Xml::ElementCollection::iterator iter = col.begin();
					//assert(col.size());
					Lunia::Xml::Element* elem = *col.begin();

					for (iter = elem->SubElements.begin(); iter != elem->SubElements.end(); ++iter)
						LoadMISCInfo((*iter)->Attributes[L"path"].c_str());

					delete[]buffer;
				}

				void MISCInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/MISCInfos.b");
					reader->Read(L"MISCInfoManager", *this);
				}

				void MISCInfoManager::LoadMISCInfo(const wchar_t* xml)
				{
					MISCMap temp;
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(xml);
					reader->Read(L"MISCs", temp);

					MISCs.insert(temp.begin(), temp.end());
				}

				MiscInfo* MISCInfoManager::Retrieve(uint32 hash)
				{
					MISCMap::iterator i = MISCs.find(hash);
					if (i == MISCs.end()) return NULL;
					return &i->second;
				}

				MiscInfo* MISCInfoManager::Retrieve(const wchar_t* id)
				{
					return Retrieve(StringUtil::Hash(id));
				}

				void MISCInfoManager::GetMISCList(std::vector<std::wstring>& l)
				{
					MISCMap::iterator i = MISCs.begin(), end = MISCs.end();
					while (i != end) {
						l.push_back((*i).second.Name);
						++i;
					}
				}
				void MISCInfoManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::MISCInfoManager");
					out.Write(L"MISCs", MISCs);
				}
				void MISCInfoManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::MISCInfoManager");
					in.Read(L"MISCs", MISCs);
				}
			}
		}
	}
}
