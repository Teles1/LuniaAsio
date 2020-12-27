#pragma once
#include "GeneralInfoManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				GeneralInfoManager::GeneralInfoManager()
				{
				}

				GeneralInfoManager::~GeneralInfoManager()
				{
				}

				void GeneralInfoManager::Init(bool bForceXmlData)
				{
					if (bForceXmlData) {//Xml loading
						LoadXmlData();
					}
					else { //Binary loading
						LoadBinaryData();
					}
				}

				void GeneralInfoManager::LoadXmlData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/GeneralInfos.xml");
					reader->Read(L"GeneralInfos", GeneralInfos);
				}

				void GeneralInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/GeneralInfos.b");
					reader->Read(L"GeneralInfoManager", *this);
				}

				void GeneralInfoManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::GeneralInfoManager");
					out.Write(L"GeneralInfos", GeneralInfos);
				}

				void GeneralInfoManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::GeneralInfoManager");
					in.Read(L"GeneralInfos", GeneralInfos);
				}
			}
		}
	}
}
