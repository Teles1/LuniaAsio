#pragma once
#include "MISCS.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void MiscInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::MiscInfo");
					out.Write(L"Name", Name);
					out.Write(L"DisplayName", DisplayName);
					out.Write(L"Radius", Radius);
					out.Write(L"Hp", Hp);
					out.Write(L"BonusBox", BonusBox);
					out.Write(L"Items", Items);
					out.Write(L"Scale", Scale);
					out.Write(L"AnimationPath", AnimationPath);
					out.Write(L"EffectCategory", EffectCategory);
					out.Write(L"EffectPath", EffectPath);
					out.Write(L"Jewels", Jewels);
					out.Write(L"States", States);
					out.Write(L"Description", Description);
				}

				void MiscInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::MiscInfo");
					in.Read(L"Name", Name);
					NameHash = StringUtil::Hash(Name.c_str());
					in.Read(L"DisplayName", DisplayName);
					in.Read(L"Radius", Radius);
					in.Read(L"Hp", Hp);
					in.Read(L"BonusBox", BonusBox, false);
					in.Read(L"Items", Items);
					in.Read(L"Scale", Scale);
					in.Read(L"AnimationPath", AnimationPath);
					in.Read(L"EffectCategory", EffectCategory);
					in.Read(L"EffectPath", EffectPath, Locator(L""));
					in.Read(L"Jewels", Jewels);
					in.Read(L"States", States, std::vector<ActionInfo::State>());
					in.Read(L"Description", Description, std::wstring());
				}
			}
		}
	}
}