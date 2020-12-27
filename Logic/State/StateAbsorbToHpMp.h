#pragma once

#include "State.h"

namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class AbsorbToHpMp : public State<Actor>
	{
	public:
		enum ConvertTo	{ Hp, Mp };
	
	private:		
		ConvertTo convertTo;
		
		const float rate;
		const float probability;

	public:
		AbsorbToHpMp(Actor* owner, ConvertTo convertTo, float inRate, float inProbability)
			:	State<Actor>(owner, Database::Info::StateInfo::Type::ABSORBTOHPMP, Database::Info::StateInfo::Sort::ORDINARYSTATE),
				convertTo(convertTo), rate(inRate), probability(inProbability)
		{
		}

		virtual void Initialize(IGameStateDatabase* db);
		virtual void Destroy();
	};

}}}}
