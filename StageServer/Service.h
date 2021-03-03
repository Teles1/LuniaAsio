#pragma once
#include <Core/Serializer/Serializer.h>
#include <Network/Api/Json.hpp>
#include <Core/Service.h>
using nlohmann::json;
namespace Lunia {
	namespace XRated {
		namespace Service {
			struct SquareInfo : public Serializer::ISerializable
			{
				std::string		squareName;
				unsigned int	stageGroupHash;
				int				accessLevel;
				int				orderNumber;
				int				maxPlayerStore;
				int				maxFishing;
				int				maxUserCnt;
				int				slimeRacingIntervalInMin;

				void Serialize(Serializer::IStreamWriter& out) const;
				void Deserialize(Serializer::IStreamReader& in);
				friend void to_json(json& j, const SquareInfo& p);
				friend void from_json(const json& j, SquareInfo& p);
			};
		}
	}
}