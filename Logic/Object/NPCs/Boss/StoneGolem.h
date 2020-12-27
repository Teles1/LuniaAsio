#include "../../NonPlayer.h"


namespace Lunia { namespace XRated {	namespace Logic {

	class StoneGolem : public NonPlayer {
	protected :

		//AI implementation
		AI::FSMState<StoneGolem> stateEnemyInSight;
		AI::FSMState<StoneGolem> stateDead;
		AI::FSMState<StoneGolem> stateCrazy;

		int hitCnt; ///< �� �ʻ��ÿ� ��Ʈ���� �������� �Ѿ�� �ʻ�� ���и� ���ؼ�.

	public :
		StoneGolem();
		virtual ~StoneGolem();

		void Initialize(Lunia::Math::RandomInt& rInt);
		bool DoDamage(Object* who, uint32 creatorActionHash, float minDmg, float maxDmg, float damage, Constants::DamageType dmgType, uint32 stateID, Constants::AttackStateType attackType, unsigned int& sFlag);
		Lunia::XRated::Serial RecalculateMostHateTarget();

	public :
		//AI Implementation
		void IdleNoneBegin();
		void IdleNoneEnd();

		void stateApproachTarget_SightBegin();
		void stateApproachTarget_SightUpdate();

		void FightBegin();
		void FightUpdate();

		void CrazyBegin();
		void CrazyUpdate();
		void CrazyEnd();

		void DeadBegin();
		void DeadUpdate();
		void DeadEnd();

		void EndOperation();
	};

}	}	}
