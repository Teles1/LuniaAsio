#pragma once
#include "StateBundles.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class StateBundleInfoManager : public Serializer::ISerializable
				{
				public:
					typedef std::map<uint32, StateBundleInfo, Less<uint32> > StateBundles;

				protected:
					StateBundles stateBundles;

					void LoadStateBundleInfo(const wchar_t* xml);

				public:
					StateBundleInfoManager();
					~StateBundleInfoManager();

					void Init(bool bForceXml);
					void LoadXmlData();
					virtual void LoadBinaryData();

					virtual StateBundleInfo* Retrieve(uint32 name);
					virtual StateBundleInfo* Retrieve(const std::wstring& name);

					void GetList(std::vector<std::wstring>& l);

				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}
