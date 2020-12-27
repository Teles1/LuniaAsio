#include "ProjectileManager.h"
#include <Core/Utils/StringUtil/GenericConversion.h>
#include "Actor.h"
#include <Info/Info/Database.h>
#include "../GameState.h"
#include "../State/States.h"

#include "../State/State.h"
#include "../State/StateDevineShd.h"

#include "NonPlayer.h"
#include "Player.h"

using namespace Lunia::StringUtil;
using namespace Lunia::XRated::Database;
using namespace Lunia::XRated::Database::Info;
using namespace Lunia::Math;

namespace Lunia { namespace XRated {	namespace Logic {

	ProjectileManager::ProjectileManager()
	{
		bufferedProjectiles.reserve(20);
	}

	ProjectileManager::~ProjectileManager()
	{
		Init();
	}

	void ProjectileManager::Init()
	{
		ProjectileMap::iterator begin = projectiles.begin(), end = projectiles.end();
		for ( ; begin != end ; ++begin)
			delete (*begin).second;
		projectiles.clear();
		std::vector<BufferedProjectile>::iterator i = bufferedProjectiles.begin(), bEnd = bufferedProjectiles.end();
		for (; i != bEnd ; ++i)
			delete (*i).projectile;
		bufferedProjectiles.clear();
	}

	void ProjectileManager::GetList(std::vector<ObjectData*>& l)
	{
		ProjectileMap::iterator begin = projectiles.begin();
		ProjectileMap::iterator end = projectiles.end();
		for ( ; begin != end ; ++begin) l.push_back(&(*begin).second->GetObjectData());
	}

	Projectile* ProjectileManager::Retrieve(Serial serial)
	{
		ProjectileMap::iterator i = projectiles.find(serial);
		if (i != projectiles.end() )
			return projectiles[serial];
		return NULL;
	}

	void ProjectileManager::GetAreaObject(const float3& targetPos, float range, Constants::AllianceType /*by*/, uint8 value, Object* /*object*/, std::vector<Projectile*>& l)
	{
		ProjectileMap::iterator iter = projectiles.begin();
		ProjectileMap::iterator end = projectiles.end();
		for ( ; iter != end ; ++iter) {
			Projectile* p = (*iter).second;
			if ( p->GetTeam() != value && Lunia::Math::Length<float>(targetPos - p->GetPosition()) <= range )
				l.push_back(p);
		}

	}

	void ProjectileManager::CopyState(Object* from, Object* to)
	{
		//if ( from->CheckState(Database::Info::StateInfo::Type::CRIMAGIC) ) {
		//	Stat::IState* state = Stat::CreateState( from->GetState(Database::Info::StateInfo::Type::CRIMAGIC, Database::Info::StateInfo::Sort::ATTACKSTATE), to);
		//	to->AddState( state );
		//}

		if(from->CheckState(Database::Info::StateInfo::INCDMG))
		{
			std::vector<Stat::IState*> stateVector = from->GetStates(Database::Info::StateInfo::INCDMG, Database::Info::StateInfo::Sort::ATTACKSTATE);

			for(std::vector<Stat::IState*>::iterator itr = stateVector.begin(); itr != stateVector.end(); ++itr)
			{
				Stat::IState* state = Stat::CreateState(*itr , to);
				to->AddState(state);
			}
		}

		if(from->CheckState(Database::Info::StateInfo::INCHEALAMOUNT))
		{
			std::vector<Stat::IState*> stateVector = from->GetStates(Database::Info::StateInfo::INCHEALAMOUNT, Database::Info::StateInfo::Sort::DEFENCESTATE);

			for(std::vector<Stat::IState*>::iterator itr = stateVector.begin(); itr != stateVector.end(); ++itr)
			{
				Stat::IState* state = Stat::CreateState(*itr , to);
				to->AddState(state);
			}
		}
	}

	Projectile* ProjectileManager::CreateHighAngleProjectile(Serial serial, float2 pos,
					uint16 /*fireAngle*/, Object* obj, float min, float max, Info::ProjectileInfo *p, IGameStateDatabase& db, float createTime, Lunia::XRated::Serial creator)
	{
		Serial targetSerial = obj->GetTarget();

		if (!targetSerial) {
			return NULL;
		}

		Actor* target = db.RetrieveActor(targetSerial);

		if (!target) {
			//Lunia_INFO(( L"[ProjectileManager::CreateHighAngleProjectile] Unknown object serial : {}.", targetSerial ));
			return NULL;
		}

		ObjectData& data = obj->GetObjectData();
		float3 direction(0,0,0), targetPosition, sourcePosition = data.Position;
		targetPosition = target->GetPosition();

		Projectile* ip;
		float len = Lunia::Math::Length<float>(targetPosition - sourcePosition); // Ÿ�ٰ��� �Ÿ�

		if (len > p->Speed * p->Duration) return NULL; //�����Ÿ� ���̶�� ����

		float3 position(pos.x, 0, pos.y);
		float3 spinnedPos = Lunia::XRated::PhysicalFunctions::RotatePositionByDirection(position, 0, data.Direction);
		position = sourcePosition + spinnedPos;

		if (position.x <= 0 || position.z <= 0)
			return NULL;

		// ������ ���� �÷��̾��� ��� �ش� ���� ����� Action�� �����Ѵ�.
		Player* creatorObject = db.RetrievePlayer(creator);
		uint32 creatorActionHash = 0;
		
		if(creatorObject != NULL)
			creatorActionHash = creatorObject->GetActionNameHash();

		ip = new ProjectileHighAngle(serial, p, Lunia::Math::Length<float>( sourcePosition + spinnedPos - targetPosition ) / p->Speed, min, max
									, position, creator, creatorActionHash, obj->GetTeam(), db);
		ip->SetDirection( float3((targetPosition.x-sourcePosition.x)/len, 0, (targetPosition.z-sourcePosition.z)/len) );

		//������Ÿ���� ���͸� �������ش�.
		{
			float2 pos( position.x, position.z );
			Sector* sector = db.GetSmallSector().GetSector( pos );
			ip->SetSmallSector(sector);
			sector = db.GetBigSector().GetSector( pos );
			ip->SetBigSector(sector);
		}

		CopyState(obj, ip);

		bufferedProjectiles.push_back( BufferedProjectile(createTime, targetPosition, obj->GetName(), ip) );
		//projectiles[serial] = ip;
		//db.ProjectileCreated(ip->GetObjectData(), targetPosition, ip->GetTarget(), obj->GetName());
		return ip;
	}

	Projectile* ProjectileManager::CreateFallProjectile(Serial serial, float2 pos, uint16 fireAngle, uint16 type/*����� ����?*/,
		Object* obj, float min, float max, bool bToMyTarget, Info::ProjectileInfo *p, IGameStateDatabase& db, float createTime, Lunia::XRated::Serial creator)
	{
		ObjectData& data = obj->GetObjectData();
		Projectile* ip;
		float3 targetPosition(0,0,0), sourcePosition = data.Position;

		if (type == 0) {// Ÿ���� ����, 1 �̸� �׳� �߻����� ��������
			Serial targetSerial = 0;
			if (bToMyTarget) { //�ڽ��� hate target���� �ƴϸ� �����ϰ�.
				targetSerial = obj->GetTarget();
				if (!targetSerial)
					return NULL;

				Actor* target = db.RetrieveActor(targetSerial);
				if (!target)
					return NULL;

				if( target->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer && target->CheckState(Database::Info::StateInfo::Type::INVINCIBLE)) 
				{
					targetSerial = 0;
				}
				else 
					targetPosition = target->GetPosition();
			} 
			
			if (targetSerial == 0) {//�������� ����.
				std::vector<Actor*> l;
				db.GetAreaObject(data.Position, data.Direction, p->Speed*p->Duration, 300, 60, p->AllianceType, obj->GetTeam(), obj, l);

				if (!l.empty() ) {
					// TODO : stageDatabase���� ������ ������ ���·� ����Ʈ�� ��� ���� ���ϰ�����.. ������ �մ�� �����.
					// �ӽ� �ڵ�� ������ ����� ����Ʈ���� ������.
					// �̰� �ӽ���?;; �굵 �׷� �ӽ÷� ���ɰ� ���� ���ܵ�..
					Actor* actor;
					for( std::vector<Actor*>::iterator iActor = l.begin(); iActor != l.end() ;) {
						actor = (*iActor);
						if ( actor->CheckState(Database::Info::StateInfo::Type::GHOST) || actor->CheckState(Database::Info::StateInfo::Type::INVISIBLE) ) {
							iActor = l.erase(iActor);
						} 
						//NonPlayer�̰� �����̸� Ÿ������ �ȵǵ��� �Ѵ�.
						else if( actor->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer && (((NonPlayer*)actor)->IsPetType() || actor->CheckState(Database::Info::StateInfo::Type::INVINCIBLE) ) ) 
						{
							iActor = l.erase(iActor);
						} 
						else ++iActor;
					}
				}

				std::vector<Actor*>::iterator i=l.begin();
				if (!l.empty() ) {
					int r = db.GetRandomInt(0, (int)l.size());
					while(r--) ++i;
					targetSerial = (*i)->GetSerial();
					targetPosition = (*i)->GetPosition();
				}
			}
		} else {
			if (bToMyTarget) {//������ ��ġ�� ��������.
				float3 spinnedPos = Lunia::XRated::PhysicalFunctions::RotatePositionByDirection(float3(pos.x, 0, pos.y), fireAngle, data.Direction);
				targetPosition =  sourcePosition + spinnedPos;
			} else {//random position���� ��������. �ڽ��� �߽����� ���������� �� �ȿ� ��ġ�� ���Ѵ�.
				float range = db.GetRandomFloat() * p->Speed * p->Duration;
				int angle = db.GetRandomInt(0, 360);
				float3 spinnedPos = Lunia::XRated::PhysicalFunctions::RotatePosition(float3(0, 0, range), angle);
				targetPosition =  sourcePosition + spinnedPos;
			}
		}

		if (targetPosition.x < 0 || targetPosition.z < 0)
			return NULL;

		// ������ ���� �÷��̾��� ��� �ش� ���� ����� Action�� �����Ѵ�.
		Player* creatorObject = db.RetrievePlayer(creator);
		uint32 creatorActionHash = 0;
		
		if(creatorObject != NULL)
			creatorActionHash = creatorObject->GetActionNameHash();

		ip = new ProjectileFall(serial, p, min, max, targetPosition, creator, creatorActionHash, obj->GetTeam(), db);
		ip->SetDirection( data.Direction );

		//������Ÿ���� ���͸� �������ش�.
		{
			float2 pos( targetPosition.x, targetPosition.z );
			Sector* sector = db.GetSmallSector().GetSector( pos );
			ip->SetSmallSector(sector);
			sector = db.GetBigSector().GetSector( pos );
			ip->SetBigSector(sector);
		}

		CopyState(obj, ip);

		bufferedProjectiles.push_back( BufferedProjectile(createTime, float3(0,0,0), obj->GetName(), ip) );
		//if (createTime > 0) { //�����ð��� ������ �ƴϴ�. ���۸��� ������.
		//	bufferedProjectiles.push_back( BufferedProjectile(createTime, float3(0,0,0), obj->GetName(), ip) );
		//} else {
		//	projectiles[serial] = ip;
		//	db.ProjectileCreated(ip->GetObjectData(), float3(0,0,0), ip->GetTarget(), obj->GetName());
		//}
		return ip;
	}

	Projectile* ProjectileManager::CreateFloatingProjectile(Serial serial, float2 pos, uint16 fireAngle, Object* obj, float min, float max, Database::Info::ProjectileInfo *p, IGameStateDatabase& db, float createTime, Lunia::XRated::Serial creator)
	{
		ObjectData& data = obj->GetObjectData();
		Projectile* ip;

		float3 spinnedPos = Lunia::XRated::PhysicalFunctions::RotatePositionByDirection(float3(pos.x, 0, pos.y), 0, data.Direction);

		float3 position = data.Position + spinnedPos;

		if (position.x <= 0 || position.z <= 0)
			return NULL;

		// ������ ���� �÷��̾��� ��� �ش� ���� ����� Action�� �����Ѵ�.
		Player* creatorObject = db.RetrievePlayer(creator);
		uint32 creatorActionHash = 0;
		
		if(creatorObject != NULL)
			creatorActionHash = creatorObject->GetActionNameHash();

		ip = new ProjectileFloating(serial, p, min, max, position, creator, creatorActionHash, obj->GetTeam(), 2, db);
		ip->SetDirection( Lunia::XRated::PhysicalFunctions::RotatePosition(data.Direction, -fireAngle) );

		//������Ÿ���� ���͸� �������ش�.
		{
			float2 pos( position.x, position.z );
			Sector* sector = db.GetSmallSector().GetSector( pos );
			ip->SetSmallSector(sector);
			sector = db.GetBigSector().GetSector( pos );
			ip->SetBigSector(sector);
		}

		CopyState(obj, ip);


		bufferedProjectiles.push_back( BufferedProjectile(createTime, float3(0,0,0), obj->GetName(), ip) );
		//projectiles[serial] = ip;
		//db.ProjectileCreated(ip->GetObjectData(), float3(0,0,0), 0, obj->GetName());
		return ip;
	}

	Projectile* ProjectileManager::CreateStraightProjectile(Serial serial, float2 pos, uint16 fireAngle, uint16 type,
								Object* obj, float min, float max, bool bToMyTarget, Database::Info::ProjectileInfo *p,
								IGameStateDatabase& db, float createTime, Lunia::XRated::Serial creator)
	{
		ObjectData& data = obj->GetObjectData();

		Projectile* ip;
		float3 targetPosition(0,0,0) , sourcePosition = data.Position;

		Serial targetSerial = 0;
		if (bToMyTarget) 
		{
			targetSerial = obj->GetTarget();
			
			if (targetSerial) 
			{
				Actor* target = db.RetrieveActor(targetSerial);
				if (target)
				{
					if( target->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer && target->CheckState(Database::Info::StateInfo::Type::INVINCIBLE)) 
					{
						targetSerial = 0;
					} 
					else
						targetPosition = target->GetPosition();
				}
				else targetSerial = 0;
			}
		} 
		
		if (targetSerial == 0)
		{
			std::vector<Actor*> l;
			db.GetAreaObject(data.Position, PhysicalFunctions::DirectionToDefinedDirection(data.Direction), p->Speed*p->Duration, 300, 60, p->AllianceType, obj->GetTeam(), obj, l);
			
			if (!l.empty() ) {
				Actor* actor;
				for( std::vector<Actor*>::iterator iActor = l.begin(); iActor != l.end() ;) {
					actor = (*iActor);
					if( actor->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer && actor->CheckState(Database::Info::StateInfo::Type::INVINCIBLE) ) 
					{
						iActor = l.erase(iActor);
					} 
					else ++iActor;
				}
			}

			std::vector<Actor*>::iterator i=l.begin();
			if (!l.empty() ) {
				int r = db.GetRandomInt(0, (int)l.size());
				while(r--)
					++i;
				targetSerial = (*i)->GetSerial();
				targetPosition = (*i)->GetPosition();
			} else
				targetSerial = 0;
		}

		float3 position(pos.x, 0, pos.y);
		float3 spinnedPos = Lunia::XRated::PhysicalFunctions::RotatePositionByDirection(position, 0, data.Direction);
		position = spinnedPos + sourcePosition;

		float3 realPos = sourcePosition+spinnedPos;
		if (realPos.x <= 0 || realPos.z <= 0)
			return NULL;

		// ������ ���� �÷��̾��� ��� �ش� ���� ����� Action�� �����Ѵ�.
		Player* creatorObject = db.RetrievePlayer(creator);
		uint32 creatorActionHash = 0;
		
		if(creatorObject != NULL)
			creatorActionHash = creatorObject->GetActionNameHash();

		ip = new Projectile(serial, p, min, max, sourcePosition+spinnedPos, creator, creatorActionHash, obj->GetTeam(), db);

		if (type) {//type �� 0 �̸� ��븦 ����, 1 �̸� �׳� �߻����� ��������
			ip->SetDirection( Lunia::XRated::PhysicalFunctions::RotatePosition(data.Direction, 360-fireAngle) );
		} else {
			if (targetSerial) {
				targetPosition.x += db.GetRandomFloat()*26 - 13;
				targetPosition.z += db.GetRandomFloat()*26 - 13;
				ip->SetDirection( Math::Normalize(targetPosition - sourcePosition) );	//type �� 0 �̸� ��븦 ����, 1 �̸� �׳� �߻����� ��������
			} else
				ip->SetDirection( data.Direction );
		}

		//������Ÿ���� ���͸� �������ش�.
		{
			float2 pos( position.x, position.z );
			Sector* sector = db.GetSmallSector().GetSector( pos );
			ip->SetSmallSector(sector);
			sector = db.GetBigSector().GetSector( pos );
			ip->SetBigSector(sector);
		}

		CopyState(obj, ip);

		bufferedProjectiles.push_back( BufferedProjectile(createTime, float3(0,0,0), obj->GetName(), ip) );
		//projectiles[serial] = ip;
		//db.ProjectileCreated(ip->GetObjectData(), float3(0,0,0), ip->GetTarget(), obj->GetName());
		return ip;
	}

	Projectile* ProjectileManager::CreateFollowProjectile(Serial serial, float2 pos, uint16 fireAngle, uint16 type,
								Object* obj, float min, float max, bool bToMyTarget, Database::Info::ProjectileInfo *p,
								IGameStateDatabase& db, float createTime, Lunia::XRated::Serial creator)
	{
		ObjectData& data = obj->GetObjectData();

		Projectile* ip;
		float3 targetPosition(0,0,0) , sourcePosition = data.Position;

		Serial targetSerial = 0;
		if (bToMyTarget) 
		{
			targetSerial = obj->GetTarget();
			if (targetSerial) 
			{
				Actor* target = db.RetrieveActor(targetSerial);
				if (target)
				{
					if( target->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer && target->CheckState(Database::Info::StateInfo::Type::INVINCIBLE) ) 
					{
						targetSerial = 0;
					} 
					else 
						targetPosition = target->GetPosition();
				}
				else targetSerial = 0;
			}
		} 
		
		if (targetSerial ==0)
		{
			std::vector<Actor*> l;
			switch ( data.Type ) 
			{
			case Constants::ObjectType::Projectile :
				db.GetAreaObject(data.Position, p->Speed*p->Duration, p->AllianceType, obj->GetTeam(), obj, l);
				break;
			default :
				db.GetAreaObject(data.Position, data.Direction, p->Speed*p->Duration, 300, 60, p->AllianceType, obj->GetTeam(), obj, l);
			};
			if (!l.empty() ) 
			{
				// TODO : stageDatabase���� ������ ������ ���·� ����Ʈ�� ��� ���� ���ϰ�����.. ������ �մ�� �����.
				// �ӽ� �ڵ�� ������ ����� ����Ʈ���� ������.
				// �̰� �ӽ���?;; �굵 �׷� �ӽ÷� ���ɰ� ���� ���ܵ�..
				Actor* actor;
				for( std::vector<Actor*>::iterator iActor = l.begin(); iActor != l.end() ;) {
					actor = (*iActor);
					if ( actor->CheckState(Database::Info::StateInfo::Type::GHOST) || actor->CheckState(Database::Info::StateInfo::Type::INVISIBLE) ) 
					{
						iActor = l.erase(iActor);
					} 
					else if( actor->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer && (((NonPlayer*)actor)->IsPetType() || actor->CheckState(Database::Info::StateInfo::Type::INVINCIBLE) ) )
					{
						//NonPlayer�̰� �����̸� Ÿ������ �ȵǵ��� �Ѵ�.
						iActor = l.erase(iActor);
					} 
					else ++iActor;
				}
				if ( !l.empty() ) {
					std::vector<Actor*>::iterator i=l.begin();
					int r = db.GetRandomInt(0, (int)l.size());
					while(r--) ++i;
					targetSerial = (*i)->GetSerial();
					targetPosition = (*i)->GetPosition();
				}
			} else
				targetSerial = 0;
		}

		float3 position(pos.x, 0, pos.y);
		float3 spinnedPos = Lunia::XRated::PhysicalFunctions::RotatePositionByDirection(position, 0, data.Direction);
		position = spinnedPos + sourcePosition;

		if (position.x <= 0 || position.z <= 0)
			return NULL;

		// ������ ���� �÷��̾��� ��� �ش� ���� ����� Action�� �����Ѵ�.
		Player* creatorObject = db.RetrievePlayer(creator);
		uint32 creatorActionHash = 0;
		
		if(creatorObject != NULL)
			creatorActionHash = creatorObject->GetActionNameHash();

		ip = new ProjectileFollow(serial, p, min, max, position, creator, creatorActionHash, obj->GetTeam(), db, targetSerial);

		if (type) {//type �� 0 �̸� ��븦 ����, 1 �̸� �׳� �߻����� ��������
			float3 sourceDirection = data.Direction;
			float3 spinnedDirection = Lunia::XRated::PhysicalFunctions::RotatePosition(sourceDirection, 360-fireAngle);
			ip->SetDirection( spinnedDirection );
		} else {
			if (targetSerial) {
				targetPosition.x += db.GetRandomFloat()*26 - 13;
				targetPosition.z += db.GetRandomFloat()*26 - 13;
				ip->SetDirection( Math::Normalize(targetPosition - sourcePosition) );	//type �� 0 �̸� ��븦 ����, 1 �̸� �׳� �߻����� ��������
			} else
				ip->SetDirection( data.Direction );
		}

		//������Ÿ���� ���͸� �������ش�.
		{
			float2 pos( position.x, position.z );
			Sector* sector = db.GetSmallSector().GetSector( pos );
			ip->SetSmallSector(sector);
			sector = db.GetBigSector().GetSector( pos );
			ip->SetBigSector(sector);
		}

		CopyState(obj, ip);

		bufferedProjectiles.push_back( BufferedProjectile(createTime, float3(0,0,0), obj->GetName(), ip) );
		//projectiles[serial] = ip;
		//db.ProjectileCreated(ip->GetObjectData(), float3(0,0,0), ip->GetTarget(), obj->GetName());
		return ip;
	}

	Projectile* ProjectileManager::CreateHealProjectile(Serial serial, float2 pos, uint16 fireAngle, uint16 type,
								Object* obj, float min, float max, bool bToMyTarget, Database::Info::ProjectileInfo *p,
								IGameStateDatabase& db, float createTime, Lunia::XRated::Serial creator)
	{
		ObjectData& data = obj->GetObjectData();

		Projectile* ip;
		float3 targetPosition(0,0,0) , sourcePosition = data.Position;

		Serial targetSerial = 0;
		if (bToMyTarget) 
		{
			targetSerial = obj->GetTarget();
			if (targetSerial) {
				Actor* target = db.RetrieveActor(targetSerial);
				if (target)
				{
					if( target->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer && target->CheckState(Database::Info::StateInfo::Type::INVINCIBLE) ) 
					{
						targetSerial = 0;
					} 
					else 
						targetPosition = target->GetPosition();
				}
				else targetSerial = 0;
			}
		} 
		
		if (targetSerial == 0) 
		{
			std::vector<Actor*> l;
			switch ( data.Type ) {
			case Constants::ObjectType::Projectile :
				db.GetAreaObject(data.Position, p->Speed*p->Duration, p->AllianceType, obj->GetTeam(), obj, l);
				break;
			default :
				db.GetAreaObject(data.Position, data.Direction, p->Speed*p->Duration, 300, 60, p->AllianceType, obj->GetTeam(), obj, l);
			};
			if (!l.empty() ) {
				// TODO : stageDatabase���� ������ ������ ���·� ����Ʈ�� ��� ���� ���ϰ�����.. ������ �մ�� �����.
				// �ӽ� �ڵ�� ������ ����� ����Ʈ���� ������.
				// �̰� �ӽ���?;; �굵 �׷� �ӽ÷� ���ɰ� ���� ���ܵ�..

				// �� ������Ÿ���� �� ���� ���� �Ѿư� Ȯ���� ��������
				std::vector< float > damagedHps;
				float totalDamagedHp = 0;

				Actor* actor;
				for( std::vector<Actor*>::iterator iActor = l.begin(); iActor != l.end() ;) {
					actor = (*iActor);
					if ( actor->CheckState(Database::Info::StateInfo::Type::GHOST) || actor->CheckState(Database::Info::StateInfo::Type::INVISIBLE) ) 
					{
						iActor = l.erase(iActor);
					} 
					else if( actor->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer && (((NonPlayer*)actor)->IsPetType() || actor->CheckState(Database::Info::StateInfo::Type::INVINCIBLE) ) ) 
					{
						//NonPlayer�̰� �����̸� Ÿ������ �ȵǵ��� �Ѵ�.
							iActor = l.erase(iActor);
					} 
					else 
					{
						float maxHp;
						float hp = actor->GetHP( maxHp );
						float damagedHp = ( maxHp - hp ) * 100 / maxHp ;
						totalDamagedHp += damagedHp;
						damagedHps.push_back( damagedHp );
						++iActor;
					}
				}
				if ( !l.empty() ) {
					std::vector<Actor*>::iterator i=l.begin();
					int r = 0;
					if( totalDamagedHp != 0 )
					{
						for( std::vector< float >::iterator iDamagedHp = damagedHps.begin() ; iDamagedHp != damagedHps.end() ; ++iDamagedHp )
						{
							*iDamagedHp /= totalDamagedHp;
						}
						Lunia::Math::RandomByPortion randomByPortion( damagedHps.begin() , damagedHps.end() );
						r = randomByPortion.Next();
					}
					else
					{
						r = db.GetRandomInt(0, (int)l.size());
					}
					
					while(r--) ++i;
					targetSerial = (*i)->GetSerial();
					targetPosition = (*i)->GetPosition();
				}
			} else
				targetSerial = 0;
		}

		float3 position(pos.x, 0, pos.y);
		float3 spinnedPos = Lunia::XRated::PhysicalFunctions::RotatePositionByDirection(position, 0, data.Direction);
		position = spinnedPos + sourcePosition;

		if (position.x <= 0 || position.z <= 0)
			return NULL;

		// ������ ���� �÷��̾��� ��� �ش� ���� ����� Action�� �����Ѵ�.
		Player* creatorObject = db.RetrievePlayer(creator);
		uint32 creatorActionHash = 0;
		
		if(creatorObject != NULL)
			creatorActionHash = creatorObject->GetActionNameHash();

		ip = new ProjectileHeal(serial, p, min, max, position, creator, creatorActionHash, obj->GetTeam(), db, targetSerial);

		if (type) {//type �� 0 �̸� ��븦 ����, 1 �̸� �׳� �߻����� ��������
			float3 sourceDirection = data.Direction;
			float3 spinnedDirection = Lunia::XRated::PhysicalFunctions::RotatePosition(sourceDirection, 360-fireAngle);
			ip->SetDirection( spinnedDirection );
		} else {
			if (targetSerial) {
				targetPosition.x += db.GetRandomFloat()*26 - 13;
				targetPosition.z += db.GetRandomFloat()*26 - 13;
				ip->SetDirection( Math::Normalize(targetPosition - sourcePosition) );	//type �� 0 �̸� ��븦 ����, 1 �̸� �׳� �߻����� ��������
			} else
				ip->SetDirection( data.Direction );
		}

		//������Ÿ���� ���͸� �������ش�.
		{
			float2 pos( position.x, position.z );
			Sector* sector = db.GetSmallSector().GetSector( pos );
			ip->SetSmallSector(sector);
			sector = db.GetBigSector().GetSector( pos );
			ip->SetBigSector(sector);
		}

		CopyState(obj, ip);

		bufferedProjectiles.push_back( BufferedProjectile(createTime, float3(0,0,0), obj->GetName(), ip) );
		//projectiles[serial] = ip;
		//db.ProjectileCreated(ip->GetObjectData(), float3(0,0,0), ip->GetTarget(), obj->GetName());
		return ip;
	}

	Projectile* ProjectileManager::CreateCircularProjectile(
		Serial serial, 
		const float2& pos, 
		uint16 fireAngle, 
		uint16 type, 
		Object* obj, 
		float min, 
		float max, 
		uint16 radius, 
		Database::Info::ProjectileInfo *p, 
		IGameStateDatabase& db, float createTime , Lunia::XRated::Serial creator)
	{
		ObjectData& data = obj->GetObjectData();

		Projectile* ip;
		float3 position = data.Position + float3(pos.x, 0, pos.y);

		float3 spinnedPos;

		// ������ ���� �÷��̾��� ��� �ش� ���� ����� Action�� �����Ѵ�.
		Player* creatorObject = db.RetrievePlayer(creator);
		uint32 creatorActionHash = 0;
		
		if(creatorObject != NULL)
			creatorActionHash = creatorObject->GetActionNameHash();

		if ( type ) { //type == 1 �̸� ������ ��ġ���� ���� ����.
			//������ ��ġ�� ���� ���� ������Ÿ���� Ư���ϰ�
			//����(0���� �ǹ�, toMyTarget�Ķ���Ͱ� radius�� �Ѿ�´�.)���� ���� fireAngle��ŭ ȸ���� ��ġ�� ������ġ�� ���
			//pos+owner�� ��ġ�� �߽������� ��´�.
			spinnedPos = Lunia::XRated::PhysicalFunctions::RotatePosition(float3(0,0,radius), fireAngle);

			ip = new ProjectileCircular(serial, 0, p, min, max, position/* + spinnedPos*/, spinnedPos, position, creator, creatorActionHash, obj->GetTeam(), db);
		} else {
			ip = new ProjectileCircular(serial, data.GameObjectSerial, p, min, max, position, float3(pos.x, 0, pos.y), position, creator, creatorActionHash, obj->GetTeam(), db);
		}

		if (position.x <= 0 || position.z <= 0) {
			if ( ip )
				delete ip;
			return NULL;
		}

		//������Ÿ���� ���͸� �������ش�.
		{
			float2 pos( position.x, position.z );
			Sector* sector = db.GetSmallSector().GetSector( pos );
			ip->SetSmallSector(sector);
			sector = db.GetBigSector().GetSector( pos );
			ip->SetBigSector(sector);
		}

		CopyState(obj, ip);

		if ( type )
			bufferedProjectiles.push_back( BufferedProjectile(createTime, spinnedPos, obj->GetName(), ip) );
		else
			bufferedProjectiles.push_back( BufferedProjectile(createTime, position, obj->GetName(), ip) );
		//projectiles[serial] = ip;
		//if ( type )
		//	db.ProjectileCreated(ip->GetObjectData(), spinnedPos, 0, obj->GetName());
		//else
		//	db.ProjectileCreated(ip->GetObjectData(), position, obj->GetSerial(), obj->GetName());
		return ip;
	}

	void ProjectileManager::CreateProjectile(uint32 name, Serial serial, float2 pos, uint16 fireAngle, uint16 type/*����� ����?*/,
		Object* obj, float min, float max, uint16 toMyTarget, IGameStateDatabase& db, float createTime,Lunia::XRated::Serial creator)
	{
		Info::ProjectileInfo *p = DatabaseInstance().InfoCollections.Projectiles.Retrieve(name);

		if ( !p ) {
			Logger::GetInstance().Info( L"[ProjectileManager::CreateProjectile] Projectile ({0}) info not found.", name );
			return;
		}

		//Test for increasing magic damage
		float incFactor = 1.0f;
		//Actor* actor = NULL;
		switch(obj->GetType()) {
		case Constants::ObjectType::Projectile :
			incFactor = ((Projectile*)obj)->GetIncFactor();
			break;
		case Constants::ObjectType::Player :
			{
				int str, dummy;
				((Actor*)obj)->GetBaseStat(str, dummy, dummy, dummy);

				if (db.GetGameType() & XRated::Constants::GameTypes::PvpGameTypeMask)
					incFactor = StatusFunctions::GetIncreasedDamageRateInPvp(str);
				else
					incFactor = StatusFunctions::GetIncreasedDamageRate(str);

				//������ �ɷ������� �������¸� Ǯ���ش�. ��ǥ���� ��)���̸��� ssa, sssa, ssssa �� ����~
				if ( ((Actor*)obj)->CheckState(Database::Info::StateInfo::Type::DEVINESHD) ) {
					Stat::IState* s = ((Actor*)obj)->GetState(Database::Info::StateInfo::Type::DEVINESHD, Database::Info::StateInfo::Sort::ORDINARYSTATE);
					if (s) ((Stat::DevineShd*)s)->Remove();
				}
			}
			break;
		case Constants::ObjectType::NonPlayer :
			{
				int str, dummy;
				((Actor*)obj)->GetBaseStat(str, dummy, dummy, dummy);
				incFactor = ((NonPlayer*)obj)->GetIncreasedDamageRate(str);
				incFactor += incFactor*((NonPlayer*)obj)->GetAtkMultiply();
			}
			break;
		case Constants::ObjectType::Misc :
			{
				int str, dummy;
				((Actor*)obj)->GetBaseStat(str, dummy, dummy, dummy);
				incFactor = StatusFunctions::GetIncreasedDamageRateForNonPlayer(str);
			}
			break;
		}

		Projectile* projectile=NULL;
		switch (p->ShootType) {
		case ProjectileInfo::ShootingType::HIGHANGLE :
			projectile = CreateHighAngleProjectile(serial, pos, fireAngle, obj, min, max, p, db, createTime, creator);
			break;
		case ProjectileInfo::ShootingType::FOLLOWTARGET :
			projectile = CreateFollowProjectile(serial, pos, fireAngle, type, obj, min, max, toMyTarget ? true : false, p, db, createTime, creator);
			break;
		case ProjectileInfo::ShootingType::HEAL :
			projectile = CreateHealProjectile(serial, pos, fireAngle, type, obj, min, max, toMyTarget ? true : false, p, db, createTime, creator);
			break;
		case ProjectileInfo::ShootingType::STRAIGHT :
			projectile = CreateStraightProjectile(serial, pos, fireAngle, type, obj, min, max, toMyTarget ? true : false, p, db, createTime, creator);
			break;
		case ProjectileInfo::ShootingType::FALL :
			projectile = CreateFallProjectile(serial, pos, fireAngle, type, obj, min, max, toMyTarget ? true : false, p, db, createTime, creator);
			break;
		case ProjectileInfo::ShootingType::FLOATING :
			projectile = CreateFloatingProjectile(serial, pos, fireAngle, obj, min, max, p, db, createTime, creator);
			break;
		case ProjectileInfo::ShootingType::CIRCULAR :
			projectile = CreateCircularProjectile(serial, pos, fireAngle, type, obj, min, max, toMyTarget /*���� ������Ÿ�Ͽ����� Ư���ϰ� �߽������κ����� �Ÿ��� ��Ÿ����.*/, p, db, createTime, creator);
			break;
		}

		if (projectile)
			projectile->SetIncFactor(incFactor);
	}

	void ProjectileManager::Update(float dt, IGameStateDatabase& db)
	{
		//���۸��� projectile���� ������ �ش�. ������Ÿ���� ���۸��Ǵ� ���� �ΰ���.
		//FallType�̾ �ٷ� ���������ʴ� ���� ������Ÿ���� ������Ÿ���� �����ϴ� ����̴�.
		std::vector<BufferedProjectile>::iterator bi = bufferedProjectiles.begin();
		for (; bi != bufferedProjectiles.end() ;) {
			BufferedProjectile& bp = (*bi);
			bp.delayedTime -= dt;
			if ( bp.delayedTime <= 0 ) {
				projectiles[bp.projectile->GetSerial()] = bp.projectile;
				switch(bp.projectile->GetShootingType()) {
				case Database::Info::ProjectileInfo::ShootingType::CIRCULAR :
				case Database::Info::ProjectileInfo::ShootingType::FALL :
				case Database::Info::ProjectileInfo::ShootingType::FOLLOWTARGET :
				case Database::Info::ProjectileInfo::ShootingType::HIGHANGLE :
				case Database::Info::ProjectileInfo::ShootingType::STRAIGHT :
				case Database::Info::ProjectileInfo::ShootingType::HEAL :
					db.ProjectileCreated( bp.projectile->GetObjectData(), bp.position, bp.projectile->GetTarget(), bp.ownerName );
					break;
				case Database::Info::ProjectileInfo::ShootingType::FLOATING :
					db.ProjectileCreated( bp.projectile->GetObjectData(), bp.position, 0, bp.ownerName );
					break;
				}
				bi = bufferedProjectiles.erase( bi );
			} else ++bi;
		}

		ProjectileMap::iterator i = projectiles.begin();
		for ( ; i != projectiles.end() ; ) {
			if ((*i).second->Update(dt, &db) ) {
				db.ObjectDestroyed((*i).second, Constants::ObjectType::Projectile/*, (*i).second->GetPosition(), float3(0,0,0), 0, 0*/);
				delete (*i).second;
				i = projectiles.erase( i );
			} else
				++i;
		}
	}

}	}	}
