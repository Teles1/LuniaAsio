#pragma once

#include "Object.h"
#include "../Map/Map.h"
#include <Info/Info.h>

namespace Lunia { namespace XRated {	namespace Logic {

	class Projectile : public Object {
	public :
		enum DamageType {
			TARGET=0, AREA
		};

		enum TargetType {
			ENEMY=0, ALLY, ALL, ALLBUTMYSELF
		};

	protected :
		Lunia::XRated::Serial creatorSerial;
		uint32 actionHash;
		Database::Info::ProjectileInfo* info;
		uint8 team;
		float createdTime;
		Database::Info::ProjectileInfo::ShootingType shootType;
		DamageType dmgType;
		float dmgMin, dmgMax;
		float incFactor;
		Map::StageMap::Height height;

		struct BufferedState {
			float endTime;
			Database::Info::StateInfo::Sort sort;
			Stat::IState* state;
			uint32 id;
			BufferedState(Stat::IState* s, Database::Info::StateInfo::Sort st, float et, uint32 i) : endTime(et), sort(st), state(s), id(i) {}
			void operator= (const BufferedState& bufferedState) {
				endTime = bufferedState.endTime;
				sort = bufferedState.sort;
				state = bufferedState.state;
				id = bufferedState.id;
			}
		};
		typedef std::list<BufferedState> BufferedStateList;
		BufferedStateList bufferedStates;

		//std::vector<Database::Info::ActionInfo::State>* states;

		struct HittedObject {
			Serial serial;
			float duration;

			inline HittedObject(Serial s) : serial(s), duration(0) {}
			inline HittedObject(Serial s, float dur) : serial(s), duration(dur) {}
		};
		std::vector<HittedObject> hitObjects;

		bool bAttackProcessed;
		float attackDelay;

		
		uint32 damageStateTypeUniqueSerial;

	public:
		Projectile(Serial serial, Database::Info::ProjectileInfo* i, float minDmg, float maxDmg, const float3& pos,
					Serial creator, uint32 creatorActionHash, uint8 team, IGameStateDatabase& sd);
		Projectile(Projectile& p);
		inline virtual ~Projectile() {}


		inline const std::wstring& GetName() const { return objectData.Name; }
		inline void SetName(const std::wstring& name) { objectData.Name = name; }
		inline Lunia::XRated::Serial GetCreatorSerial() { return creatorSerial; }
		inline uint32 GetActionHash() { return actionHash; }

		void Destroy() { createdTime = 10000; } 

		virtual void UpdateState(float dt, IGameStateDatabase* db);
		virtual void UpdateHitObject(float dt);
		virtual bool Update(float dt, IGameStateDatabase* db);

		inline uint8 GetTeam() const { return team; }

		inline const Database::Info::ProjectileInfo::ShootingType& GetShootingType() const { return shootType; }
		inline const DamageType& GetDamageType() const { return dmgType; }
		inline void SetDamageType(DamageType type) { dmgType = type; }
		inline const float& GetSpeed() const { return info->Speed; }
		inline void SetDamage(float min, float max) { dmgMax = max; dmgMin = min; }
		inline void GetDamage(float& min, float& max) { max = dmgMax; min = dmgMin; }
		inline virtual Lunia::XRated::Serial GetTarget() const {return 0;} //overrided in ProjectileFollow class
		inline const float3& GetDefinedDirection() const { return PhysicalFunctions::DirectionToDefinedDirection(objectData.Direction); }
		const float3& GetDirection() const;
		void SetIncFactor(float factor) { incFactor = factor; }
		float GetIncFactor() const { return incFactor; }

		void AddBundleToActor(const Database::Info::StateBundleInfo::Condition&, Actor* obj=NULL);
		void ProcessStateBundle(float dt);
		void ProcessStateBundle(Actor* hittedObject);
	private :
		void operator= (Projectile&);
	};

	class ProjectileFollow : public Projectile {
	protected :
		Lunia::XRated::Serial targetSerial;
		float lastUpdatedTick;

	public :
		ProjectileFollow(Serial serial, Database::Info::ProjectileInfo* i, float minDmg, float maxDmg, const float3& pos,
					Serial creator, uint32 creatorActionHash, uint8 team, IGameStateDatabase& sd, Serial target);
		~ProjectileFollow() {}

		Lunia::XRated::Serial GetTarget() const { return targetSerial; }
		bool Update(float dt, IGameStateDatabase* db);
	};

	class ProjectileHeal : public Projectile {
	protected :
		Lunia::XRated::Serial targetSerial;
		float lastUpdatedTick;

	public :
		ProjectileHeal(Serial serial, Database::Info::ProjectileInfo* i, float minDmg, float maxDmg, const float3& pos,
					Serial creator, uint32 creatorActionHash, uint8 team, IGameStateDatabase& sd, Serial target);
		~ProjectileHeal() {}

		Lunia::XRated::Serial GetTarget() const { return targetSerial; }
		bool Update(float dt, IGameStateDatabase* db);
	};

	class ProjectileHighAngle : public Projectile {
		float duration;
	public :
		ProjectileHighAngle(Serial serial, Database::Info::ProjectileInfo* i, float dur, float minDmg, float maxDmg, const float3& pos,
					Serial creator, uint32 creatorActionHash, uint8 team, IGameStateDatabase& sd);

	public :
		bool Update(float dt, IGameStateDatabase* db);

	};

	class ProjectileFall : public Projectile {
	public :
		ProjectileFall(Serial serial, Database::Info::ProjectileInfo* i, float minDmg, float maxDmg
						, const float3& pos, Serial creator, uint32 creatorActionHash, uint8 team, IGameStateDatabase& sd);
	};

	class ProjectileFloating : public Projectile {
		float changeDirectionDelay, lastChangeTime;
	public :
		ProjectileFloating(Serial serial, Database::Info::ProjectileInfo* i, float minDmg, float maxDmg
						, const float3& pos, Serial creator, uint32 creatorActionHash, uint8 team, float delay, IGameStateDatabase& sd);

	public :
		bool Update(float dt, IGameStateDatabase* db);

	};

	class ProjectileCircular : public Projectile {
		Lunia::XRated::Serial targetSerial;
		float3 createdPos;
		float3 centerPos;

	public :
		ProjectileCircular(Serial serial
			, Lunia::XRated::Serial target
			, Database::Info::ProjectileInfo* i
			, float minDmg
			, float maxDmg
			, const float3& pos
			, const float3& createdPosition
			, const float3& centerPosition
			, Serial creator
			, uint32 creatorActionHash
			, uint8 team
			, IGameStateDatabase& sd);

		Lunia::XRated::Serial GetTarget() const {return targetSerial;}
		bool Update(float dt, IGameStateDatabase* db);
	};

}	}	}
