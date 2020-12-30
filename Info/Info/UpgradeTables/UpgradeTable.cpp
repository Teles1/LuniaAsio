#pragma once
#include "UpgradeTable.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				bool UpgradeTableInfo::NewInfoFinder::operator()(const UpgradeTableInfo& info) const
				{
					return ((info.ClassType == XRated::Constants::ClassType::AnyClassType ||
						Class == XRated::Constants::ClassType::AnyClassType ||
						info.ClassType == Class) &&
						info.EquipParts == Parts);
				}

				void UpgradeTableInfo::ProbabilityTable::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::UpgradeTableInfo::ProbabilityTable");
					out.Write(L"Id", Id);
					out.Write(L"StackedCount", StackedCount);
					out.Write(L"Probability", Probability);
					out.Write(L"ExpireMin", static_cast<uint32>(ExpireMin));
					out.Write(L"CashEnchant1Index", CashEnchant1Index);
					out.Write(L"CashEnchant2Index", CashEnchant2Index);
					out.Write(L"CashEnchant3Index", CashEnchant3Index);
					out.Write(L"CashEnchant4Index", CashEnchant4Index);
					out.Write(L"Instance", static_cast<uint32>(Instance)); //this might be datetime.
					out.Write(L"RestoreBelonging", RestoreBelonging);
				}

				void UpgradeTableInfo::ProbabilityTable::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::UpgradeTableInfo::ProbabilityTable");
					in.Read(L"Id", Id);
					in.Read(L"StackedCount", StackedCount, static_cast<uint8>(1));
					in.Read(L"Probability", Probability);
					in.Read(L"ExpireMin", ExpireMin, static_cast<uint32>(0));
					in.Read(L"CashEnchant1Index", CashEnchant1Index, static_cast<uint16>(0));
					in.Read(L"CashEnchant2Index", CashEnchant2Index, static_cast<uint16>(0));
					in.Read(L"CashEnchant3Index", CashEnchant3Index, static_cast<uint16>(0));
					in.Read(L"CashEnchant4Index", CashEnchant4Index, static_cast<uint16>(0));
					uint32 temp(0);
					in.Read(L"Instance", temp);
					Instance = static_cast<uint64>(temp);
					/* Enchant::NormalBitfields field(Instance);
					if(CashEnchant3Index == 0)
						CashEnchant3Index = field.CashEnchant3;
					if(CashEnchant4Index == 0)
						CashEnchant4Index = field.CashEnchant4; */
					in.Read(L"RestoreBelonging", RestoreBelonging, true);
				}

				void UpgradeTableInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::UpgradeTableInfo");
					out.Write(L"Id", Id);
					out.WriteEnum(L"ClassType", ClassType);
					out.WriteEnum(L"EquipParts", EquipParts);
					out.WriteEnum(L"ClassSpecializedRate", ClassSpecializedRate);
					out.Write(L"ProbabilityTables", ProbabilityTables);
				}

				void UpgradeTableInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::UpgradeTableInfo");
					in.Read(L"Id", Id, std::wstring());
					in.ReadEnum(L"ClassType", ClassType, Constants::ClassType::AnyClassType);
					in.ReadEnum(L"EquipParts", EquipParts, Constants::EquipParts::MaxPartCnt);
					in.ReadEnum(L"ClassSpecializedRate", ClassSpecializedRate, Constants::ClassType::AnyClassType);
					in.Read(L"ProbabilityTables", ProbabilityTables);
				}
			}
		}
	}
}
