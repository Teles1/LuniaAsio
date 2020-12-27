#pragma once

#include "State.h"

namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class IncDmg : public State<Object>
	{
	//public:
	//			enum ChargeType
	//			{
	//				RATE = 0,
	//				AMOUNT
	//			};
	private:
				uint16 damageType;
				
				float value;
				//ChargeType chargeType;

	public:
				//IncDmg(Object* owner, Lunia::XRated::Constants::DamageType, float value, ChargeType chargeType);
				IncDmg(Object* owner, uint16 damageType, float value);
		virtual	~IncDmg();

		virtual void Initialize(IGameStateDatabase* db);
		virtual void Destroy();
	};

	class IncHealAmount : public State<Object>
	{
	//public:
	//			enum ChargeType
	//			{
	//				RATE = 0,
	//				AMOUNT
	//			};
	private:
				float value;
				//ChargeType chargeType;

	public:
				//IncHealAmount(Object* owner, ChargeType chargeType, float value);
				IncHealAmount(Object* owner, float value);
		virtual	~IncHealAmount();

		virtual void Initialize(IGameStateDatabase* db);
		virtual void Destroy();
	};

}}}}