#pragma once
#include "MISCS.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class MISCInfoManager : public Serializer::ISerializable {
				protected:
					typedef std::map<uint32, MiscInfo, Less<uint32> > MISCMap;

				protected:
					MISCMap MISCs;

				private:
					void LoadMISCInfo(const wchar_t* xml);

				public:
					MISCInfoManager();
					virtual ~MISCInfoManager() {}

					void Init(bool bForceXml);
					void LoadXmlData();
					virtual void LoadBinaryData();

					virtual MiscInfo* Retrieve(const wchar_t* id);
					virtual MiscInfo* Retrieve(uint32 hash);
					void GetMISCList(std::vector<std::wstring>& l);

				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}
