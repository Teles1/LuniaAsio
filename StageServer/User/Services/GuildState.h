#ifndef GuildState_H
#define GuildState_H

#pragma once
#include <Info/Info/GuildFamily/GuildInfo.h>

#pragma once
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;
			typedef std::shared_ptr<User> UserSharedPtr;
			class GuildState {
			public:
				GuildState(User& owner);

				const std::vector<uint32>& GetAppliedPassive() const;
				float GetAddedExpFactor() const;
				void AddExpFactor(float factor);
				void SetGuildLevelInfo(const Database::Info::GuildInfo::Level* info);
				void ClearGuildPassive();
				void ClearGuildLevelInfo();
				void Clear();
			private:
				float addExpFactor;
				User& owner;
				std::vector<uint32>	appliedPassive;
				const Database::Info::GuildInfo::Level* guildInfo;
			};
		}
	}
}
#endif