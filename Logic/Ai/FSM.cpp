#include "FSM.h"


namespace Lunia { namespace XRated { namespace Logic {
	namespace AI {

		//const float FSM::updateTick = 0.5f;

		FSM::FSM() : actionTimeout(0), actionTimeStamp(0), updateTimeStamp(0)
		{
			newState = NULL;
			dummyState.Set(L"Nothing", this, &FSM::StateNothing,  &FSM::StateNothing,  &FSM::StateNothing);
			currentState = &dummyState;

			updateTick = 0.5f;
		}

		void FSM::ChangeState(IFSMState& newState, float newUpdateTick)
		{
			this->newState = &newState; 
			SetUpdateTick(newUpdateTick);
		}

		void FSM::SetUpdateTick(float newUpdateTick) 
		{ 
			updateTick = newUpdateTick; 
		}

		void FSM::Update(float dt)
		{
			//dummyTimer.GetTimeDelta();

			if (newState) {
				currentState->DestroyState();
				currentState = newState;
				currentState->InitializeState();
				newState = NULL;
			}

			actionTimeStamp += dt;
			updateTimeStamp += dt;
		
			if (updateTimeStamp >= updateTick) {
				updateTimeStamp -= updateTick;

				currentState->Update();
			}
		}

	}
}	}	}