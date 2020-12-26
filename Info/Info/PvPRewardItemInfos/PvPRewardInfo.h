#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct PvPRewardInfo : public Serializer::ISerializable
				{
				public:
					struct RewardItemInfo : public Serializer::ISerializable
					{
					public:
						RewardItemInfo();

						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);

						uint32 Hash;
						uint32 Count;
					};

					PvPRewardInfo();

					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);

					RewardItemInfo WinnerItem;
					RewardItemInfo LoserItem;
				};

			}
		}
	}
}
