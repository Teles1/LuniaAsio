#pragma once

#include "State.h"

namespace Lunia 
{ 
	namespace XRated 
	{ 
		namespace Logic 
		{ 
			namespace Stat 
			{
				///< by kpongky ( 09.11.26 ) ���� ġ��Ÿ ���� ���� ������Ʈ
				class CriAtkInc 
					: public State< Actor > 
				{
				public :
					CriAtkInc(Actor* owner, float amount_ )
						: State< Actor >( owner, Database::Info::StateInfo::Type::CRIATKINC, Database::Info::StateInfo::Sort::ATTACKSTATE )
						, amount( amount_ )
					{
					}

					virtual ~CriAtkInc()
					{
					}

					virtual void Initialize(IGameStateDatabase* db);

					virtual void Destroy();
						
				protected :
					float amount;
				};
			}
		}
	}
}
