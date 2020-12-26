#pragma once
#include "EventInfo.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class EventInfoManager
				{
				public:
					void Load(bool xml);
					void Save(bool xml);

					void GetEventList(std::vector<EventUnit>& eventList);
					uint32 GetEventNPC(uint32 eventID, uint32 oldNPCHash);
				private:
					typedef std::map< uint32 /*oldNPCHash*/, uint32 /*newNPCHash*/ > ChangeNPCTable;
					std::map< int /*nationalCode*/, std::map< uint32 /*eventID*/, ChangeNPCTable > > scriptEventNPCs;
					std::map< int /*nationalCode*/, std::vector<EventUnit> > infos;
				};
			}
		}
	}
}
