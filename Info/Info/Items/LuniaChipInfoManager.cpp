#include "LuniaChipInfoManager.h"
#include <Core/Resource/Resource.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void LuniaChipInfoManager::Load(bool xml)
				{
					if (xml == true)
					{
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Gamble/ChipItems.xml");
						reader->Read(L"ChipInfos", infos);
					}
					else
					{
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(L"Database/ChipItems.b");
						reader->Read(L"ChipInfos", infos);
					}
				}
				void LuniaChipInfoManager::Save(bool xml)
				{
					if (xml == true)
					{
						Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/Items/Gamble/ChipItems.xml");
						writer->Write(L"ChipInfos", infos);
					}
					else
					{
						Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(L"Database/ChipItems.b");
						writer->Write(L"ChipInfos", infos);
					}
				}

				void LuniaChipInfoManager::Init(bool xml)
				{
					Load(xml);
				}

				bool LuniaChipInfoManager::IsChip(uint32 hash)
				{
					ChipIter it = infos.begin();
					ChipIter endIt = infos.end();
					for (; it != endIt; ++it)
					{
						if (it->second == hash)
							return true;
					}
					return false;
				}
				int LuniaChipInfoManager::GetChipType(uint32 hash)
				{
					ChipIter it = infos.begin();
					ChipIter endIt = infos.end();
					for (; it != endIt; ++it)
					{
						if (it->second == hash)
							return it->first;
					}
					return 0;
				}

				uint32 LuniaChipInfoManager::GetChipHash(int ChipType)
				{
					ChipIter it = infos.find(ChipType);
					if (it != infos.end())
						return it->second;
					return 0;
				}

			}
		}
	}
}