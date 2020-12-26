#pragma once
#include "GeneralInfo.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class GeneralInfoManager : public Serializer::ISerializable {
				protected:
					GeneralInfo GeneralInfos;

				public:
					GeneralInfoManager();
					~GeneralInfoManager();

					void Init(bool bForceXml);
					void LoadXmlData();
					virtual void LoadBinaryData();
					const GeneralInfo& Retrieve() const { return GeneralInfos; }

				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};

			}
		}
	}
}
