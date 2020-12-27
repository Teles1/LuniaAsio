#include "Wolf.h"

namespace Lunia { namespace XRated {	namespace Logic {

	class WolfBoss : public Wolf {
	protected :
	public :
		WolfBoss();
		virtual ~WolfBoss() {}

		bool Update(float dt, IGameStateDatabase* db);
	public :
		void stateCaution_SightUpdate();
		void stateCaution_HateListUpdate();

		void CooperationBegin();
		void CooperationUpdate();
		void CooperationEnd() {}

	};

}	}	}
