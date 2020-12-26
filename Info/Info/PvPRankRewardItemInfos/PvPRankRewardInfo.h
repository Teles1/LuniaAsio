#ifndef PvpRankRewardInfo_H
#define PvpRankRewardInfo_H

#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/StateBundles/StateBundles.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct PvPRankRewardInfo : public Serializer::ISerializable
				{
					StateBundleInfo::ConditionList StateBundleConditions;

				public:
					PvPRankRewardInfo() { }
					~PvPRankRewardInfo() { }

				public: /* ISerializable implementation */
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}

#endif // !PvpRankRewardInfo_H