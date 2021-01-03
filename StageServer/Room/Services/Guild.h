#ifndef Guild_H
#define Guild_H
#pragma once
#ifndef GameConstants_GUARD
	#include <Core/GameConstants.h>
#endif // !GameConstants_GUARD

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;
			typedef std::shared_ptr<User> UserSharedPtr;
			struct Guild {
				typedef std::vector< std::pair<Serial, UserSharedPtr> >	MemberList;
				MemberList					Members;
				int GuildId;
				static bool Comp(const std::pair<Serial, UserSharedPtr>& s, const std::pair<Serial, UserSharedPtr>& t);
				class UserFinder {
					Serial serial;
				public:
					UserFinder(Serial serial) :serial(serial) {}
					bool operator ()(const std::pair<Serial, UserSharedPtr>& p) const;
				};
				class Finder {
					int id;
				public:
					Finder(int guildId) :id(guildId) {}
					bool operator ()(const Guild& p) const;
				};
				float AddExpFactor;
				Guild(int guildId) :GuildId(guildId), AddExpFactor(1.0f) {}
			};
		}
	}
}
#endif // !Guild_H