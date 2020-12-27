#include	"StateIncrease.h"
#include	"../Object/Object.h"

using namespace Lunia::StringUtil;
using namespace Lunia::Math;
using namespace Lunia::XRated::Database;

namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	//IncDmg::IncDmg(Object* owner, Lunia::XRated::Constants::DamageType damageType, float value, ChargeType chargeType)
	//	:	State<Object>(owner, Database::Info::StateInfo::INCDMG, Database::Info::StateInfo::Sort::ATTACKSTATE),
	//		damageType(damageType), value(value), chargeType(chargeType)
	IncDmg::IncDmg(Object* owner, uint16 damageType, float value)
		:	State<Object>(owner, Database::Info::StateInfo::INCDMG, Database::Info::StateInfo::Sort::ATTACKSTATE),
			damageType(damageType), value(value)
	{
	}

	IncDmg::~IncDmg()
	{
	}

	void IncDmg::Initialize(IGameStateDatabase*)
	{
		owner->AddIncDamage(damageType, value);
	}

	void IncDmg::Destroy()
	{
		owner->AddIncDamage(damageType, -value);
	}

	//IncHealAmount::IncHealAmount(Object* owner, ChargeType chargeType, float value)
	//	:	State<Object>(owner, Database::Info::StateInfo::INCHEALAMOUNT, Database::Info::StateInfo::Sort::DEFENCESTATE),
	//		value(value), chargeType(chargeType)
	IncHealAmount::IncHealAmount(Object* owner, float value)
		:	State<Object>(owner, Database::Info::StateInfo::INCHEALAMOUNT, Database::Info::StateInfo::Sort::DEFENCESTATE),
			value(value)
	{
	}

	IncHealAmount::~IncHealAmount()
	{
	}

	void IncHealAmount::Initialize(IGameStateDatabase*)
	{
		owner->AddIncHealRate(value);
	}

	void IncHealAmount::Destroy()
	{
		owner->AddIncHealRate(-value);
	}


}}}}