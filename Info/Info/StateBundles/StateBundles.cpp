#pragma once
#include "StateBundles.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void StateBundleInfo::State::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::StateBundleInfo::State");
					std::wstring strType;
					StateInfo::TypeToString(type, strType);
					out.Write(L"type", strType);
					out.Write(L"params", params);
				}

				void StateBundleInfo::State::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::StateBundleInfo::State");
					std::wstring strType;
					in.Read(L"type", strType);
					type = StateInfo::StringToType(strType.c_str());
					in.Read(L"params", params, std::map<std::wstring, std::wstring>());
				}

				void StateBundleInfo::Condition::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::StateBundleInfo::Condition");
					out.Write(L"startTime", startTime);
					out.Write(L"name", name);
					out.Write(L"rangeType", (int)rangeType);
					out.Write(L"targetType", targetType);
					out.Write(L"targetNpcType", targetNpcType);
					out.Write(L"params", params);
				}

				void StateBundleInfo::Condition::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::StateBundleInfo::Condition");
					in.Read(L"startTime", startTime);
					in.Read(L"name", name);
					hash = StringUtil::Hash(name.c_str());
					in.Read(L"rangeType", (int&)rangeType);
					in.Read(L"targetType", targetType, (int)Constants::AttackTargetType::All);
					in.Read(L"targetNpcType", targetNpcType, (uint8)0x7F);
					in.Read(L"params", params);
				}

				void StateBundleInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::StateBundleInfo");
					out.Write(L"name", name);
					out.Write(L"level", level);
					out.Write(L"categoryName", categoryName);
					out.Write(L"subCategoryName", subCategoryName);
					out.Write(L"sort", sort);
					out.Write(L"duration", duration);
					out.Write(L"allianceType", (int)allianceType);
					out.Write(L"buffRate", buffRate);
					out.Write(L"debuffRate", debuffRate);
					out.Write(L"states", states);
					out.Write(L"EffectPath", EffectPath);
					out.Write(L"morphBundle", (int)morphBundle);
				}

				void StateBundleInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::StateBundleInfo");
					in.Read(L"name", name);
					in.Read(L"level", level);
					in.Read(L"categoryName", categoryName);
					in.Read(L"subCategoryName", subCategoryName, std::wstring(L"Default"));
					in.Read(L"sort", sort);
					in.Read(L"duration", duration);
					in.Read(L"allianceType", (int&)allianceType);
					in.Read(L"buffRate", buffRate);
					in.Read(L"debuffRate", debuffRate);
					in.Read(L"states", states, StateList());
					in.Read(L"EffectPath", EffectPath, Locator(L""));
					
					hash = StringUtil::Hash(name.c_str());
					categoryHash = StringUtil::Hash(categoryName.c_str());
					subCategoryHash = StringUtil::Hash(subCategoryName.c_str());

					int tmp;
					in.Read(L"morphBundle", tmp, 0);
					morphBundle = tmp ? true : false;
				}

			}
		}
	}
}