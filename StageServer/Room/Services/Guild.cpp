#include "Guild.h"
#ifndef User_H
	#include <StageServer/User/User.h>
#endif // !User_H
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			bool Guild::Comp(const std::pair<Serial, UserSharedPtr>& s, const std::pair<Serial, UserSharedPtr>& t)
			{
				return s.second->GetGuildState().GetAddedExpFactor() >= t.second->GetGuildState().GetAddedExpFactor();
			}

			bool Guild::Finder::operator()(const Guild& p) const
			{
				if (p.GuildId == id) {
					return true;
				}
				return false;
			}
			bool Guild::UserFinder::operator()(const std::pair<Serial, UserSharedPtr>& p) const
			{
				if (p.first == serial) {
					return true;
				}
				return false;
			}
		}
	}
}
	

