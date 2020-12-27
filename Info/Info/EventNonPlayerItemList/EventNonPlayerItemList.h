#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>
#include <Info/Info/Actions/Actions.h>
#include <Info/Info/NPCs/NPC.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct NpcDropEventItems : public Serializer::ISerializable
				{
				public:
					std::map<uint32/*NpcHash*/, std::vector<NonPlayerInfo::Item> > NpcItems;  
					std::map<XRated::StageLocation, std::vector<NonPlayerInfo::Item> > StageItems; 
					std::vector<NonPlayerInfo::Item> AnyWhereItems; 

					void ValidEvents();

					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}