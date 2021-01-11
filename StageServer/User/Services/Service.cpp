#include <StageServer/Service.h>
#include <Core/Serializer/Serializer.h>
namespace Lunia {
	namespace XRated {
		namespace Service {
			void SquareInfo::Serialize(Serializer::IStreamWriter& out) const
			{
				out.Begin(L"AllM::XRated::Service::Stage::SquareInfo");
				out.Write(L"squareName", squareName);
				out.Write(L"stageGroupHash", stageGroupHash);
				out.Write(L"accessLevel", accessLevel);
				out.Write(L"orderNumber", orderNumber);
				out.Write(L"maxPlayerStore", maxPlayerStore);
				out.Write(L"maxFishing", maxFishing);
				out.Write(L"maxUserCnt", maxUserCnt);
				out.Write(L"slimeRacingIntervalInMin", slimeRacingIntervalInMin);
			}


			void SquareInfo::Deserialize(Serializer::IStreamReader& in)
			{
				in.Begin(L"AllM::XRated::Service::Stage::SquareInfo");
				in.Read(L"squareName", squareName);
				in.Read(L"stageGroupHash", stageGroupHash);
				in.Read(L"accessLevel", accessLevel);
				in.Read(L"orderNumber", orderNumber, 0);
				in.Read(L"maxPlayerStore", maxPlayerStore, 30);
				in.Read(L"maxFishing", maxFishing, 20);
				in.Read(L"maxUserCnt", maxUserCnt, 0);
				in.Read(L"slimeRacingIntervalInMin", slimeRacingIntervalInMin, 5);
			}
			void to_json(json& j, const SquareInfo& p)
			{
				j = json{
					{"squareName", p.squareName},
					{"stageGroupHash", p.stageGroupHash},
					{"accessLevel", p.accessLevel},
					{"orderNumber", p.orderNumber},
					{"maxPlayerStore", p.maxPlayerStore},
					{"maxFishing", p.maxFishing},
					{"maxUserCnt", p.maxUserCnt},
					{"slimeRacingIntervalInMin", p.slimeRacingIntervalInMin}
				};
			}
			void from_json(const json& j, SquareInfo& p)
			{
				j.at("squareName").get_to(p.squareName);
				j.at("stageGroupHash").get_to(p.stageGroupHash);
				j.at("accessLevel").get_to(p.accessLevel);
				j.at("orderNumber").get_to(p.orderNumber);
				j.at("maxPlayerStore").get_to(p.maxPlayerStore);
				j.at("maxFishing").get_to(p.maxFishing);
				j.at("maxUserCnt").get_to(p.maxUserCnt);
				if (!j.at("slimeRacingIntervalInMin").is_null())
					j.at("slimeRacingIntervalInMin").get_to(p.slimeRacingIntervalInMin);
				else
					p.slimeRacingIntervalInMin = 1;
			}
		}
	}
}