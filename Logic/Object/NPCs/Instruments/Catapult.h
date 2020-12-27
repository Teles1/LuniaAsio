#include "../../NonPlayer.h"

namespace Lunia { namespace XRated {	namespace Logic {

	class Catapult : public NonPlayer {
	protected :
		float minimumDistance;

	public :
		Catapult();
		virtual ~Catapult() {}

		void Initialize(Lunia::Math::RandomInt& rInt);
		bool DoDamage(Object* who, uint32 creatorActionHash, float minDmg, float maxDmg, float damage, Constants::DamageType dmgType, uint32 stateID, Constants::AttackStateType attackType, unsigned int& sFlag);
		bool Update(float dt, IGameStateDatabase* db);

	public :
		bool HandleTrigger(AI::Trigger& trigger);

		//AI Implementation
		void stateApproachTarget_SightUpdate();
		void stateApproachTarget_HateListUpdate();

		void stateCaution_HateListUpdate();
		void stateCaution_SightUpdate();

		void FightUpdate();

		void stateRetreat_Begin();
		void stateRetreat_Update();
		void stateRetreat_End();

		void EndOperationBegin();

	};

}	}	}
