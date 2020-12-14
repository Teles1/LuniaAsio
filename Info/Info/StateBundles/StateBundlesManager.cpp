#pragma once
#include "StateBundlesManager.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				StateBundleInfoManager::StateBundleInfoManager()
				{
				}

				StateBundleInfoManager::~StateBundleInfoManager()
				{
				}

				void StateBundleInfoManager::Init(bool bForceXmlData)
				{
					stateBundles.clear();

					if (bForceXmlData) {//Xml loading
						LoadXmlData();
					}
					else { //Binary loading
						std::wstring fullLoading;
						StateBundleInfoManager::LoadBinaryData();
					}
				}

				void StateBundleInfoManager::LoadStateBundleInfo(const wchar_t* xml)
				{
					StateBundles temp;
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(xml);
					reader->Read(L"stateBundles", temp);

					stateBundles.insert(temp.begin(), temp.end());

				}

				void StateBundleInfoManager::LoadXmlData()
				{
					std::vector<std::wstring> stateBundleList;

					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/StateBundles/StateBundleList.xml");
					reader->Read(L"StateBundleList", stateBundleList);

					std::vector<std::wstring>::iterator itr, end;
					for (itr = stateBundleList.begin(), end = stateBundleList.end(); itr != end; ++itr)
						LoadStateBundleInfo((*itr).c_str());
				}

				void StateBundleInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/StateBundleInfos.b");
					reader->Read(L"StateBundleInfoManager", *this);
				}

				StateBundleInfo* StateBundleInfoManager::Retrieve(uint32 hash)
				{
					StateBundles::iterator i = stateBundles.find(hash);
					if (i != stateBundles.end())
						return &(*i).second;
					return NULL;
				}

				StateBundleInfo* StateBundleInfoManager::Retrieve(const std::wstring& name)
				{
					return Retrieve(StringUtil::Hash(name.c_str()));
				}

				void StateBundleInfoManager::GetList(std::vector<std::wstring>& l)
				{
					StateBundles::iterator i = stateBundles.begin();
					while (i != stateBundles.end()) {
						l.push_back((*i).second.name);
						++i;
					}
				}

				void StateBundleInfoManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::StateBundleInfoManager");
					out.Write(L"stateBundles", stateBundles);
				}

				void StateBundleInfoManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::StateBundleInfoManager");
					in.Read(L"stateBundles", stateBundles);
				}

			}
		}
	}
}