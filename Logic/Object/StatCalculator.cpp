#include <Logic/Object/StatCalculator.h>
#include <Info/Info.h>
namespace Lunia {
	namespace XRated {
		void StaticStatCalculator::CalculateSecondStat(const XRated::Constants::ClassType classType, const BasicStat& basicStat, SecondStat& secondStat, const XRated::Constants::GameTypes gameType) const
		{
			std::wstring classStr = XRated::Constants::GetClassStr(classType);
			XRated::Constants::CharacterMajorStatType majorStat = Database::DatabaseInstance().InfoCollections.Statuses.GetMajorStatType(classStr);

			switch (majorStat)
			{
			case XRated::Constants::CharacterMajorStatType::Dex:
				secondStat.hitIgnoreBySpacificStat = -1.0f;
				secondStat.skillDamageIncreaseFactor = CalculateSkillDamageSpecificWithDexterity(basicStat.dexterity) + CalculateSkillDamageFactorWithStrength(basicStat.strength, gameType);
				secondStat.coolDownTimeReduceByDexterity = CalculateCoolDownSpecificWith(basicStat.dexterity);
				secondStat.criticalProbabilityByDexterity = CalculateCriticalProbabilitySpecificWith(basicStat.dexterity);
				secondStat.magicalCriticalFactorByIntelligence = CalculateMagicalCriticalFactorNormalWith(basicStat.intelligence);
				secondStat.physicalCriticalFactorByVitality = CalculatePhysicalCriticalFactorNormalWith(basicStat.vitality);
				break;
			case XRated::Constants::CharacterMajorStatType::Vit:
				secondStat.hitIgnoreBySpacificStat = -1.0f;	//CalculateHitIgnorHpRateSpecificWith( basicStat.vitality );
				secondStat.skillDamageIncreaseFactor = CalculateSkillDamageSpecificWithVitality(basicStat.vitality) + CalculateSkillDamageFactorWithStrength(basicStat.strength, gameType);
				secondStat.coolDownTimeReduceByDexterity = CalculateCoolDownNormalWith(basicStat.dexterity);
				secondStat.criticalProbabilityByDexterity = CalculateCriticalProbabilityNormalWith(basicStat.dexterity);
				secondStat.magicalCriticalFactorByIntelligence = CalculateMagicalCriticalFactorNormalWith(basicStat.intelligence);
				secondStat.physicalCriticalFactorByVitality = CalculatePhysicalCriticalFactorSpecificWith(basicStat.vitality);
				break;
			case XRated::Constants::CharacterMajorStatType::Int:
				secondStat.hitIgnoreBySpacificStat = -1.0f;
				secondStat.skillDamageIncreaseFactor = CalculateSkillDamageSpecificWithIntelligence(basicStat.intelligence) + CalculateSkillDamageFactorWithStrength(basicStat.strength, gameType);
				secondStat.coolDownTimeReduceByDexterity = CalculateCoolDownNormalWith(basicStat.dexterity);
				secondStat.criticalProbabilityByDexterity = CalculateCriticalProbabilityNormalWith(basicStat.dexterity);
				secondStat.magicalCriticalFactorByIntelligence = CalculateMagicalCriticalFactorSpecificWith(basicStat.intelligence);
				secondStat.physicalCriticalFactorByVitality = CalculatePhysicalCriticalFactorNormalWith(basicStat.vitality);
				break;
			case XRated::Constants::CharacterMajorStatType::Str:
			default:
				secondStat.hitIgnoreBySpacificStat = -1.0f;
				secondStat.skillDamageIncreaseFactor = 0.0f;
				secondStat.coolDownTimeReduceByDexterity = 0.0f;
				secondStat.criticalProbabilityByDexterity = 0.0f;
				secondStat.magicalCriticalFactorByIntelligence = 0.0f;
				secondStat.physicalCriticalFactorByVitality = 0.0f;
				break;
			};


			secondStat.healIncRate = CalculateHealIncRate(basicStat.intelligence);
			secondStat.maxHp = CalculateMaxHp(basicStat.vitality, gameType);
			secondStat.maxMp = CalculateMaxMp(basicStat.intelligence, gameType);
		}

		float StaticStatCalculator::GetDecreaseCoolDownRate(const XRated::Constants::ClassType classType, const uint32 dexterity, const XRated::Constants::GameTypes gameType) const
		{
			SecondStat result;
			CalculateSecondStat(classType, BasicStat(0, 0, dexterity, 0), result, gameType);

			return result.coolDownTimeReduceByDexterity;
		}

		float StaticStatCalculator::GetIncreasedHealRate(const XRated::Constants::ClassType classType, const uint32 intelligence, const XRated::Constants::GameTypes gameType) const
		{
			SecondStat result;
			CalculateSecondStat(classType, BasicStat(0, intelligence, 0, 0), result, gameType);

			return result.healIncRate;
		}

		float StaticStatCalculator::GetMaxHpByStat(const XRated::Constants::ClassType classType, const uint32 vitality, const XRated::Constants::GameTypes gameType) const
		{
			SecondStat result;
			CalculateSecondStat(classType, BasicStat(0, 0, 0, vitality), result, gameType);

			return result.maxHp;
		}

		float StaticStatCalculator::GetMaxMpByStat(const XRated::Constants::ClassType classType, const uint32 intelligence, const XRated::Constants::GameTypes gameType) const
		{
			SecondStat result;
			CalculateSecondStat(classType, BasicStat(0, intelligence, 0, 0), result, gameType);

			return result.maxMp;
		}

		float StaticStatCalculator::GetSkillDamageIncRate(const uint32 strength, const XRated::Constants::GameTypes gameType) const
		{
			return CalculateSkillDamageFactorWithStrength(strength, gameType);
		}

		float StaticStatCalculator::GetHitIgnoreValue(const XRated::Constants::ClassType classType, int Dex, int Vit, int Int) const
		{
			std::wstring classStr = XRated::Constants::GetClassStr(classType);
			XRated::Constants::CharacterMajorStatType majorStat = XRated::Database::DatabaseInstance().InfoCollections.Statuses.GetMajorStatType(classStr);

			switch (majorStat)
			{
			case XRated::Constants::CharacterMajorStatType::Dex:
				return static_cast<float>(Dex) / 5.0f;
			case XRated::Constants::CharacterMajorStatType::Vit:
				return static_cast<float>(Vit) / 5.0f;
			case XRated::Constants::CharacterMajorStatType::Int:
				return static_cast<float>(Int) / 5.0f;
			};

			return 0.0f;
		}

		float StaticStatCalculator::CalculateHitIgnorHpRateSpecificWith(const uint32) const
		{
			return 0.2f;
		}

		float StaticStatCalculator::CalculateSkillDamageSpecificWithVitality(const uint32) const
		{
			return 0.0f;
		}

		float StaticStatCalculator::CalculateCoolDownSpecificWith(const uint32 dexterity) const
		{
			if (dexterity < 500)
			{
				return static_cast<float>(dexterity) / 3300.0f;
			}
			else
			{
				return static_cast<float>(dexterity - 500.0f) * 0.00015f + 0.15152f;
			}
		}

		float StaticStatCalculator::CalculateCriticalProbabilitySpecificWith(const uint32 dexterity) const
		{
			if (dexterity < 500)
			{
				return static_cast<float>(dexterity) * 0.0004f;
			}
			else
			{
				return static_cast<float>(dexterity) * 0.00015f + 0.125f;
			}
		}

		float StaticStatCalculator::CalculateSkillDamageSpecificWithDexterity(const uint32) const
		{
			return 0.0f;
		}

		float StaticStatCalculator::CalculateCoolDownNormalWith(const uint32 dexterity) const
		{
			if (dexterity < 500)
			{
				return static_cast<float>(dexterity) / 3300.0f;
			}
			else
			{
				return (static_cast<float>(dexterity) - 500.0f) * 0.00001f + 0.15152f;
			}
		}

		float StaticStatCalculator::CalculateCriticalProbabilityNormalWith(const uint32 dexterity) const
		{
			if (dexterity < 500)
			{
				return static_cast<float>(dexterity) * 0.0004f;
			}
			else
			{
				return (static_cast<float>(dexterity) - 500.0f) * 0.00001f + 0.2f;
			}
		}

		float StaticStatCalculator::CalculatePhysicalCriticalFactorNormalWith(const uint32 vitality) const
		{
			return static_cast<float>(vitality) * 0.00005f;
		}

		float StaticStatCalculator::CalculatePhysicalCriticalFactorSpecificWith(const uint32 vitality) const
		{
			if (vitality < 500)
			{
				return static_cast<float>(vitality) * 0.0004f;
			}
			else
			{
				return (static_cast<float>(vitality) - 500.0f) * 0.001f + 0.2f;
			}
		}

		float StaticStatCalculator::CalculateMagicalCriticalFactorSpecificWith(const uint32 intelligence) const
		{
			if (intelligence >= 500)
			{
				return static_cast<float>(intelligence - 500) * 0.001f + 0.2f;
			}
			else
			{
				return static_cast<float>(intelligence) * 0.0004f;
			}
		}

		float StaticStatCalculator::CalculateMagicalCriticalFactorNormalWith(const uint32 intelligence) const
		{
			return static_cast<float>(intelligence) * 0.00005f;
		}

		float StaticStatCalculator::CalculateSkillDamageSpecificWithIntelligence(const uint32) const
		{
			return 0.0f;
		}

		float StaticStatCalculator::GetIncreasedRateFactor(const uint32 strength) const
		{
			if (strength > 2000)
			{
				return 0.5f;
			}
			else if (strength > 1500)
			{
				return 0.4f;
			}
			else if (strength > 1000)
			{
				return 0.3f;
			}
			else if (strength > 500)
			{
				return 0.2f;
			}

			return 0.1f;
		}

		float StaticStatCalculator::CalculateSkillDamageFactorWithStrength(const uint32 strength, const XRated::Constants::GameTypes gameType) const
		{
			if (gameType & XRated::Constants::GameTypes::PvpGameTypeMask)
			{
				if (strength > 500)
				{
					return 1.0f + (pow((0.6f * static_cast<float>(strength) - 300), 0.17f) * 10.f - 1.9f) * 0.01f;
				}
				else
				{
					return 1.0f + static_cast<float>(strength) / 330.0f * 0.1f;
				}
			}
			else
			{
				if (strength > 10000)
				{
					float value = (static_cast<float>(strength) - 1000.0f) / 1000.0f * 1.35f;

					return static_cast<float>(value * value * 0.03f) + (0.0008f * static_cast<float>(strength)) + 8.28f;
				}
				else if (strength >= 3000)
				{

					float value = (static_cast<float>(strength) - 1000.0f) / 500.0f;

					return static_cast<float>(value * value * 0.03f) + (0.0008f * static_cast<float>(strength)) + 2.99f;
				}
				else
				{
					return 1.0f + static_cast<float>(strength) / 330.0f * GetIncreasedRateFactor(strength);
				}
			}
			//#endif
		}

		float StaticStatCalculator::CalculateHealIncRate(const uint32 intelligence) const
		{
			if (intelligence >= 500)
			{
				return 1.0f + (0.00003f * static_cast<float>(intelligence) * static_cast<float>(intelligence) + 0.27f * static_cast<float>(intelligence) - 95.0f) / 100.0f;
			}
			else
			{
				return 1.0f + static_cast<float>(intelligence) / 330.0f * GetIncreasedRateFactor(intelligence);
			}

		}

		float StaticStatCalculator::CalculateMaxHp(const uint32 vitality, const XRated::Constants::GameTypes gameType) const
		{
			if (gameType & XRated::Constants::GameTypes::PvpGameTypeMask)
			{
				if (vitality >= 500)
				{
					return pow((static_cast<float>(vitality) - 365.f), 0.17f) * 5300.f;
				}
				else
				{
					return static_cast<float>(vitality) * 25.f;
				}
			}
			else
			{
				if (vitality >= 500)
				{
					return static_cast<float>(vitality) * (0.0001f * static_cast<float>(vitality) + 8.0f);
				}
				else
				{
					return 0.0f;
				}
			}
		}

		float StaticStatCalculator::CalculateMaxMp(const uint32 intelligence, const XRated::Constants::GameTypes gameType) const
		{
			if (gameType & XRated::Constants::GameTypes::PvpGameTypeMask)
			{
				if (intelligence >= 500)
				{
					return pow((static_cast<float>(intelligence) - 365.f), 0.17f) * 3180;
				}
				else
				{
					return static_cast<float>(intelligence) * 15.f;
				}
			}
			else
			{
				if (intelligence >= 500)
				{
					return static_cast<float>(intelligence) * (0.0001f * intelligence + 8.0f);
				}
				else
				{
					return 0.0f;
				}
			}
		}
	}
}