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
				///< by kpongky ( 09.11.26 ) 마법 치명타 배율 증가 스테이트
				class CriMagicInc 
					: public State< Actor > 
				{
				public :
					CriMagicInc(Actor* owner, float amount_ )
						: State< Actor >( owner, Database::Info::StateInfo::Type::CRIMAGICINC, Database::Info::StateInfo::Sort::ATTACKSTATE )
						, amount( amount_ )
					{
					}

					virtual ~CriMagicInc()
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
