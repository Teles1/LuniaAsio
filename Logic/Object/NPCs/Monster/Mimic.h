#include "../../NonPlayer.h"

namespace Lunia { namespace XRated {	namespace Logic {

	class Mimic : public NonPlayer {
	protected :

	public :
		Mimic();
		virtual ~Mimic() {}

		void Initialize(Lunia::Math::RandomInt& rInt);
		bool DoDamage(Object* who, uint32 creatorActionHash, float minDmg, float maxDmg, float damage, Constants::DamageType dmgType, uint32 stateID, Constants::AttackStateType attackType, unsigned int& sFlag);
		bool Update(float dt, IGameStateDatabase* db);

	public :
		bool HandleTrigger(AI::Trigger& trigger);

		void Taunted (Lunia::XRated::Serial, float) { return; }

		//AI Implementation
		void IdleNoneBegin();
		void IdleNoneUpdate();
		void IdleNoneEnd();

		//void stateApproachTarget_HateListUpdate();

		//void stateCaution_HateListUpdate();

		void FightUpdate();

	};

}	}	}
