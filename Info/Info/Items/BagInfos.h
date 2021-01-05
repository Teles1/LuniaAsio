#pragma once
#include <Core/Serializer/Serializer.h>
#include <Core/GameConstants.h>
#include <Info/Info/Items/Item.h>
#include <Info/Info/ETC/SwapSetInfo.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class BagInfos
				{
					struct BagInfo
						: public Serializer::ISerializable
					{
						std::pair< uint8, uint8 > BagNumberRanges;
						bool	IsDefault;

					public:
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};

					typedef std::map< int/*BagType*/, std::vector<BagInfo> > Bags;
					typedef Bags::const_iterator	BagIter;
					Bags inventoryBags;
					Bags bankBags;

				public:
					enum BagType
					{
						Normal,
						Cash,
						Fame,
						Key,
						Equip,
						MaxCnt,
					};

					bool EqualBagType(BagType type, uint8 bagNumber);
					bool EqualBankType(BagType type, uint8 bagNumber);
					BagType GetBagType(uint8 bagNumber);
					BagType GetBankType(uint8 bagNumber);

					void GetDefaultBank(std::vector<uint8>& banks);
					void GetDefaultBag(std::vector<uint8>& bags);

					void GetBankNumbers(BagType type, std::vector<uint8>& bags);
					void GetBagNumbers(BagType type, std::vector<uint8>& banks);

					bool UsableItem(const BagType& type, const ItemInfo* info);

				private:
					std::vector< SwapSetInfo > swapEquipSetInfos;
					std::vector< SwapSetInfo > swapCashSetInfos;
					typedef std::vector< SwapSetInfo >::const_iterator	SetIter;

				public:
					uint8 SetIndexToBagNumber(uint8 setIndex);
					uint8 BagNumberToSetIndex(uint8 bagNumber);
					std::wstring	GetEquipSetName(uint8 setIndex);
					uint8	GetNextEquipIndex(uint8 setIndex);

					uint8 SetCashIndexToBagNumber(uint8 setIndex);
					uint8 BagNumberToSetCashIndex(uint8 bagNumber);
					std::wstring GetCashEquipSetName(uint8 setIndex);
					uint8 GetNextCashEquipIndex(uint8 setIndex);

				public:
					void Init(const bool& xml);
					void Load(bool xml);
					void Save(bool xml);
				};

			}
		}
	}
}