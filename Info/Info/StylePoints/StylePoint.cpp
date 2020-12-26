#pragma once
#include "StylePoint.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void StylePointInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::StylePointInfo");
					out.Write(L"StylePointGaugeMaxScore", StylePointGaugeMaxScore);
					out.Write(L"StylePointGaugeStartScore", StylePointGaugeStartScore);
					out.Write(L"StylePointGaugeMinusScore", StylePointGaugeMinusScore);
					out.Write(L"StylePointGaugeMinusTimeInSec", StylePointGaugeMinusTimeInSec);
					out.Write(L"StyleProintStartProcessGab", StyleProintStartProcessGab);
					out.Write(L"StylePointDatas", StylePointDatas);
				}
				void StylePointInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::StylePointInfo");
					in.Read(L"StylePointGaugeMaxScore", StylePointGaugeMaxScore, 0);
					in.Read(L"StylePointGaugeStartScore", StylePointGaugeStartScore, 0);
					in.Read(L"StylePointGaugeMinusScore", StylePointGaugeMinusScore, 0);
					in.Read(L"StylePointGaugeMinusTimeInSec", StylePointGaugeMinusTimeInSec, 0.f);
					in.Read(L"StyleProintStartProcessGab", StyleProintStartProcessGab, 0.f);
					in.Read(L"StylePointDatas", StylePointDatas, StylePointDataList());
				}

				void StylePointInfo::StylePointData::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::StylePointInfo::StylePointData");
					out.Write(L"Point", Point);
					out.Write(L"SubPoint", SubPoint);
					out.Write(L"Type", static_cast<int>(Type));
				}

				void StylePointInfo::StylePointData::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::StylePointInfo::StylePointData");
					in.Read(L"Point", Point);
					in.Read(L"SubPoint", SubPoint);
					int typeTemp = 0;
					in.Read(L"Type", typeTemp);
					Type = static_cast<CalculationType>(typeTemp);
				}
			}
		}
	}
}
