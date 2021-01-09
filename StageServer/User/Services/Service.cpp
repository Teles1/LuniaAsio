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
		}
	}
}