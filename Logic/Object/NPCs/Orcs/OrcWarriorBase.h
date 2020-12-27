#include "../../NonPlayer.h"


namespace Lunia { namespace XRated {	namespace Logic {

	class OrcWarriorBase : public NonPlayer {
	protected :
		float minimumRange;
	public :
		OrcWarriorBase();
		virtual ~OrcWarriorBase() {}

		Lunia::XRated::Serial RecalculateMostHateTarget();
	public :
		//AI Implementation
		void stateApproachTarget_SightUpdate();
		void stateApproachTarget_HateListUpdate();

		void stateCaution_HateListUpdate();
		void stateCaution_SightUpdate();

		void FightUpdate();
	};

}	}	}
