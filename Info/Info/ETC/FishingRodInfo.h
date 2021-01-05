#pragma once
#include <Core/Serializer/Serializer.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct FishingRodInfo : public Serializer::ISerializable
				{
					const static uint32 DefaultStageHash;

					struct ProbabilityTable : public Serializer::ISerializable
					{
						uint32				ItemHash;
						float				Probability;
						int32				Rare;

						struct Finder
						{
							uint32 Hash;
							Finder(uint32 h) :Hash(h) {}
							bool operator()(const ProbabilityTable& info) const { return info.ItemHash == Hash; }
						};
						bool operator == (const ProbabilityTable& rhs) const
						{
							return (ItemHash == rhs.ItemHash && Probability == rhs.Probability && Rare == rhs.Rare);
						}
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};

					struct RodInfo
					{
						float  SuccessProb;  ///< Success probability
						uint32 Exp;			 ///< Exp. getting amount
						uint32 NumberOfBait;

						std::vector< ProbabilityTable >	ProbabilityTables;	///< Getting-item table]

						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};

					uint32									  Id;					///< Item ID, FisingRod
					std::map< uint32 /*stageHash*/, RodInfo > StageHashIndexRodInfos;
					std::wstring							  ReadFile;

					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);

				};
			}
		}
	}
}