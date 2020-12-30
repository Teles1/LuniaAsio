#include "ExpFactorManager.h"
#include <Core/Utils/ConfigReader.h>
#include <Core/GameConstants.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			ExpFactorManager::ExpFactorManager() : isExtra(false), isPenal(false), isGuild(false), isEpisodeStageAdventage(false)
				, guildExpFactor(0.0f), familyExpFactor(0.0f), difficultyExpFactor(0.0f)
				, itemExpFactorList(XRated::Constants::ExpFactorCategoryFromItem::MaxCategory)
			{
				for(auto& x : itemExpFactorList)
					x.first = false;
			}

			ExpFactorManager::~ExpFactorManager()
			{
			}

			void ExpFactorManager::SetExtra(bool isExtra)
			{
				this->isExtra = isExtra;
			}


			void ExpFactorManager::SetPenal(bool isPenal)
			{
				this->isPenal = isPenal;
			}

			void ExpFactorManager::SetGuild(bool isGuild)
			{
				this->isGuild = isGuild;
			}

			void ExpFactorManager::SetDifficultyExpFactor(float factor)
			{
				difficultyExpFactor = factor;
			}

			void ExpFactorManager::SetGuildExpFactor(float factor)
			{
				guildExpFactor = factor;
			}

			float ExpFactorManager::GetGuildExpFactor() const
			{
				return guildExpFactor;
			}



			void ExpFactorManager::Clear()
			{
				isExtra = false;
				isPenal = false;
				isGuild = false;
				isEpisodeStageAdventage = false;
				guildExpFactor = 0.0f;
				difficultyExpFactor = 0.0f;
				for (auto& x : itemExpFactorList)
					x.first = false;
				familyExpFactor = 0.0f;
			}


			float ExpFactorManager::BaseExpFactor()
			{ 
				return Config::GetInstance().Get("BaseExp", 1.0f);
			}


			float ExpFactorManager::ExtraExpFactor()
			{
				if (isExtra)
					return Config::GetInstance().Get("ExtraExp", 2.0f);
				else 
					return 1.0f;
				
			}


			float ExpFactorManager::PenalExpFactor()
			{
				if (isPenal)
					return Config::GetInstance().Get("ExtraExp", 0.1f);
				else
					return 1.0f;
			}


			float ExpFactorManager::TotalExpFactor()
			{
				float extraFactor = 1.0f;
				std::pair<bool, float> normalExpFactor = itemExpFactorList.at(XRated::Constants::ExpFactorCategoryFromItem::CategoryNone);
				if ((ExtraExpFactor() < normalExpFactor.second) && (normalExpFactor.first == true)) {
					extraFactor = normalExpFactor.second;
				}
				else {
					extraFactor = ExtraExpFactor();
				}

				float communityFactor = 1.0f;
				if (isGuild == true) {
					communityFactor += guildExpFactor;
					std::pair<bool, float> guildCategory = itemExpFactorList.at(XRated::Constants::ExpFactorCategoryFromItem::CategoryGuild);

					if (guildCategory.first == true) {
						communityFactor += (guildCategory.second - 1.0f);
					}
				}
				communityFactor += familyExpFactor;

				if (isEpisodeStageAdventage)
				{
					communityFactor += 0.1f;
				}

				communityFactor += difficultyExpFactor;

				return ((BaseExpFactor() * PenalExpFactor() * extraFactor) * communityFactor);
			}

			void ExpFactorManager::SetItemExpFactor(int category, float factor)
			{
				if (itemExpFactorList.size() > category) {
					if ((itemExpFactorList.at(category).first == false) || (itemExpFactorList.at(category).second < factor)) {
						itemExpFactorList.at(category).first = true;
						itemExpFactorList.at(category).second = factor;
					}
				}
				else {
					Logger::GetInstance().Error(L"SetItemExpFactor, not support item exp factor category : {0}", category);
				}
			}
			void ExpFactorManager::SetFamilyExpFactor(float factor)
			{
				familyExpFactor = factor;
			}
			void ExpFactorManager::ClearFamilyExpFactor()
			{
				familyExpFactor = 0.0f;
			}
			float ExpFactorManager::GetItemExpFactor(int category) const
			{
				if (itemExpFactorList.size() > category) {
					if ((itemExpFactorList.at(category).first == true)) {
						return itemExpFactorList.at(category).second;
					}
				}
				return 1.0f;
			}
			void ExpFactorManager::ClearItemExpFactor()
			{
				ItemExpFactorList::iterator iter = itemExpFactorList.begin();
				ItemExpFactorList::iterator end = itemExpFactorList.end();
				while (iter != end) {
					iter->first = false;
					++iter;
				}
			}
			void ExpFactorManager::ClearGuildExpFactor()
			{
				guildExpFactor = 0.0f;
				isGuild = false;
			}

			void ExpFactorManager::ClearDifficultyExpFactor()
			{
				difficultyExpFactor = 0.0f;
			}

			void ExpFactorManager::ClearitemExpFactor(int category)
			{
				if (itemExpFactorList.size() > category) {
					itemExpFactorList.at(category).first = false;
				}
			}

			void ExpFactorManager::SetEpisodeStageAdventage(bool episodeStageAdventage)
			{
				this->isEpisodeStageAdventage = episodeStageAdventage;
			}

			void ExpFactorManager::ClearEpisodeStageAdventage()
			{
				isEpisodeStageAdventage = false;
			}

		}
	}
}