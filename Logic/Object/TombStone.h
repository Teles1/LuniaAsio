#pragma once
#include "Actor.h"

namespace Lunia { namespace XRated {	namespace Logic {

	class TombStone : public Actor {
	public :
		const static float ReviveRange;

	protected :
		std::wstring whos;
		Serial ownerSerial;
		std::wstring animationName;
		//bool bInSighted;
	public :
		TombStone(const wchar_t* name, Lunia::XRated::Serial ownerSerial);
		void operator= (TombStone&) {}

		//bool IsInSight() { return bInSighted; }

	public ://IObject  implementation
		Serial GetOwnerSerial() { return ownerSerial; }
		const std::wstring& GetName() const { return whos; }
		void SetName(const std::wstring& name) { whos = name; }
		void SetPosition(const float3& pos) { InitPosition(pos); }
		bool DoDamage(Object*, uint32, float, float, float, Constants::DamageType, uint32, Constants::AttackStateType, unsigned int&) { return false; }
		bool Update(float dt, IGameStateDatabase* db);
	};

}	}	}
