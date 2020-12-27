#include "../NonPlayer.h"

namespace Lunia { namespace XRated {	namespace Logic {

	class NPCChecker : public NonPlayer {
		int actionCnt;
		float actionPlayTime;

	public :
		NPCChecker() : actionCnt(0), actionPlayTime(0) {}

	public :
		void IdleNoneBegin();
		void IdleNoneUpdate();
		void IdleNoneEnd();

		void SetAction( uint32 name );
		bool NextAction();

	};

}	}	}

