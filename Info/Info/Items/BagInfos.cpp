#include "BagInfos.h"
#include <Info/Info/Enchant/Enchant.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				void BagInfos::BagInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::BagInfos::BagInfo");
					out.Write(L"BagNumberRanges", BagNumberRanges);
					out.Write(L"IsDefault", IsDefault);
				}

				void BagInfos::BagInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::BagInfos::BagInfo");
					in.Read(L"BagNumberRanges", BagNumberRanges);
					in.Read(L"IsDefault", IsDefault);
				}

				bool BagInfos::EqualBagType(BagInfos::BagType type, uint8 bagNumber)
				{
					BagIter it = inventoryBags.find(static_cast<int>(type));
					if (it != inventoryBags.end())
					{
						for (size_t rangeIndex = 0; rangeIndex < it->second.size(); ++rangeIndex)
						{
							if (bagNumber >= it->second[rangeIndex].BagNumberRanges.first && bagNumber <= it->second[rangeIndex].BagNumberRanges.second)
							{
								return true;
							}
						}
					}
					return false;
				}

				bool BagInfos::EqualBankType(BagInfos::BagType type, uint8 bankBagNumber)
				{
					BagIter it = bankBags.find(static_cast<int>(type));
					if (it != bankBags.end())
					{
						for (size_t rangeIndex = 0; rangeIndex < it->second.size(); ++rangeIndex)
						{
							if (bankBagNumber >= it->second[rangeIndex].BagNumberRanges.first && bankBagNumber <= it->second[rangeIndex].BagNumberRanges.second)
							{
								return true;
							}
						}
					}
					return false;
				}

				BagInfos::BagType BagInfos::GetBagType(uint8 bagNumber)
				{
					for (size_t i = 0; i < BagType::MaxCnt; ++i)
					{
						if (EqualBagType(static_cast<BagType>(i), bagNumber))
						{
							return static_cast<BagType>(i);
						}
					}

					return BagInfos::Normal;
				}

				BagInfos::BagType BagInfos::GetBankType(uint8 bagNumber)
				{
					for (size_t i = 0; i < BagType::MaxCnt; ++i)
					{
						if (EqualBankType(static_cast<BagType>(i), bagNumber))
						{
							return static_cast<BagType>(i);
						}
					}

					return BagInfos::Normal;
				}

				void BagInfos::GetBankNumbers(BagType type, std::vector<uint8>& banks)
				{
					banks.clear();

					BagIter it = bankBags.find(static_cast<int>(type));
					if (it != bankBags.end())
					{
						for (size_t Index = 0; Index < it->second.size(); ++Index)
						{
							for (size_t rangeIndex = it->second[Index].BagNumberRanges.first; rangeIndex <= it->second[Index].BagNumberRanges.second; ++rangeIndex)
							{
								banks.push_back((uint8)rangeIndex);
							}
						}
					}
				}

				void BagInfos::GetBagNumbers(BagType type, std::vector<uint8>& bags)
				{
					bags.clear();

					BagIter it = inventoryBags.find(static_cast<int>(type));
					if (it != inventoryBags.end())
					{
						for (size_t Index = 0; Index < it->second.size(); ++Index)
						{
							for (size_t rangeIndex = it->second[Index].BagNumberRanges.first; rangeIndex <= it->second[Index].BagNumberRanges.second; ++rangeIndex)
							{
								bags.push_back((uint8)rangeIndex);
							}
						}
					}
				}

				void BagInfos::GetDefaultBank(std::vector<uint8>& banks)
				{
					banks.clear();
					for (BagIter it = bankBags.begin(), endIt = bankBags.end(); it != endIt; ++it)
					{
						for (size_t Index = 0; Index < it->second.size(); ++Index)
						{
							if (it->second[Index].IsDefault)
							{
								for (size_t rangeIndex = it->second[Index].BagNumberRanges.first; rangeIndex <= it->second[Index].BagNumberRanges.second; ++rangeIndex)
								{
									banks.push_back((uint8)rangeIndex);
								}
							}
						}
					}
				}

				void BagInfos::GetDefaultBag(std::vector<uint8>& bags)
				{
					bags.clear();
					for (BagIter it = inventoryBags.begin(), endIt = inventoryBags.end(); it != endIt; ++it)
					{
						for (size_t Index = 0; Index < it->second.size(); ++Index)
						{
							if (it->second[Index].IsDefault)
							{
								for (size_t rangeIndex = it->second[Index].BagNumberRanges.first; rangeIndex <= it->second[Index].BagNumberRanges.second; ++rangeIndex)
								{
									bags.push_back((uint8)rangeIndex);
								}
							}
						}
					}
				}
				bool BagInfos::UsableItem(const BagType& type, const Database::Info::ItemInfo* info) {
					switch (type) {
					case BagType::Key:
					{
						if (info->MarketCategory == 404)
						{
							return true;
						}
						return false;
					}
					case BagType::Fame:
					{
						if (info->MarketCategory == 901)
						{
							return true;
						}
						return false;
					}
					case BagType::Cash:
					{
						if (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CASH)
						{
							return true;
						}
						if (info->EquipParts >= XRated::Constants::EquipParts::CASH_WEAPON && info->EquipParts <= XRated::Constants::EquipParts::CASH_ETC2)
							return true;

						return false;
					}
					case BagType::Normal:
						return true;
					case BagType::Equip:
						Enchant::IsEnchantable(info);
					}
					return false;
				}

				void BagInfos::Init(const bool& xml)
				{
					Load(xml);
				}

				void BagInfos::Load(bool xml)
				{
					if (xml == true)
					{
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Bags/Bags.xml");
						reader->Read(L"InventoryBags", inventoryBags);
						reader->Read(L"BankBags", bankBags);
						reader->Read(L"SwapEquipSetInfos", swapEquipSetInfos);
						reader->Read(L"SwapCashSetInfos", swapCashSetInfos);
					}
					else
					{
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerBinaryStreamReader(L"Database/Bags.b");
						reader->Read(L"InventoryBags", inventoryBags);
						reader->Read(L"BankBags", bankBags);
						reader->Read(L"SwapEquipSetInfos", swapEquipSetInfos);
						reader->Read(L"SwapCashSetInfos", swapCashSetInfos);
					}
				}

				void BagInfos::Save(bool xml)
				{
					if (xml == true)
					{
						Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/Items/Bags/Bags.xml");
						writer->Write(L"InventoryBags", inventoryBags);
						writer->Write(L"BankBags", bankBags);
						writer->Write(L"SwapEquipSetInfos", swapEquipSetInfos);
						writer->Write(L"SwapCashSetInfos", swapCashSetInfos);
					}
					else
					{
						Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerBinaryStreamWriter(L"Database/Bags.b");
						writer->Write(L"InventoryBags", inventoryBags);
						writer->Write(L"BankBags", bankBags);
						writer->Write(L"SwapEquipSetInfos", swapEquipSetInfos);
						writer->Write(L"SwapCashSetInfos", swapCashSetInfos);
					}
				}

				uint8 BagInfos::SetIndexToBagNumber(uint8 setIndex)
				{
					SetIter it = std::find_if(swapEquipSetInfos.begin(), swapEquipSetInfos.end(), SwapSetInfo::FindByIndex(setIndex));
					if (it != swapEquipSetInfos.end())
					{
						return it->BagNumber;
					}
					return 0;
				}

				uint8 BagInfos::BagNumberToSetIndex(uint8 bagNumber)
				{
					SetIter it = std::find_if(swapEquipSetInfos.begin(), swapEquipSetInfos.end(), SwapSetInfo::FindByBagNumber(bagNumber));
					if (it != swapEquipSetInfos.end())
					{
						return it->SetIndex;
					}
					return 0;
				}

				std::wstring BagInfos::GetEquipSetName(uint8 setIndex)
				{
					SetIter it = std::find_if(swapEquipSetInfos.begin(), swapEquipSetInfos.end(), SwapSetInfo::FindByIndex(setIndex));
					if (it != swapEquipSetInfos.end())
					{
						return it->EquipSetName;
					}

					return L"";
				}

				uint8 BagInfos::GetNextEquipIndex(uint8 setIndex)
				{
					std::vector< SwapSetInfo >::iterator it = std::find_if(swapEquipSetInfos.begin(), swapEquipSetInfos.end(), SwapSetInfo::FindByIndex(setIndex));
					if (it != swapEquipSetInfos.end())
					{
						if (std::distance(swapEquipSetInfos.begin(), it) + 1 >= (__int64)swapEquipSetInfos.size())
						{
							return 0;
						}
						else
						{
							return (++it)->SetIndex;
						}
					}
					return 0;
				}

				uint8 BagInfos::SetCashIndexToBagNumber(uint8 setIndex)
				{
					SetIter it = std::find_if(swapCashSetInfos.begin(), swapCashSetInfos.end(), SwapSetInfo::FindByIndex(setIndex));
					if (it != swapCashSetInfos.end())
					{
						return it->BagNumber;
					}
					return 0;
				}

				uint8 BagInfos::BagNumberToSetCashIndex(uint8 bagNumber)
				{
					SetIter it = std::find_if(swapCashSetInfos.begin(), swapCashSetInfos.end(), SwapSetInfo::FindByBagNumber(bagNumber));
					if (it != swapCashSetInfos.end())
					{
						return it->SetIndex;
					}
					return 0;
				}

				std::wstring BagInfos::GetCashEquipSetName(uint8 setIndex)
				{
					SetIter it = std::find_if(swapCashSetInfos.begin(), swapCashSetInfos.end(), SwapSetInfo::FindByIndex(setIndex));
					if (it != swapCashSetInfos.end())
					{
						return it->EquipSetName;
					}

					return L"";
				}

				uint8 BagInfos::GetNextCashEquipIndex(uint8 setIndex)
				{
					std::vector< SwapSetInfo >::iterator it = std::find_if(swapCashSetInfos.begin(), swapCashSetInfos.end(), SwapSetInfo::FindByIndex(setIndex));
					if (it != swapCashSetInfos.end())
					{
						if (std::distance(swapCashSetInfos.begin(), it) + 1 >= (__int64)swapCashSetInfos.size())
						{
							return 0;
						}
						else
						{
							return (++it)->SetIndex;
						}
					}
					return 0;
				}

			}
		}
	}
}