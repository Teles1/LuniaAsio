#pragma once
#include "StylePoint.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class StylePointInfoManager : public Serializer::ISerializable {
				protected:
					StylePointInfo StylePoints;
				public:
					void Init(bool bForceXml);
					void LoadXmlData();
					virtual void LoadBinaryData();

					const StylePointInfo& Retrieve() const { return StylePoints; }
					int GetAddStylePoint(const XRated::Constants::StylePointType type, const int attackCount);
					void SetStylePoint(const XRated::Constants::StylePointType type, const int point);
				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}
