#include "../../NonPlayer.h"

namespace Lunia { namespace XRated {	namespace Logic {

	class Wolf : public NonPlayer {
	protected :
		float cautionRange;
		float approachRange;
		float attackProbability;
		int waitTick;	//대장의 소환을 받고 공격대기 시간

	public :
		Wolf();
		virtual ~Wolf() {}

		bool HandleTrigger(AI::Trigger& /*trigger*/);

	public :
		void stateApproachTarget_SightUpdate();
		void stateApproachTarget_HateListUpdate();

		void stateCaution_SightBegin();
		virtual void stateCaution_SightUpdate();
		void stateCaution_HateListBegin();
		virtual void stateCaution_HateListUpdate();
		void FightBegin();
		void FightUpdate();

		virtual void CooperationBegin();
		virtual void CooperationUpdate();
		virtual void CooperationEnd() {}

	};

}	}	}
