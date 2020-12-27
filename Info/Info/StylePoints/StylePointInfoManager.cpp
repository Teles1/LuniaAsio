#pragma once
#include "StylePointInfoManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void StylePointInfoManager::Init(bool bForceXmlData)
				{
					if (bForceXmlData) //Xml loading
						LoadXmlData();
					else //Binary loading
						LoadBinaryData();
				}

				void StylePointInfoManager::LoadXmlData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/StylePoints.xml");
					reader->Read(L"StylePoints", StylePoints);
				}

				void StylePointInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/StylePoints.b");
					reader->Read(L"StylePointInfoManager", *this);
				}
				void StylePointInfoManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::StylePointInfoManager");
					out.Write(L"StylePoints", StylePoints);
				}

				void StylePointInfoManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::StylePointInfoManager");
					in.Read(L"StylePoints", StylePoints);
				}
				int StylePointInfoManager::GetAddStylePoint(const XRated::Constants::StylePointType type, const int attackCount)
				{
					const Info::StylePointInfo::StylePointData* data = &StylePoints.StylePointDatas.at(static_cast<int>(type));

					switch (data->Type)
					{
					case StylePointInfo::CalculationType::Add:
						return data->Point + (data->SubPoint * attackCount);
					case StylePointInfo::CalculationType::Multiplication:
						return data->Point + (data->SubPoint * attackCount * attackCount);
					}
					return 0;
				}
				void StylePointInfoManager::SetStylePoint(const XRated::Constants::StylePointType type, const int point)
				{
					Info::StylePointInfo::StylePointData* data = &StylePoints.StylePointDatas.at(static_cast<int>(type));
					data->Point = point;
				}
			}
		}
	}
}
