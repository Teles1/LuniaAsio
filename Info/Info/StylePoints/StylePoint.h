#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct StylePointInfo : public Serializer::ISerializable {
					enum CalculationType
					{
						Add = 0, 
						Multiplication 
					};
					struct StylePointData : public Serializer::ISerializable {
						int Point;
						int SubPoint; 
						CalculationType Type;

						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
					typedef std::vector<StylePointData> StylePointDataList;

					StylePointDataList StylePointDatas;
					int StylePointGaugeMaxScore; 
					int StylePointGaugeStartScore; 
					int StylePointGaugeMinusScore; 
					float StylePointGaugeMinusTimeInSec; 
					float StyleProintStartProcessGab; 

					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}
