#pragma once
#include <Core/GameConstants.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;
			typedef std::shared_ptr<User> UserSharedPtr;
			class FamilyManager {
			public:
				FamilyManager(User& owner);
			private:
				User& m_Owner;
				std::mutex m_Mtx;
				Family::Info			m_Info;
				Family::MemberInfo		m_MyInfo;
				Family::RewardCondition	m_Condition;
			};
		}
	}
}
