#pragma once
#include "../State.h"

namespace Lunia { namespace XRated { namespace Logic {

	class Actor;

	namespace Stat {

	class StateBundle {
	private :
		Database::Info::StateBundleInfo* info;
		struct BufferedState {
			Stat::IState* iState;
			uint32 id;
			Database::Info::StateInfo::Sort sort;
			BufferedState(Stat::IState* state) : iState(state), id(iState->GetId()), sort(iState->GetSort()) {}
		};
		std::vector<BufferedState> stateList;

		float duration, currentTime;

	public :
		Lunia::XRated::Serial ByWhom;
		StateBundle() : info(NULL), duration(0.f), currentTime(0.f) { }
		virtual ~StateBundle() {}

	public :
		uint16 GetLevel() const { return info->level; }
		const std::wstring& GetName() const { return info->name; }
		uint32 GetHash() const { return info->hash; }
		const std::wstring& GetCategoryName() const { return info->categoryName; }
		uint32 GetCategoryHash() const { return info->categoryHash; }
		uint32 GetSubCategoryHash() const { return info->subCategoryHash; }

		bool CheckState(Stat::IState* state); //해당 스테이트가 들어있는지를 검사한다.
		bool CheckState(Database::Info::StateInfo::Type type); //해당 type의 스테이트가 들어있는지를 검사한다.
		void Clear() { duration = 0; } //버프를 풀어 버린다.

		void Clear(Actor& owner);
		void Set(Actor& owner, Database::Info::StateBundleInfo* info,Lunia::XRated::Serial creator,Lunia::XRated::Serial byWhom=0);
		void AddState(Stat::IState* state);
		bool Debuff(float rate, float randomValue);
		bool Update(float dt);
	};

	class StateBundleCategory {
	protected :
		const int SlotSize;
		std::list<StateBundle> bundles;

		Actor& owner;
		IGameStateDatabase* stageData;

		void RemoveStateFromCharacterData(uint32 hash);
		void AddStateToCharacterData(uint32 hash);
	public :
		StateBundleCategory(Actor& actor, int sz) : owner(actor), SlotSize(sz) {}

		void Init(IGameStateDatabase* db) { bundles.clear(); stageData = db; }
		void Clear(); //이벤트를 날리면서 statebundle들을 지워준다. 케릭터가 죽어서 초기화 될경우에 사용된다.
		bool Remove(Database::Info::StateBundleInfo* info);
		bool Remove(Stat::IState* state);
		bool Remove(Database::Info::StateInfo::Type type);
		//state type에 맞는 statebundle을 모두 삭제한다.
		bool RemoveAll(Database::Info::StateInfo::Type type);

		virtual void Debuff(float debuffRate, uint32 subCategoryHash);
		virtual bool Add(Database::Info::StateBundleInfo* info,Lunia::XRated::Serial creator,Lunia::XRated::Serial byWhom=0);
		virtual void Update(float dt);
	};

	class CategoryBuff : public StateBundleCategory {
	public :
		CategoryBuff(Actor& actor, int sz) : StateBundleCategory(actor, sz) {}
	};

	class CategoryCurse : public StateBundleCategory {

	public :
		CategoryCurse(Actor& actor, int sz) : StateBundleCategory(actor, sz) {}

		bool Add(Database::Info::StateBundleInfo* info,Lunia::XRated::Serial creator);
	};

	class CategoryPassive : public StateBundleCategory {
	public :
		CategoryPassive(Actor& actor, int sz) : StateBundleCategory(actor, sz) {}

		void Debuff(float /*debuffRate*/, uint32 /*subCategoryHash*/) { return ; } //passive는 디버프 되지 않는다.
		bool Add(Database::Info::StateBundleInfo* info,Lunia::XRated::Serial creator);
		void Update(float /*dt*/) { return; }
	};

	class CategoryOne : public StateBundleCategory {
	public :
		CategoryOne(Actor& actor) : StateBundleCategory(actor, 1) {}

		bool Add(Database::Info::StateBundleInfo* info,Lunia::XRated::Serial creator);
	};

	class StateBundleManager {
	public :
		struct Constants {
			const static int Released = 0;
		};

	private :
		CategoryBuff	buff;
		CategoryBuff	cashBuff;
		CategoryCurse	curse;
		CategoryOne		item;
		CategoryPassive	passive;

		CategoryBuff    buffFromOtherObject;


		Actor& owner;
		IGameStateDatabase* stageData;

		void CheckUnique(Database::Info::StateBundleInfo* info);
		void Remove(Database::Info::StateInfo::Type type);

	public :
		StateBundleManager(Actor& obj);
		~StateBundleManager() {}

		void Init(IGameStateDatabase* db);

		void ClearBuff(); //케릭터가 죽는 경우같이 버프를 모두 지워주기 위해 호출되는 함수.

		//새로운 상태를 더한다. 저주 계얼은 저항여부를 판단한다. 버프나 저주등에 걸릴때 호출된다.
		bool Add(Database::Info::StateBundleInfo* info,Lunia::XRated::Serial creator,Lunia::XRated::Serial byWhom=0);
		bool Remove(Database::Info::StateBundleInfo* info);
		//state에서 스스로 없어질 상황을 판단했을 때, statebundle에게 메세지를 보내기 위한 함수.
		bool Remove(Stat::IState* state);
		//state type에 맞는 statebundle을 모두 삭제한다.
		bool RemoveAll(Database::Info::StateInfo::Type type);
		//새로운 상태를 조건에 맞는 object에 추가한다. 버프나 저주등을 자신이 발동할 때 호출된다.
		void ProcessStateBundle(const Database::Info::StateBundleInfo::Condition& condition);
		//Debuff
		void Debuff(float debuffRate, int slot, uint32 subCategoryHash);
		//void Remove(int sort);

		void Update(float dt);	

	private :
		void operator= ( const StateBundleManager& );
	};

}	}	}	}
