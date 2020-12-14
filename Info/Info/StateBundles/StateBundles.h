#pragma once
#include <Info/Info/Loader.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct StateBundleInfo : public Serializer::ISerializable {
					const static int PassiveSlot = 4;
					struct State : public Serializer::ISerializable
					{
						StateInfo::Type	type;
						std::map<std::wstring, std::wstring> params;

					public:
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
					typedef std::vector<State> StateList;

					//Condition of activating StateBundle
					struct Condition : public Serializer::ISerializable {
						enum Circle { Radius = 0 };
						enum Segment { Range = 0 };
						enum Rectangle { Length = 0, Width = 1 };
						enum Fanshape { StartAngle = 1, EndAngle = 2 };

					public:
						float startTime;
						std::wstring name;
						uint32 hash;
						StateInfo::RangeType rangeType;
						int targetType; 
						uint8 targetNpcType;
						std::vector<float> params;

					public:
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
					typedef std::vector<Condition> ConditionList;

				public:
					std::wstring name;
					uint32 hash;
					uint16 level;
					std::wstring categoryName;
					uint32 categoryHash;
					std::wstring subCategoryName;
					uint32 subCategoryHash;
					uint8 sort;
					float duration;
					float buffRate;
					float debuffRate;
					Constants::AllianceType allianceType;
					StateList states;
					Locator EffectPath;
					bool morphBundle;

				public:
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}