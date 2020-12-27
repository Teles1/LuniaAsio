#include "TriggerSystem.h"
#include "../Object/Object.h"


namespace Lunia { namespace XRated { namespace Logic { namespace AI {

	TriggerSystem::TriggerSystem() : triggerSerial(0)
	{
	}

	TriggerSystem::~TriggerSystem()
	{
	}

	void TriggerSystem::Init()
	{
		triggerMap.clear();
		triggerSerial = 0;
	}

	TriggerID TriggerSystem::AddTrigger(uint16 priority, Trigger::TriggerType triggerType, Serial creator, Constants::ObjectType objectType, float3 pos, float rad,
					float timeLimit, bool bUpdate, bool bImmediately)
	{
		if (bImmediately) {
			Trigger t(triggerType, ++triggerSerial, creator, objectType, pos, rad, timeLimit, bUpdate);
			triggerMap.insert(TriggerMap::value_type(priority, t));
		} else {
			triggerBuffer.insert(TriggerMap::value_type(priority, Trigger(triggerType, ++triggerSerial, creator, objectType, pos, rad, timeLimit, bUpdate)));
		}

		return triggerSerial;
	}

	void TriggerSystem::RemoveTrigger(TriggerID serial)
	{
		TriggerMap::iterator i = triggerMap.begin();
		while( i != triggerMap.end() ) {
			if (i->second.id == serial) {
				triggerMap.erase(i);
				return;
			}
			++i;
		}
	}

	void TriggerSystem::Update(float dt, IGameStateDatabase* db)
	{
		if (!triggerBuffer.empty()) {
			TriggerMap::iterator i = triggerBuffer.begin(), end = triggerBuffer.end();
			triggerMap.insert(i, end);
			triggerBuffer.clear();
		}

		Trigger* trigger;

		TriggerMap::iterator i = triggerMap.begin();
		TriggerMap::iterator end = triggerMap.end();
		for (; i != end ;) {
			trigger = &(*i).second;
			trigger->timeStamp += dt;
			if (trigger->bExpired) {
				i = triggerMap.erase(i);
				continue;
			}
			if (trigger->timeLimit && trigger->timeStamp >= trigger->timeLimit) {
				i = triggerMap.erase(i);
				continue;
			}
			if (trigger->bUpdatePos) {
				Object* obj;
				switch(trigger->objectType) {
				case Constants::ObjectType::Player :
					obj = (Object*)db->RetrievePlayer(trigger->creator);
					break;
				case Constants::ObjectType::NonPlayer :
					obj = (Object*)db->RetrieveNPC(trigger->creator);
					break;
				default :
					obj = db->Retrieve(trigger->creator);
					break;
				}
				if (!obj) {
					i = triggerMap.erase(i);
					continue;
				}
				trigger->position = obj->GetPosition();
			}

			++i;
		}
	}

}	}	}	}
