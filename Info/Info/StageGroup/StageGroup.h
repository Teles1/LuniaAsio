#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>
#include <Info/Info/StageInfo/StageInfo.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct StageGroup : public Serializer::ISerializable
				{
					std::wstring Name;
					
					std::vector<uint32> StageHashes;
					uint16 InitailLife;
					bool SharedLife; 
					uint16 Capacity; 
					uint16 MaxPlayerStore;
					float AirDamage;
					float AirDamageByNpc;
					bool Looping; 
					Constants::GameTypes GameType;
					Constants::GhostRules::Type GhostRule;
					bool UseItem; 
					bool bQuickJoin;
					bool HasDifficulty; 

					std::vector<StageInfo*> Stages; ///< by StageHashes
					std::vector<DifficultyInfo> Difficulties;
					uint32 Hash; ///< by Name
					int EpisodeInterval;
					bool AquireNextStageLicense;

					bool GuildStage; 

					uint32 GetDropCount(uint8 difficulty, float randomFloat);

				public:/* lSerializable implements */
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}