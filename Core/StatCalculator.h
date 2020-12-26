#pragma once

#include <Core/GameConstants.h>

namespace Lunia
{
	namespace XRated
	{
		namespace Lunia
		{
			struct SecondStat;

			struct BasicStat;

			class StatCalculator
			{
			public:
				virtual void CalculateSecondStat(const XRated::Constants::ClassType classType, const BasicStat& basicStat, SecondStat& secondStat, const XRated::Constants::GameTypes gameType) const = 0;
				virtual float GetDecreaseCoolDownRate(const XRated::Constants::ClassType classType, const uint32 dexterity, const XRated::Constants::GameTypes gameType) const = 0;
				virtual float GetIncreasedHealRate(const XRated::Constants::ClassType classType, const uint32 intelligence, const XRated::Constants::GameTypes gameType) const = 0;
				virtual float GetMaxHpByStat(const XRated::Constants::ClassType classType, const uint32 vitality, const XRated::Constants::GameTypes gameType) const = 0;
				virtual float GetMaxMpByStat(const XRated::Constants::ClassType classType, const uint32 intelligence, const XRated::Constants::GameTypes gameType) const = 0;
				virtual float GetSkillDamageIncRate(const uint32 strength, const XRated::Constants::GameTypes gameType) const = 0;
				virtual float GetHitIgnoreValue(const XRated::Constants::ClassType classType, int Dex, int Vit, int Int) const = 0;
				virtual ~StatCalculator()
				{
				}
			};

			class StaticStatCalculator
				: public StatCalculator
			{
			public:
				void CalculateSecondStat(const XRated::Constants::ClassType classType, const BasicStat& basicStat, SecondStat& secondStat, const XRated::Constants::GameTypes gameType) const;
				float GetDecreaseCoolDownRate(const XRated::Constants::ClassType classType, const uint32 dexterity, const XRated::Constants::GameTypes gameType) const;
				float GetIncreasedHealRate(const XRated::Constants::ClassType classType, const uint32 intelligence, const XRated::Constants::GameTypes gameType) const;
				float GetMaxHpByStat(const XRated::Constants::ClassType classType, const uint32 vitality, const XRated::Constants::GameTypes gameType) const;
				float GetMaxMpByStat(const XRated::Constants::ClassType classType, const uint32 intelligence, const XRated::Constants::GameTypes gameType) const;
				float GetSkillDamageIncRate(const uint32 strength, const XRated::Constants::GameTypes gameType) const;
				float GetHitIgnoreValue(const XRated::Constants::ClassType classType, int Dex, int Vit, int Int) const;

			private:
				inline float GetIncreasedRateFactor(const uint32 strength) const;
				inline float CalculateSkillDamageFactorWithStrength(const uint32 strength, const XRated::Constants::GameTypes gameType) const;

				inline float CalculateHitIgnorHpRateSpecificWith(const uint32 vitality) const;
				inline float CalculateSkillDamageSpecificWithVitality(const uint32 vitality) const;

				inline float CalculateCoolDownSpecificWith(const uint32 dexterity) const;
				inline float CalculateCriticalProbabilitySpecificWith(const uint32 dexterity) const;
				inline float CalculateSkillDamageSpecificWithDexterity(const uint32 dexterity) const;

				inline float CalculateCoolDownNormalWith(const uint32 dexterity) const;
				inline float CalculateCriticalProbabilityNormalWith(const uint32 dexterity) const;

				inline float CalculatePhysicalCriticalFactorNormalWith(const uint32 vitality) const;
				inline float CalculatePhysicalCriticalFactorSpecificWith(const uint32 vitality) const;
				inline float CalculateMagicalCriticalFactorSpecificWith(const uint32 intelligence) const;
				inline float CalculateMagicalCriticalFactorNormalWith(const uint32 intelligence) const;
				inline float CalculateSkillDamageSpecificWithIntelligence(const uint32 intelligence) const;

				inline float CalculateHealIncRate(const uint32 intelligence) const;
				inline float CalculateMaxHp(const uint32 vitality, const XRated::Constants::GameTypes gameType) const;
				inline float CalculateMaxMp(const uint32 intelligence, const XRated::Constants::GameTypes gameType) const;

				struct Method
				{
					enum Type
					{
						Static = 0,
						Script,
					};
				};

				Method::Type method;
			};
		}
	}
}
