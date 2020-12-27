#pragma once

#include "Trigger.h"
#include "../GameState.h"

namespace Lunia { namespace XRated { namespace Logic { namespace AI {

	class TriggerSystem {
	public :
		typedef std::multimap<uint16, Trigger, std::greater<uint16> > TriggerMap;
	

	protected :
		TriggerMap triggerMap;
		TriggerMap triggerBuffer;//trigger�� �ٷ� ��ϵ��� �ʴ´�. ���۸��Ǿ��ٰ� update�ÿ� ��ϵȴ�.
		TriggerID triggerSerial;

	public :
		TriggerSystem();
		virtual ~TriggerSystem();

		void Init();

		TriggerID AddTrigger(uint16 priority, Trigger::TriggerType triggerType, Serial creator, Constants::ObjectType objectType, float3 pos, float rad,
						float timeLimit, bool bUpdate, bool bImmediately);
		void RemoveTrigger(TriggerID serial);

		TriggerMap& GetTriggerList() { return triggerMap; }

		void Update(float dt, IGameStateDatabase* db);
	};

}	}	}	}
