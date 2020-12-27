#include "../../NonPlayer.h"

namespace Lunia { namespace XRated {	namespace Logic {

	class SlimeBoss : public NonPlayer {
	protected :
		int hitCnt; ///< 필살기시에 힛트수가 일정수가 넘어가면 필살기 실패를 위해서.
		bool bSplitted;
	public :
		SlimeBoss();
		virtual ~SlimeBoss() {}

		virtual bool DoDamage(Object* who, uint32 creatorActionHash, float minDmg, float maxDmg, float damage, Constants::DamageType dmgType, uint32 stateID, Constants::AttackStateType attackType, unsigned int& sFlag);

	public :
		virtual void FightUpdate();
	};

}	}	}
