#include "FishingRodInfo.h"
#include <Core/Resource/Resource.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				const uint32 FishingRodInfo::DefaultStageHash = 0;
				void FishingRodInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::FishingRodInfo");
					out.Write(L"Id", Id);
					out.Write(L"ReadFile", ReadFile);
					out.Write(L"StageHashIndexRodInfos", StageHashIndexRodInfos);
				}

				void FishingRodInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::FishingRodInfo");
					in.Read(L"Id", Id);
					in.Read(L"ReadFile", ReadFile);
					in.Read(L"StageHashIndexRodInfos", StageHashIndexRodInfos);

					if (StageHashIndexRodInfos.size() == 0)
					{
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(ReadFile.c_str());
						reader->Read(L"StageHashIndexRodInfos", StageHashIndexRodInfos);
					}
				}

				void FishingRodInfo::ProbabilityTable::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::FishingRodInfo::ProbabilityTable");
					out.Write(L"ItemHash", ItemHash);
					out.Write(L"Probability", Probability);
					//out.Write( L"StackedCount", StackedCount );
					out.Write(L"Rare", Rare);
				}

				void FishingRodInfo::ProbabilityTable::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::FishingRodInfo::ProbabilityTable");
					in.Read(L"ItemHash", ItemHash);
					in.Read(L"Probability", Probability);
					//in.Read( L"StackedCount", StackedCount );
					in.Read(L"Rare", Rare);
				}

				void FishingRodInfo::RodInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::FishingRodInfo::RodInfo");
					out.Write(L"SuccessProb", SuccessProb);
					out.Write(L"Exp", Exp);
					out.Write(L"NumberOfBait", NumberOfBait);
					out.Write(L"ProbabilityTables", ProbabilityTables);
				}

				void FishingRodInfo::RodInfo::Deserialize(Serializer::IStreamReader& in) {
					in.Begin(L"AllM::XRated::Database::Info::FishingRodInfo::RodInfo");
					in.Read(L"SuccessProb", SuccessProb);
					in.Read(L"Exp", Exp);
					in.Read(L"NumberOfBait", NumberOfBait);
					in.Read(L"ProbabilityTables", ProbabilityTables);
				}
			}
		}
	}
}
