#pragma once

#include "FSMState.h"

namespace Lunia { namespace XRated { namespace Logic {
	namespace AI {

		class FSM {
		protected :
			IFSMState* currentState;
			IFSMState* newState;

			FSMState<FSM> dummyState;

			float updateTimeStamp;
			float updateTick;
			//const static float updateTick;

			float actionTimeout;
			float actionTimeStamp;

		public :
			FSM();
			virtual ~FSM() {}

			virtual void Update(float dt);
			inline bool IsState(IFSMState& state) const {return (currentState == &state);}
			inline const wchar_t* GetName() const { return (currentState ? currentState->GetName() :  NULL); }
            void ChangeState(IFSMState& newState, float newUpdateTick = 0.5f);

			void SetUpdateTick(float newUpdateTick);

			inline virtual void StateNothing() {}
		};

	}
}	}	}
