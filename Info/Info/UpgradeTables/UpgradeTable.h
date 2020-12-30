#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct UpgradeTableInfo : public Serializer::ISerializable
				{
					struct ProbabilityTable : public Serializer::ISerializable {
						std::wstring Id;
						uint8 StackedCount;
						float Probability;

						/* in case of unenchantable item */
						uint32 ExpireMin; 
						uint16 CashEnchant1Index;
						uint16 CashEnchant2Index;
						uint16 CashEnchant3Index;
						uint16 CashEnchant4Index;
						// AllM Is trolling me man there is no fucking way.

						/* in case of enchantable item */
						uint64 Instance;

						bool RestoreBelonging;

					public: /* ISerializable implementation */
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};

					std::wstring Id;
					XRated::Constants::ClassType	ClassType;
					XRated::Constants::ClassType	ClassSpecializedRate;
					XRated::Constants::EquipParts	EquipParts;
					std::vector<ProbabilityTable> ProbabilityTables;
					typedef std::vector<ProbabilityTable>::iterator ProbIter;

				public:
					struct NewInfoFinder
					{
						XRated::Constants::ClassType	Class;
						XRated::Constants::EquipParts	Parts;

						NewInfoFinder(XRated::Constants::ClassType	classType, XRated::Constants::EquipParts equipParts)
							: Class(classType)
							, Parts(equipParts)
						{
						}
						bool operator()(const UpgradeTableInfo& info) const;
					};
				public: /* ISerializable implementation */
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}
