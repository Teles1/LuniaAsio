#pragma once
#include "../Actor.h"

namespace Lunia { namespace XRated {	namespace Logic {

	class Misc : public Actor {
	private :
		Database::Info::MiscInfo* info;
		float duration;

	public :
		Misc();
		void Initialize(IGameStateDatabase* db) { Actor::Initialize(db); duration=0; }

		virtual int GetClass() const { return 0; }

	public ://IObject  implementation
		const std::wstring& GetName() const { return objectData.Name; }
		void SetName(const std::wstring& name) { objectData.Name = name; }
		const Database::Info::MiscInfo& GetMiscInfo() const { return *info;}

		bool LoadStatus(uint32 misc);

		virtual bool SetAction(uint32 name, Action::Action::IActionParam* param, bool force = false);

		virtual bool DoDamage(Object*, uint32, float, float, float, Constants::DamageType, uint32 , Constants::AttackStateType , unsigned int& );
		void SetDuration(float dur) { duration = dur; }
		virtual bool Update(float dt, IGameStateDatabase* db);
	};

	//class MiscFinite : public Misc {
	//	float duration;

	//public :
	//	MiscFinite() : duration(0) {}
	//	MiscFinite(float dur) : duration(dur) {}

	//	void SetDuration(float dur) { duration = dur; }
	//	virtual bool Update(float dt, IGameStateDatabase* db);
	//};

	//class MiscTemp : public Misc {
	//public :
	//	MiscTemp();
	//};

}	}	}
