#include "EpisodeSquareAdventageManager.h"
#include <Core/Resource/Resource.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void EpisodeSquareAdventageManager::Init(bool bForceXmlData)
				{
					Load(bForceXmlData);
				}
				void EpisodeSquareAdventageManager::Load(bool bForceXmlData)
				{
					if (bForceXmlData == true)
					{
						Resource::SerializerStreamReader reader;

						reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/EpisodeSquareAdventage.xml");
						reader->Read(L"EpisodeSquareAdventageInfos", EpisodeSquareAdventageInfos);
					}
					else
					{
						Resource::SerializerStreamReader reader;

						reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(L"Database/EpisodeSquareAdventage.b");
						reader->Read(L"EpisodeSquareAdventageInfos", EpisodeSquareAdventageInfos);
					}
				}

				void EpisodeSquareAdventageManager::Save(bool bForceXmlData)
				{
					if (bForceXmlData == true)
					{
						Resource::SerializerStreamWriter writer;

						writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/EpisodeSquareAdventage.xml");
						writer->Write(L"EpisodeSquareAdventageInfos", EpisodeSquareAdventageInfos);
					}
					else
					{
						Resource::SerializerStreamWriter writer;

						writer = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(L"Database/EpisodeSquareAdventage.b");
						writer->Write(L"EpisodeSquareAdventageInfos", EpisodeSquareAdventageInfos);
					}
				}

				void EpisodeSquareAdventageManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::LuniaBase::Info::EpisodeSquareAdventageManager");
					out.Write(L"EpisodeSquareAdventageInfos", EpisodeSquareAdventageInfos);
				}

				void EpisodeSquareAdventageManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::LuniaBase::Info::EpisodeSquareAdventageManager");
					in.Read(L"EpisodeSquareAdventageInfos", EpisodeSquareAdventageInfos);
				}

				uint8 EpisodeSquareAdventageManager::GetEpisodeSquareAdventageFlag(std::wstring channelName)
				{
					if (EpisodeSquareAdventageInfos.empty()) return 0;

					EpisodeSquareAdventages::const_iterator iter = EpisodeSquareAdventageInfos.begin();
					while (iter != EpisodeSquareAdventageInfos.end())
					{
						if (iter->second == channelName)
							return iter->first;

						++iter;
					}
					return 0;
				}

				std::wstring EpisodeSquareAdventageManager::GetEpisodeSquareAdventageChannelName(uint8 flag)
				{
					if (EpisodeSquareAdventageInfos.empty()) return 0;

					EpisodeSquareAdventages::const_iterator iter = EpisodeSquareAdventageInfos.find(flag);
					if (iter != EpisodeSquareAdventageInfos.end())
						return iter->second;

					return L"";
				}
			}
		}
	}
}