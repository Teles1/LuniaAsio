#pragma once

#include "../../NonPlayer.h"


namespace Lunia { namespace XRated {	namespace Logic {


	class DnFNPC : public NonPlayer
	{
	public:
		DnFNPC(void);
		virtual ~DnFNPC(void);

		virtual void Initialize(Lunia::Math::RandomInt& rInt);

		//virtual void IdleDefenceBegin();
		//virtual void IdleDefenceUpdate();
		//virtual void IdleDefenceEnd();

		virtual void stateApproachTarget_SightBegin();
		virtual void stateApproachTarget_SightUpdate();
		virtual void stateApproachTarget_SightEnd();

		virtual void stateCaution_SightBegin();
		virtual void stateCaution_SightUpdate();
		virtual void stateCaution_SightEnd();

		virtual void FightBegin();

	private:
		float gostoptime;
		bool  isgo;

	};


} } }