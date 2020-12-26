#pragma once
#include "EventNonPlayerItemList.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class EventNonPlayerItemList : public Serializer::ISerializable
				{
				protected:
					struct NpcDropEvent : public Serializer::ISerializable
					{
					public:
						bool EventEnabled;
						NpcDropEventItems EventItems;

						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);
					};

					std::map<uint32/*EventID*/, NpcDropEvent> Items;

				public:
					void Init();
					void LoadXmlData();

					const NonPlayerInfo::Item* GetEventItem(float prob, uint32 nonPlayerHash, const XRated::StageLocation& stageLicense);
					void EnableEvent(uint32 eventID, bool enable);

				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}