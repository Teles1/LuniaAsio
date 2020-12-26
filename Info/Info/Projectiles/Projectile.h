#pragma once
#include <Info/Info/IndexedInfoManager.h>
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>
#include <Info/Info/StateBundles/StateBundlesManager.h>
#include <Info/Info/Actions/ActionsManager.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct ProjectileInfo : public Serializer::ISerializable {
					enum ShootingType {
						FOLLOWTARGET = 0, STRAIGHT = 1, HIGHANGLE = 2, FALL = 3, FLOATING = 4, CIRCULAR = 5, HEAL = 6
					};

					std::wstring Name;
					uint32 Hash;
					std::wstring AnimationPath;
					std::wstring EffectCategory;
					Locator EffectPath, DestroyEffectPath;
					float3 Scale;

					float Radius, Speed, Duration;
					ShootingType ShootType;
					bool DestroyWhenHit;
					float HitDelay;
					uint16 SpinLimit;
					Constants::AllianceType AllianceType;
					std::vector<ActionInfo::State> States;
					StateBundleInfo::ConditionList StateBundleConditions;

				public:
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}