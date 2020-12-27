#pragma once

#include "State.h"
#include "../Object/Player.h"
#include "../GameState.h"

namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Attack : public State<Object> {
	public :
		enum Type { Segment=0, Fanshape, Circle, Rectangle };
		enum AttackDir {
			L=0, F=1, R=2, B=3, I=4, O=5
		};
		enum AttackType { NormalAtk=0, DownAtk };
		//struct AttackTargetType {
		//	const static int Ground = 1;
		//	const static int Stand = 2;
		//	const static int Air = 4;
		//};
		struct BackstabInfo {
			bool bBackstab;
			bool bStun;
			float backstabDmg;
			float stunLength;
			BackstabInfo() : bBackstab(false), bStun(false), backstabDmg(0), stunLength(0) {}
		};

	protected :
		Type type;
		StatConstants::AttackEffectType::Type effectType;
		int targetType;
		
		Constants::DamageType damageType;
		Constants::AllianceType dmgTo;
		float3 direction;
		float3 ownerPosition;
		float attackRange, stunLength;
		AttackDir attackDir;

		float knockBack, airKnockBack;	//맞은 상대방이 밀리는 정도.
		float makeFly, airMakeFly;		//지상에서 공중에 띄우는 높이와 공중에서 다시 띄우는 높이.

		float stopAnimation;		//히트 판정시 애니메이션 스탑되는 시간
		bool bAbsoluteDmg;
		float minimum, maximum;

		float3 startPos;
		float3 spinnedPos[7];

		float heightMax; //공격 판정이 들어가는 높이의 상한선.

		//Backstab
		BackstabInfo backstabInfo;

		AttackType attackType;

		void HitAir(Actor* actor);
		void PhoenixDown(Actor* actor);
		void PhoenixDown2(Actor* actor); //@Oralce 01JAN01 13:49 EST
		void PhoenixDown2Polyfill(Actor* actor); //@Oralce 01JAN01 16:24 EST
		void CenterMove(Actor* actor, Object* owner); //@Oralce 01JAN01 13:49 EST
		void AirStun(Actor* actor);
		void Stun(Actor* actor);
		void Down(Actor* actor);
		void Faint(Actor* actor);
		void StepBack(Actor* actor);
		void StandFaint(Actor* actor);

		bool IsGuard(Actor* actor);

	public :
		Attack(Object* owner, StatConstants::AttackEffectType::Type effectType, int targetType, Constants::DamageType dmgType, Constants::AllianceType dmgTo,
				float3 startPos, float attackRange, bool bAbDmg, float mn, float mx, float knockBack, float airKnockBack,
				float stunLength, AttackDir attackDir, float amf, float mf, float stop, float heightMax, BackstabInfo backstab);
		virtual ~Attack() {}

		Type GetAttackType() { return type; }

		void Initialize(IGameStateDatabase* db);
		void SetDownAtkFlag();
		void Process(std::vector<Object*>&, IGameStateDatabase*);
		virtual bool Do(float, IGameStateDatabase*) { return true; }
	};

	class AttackSegment : public Attack {
	public :
		AttackSegment(Object* owner, StatConstants::AttackEffectType::Type effectType, int targetType, Constants::DamageType dmgType, Constants::AllianceType dmgTo,
				float3 startPos, float attackRange, bool bAbDmg, float mn, float mx, float knockBack, float airKnockBack,
				float stunLength, AttackDir attackDir, float amf, float mf, float stop, float heightMax, BackstabInfo backstab)
			: Attack(owner, effectType, targetType, dmgType, dmgTo, startPos, attackRange, bAbDmg, mn, mx, knockBack, airKnockBack, stunLength, attackDir, amf, mf, stop, heightMax, backstab)
		{ type = Attack::Type::Segment; attackType = Attack::AttackType::NormalAtk; }
		~AttackSegment() {}

		bool Do(float dt, IGameStateDatabase* db);
	};

	class AttackFanshape : public Attack {
	protected :
		float startAngle, endAngle;

	public :
		AttackFanshape(Object* owner, StatConstants::AttackEffectType::Type effectType, int targetType, Constants::DamageType dmgType, Constants::AllianceType dmgTo,
				float3 startPos, float attackRange, bool bAbDmg, float mn, float mx, float knockBack, float airKnockBack,
				float stunLength, AttackDir attackDir, float sa, float ea, float amf, float mf, float stop, float heightMax, BackstabInfo backstab)
			: Attack(owner, effectType, targetType, dmgType, dmgTo, startPos, attackRange, bAbDmg, mn, mx, knockBack, airKnockBack, stunLength, attackDir, amf, mf, stop, heightMax, backstab)
			, startAngle(sa), endAngle(ea) { type = Attack::Type::Fanshape; attackType = Attack::AttackType::NormalAtk; }
		~AttackFanshape() {}

		bool Do(float dt, IGameStateDatabase* db);
	};

	class AttackCircle : public Attack {
	public :
		AttackCircle(Object* owner, StatConstants::AttackEffectType::Type effectType, int targetType, Constants::DamageType dmgType, Constants::AllianceType dmgTo,
				float3 startPos, float attackRange, bool bAbDmg, float mn, float mx, float knockBack, float airKnockBack,
				float stunLength, AttackDir attackDir, float amf, float mf, float stop, float heightMax, BackstabInfo backstab)
			: Attack(owner, effectType, targetType, dmgType, dmgTo, startPos, attackRange, bAbDmg, mn, mx, knockBack, airKnockBack, stunLength, attackDir, amf, mf, stop, heightMax, backstab)
			{ type = Attack::Type::Circle; attackType = Attack::AttackType::NormalAtk; }
		~AttackCircle() {}

		bool Do(float dt, IGameStateDatabase* db);
	};

	class AttackRectangle : public Attack {
	protected :
		float attackWidth;

	public :
		AttackRectangle(Object* owner, StatConstants::AttackEffectType::Type effectType, int targetType, Constants::DamageType dmgType, Constants::AllianceType dmgTo,
				float3 startPos, float attackRange, bool bAbDmg, float mn, float mx, float knockBack, float airKnockBack,
				float stunLength, AttackDir attackDir, float width, float amf, float mf, float stop, float heightMax, BackstabInfo backstab);
		~AttackRectangle() {}

		bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
