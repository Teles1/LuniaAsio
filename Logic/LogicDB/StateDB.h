#pragma once
#include "../State/State.h"


namespace Lunia { namespace XRated { namespace Logic {

	namespace Stat {
		class StateBundle;
	}

	class StateDB {
	public :
		typedef std::vector<Stat::IState*> StateVector;
		typedef std::map< uint32, std::vector<Stat::IState*> > StateMap;

		enum Type { TypeNpc=0, TypeItem, TypeStateBundle, TypeProjectile };

		struct ItemStates {
			StateVector equipStates;
			StateVector useStates;
			Database::Info::StateBundleInfo::ConditionList* bundleConditions;
		};

	protected :
		StateMap stateNPC;
		StateMap stateItem;
		StateMap stateStateBundle;
		StateMap stateProjectile;

		std::map<uint32 /*key, not item hash*/, ItemStates> stateCashItems;

		void Clear();
		void InitNPC();
		void InitItem();
		void InitStateBundle();
		void InitProjectile();
		void InitCashItem();

		bool ValidateBundleState(Database::Info::StateInfo::Type type) const;

	private:
#ifdef _SINGLE
		void InitNPCObject(uint32 hash);
		void InitItemObject(uint32 hash);
		void InitStateBundleObject(uint32 hash);
		void InitProjectileObject(uint32 hash);
		void InitCashItemObject(uint32 hash);
#endif
	public :
		StateDB() {}
		~StateDB();

		void Init();

		void AddNPCState(uint32 code, Object* obj);
		void AddItemState(uint32 code, Object* obj);
		void AddItemState(uint32 code, Object* obj, std::vector<Stat::IState*>& l);
		void AddCashItemState(uint32 id, Object* obj);
		void AddCashItemState(uint32 id, Object* obj, std::vector<Stat::IState*>& l);

		void AddStateBundleState(uint32 code, Object* obj, Stat::StateBundle* stateBundle,Lunia::XRated::Serial creator);
		Stat::IState* CreateState(Type type, uint32 code, Object* obj, int cnt);
		Stat::IState* CreateProjectileState(uint32 code, Projectile* obj, Actor* target,int cnt, IGameStateDatabase* db, uint32 damageStateTypeUniqueSerial = 0); 

	private:
		StateDB(const StateDB&); // unable to copy, because of potential problem of double delete at destructor
	};

}	}	}

