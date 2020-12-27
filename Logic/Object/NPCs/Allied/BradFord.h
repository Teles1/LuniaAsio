#include "../../NonPlayer.h"

namespace Lunia { namespace XRated {	namespace Logic {

	class BradFord : public NonPlayer {

	public :
		BradFord();
		virtual ~BradFord() {}

		//void Initialize(const wchar_t* name);
		//virtual bool Update(float dt, IGameStateDatabase* db);

	public :
		//virtual bool HandleTrigger(AI::Trigger& trigger);

		////AI Implementation
		void IdleNoneBegin();
		void IdleNoneUpdate();
		void IdleNoneEnd();

		void IdlePathFindingBegin();
		void IdlePathFindingUpdate();
		void IdlePathFindingEnd();
	};

}	}	}
