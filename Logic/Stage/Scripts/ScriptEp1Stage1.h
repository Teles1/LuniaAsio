#pragma once
#include "../ScriptEngine.h"

namespace AllM { namespace XRated {	namespace Logic {

	class ScriptEp1Stage1 : public ScriptEngine {
	public :
		ScriptEp1Stage1(IStageScript* s) : ScriptEngine(s) {}
		void Update(float dt);

	public ://IStageScript::IStageEventListener implementation
		/* general events */
		void ValueChanged(const std::wstring& key, int32 oldValue, int32& newVlaue);
		void TimerAlarmed(int timer, float alarmAt);

		/* object related */
		void ObjectEntered(Constants::ObjectType type, const std::wstring& objectId, int location, uint8 team);
		void ObjectLeft(const std::wstring& objectId, int location, uint8 team);
		void ObjectCreated(const std::wstring& objectId);
		void ObjectDestroyed(const std::wstring& objectId);
	};

}	}	}