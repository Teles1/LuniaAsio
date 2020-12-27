#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class IgnoreStun : public State<Actor> {
		uint32 flag; //�����ϰ� ���� �׸���� & ��
		bool bIgnore; //flag�� �׸���� �����Ұ����� Ǯ����� ������.
		uint8 useState;
		bool  bUse;

		uint32 old; //�ڽ��� ������ �ϱ����� ��.

	public :
		IgnoreStun(Actor* owner, uint32 f, bool ignore, uint8 usestate)
			: State<Actor>(owner, Database::Info::StateInfo::Type::IGNORESTUN, Database::Info::StateInfo::Sort::DEFENCESTATE)
			, flag(f), bIgnore(ignore), useState(usestate), old(0), bUse(true)
		{}
		virtual ~IgnoreStun() {}

		void Initialize(IGameStateDatabase* db);
		void Destroy();
	};

}	}	}	}
