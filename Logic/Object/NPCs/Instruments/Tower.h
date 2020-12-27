#include "../../NonPlayer.h"

namespace Lunia { namespace XRated {	namespace Logic {

	class Tower : public NonPlayer {
	public :
		Tower();
		virtual ~Tower() {}

		void Initialize(Lunia::Math::RandomInt& rInt);
		bool DoDamage(Object* who, uint32 creatorActionHash, float minDmg, float maxDmg, float damage, Constants::DamageType dmgType, uint32 stateID, Constants::AttackStateType attackType, unsigned int& sFlag);
		bool Update(float dt, IGameStateDatabase* db);

		void Taunted (Lunia::XRated::Serial, float) {}

	public :
		//AI Implementation
		void IdleNoneBegin();
		void IdleNoneUpdate();
		void WarningBegin();
		void stateApproachTarget_SightUpdate();
		void stateCaution_SightUpdate();
		void FightUpdate();
		void EndOperationBegin();
	};

}	}	}
