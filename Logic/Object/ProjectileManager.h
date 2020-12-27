#pragma once
#include "Projectile.h"

namespace Lunia { namespace XRated {	namespace Logic {

	class ProjectileManager {
	public :
		typedef std::map<Serial, Projectile*> ProjectileMap;

	protected :
		ProjectileMap projectiles;

		struct BufferedProjectile {
			float delayedTime;
			float3 position;
			std::wstring ownerName;
			Projectile* projectile;

			BufferedProjectile(float dt, const float3& pos, const std::wstring& name, Projectile* p) : delayedTime(dt), position(pos), ownerName(name), projectile(p) {}
		};

		std::vector<BufferedProjectile> bufferedProjectiles;

		void CopyState(Object* from, Object* to);

		Projectile* CreateStraightProjectile(Serial serial, float2 pos, uint16 fireAngle, uint16 type, Object* obj, float min, float max, bool bToMyTarget, Database::Info::ProjectileInfo *p, IGameStateDatabase& db, float createTime, Lunia::XRated::Serial creator);
		Projectile* CreateFollowProjectile(Serial serial, float2 pos, uint16 fireAngle, uint16 type, Object* obj, float min, float max, bool bToMyTarget, Database::Info::ProjectileInfo *p, IGameStateDatabase& db, float createTime, Lunia::XRated::Serial creator);
		Projectile* CreateHealProjectile(Serial serial, float2 pos, uint16 fireAngle, uint16 type, Object* obj, float min, float max, bool bToMyTarget, Database::Info::ProjectileInfo *p, IGameStateDatabase& db, float createTime, Lunia::XRated::Serial creator);
		Projectile* CreateHighAngleProjectile(Serial serial, float2 pos, uint16 fireAngle, Object* obj, float min, float max, Database::Info::ProjectileInfo *p, IGameStateDatabase& db, float createTime, Lunia::XRated::Serial creator);
		Projectile* CreateFallProjectile(Serial serial, float2 pos, uint16 fireAngle, uint16 type, Object* obj, float min, float max, bool bToMyTarget, Database::Info::ProjectileInfo *p, IGameStateDatabase&, float createTime, Lunia::XRated::Serial creator);
		Projectile* CreateFloatingProjectile(Serial serial, float2 pos, uint16 fireAngle, Object* owner, float min, float max, Database::Info::ProjectileInfo *p, IGameStateDatabase& db, float createTime, Lunia::XRated::Serial creator);
		Projectile* CreateCircularProjectile(Serial serial, const float2& pos, uint16 fireAngle, uint16 type, Object* owner, float min, float max, uint16 toMyTarget, Database::Info::ProjectileInfo *p, IGameStateDatabase& db, float createTime, Lunia::XRated::Serial creator);

	public :
		ProjectileManager();
		~ProjectileManager();

		void Init();
		int GetCnt() const { return (int)projectiles.size(); }
		void GetList(std::vector<ObjectData*>& l);
		Projectile* Retrieve(Serial serial);
		//지금은 다른팀 밖에 검색할 수 없다.
		void GetAreaObject(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Projectile*>& l);	//원형


		void CreateProjectile(uint32 name, Serial serial, float2 pos, uint16 fireAngle, uint16 type, Object* obj, float min, float max, uint16 toMyTarget, IGameStateDatabase& db, float createTime, Lunia::XRated::Serial creator);
		void Update(float dt, IGameStateDatabase& db);

	};

}	}	}

