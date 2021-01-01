#pragma once
#include "GuildState.h"
#include <StageServer/User/User.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			GuildState::GuildState(User& in) : owner(in), guildInfo(NULL)
			{
				appliedPassive.reserve(4);
			}
			void GuildState::Clear()
			{
				addExpFactor = 1.0f;
				guildInfo = NULL;
				appliedPassive.clear();
				appliedPassive.reserve(4);
			}
			void GuildState::AddExpFactor(float factor)
			{
				addExpFactor = factor;
			}
			void GuildState::SetGuildLevelInfo(const Database::Info::GuildInfo::Level* info)
			{
				if (guildInfo != info) {
					ClearGuildLevelInfo();
					for (auto& passive : info->GuildPassives) {
						owner.AddPassive(passive.first);
						appliedPassive.push_back(passive.first);
					}
				}
			}
			void GuildState::ClearGuildLevelInfo()
			{
				ClearGuildPassive();
				guildInfo = NULL;
			}
			void GuildState::ClearGuildPassive()
			{
				size_t max = appliedPassive.size();
				for (size_t i = 0; i < max; ++i)
				{
					owner.RemovePassive(appliedPassive.at(i));
				}

				appliedPassive.clear();
				appliedPassive.reserve(4);
			}
			float GuildState::GetAddedExpFactor() const
			{
				return addExpFactor;
			}
			const std::vector<uint32>& GuildState::GetAppliedPassive() const
			{
				return appliedPassive;
			}
		}
	}
}
