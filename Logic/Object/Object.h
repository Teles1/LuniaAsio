/*
    Copyright (c) 2000-, Lunia Corp. All rights reserved.
        http://www.Lunia.co.kr
            tel:82-2-888-1555
                fax:82-2-888-4414
                    mailTo:webmaster@Lunia.co.kr
*/
#pragma once
#include "../State/State.h"

namespace Lunia { namespace XRated {	namespace Logic {

	class IGameStateDatabase;
	struct Sectors;
	class Sector;

	class Object {
	public :

		struct Sectors {
			Sector* smallSector;
			Sector* bigSector;
		} sectors;

	protected :

		typedef std::list<Stat::IState*> StateList;


		Lunia::XRated::ObjectData objectData;

		//bool bUpdate;	//update�߿� �ڽſ��� ���� state�� �߰� �Ϸ��� �ϸ� ������ ���� ������ ���۸��� �Ѵ�.
		//				//������ critical section.
		//std::vector<State::IState*> addStateBuffer;
		//std::vector<State::IState*> removeStateBuffer;

		bool RemoveState(StateList& list, Database::Info::StateInfo::Type type);
		Stat::IState *FindState(const StateList &stateList, uint32 id);

		struct StateBuffers {//����, ����, ��� �����ϴ� state list�� �ٸ���.
			StateList state_attack;
			StateList state_defence;
			StateList state_ordinary;
			StateList state_condition;
		} stateBuffers;

		std::vector<uint8> stateFlags;//� state���� �ߵ��� �ִ°��� �˻��ϴ� flag��. reference count ����������.
		uint32 stateCnt;

		IGameStateDatabase* stageData;

		void Clear();
		void UpdateStates(float dt, StateList& list);

		struct IncreaseState
		{
			float incHealRate;
			float incDmg[Constants::DamageType::CNT];			

			float criAtkInc;
			float criMagicInc;

			IncreaseState();

			void Init();

			void AddIncDamage(XRated::Constants::DamageType damageType, float incDmg);
			float CalculrateIncDamage(XRated::Constants::DamageType damageType, const float& dmg);

			void AddIncHealRate(float amount);
			float CalculrateIncHealAmount(const float& amount);

			void AddCriAtkInc( float amount );
			void AddCriMagicInc( float amount );

		}increase;

	public :
		Object(Constants::ObjectType type);
		virtual ~Object();

		virtual void Initialize(IGameStateDatabase* db);

		virtual const std::wstring& GetName() const { return objectData.Name; }
		uint32 GetNameHash() const { return objectData.NameHash; }
		const Constants::ObjectType GetType() const { return objectData.Type; }
		const Lunia::XRated::Serial GetSerial() const { return objectData.GameObjectSerial; }
		void SetSerial(const Lunia::XRated::Serial serial) { objectData.GameObjectSerial=serial; }
		const float3& GetPosition() const { return objectData.Position; }
		virtual void SetPosition(const float3& pos);
		virtual void InitPosition(const float3& pos);	//sector�� �������� �ʰ� ��ġ�� �ʱ�ȭ ���ֱ� ���ؼ�. -_-;
		virtual const float3& GetDirection() const { return objectData.Direction; } //Projectile class ���� �����ε���.
		void SetDirection(const float3& dir);
		const float& GetRadius() const {return objectData.Radius;}
		void SetRadius(const float& r) { objectData.Radius = r;}
		virtual uint8 GetTeam() const { return 0; }	// TODO : �־�� �ϴ°�.. Object������ ��� ������ �ʴ´�. -_-;;
		virtual uint8 GetAlliance() const { return 0; } // object������ ��� ������ �ʴ´�.
		//virtual int GetSkillLevel(const wchar_t* /*skill*/) { return 1; }
		void SetSmallSector(Sector* sSector);
		void SetBigSector(Sector* bSector);
		Sectors& GetSector() { return sectors; }
		bool IsMoving() const { return objectData.Move; }
		virtual Serial GetTarget() const { return 0; }

		Lunia::XRated::ObjectData& GetObjectData() { return objectData; }

		virtual bool AddState(Stat::IState* state, bool bFront = false);
		virtual bool RemoveState(Database::Info::StateInfo::Sort sort, uint32 id);
		virtual bool EnableState(Database::Info::StateInfo::Sort sort, uint32 id, bool enable);
		virtual Stat::IState* GetState(Database::Info::StateInfo::Type type);
		virtual Stat::IState* GetState(Database::Info::StateInfo::Type type, Database::Info::StateInfo::Sort sort);
		virtual std::vector<Stat::IState*> GetStates(Database::Info::StateInfo::Type type, Database::Info::StateInfo::Sort sort);
		virtual bool CheckState(Database::Info::StateInfo::Type type) const { return stateFlags.at(type) ? true : false; }
		uint8 AddStateFlag(Database::Info::StateInfo::Type type) { return ++stateFlags.at(type); }
		uint8 RemoveStateFlag(Database::Info::StateInfo::Type type) { return stateFlags.at(type) > 0 ? --stateFlags[type] : 0; }

		/// ------------------------------------------ Increase State -
		virtual void AddIncDamage(XRated::Constants::DamageType damageType, float incDmg);
		virtual void AddIncDamage(uint16 damageType, float incDmg);
		void AddCriAtkInc( float amount );
		void AddCriMagicInc( float amount );
		virtual float CalculrateIncDamage(XRated::Constants::DamageType damageType, const float& dmg);

		virtual void AddIncHealRate(float amount);
		virtual float CalculrateIncHealAmount(const float& amount);
		/// - Increase State ------------------------------------------

		virtual void PauseAction(float) {}

		virtual bool Update(float dt, IGameStateDatabase* db);

		//Helper
		virtual bool IsCharacter() const { return false; }
		virtual bool IsOverlappable() const { return stateFlags.at(Database::Info::StateInfo::Type::PASSABLE) ? true : false; }
		virtual void SetOverlappable() {}
	protected:
		Object(const Object&) { throw; }
		Object& operator= (const Object&) { throw; }
	};

}	}	}
