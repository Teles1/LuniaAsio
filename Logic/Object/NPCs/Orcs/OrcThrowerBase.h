#include "../../NonPlayer.h"


namespace Lunia { namespace XRated {	namespace Logic {

	class OrcThrowerBase : public NonPlayer {
		int stepBackCnt;
	public :
		OrcThrowerBase();
		virtual ~OrcThrowerBase();

		Lunia::XRated::Serial RecalculateMostHateTarget();

	public :
		//AI Implementation
		void stateApproachTarget_SightUpdate();
		void stateApproachTarget_HateListUpdate();

		void stateCaution_HateListBegin() {}
		void stateCaution_HateListUpdate();
		void stateCaution_HateListEnd() {}

		void stateCaution_SightBegin() {}
		void stateCaution_SightUpdate();
		void stateCaution_SightEnd() {}

		void FightBegin();
		void FightUpdate();
	};

}	}	}
