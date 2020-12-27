#pragma once

#include "State.h"


namespace AllM { namespace XRated { namespace Logic { namespace Stat {

	class AddItem : public State<Player> {
	protected :
		std::wstring itemName;

	public :
		AddItem(AllM::XRated::Logic::Player* owner, const wchar_t* item);
	};

}	}	}	}
