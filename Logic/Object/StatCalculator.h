#pragma once

#include <Core/GameConstants.h>
#include <memory>

namespace Lunia
{
	namespace XRated
	{
		struct BasicStat
		{
			BasicStat(uint32 strength_, uint32 intelligence_, uint32 dexterity_, uint32 vitality_)
				: strength(strength_)
				, intelligence(intelligence_)
				, dexterity(dexterity_)
				, vitality(vitality_)
			{
			}

			uint32 strength;
			uint32 intelligence;
			uint32 dexterity;
			uint32 vitality;
		};
		struct SecondStat
		{
			SecondStat()
				: skillDamageIncreaseFactor(0.0f)
				, criticalProbabilityByDexterity(0.0f)
				, coolDownTimeReduceByDexterity(0.0f)
				, magicalCriticalFactorByIntelligence(0.0f)
				, hitIgnoreBySpacificStat(0.0f)
				, healIncRate(0.0f)
				, maxHp(0.0f)
				, maxMp(0.0f)
			{
			}
			float skillDamageIncreaseFactor;
			float criticalProbabilityByDexterity;
			float coolDownTimeReduceByDexterity;
			float magicalCriticalFactorByIntelligence;
			float physicalCriticalFactorByVitality;
			float hitIgnoreBySpacificStat;
			float healIncRate;
			float maxHp;
			float maxMp;
		};

		class IStatCalculator
		{
		public:
			virtual void CalculateSecondStat(const XRated::Constants::ClassType classType, const BasicStat& basicStat, SecondStat& secondStat, const XRated::Constants::GameTypes gameType) const = 0;
			virtual float GetDecreaseCoolDownRate(const XRated::Constants::ClassType classType, const uint32 dexterity, const XRated::Constants::GameTypes gameType) const = 0;
			virtual float GetIncreasedHealRate(const XRated::Constants::ClassType classType, const uint32 intelligence, const XRated::Constants::GameTypes gameType) const = 0;
			virtual float GetMaxHpByStat(const XRated::Constants::ClassType classType, const uint32 vitality, const XRated::Constants::GameTypes gameType) const = 0;
			virtual float GetMaxMpByStat(const XRated::Constants::ClassType classType, const uint32 intelligence, const XRated::Constants::GameTypes gameType) const = 0;
			virtual float GetSkillDamageIncRate(const uint32 strength, const XRated::Constants::GameTypes gameType) const = 0;
			virtual float GetHitIgnoreValue(const XRated::Constants::ClassType classType, int Dex, int Vit, int Int) const = 0;
			virtual ~IStatCalculator()
			{
			}
		};

		class StaticStatCalculator : public IStatCalculator
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
			float GetIncreasedRateFactor(const uint32 strength) const;
			float CalculateSkillDamageFactorWithStrength(const uint32 strength, const XRated::Constants::GameTypes gameType) const;

			float CalculateHitIgnorHpRateSpecificWith(const uint32 vitality) const;
			float CalculateSkillDamageSpecificWithVitality(const uint32 vitality) const;

			float CalculateCoolDownSpecificWith(const uint32 dexterity) const;
			float CalculateCriticalProbabilitySpecificWith(const uint32 dexterity) const;
			float CalculateSkillDamageSpecificWithDexterity(const uint32 dexterity) const;

			float CalculateCoolDownNormalWith(const uint32 dexterity) const;
			float CalculateCriticalProbabilityNormalWith(const uint32 dexterity) const;

			float CalculatePhysicalCriticalFactorNormalWith(const uint32 vitality) const;
			float CalculatePhysicalCriticalFactorSpecificWith(const uint32 vitality) const;
			float CalculateMagicalCriticalFactorSpecificWith(const uint32 intelligence) const;
			float CalculateMagicalCriticalFactorNormalWith(const uint32 intelligence) const;
			float CalculateSkillDamageSpecificWithIntelligence(const uint32 intelligence) const;

			float CalculateHealIncRate(const uint32 intelligence) const;
			float CalculateMaxHp(const uint32 vitality, const XRated::Constants::GameTypes gameType) const;
			float CalculateMaxMp(const uint32 intelligence, const XRated::Constants::GameTypes gameType) const;

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

		namespace Logic 
		{
			static std::shared_ptr<StaticStatCalculator> impl = std::make_shared<StaticStatCalculator>();
			class StatCalculator
			{
			public:
				struct Method
				{
					enum Type
					{
						Static = 0,
						Script,
					};
				};

				inline StatCalculator(){}

				inline void CalculateSecondStat( const XRated::Constants::ClassType classType, const BasicStat& basicStat, SecondStat& secondStatEx, const XRated::Constants::GameTypes gameType ) const
				{
					impl->CalculateSecondStat( classType, basicStat, secondStatEx, gameType );					
				}

				inline float StatCalculator::GetMaxHpByStat( const XRated::Constants::ClassType classType, const uint32 vitality, const XRated::Constants::GameTypes gameType ) const
				{
					return impl->GetMaxHpByStat( classType, vitality, gameType );
				}

				inline float StatCalculator::GetMaxMpByStat( const XRated::Constants::ClassType classType, const uint32 intelligence, const XRated::Constants::GameTypes gameType ) const
				{
					return impl->GetMaxMpByStat( classType, intelligence, gameType );
				}

				inline float StatCalculator::GetHitIgnoreValue( const XRated::Constants::ClassType classType, int Dex, int Vit, int Int ) const
				{
					return impl->GetHitIgnoreValue( classType, Dex, Vit, Int );
				}
			};
		}
	}
}