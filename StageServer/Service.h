#pragma once
#include <Core/Serializer/Serializer.h>
#include <Network/Api/Json.hpp>
using nlohmann::json;
namespace Lunia {
	namespace XRated {
		struct StageStates {
			unsigned NumberOfTeams : 4;  // 0 ~ 15
			unsigned IsPrivate : 1;      // 0 ~ 1

			inline operator int() const { return reinterpret_cast<const int&>(*this); }
			inline StageStates& operator=(int rhs) {
				reinterpret_cast<int&>(*this) = rhs;
				return *this;
			}
			inline StageStates() { *this = 0; }
			~StageStates() {}
		};
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