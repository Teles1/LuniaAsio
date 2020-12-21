#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct ItemStageInfo : public Serializer::ISerializable {
					struct DropTable : public Serializer::ISerializable {
						std::wstring Name;
						std::vector< std::vector<uint32> > Table; ///< Item drop table per rank in stage

						uint32 Hash;
					public: /* ISerializable implementation */
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
					struct DropProbability : public Serializer::ISerializable {
						std::wstring Name;
						std::vector< std::vector< std::vector<float> > > Probability; ///< Drop probabilitiy table for stage rank, user count

						uint32 Hash;
					public: /* ISerializable implementation */
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};

					struct ItemStageInfoData {
						uint32 Hash;
						ItemStageInfo* Info;
						DropTable* Table;
						DropProbability* Probability;
						ItemStageInfoData(uint32 h, ItemStageInfo* i, DropTable* t, DropProbability* p) : Hash(h), Info(i), Table(t), Probability(p) {}
					};

				public:
					std::wstring Name;
					std::vector<float3> DropPosition; ///< Item Drop position in stage
					uint32 DropTableHash;
					uint32 DropProbabilityHash;

					uint32 Hash;

				public: /* ISerializable implementation */
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}