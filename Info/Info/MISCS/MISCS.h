#pragma once
#include <Core/Resource/Xml/XmlBase.h>
#include <Core/Serializer/Binary/XmlStreamWriter.h>
#include <Core/Resource/Xml/XmlParser.h>
#include <Info/Info/IndexedInfoManager.h>
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>
#include <Info/Info/NPCs/NPCManager.h>
#include <Info/Info/Actions/ActionsManager.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct MiscInfo : public Serializer::ISerializable
				{
					std::wstring Name, DisplayName;
					uint32 NameHash;
					float Radius;
					uint32 Hp;
					bool BonusBox;

					std::vector<NonPlayerInfo::Item> Items;

					//Local Specific
					float3 Scale;
					std::wstring AnimationPath;
					std::wstring EffectCategory;
					Locator EffectPath;
					std::vector<NonPlayerInfo::Jewel> Jewels;
					std::vector<ActionInfo::State> States;

					std::wstring Description;

				public: /* ISerializable implementation */
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};

			}
		}
	}
}