#pragma once
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class ExpFactorManager
			{
			public:
				ExpFactorManager();
				~ExpFactorManager();

				void SetExtra(bool isExtra);
				void SetPenal(bool isPenal);
				void SetGuild(bool isGuild);
				void SetFamilyExpFactor(float factor);
				void SetGuildExpFactor(float factor);
				void SetDifficultyExpFactor(float factor);
				void SetItemExpFactor(int category, float factor);
				float GetItemExpFactor(int category) const;
				float GetGuildExpFactor() const;
				void ClearItemExpFactor();
				void ClearitemExpFactor(int category);
				void ClearGuildExpFactor();
				void ClearDifficultyExpFactor();
				void ClearFamilyExpFactor();
				void ClearEpisodeStageAdventage();

				float BaseExpFactor();
				float ExtraExpFactor();
				float PenalExpFactor();

				void Clear();

				float TotalExpFactor();

				void SetEpisodeStageAdventage(bool episodeStageAdventage);

			private:
				bool isEpisodeStageAdventage;
				bool isGuild; // guild user.
				bool isExtra; // PC-Room
				bool isPenal; // Appointment PvP, RePvP of above 4 times		
				float guildExpFactor;
				float difficultyExpFactor;
				typedef std::vector< std::pair<bool, float> >			ItemExpFactorList;
				ItemExpFactorList										itemExpFactorList;
				float familyExpFactor;
			};
		}
	}
}


