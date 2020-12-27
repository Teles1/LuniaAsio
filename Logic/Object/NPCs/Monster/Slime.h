#include "../../NonPlayer.h"

namespace Lunia { namespace XRated {	namespace Logic {

	class SlimeBoss : public NonPlayer {
	protected :
		int hitCnt; ///< �ʻ��ÿ� ��Ʈ���� �������� �Ѿ�� �ʻ�� ���и� ���ؼ�.
		bool bSplitted;
	public :
		SlimeBoss();
		virtual ~SlimeBoss() {}

		virtual bool DoDamage(Object* who, uint32 creatorActionHash, float minDmg, float maxDmg, float damage, Constants::DamageType dmgType, uint32 stateID, Constants::AttackStateType attackType, unsigned int& sFlag);

	public :
		virtual void FightUpdate();
	};

}	}	}
