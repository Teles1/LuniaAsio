#pragma once
#include "Projectile.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void ProjectileInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::ProjectileInfo");
					out.Write(L"Name", Name);
					out.Write(L"AnimationPath", AnimationPath);
					out.Write(L"EffectCategory", EffectCategory);
					out.Write(L"EffectPath", EffectPath);
					out.Write(L"DestroyEffectPath", DestroyEffectPath);
					out.Write(L"Scale", Scale);
					out.Write(L"Radius", Radius);
					out.Write(L"Speed", Speed);
					out.Write(L"Duration", Duration);
					out.Write(L"ShootType", (int)ShootType);
					out.Write(L"DestroyWhenHit", (int)DestroyWhenHit);
					out.Write(L"HitDelay", HitDelay);
					out.Write(L"SpinLimit", SpinLimit);
					out.Write(L"AllianceType", (int)AllianceType);
					out.Write(L"States", States);
					out.Write(L"StateBundleConditions", StateBundleConditions);
				}

				void ProjectileInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::ProjectileInfo");
					in.Read(L"Name", Name);
					Hash = StringUtil::Hash(Name.c_str());
					in.Read(L"AnimationPath", AnimationPath);
					in.Read(L"EffectCategory", EffectCategory, std::wstring());
					in.Read(L"EffectPath", EffectPath, Locator(L""));
					in.Read(L"DestroyEffectPath", DestroyEffectPath, Locator());
					in.Read(L"Scale", Scale);
					in.Read(L"Radius", Radius);
					in.Read(L"Speed", Speed);
					in.Read(L"Duration", Duration);
					in.Read(L"ShootType", (int&)ShootType);
					int destroy;
					in.Read(L"DestroyWhenHit", destroy);
					DestroyWhenHit = destroy ? true : false;
					in.Read(L"HitDelay", HitDelay, 0.5f);
					in.Read(L"SpinLimit", SpinLimit);
					in.Read(L"AllianceType", (int&)AllianceType, 2 /* ExTeam */);
					in.Read(L"States", States);
					in.Read(L"StateBundleConditions", StateBundleConditions, StateBundleInfo::ConditionList());
				}
			}
		}
	}
}