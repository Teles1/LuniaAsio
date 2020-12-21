#pragma once
#include "StageGroup.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void StageGroup::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::StageGroup");
					out.Write(L"Name", Name);
					out.Write(L"StageHashes", StageHashes);
					out.Write(L"Difficulties", Difficulties);
					out.Write(L"InitailLife", InitailLife);
					out.Write(L"SharedLife", SharedLife);
					out.Write(L"Capacity", Capacity);
					out.Write(L"MaxPlayerStore", MaxPlayerStore);
					out.Write(L"AirDamage", AirDamage);
					out.Write(L"AirDamageByNpc", AirDamageByNpc);
					out.Write(L"Looping", Looping);
					out.Write(L"GameType", (int)GameType);
					out.Write(L"GhostRule", (int)GhostRule);
					out.Write(L"UseItem", UseItem);
					out.Write(L"bQuickJoin", bQuickJoin);
					out.Write(L"HasDifficulty", HasDifficulty);
					out.Write(L"EpisodeInterval", EpisodeInterval);
					out.Write(L"AquireNextStageLicense", AquireNextStageLicense);
					out.Write(L"GuildStage", GuildStage);

				}

				void StageGroup::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::StageGroup");
					in.Read(L"Name", Name);
					in.Read(L"StageHashes", StageHashes);
					in.Read(L"Difficulties", Difficulties, std::vector<DifficultyInfo>());
					in.Read(L"InitailLife", InitailLife);
					in.Read(L"SharedLife", SharedLife);
					in.Read(L"Capacity", Capacity);
					in.Read(L"MaxPlayerStore", MaxPlayerStore, static_cast<uint16>(0));
					in.Read(L"AirDamage", AirDamage, 1.0f);
					in.Read(L"AirDamageByNpc", AirDamageByNpc, 1.0f);
					in.Read(L"Looping", Looping);
					int type;
					in.Read(L"GameType", type, 1); // Default value is StoryGameType
					GameType = (Constants::GameTypes)type;
					in.Read(L"GhostRule", type, 0);
					GhostRule = (Constants::GhostRules::Type)type;
					in.Read(L"UseItem", UseItem, false);
					in.Read(L"bQuickJoin", bQuickJoin, false);
					in.Read(L"HasDifficulty", HasDifficulty, false);
					in.Read(L"EpisodeInterval", EpisodeInterval, 0);
					in.Read(L"AquireNextStageLicense", AquireNextStageLicense, true);
					in.Read(L"GuildStage", GuildStage, false);

					/* internals */
					Hash = StringUtil::Hash(Name.c_str());
				}

				uint32 StageGroup::GetDropCount(uint8 difficulty, float randomFloat)
				{
					if (!HasDifficulty) return 1;

					uint32 dropCount = 1;

					if (Difficulties.size() > difficulty)
					{
						float dropFactor = Difficulties[difficulty].DropFactor;
						dropCount = static_cast<uint32>(dropFactor);
						dropFactor -= dropCount;
						if (dropFactor > 0.0f)
						{
							if (dropFactor > randomFloat)
							{
								++dropCount;
							}
						}
					}

					return dropCount;
				}
			}
		}
	}
}