#pragma once

#include <Core/GameConstants.h>


namespace Lunia { namespace XRated { namespace Logic { namespace AI {

	typedef Lunia::XRated::Serial TriggerID;

	struct Trigger {
		enum TriggerType {
			//ENEMY_APPROACH = 1,
			GOT_HATE = 1 << 0,
			NEED_HEALING = 1 << 1,
			NEED_HELP = 1 << 2,
			NEED_COOPERATION = 1 << 3,
			CAST_MAGIC = 1 << 4,
			DESTINATION_ARRIVED = 1 << 5,
			BOSS_DEAD = 1 << 7,
		};

		TriggerType type;
		TriggerID id;
		Serial creator;
		Constants::ObjectType objectType;
		float3 position;
		float radius;
		float timeStamp;
		float timeLimit;
		bool bUpdatePos;
		bool bExpired;


	public :
		Trigger(Trigger::TriggerType triggerType, TriggerID triggerId, Serial c, Constants::ObjectType objType, float3 pos, float rad, float limit, bool bUpdate)
					: type(triggerType), id(triggerId), creator(c), objectType(objType), position(pos), radius(rad),
								timeLimit(limit), bUpdatePos(bUpdate), timeStamp(0), bExpired(false) {}
	};

}	}	}	}
