#pragma once
#include "ItemStageInfo.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void ItemStageInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::ItemStageInfo");
					out.Write(L"Name", Name);
					out.Write(L"DropPosition", DropPosition);
					out.Write(L"DropTableHash", DropTableHash);
					out.Write(L"DropProbabilityHash", DropProbabilityHash);
				}

				void ItemStageInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::ItemStageInfo");
					in.Read(L"Name", Name);
					in.Read(L"DropPosition", DropPosition);
					in.Read(L"DropTableHash", DropTableHash);
					in.Read(L"DropProbabilityHash", DropProbabilityHash);

					Hash = StringUtil::Hash(Name.c_str());
				}

				void ItemStageInfo::DropTable::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::ItemStageInfo::DropTable");
					out.Write(L"Name", Name);
					out.Write(L"Table", Table);
				}

				void ItemStageInfo::DropTable::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::ItemStageInfo::DropTable");
					in.Read(L"Name", Name);
					in.Read(L"Table", Table);

					Hash = StringUtil::Hash(Name.c_str());
				}

				void ItemStageInfo::DropProbability::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::ItemStageInfo::DropProbability");
					out.Write(L"Name", Name);
					out.Write(L"Probability", Probability);
				}

				void ItemStageInfo::DropProbability::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::ItemStageInfo::DropProbability");
					in.Read(L"Name", Name);
					in.Read(L"Probability", Probability);

					Hash = StringUtil::Hash(Name.c_str());
				}

			}
		}
	}
}