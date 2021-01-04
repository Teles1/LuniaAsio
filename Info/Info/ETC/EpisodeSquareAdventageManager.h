#pragma once
#include <Core/Serializer/Serializer.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class EpisodeSquareAdventageManager : public Serializer::ISerializable
				{
				public:
					void Init(bool bForceXmlData);
					void Load(bool bForceXmlData);
					void Save(bool bForceXmlData);
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);

					uint8 GetEpisodeSquareAdventageFlag(std::wstring channelName);
					std::wstring GetEpisodeSquareAdventageChannelName(uint8 flag);

				private:
					typedef std::map<uint8, std::wstring> EpisodeSquareAdventages;
					EpisodeSquareAdventages EpisodeSquareAdventageInfos;
				};
			}
		}
	}
}