#include "Enchanter.h"
#include <Core/Utils/Math/Random.h>
#include <Info/Info.h>

namespace Lunia { namespace XRated { namespace StageServer {

	IdentifyEnchanter::IdentifyEnchanter()
		: status( Database::DatabaseInstance().InfoCollections.EnchantTables.GetIdentifyStatusTypeProb().begin(), Database::DatabaseInstance().InfoCollections.EnchantTables.GetIdentifyStatusTypeProb().end() )
		, levels( Database::DatabaseInstance().InfoCollections.EnchantTables.GetIdentifyGradeProb().begin(), Database::DatabaseInstance().InfoCollections.EnchantTables.GetIdentifyGradeProb().end() )
	{
	}

	Database::Enchant::EnchantBitfields IdentifyEnchanter::operator()(const Database::Info::ItemInfo& /*info*/, Database::Enchant::EnchantBitfields source/*, int identifyIndex*/) const
	{
		source.Identified = 1; // mark as identified
		source.NormalIdentified = 1;
		source.Option1 = status.Next();
		source.Option2 = status.Next();
		source.Option3 = status.Next();
		source.Level1 = levels.Next();
		source.Level2 = levels.Next();
		source.Level3 = levels.Next();
			//switch(identifyIndex)
			//{
			//case 0: // Identified1
			//	source.Option1 = status.Next();
			//	source.Level1 = levels.Next();
			//	source.Identified1 = 1; // mark as identified
			//	break;

			//case 1: // Identified2
			//	source.Option2 = status.Next();
			//	source.Level2 = levels.Next();
			//	source.Identified2 = 1; // mark as identified
			//	break;

			//case 2: // Identified3
			//	source.Option3 = status.Next();
			//	source.Level3 = levels.Next();
			//	source.Identified3 = 1; // mark as identified
			//	break;

			//default:
			//	ALLM_WARNING((L"invalid identify index(%d)", identifyIndex));
			//	break;
			//}

		return source;
	}

	AdvancedIdentifyEnchanter::AdvancedIdentifyEnchanter()
		: status( Database::DatabaseInstance().InfoCollections.EnchantTables.GetAdvancedIdentifyStatusTypeProb().begin(), Database::DatabaseInstance().InfoCollections.EnchantTables.GetAdvancedIdentifyStatusTypeProb().end() )
		, levels( Database::DatabaseInstance().InfoCollections.EnchantTables.GetAdvancedIdentifyGradeProb().begin(), Database::DatabaseInstance().InfoCollections.EnchantTables.GetAdvancedIdentifyGradeProb().end() )
	{
	}

	Database::Enchant::EnchantBitfields AdvancedIdentifyEnchanter::operator()(const Database::Info::ItemInfo& /*info*/, Database::Enchant::EnchantBitfields source, const std::vector< uint32 >& statusTypeInfo/*, int identifyIndex*/) const
	{
		source.Identified = 1; // mark as identified
		source.NormalIdentified = 1;

		source.Level1 = levels.Next();
		source.Level2 = levels.Next();
		source.Level3 = levels.Next();

		if( statusTypeInfo.size() > 0 )
		{
			source.Option1 = statusTypeInfo[ 0 ];
			if( source.Level1 < 2 )
			{
				source.Level1 = 2;
			}
		}
		else
		{
			source.Option1 = status.Next();
		}

		if( statusTypeInfo.size() > 1 )
		{
			source.Option2 = statusTypeInfo[ 1 ];
			if( source.Level2 < 2 )
			{
				source.Level2 = 2;
			}
		}
		else
		{
			source.Option2 = status.Next();
		}

		if( statusTypeInfo.size() > 2 )
		{
			source.Option3 = statusTypeInfo[ 2 ];
			if( source.Level3 < 2 )
			{
				source.Level3 = 2;
			}
		}
		else
		{
			source.Option3 = status.Next();
		}

		

		return source;
	}

	Database::Enchant::EnchantBitfields AdvancedIdentifyEnchanter::operator()(const Database::Info::ItemInfo& /*info*/, Database::Enchant::EnchantBitfields source/*, int identifyIndex*/) const
	{
		source.Identified = 1; // mark as identified
		source.NormalIdentified = 1;
		source.Option1 = status.Next();
		source.Option2 = status.Next();
		source.Option3 = status.Next();
		source.Level1 = levels.Next();
		source.Level2 = levels.Next();
		source.Level3 = levels.Next();

		return source;
	}

	CashIdentifyEnchanter::CashIdentifyEnchanter()
		: status( Database::DatabaseInstance().InfoCollections.EnchantTables.GetCashIdentifyStatusTypeProb().begin(), Database::DatabaseInstance().InfoCollections.EnchantTables.GetCashIdentifyStatusTypeProb().end() )
		, levels( Database::DatabaseInstance().InfoCollections.EnchantTables.GetCashIdentifyGradeProb().begin(), Database::DatabaseInstance().InfoCollections.EnchantTables.GetCashIdentifyGradeProb().end() )
	{
	}

	Database::Enchant::EnchantBitfields CashIdentifyEnchanter::operator()(const Database::Info::ItemInfo& /*info*/, Database::Enchant::EnchantBitfields source/*, int identifyIndex*/) const
	{
		source.Identified = 1; // mark as identified
		source.NormalIdentified = 1; // mark as identified
		source.Option1 = status.Next();
		source.Option2 = status.Next();
		source.Option3 = status.Next();
		source.Level1 = levels.Next();
		source.Level2 = levels.Next();
		source.Level3 = levels.Next();

		return source;
	}

	EnchantResult ReinforceEnchanter::operator()(const Database::Info::ItemInfo& info, Database::Enchant::EnchantBitfields source, uint32 catalystHash, uint16 numberOfCatalysts) const
	{

		Database::Enchant::EnchantTableManager& tables=Database::DatabaseInstance().InfoCollections.EnchantTables;

		uint32 minCatalystCount = 0;
		if ( Constants::IsAccessory( info.EquipParts ) )
		{
			minCatalystCount = tables.GetCatalystMinCountForReinforceOfAccessory(catalystHash, std::max<int>(info.Limits.Level, info.Limits.StoredLevel));
		}
		else
		{
			minCatalystCount = tables.GetCatalystMinCountForReinforce(catalystHash, std::max<int>(info.Limits.Level, info.Limits.StoredLevel));
		}
		const Database::Enchant::EnchantTableManager::ProbabilityInfo* probabilityInfo;
		if( minCatalystCount <= numberOfCatalysts )
		{
			probabilityInfo = tables.GetReinforcementProb(catalystHash);
		}
		else
		{
			probabilityInfo =  tables.GetReinforcementProb(0);
		}

		if( probabilityInfo == NULL ) 
		{
			return EnchantResult(0,EnchantResult::States::Error);
		}
		float distroyPorb= probabilityInfo->DestroyTable.at( source.Reinforcement );
			//tables.GetReinforcementDestroyProb()[ source.Reinforcement ];
		if (Math::RandomByRate(distroyPorb)) /* be destroyed */
		{
			source.DestroyedByReinforcement=1;
		}
		else /* no destroy */
		{
			float successProb=probabilityInfo->SuccessTable.at( source.Reinforcement );
				//tables.GetReinforcementSuccessProb()[ source.Reinforcement ];

			if( minCatalystCount <= numberOfCatalysts )
			{
				float catalystProb
					= tables.GetCatalystProbPerLevel(catalystHash, info.Limits.Level) // (10-(info.Limits.Level-1)/10)
					* tables.GetCatalystProbPerCnt(catalystHash, numberOfCatalysts); // (numberOfCatalysts/2.0f)

				successProb += catalystProb; /* applying catalyst */
			}

			if (Math::RandomByRate(successProb)) /* successed in reinforcement */
			{
				++source.Reinforcement;
			}
			else /* failed in reinforcement */
			{
				float gradeDownProb=probabilityInfo->GradeDownTable.at( source.Reinforcement );
					//tables.GetReinforcementGradeDownProb()[ source.Reinforcement ];
				if (Math::RandomByRate(gradeDownProb) && source.Reinforcement) /* be graded down */
				{
					--source.Reinforcement;
				} /* else no change */
			}
		}

		return EnchantResult(source,EnchantResult::States::Compleate);
	}



	std::pair<uint32/*ItemHash*/, uint16/*count*/> ExtractEnchanter::operator()(const Database::Info::ItemInfo& info, Database::Enchant::EnchantBitfields source) const
	{
		std::pair<uint32, uint16> ret(0, 0); /* first;item hash, second;count */
		if (Database::Enchant::IsEnchantable(&info))
		{
			Database::Enchant::EnchantTableManager& tables=Database::DatabaseInstance().InfoCollections.EnchantTables;
			bool jewelAcquired=false;

			uint16 level = std::max<uint16>( info.Limits.Level, info.Limits.StoredLevel );
			if (Math::RandomByRate(tables.GetExtractionJewelProb())) /* is the result jewel? */
			{
				bool next=source.LightLevel < XRated::Constants::LocaleSpecificSetting::LIGHTREINFORCEMENT_LEVEL /* maximum light level */ && Math::RandomByRate(tables.GetExtractionNextJewelProb()); /* next jewel? */

				if ( (source.LightLevel==0 && next==false) || (source.LightLevel==5 && next==true) )
				{
					/* avoiding invalid state - leave here empty */
				}
				else
				{
					if (next)
					{
						ret.first=Database::DatabaseInstance().InfoCollections.EnchantTables.GetConstants().HashJewel[ source.LightLevel ]; /* HashJewel starts with light leve 1 */
						ret.second=tables.RetrieveNextJewelCntPerLevel( level );
						ret.second+=tables.RetrieveNextJewelCntPerReinforcement(source.Reinforcement);
					}
					else
					{
						ret.first=Database::DatabaseInstance().InfoCollections.EnchantTables.GetConstants().HashJewel[ source.LightLevel - 1 ]; /* HashJewel starts with light leve 1 */
						ret.second=tables.RetrieveJewelCntPerLevel( level );
						ret.second+=tables.RetrieveJewelCntPerReinforcement(source.Reinforcement);
					}                
					jewelAcquired=true; // or return ret;
				}
			}

			if (jewelAcquired==false)
			{
				if (Math::RandomByRate( tables.GetExtractionMineralProb())) /* is the result mineral */
				{
					ret.first=Database::DatabaseInstance().InfoCollections.EnchantTables.GetConstants().HashMineral;
					ret.second=tables.RetrieveMineralCntPerLevel( level );
					ret.second+=tables.RetrieveMineralCntPerReinforcement(source.Reinforcement);
				}
				else /* it's catalyst */
				{
					ret.first=Database::DatabaseInstance().InfoCollections.EnchantTables.GetConstants().HashCatalyst;
					ret.second=tables.RetrieveCatalystCntPerLevel( level );
					ret.second+=tables.RetrieveCatalystCntPerReinforcement(source.Reinforcement);
				}
			}
		}
		else /* abroad specific - coin processing */
		{
			ret.first=Database::DatabaseInstance().InfoCollections.EnchantTables.GetConstants().HashCoin;

			ret.second = (info.SellPrice) / 1000 ;
			if( ret.second == 0 ) 
			{
				ret.second = 1;
			}
			else if( ret.second > 10 ) 
			{
				ret.second = 10;
			}

			switch(info.EquipParts)
			{
				case XRated::Constants::EquipParts::CASH_WEAPON:
				case XRated::Constants::EquipParts::CASH_HEAD:
				case XRated::Constants::EquipParts::CASH_CHEST:
				case XRated::Constants::EquipParts::CASH_LEG:
					ret.second *= 2;
					break;
			}
		}

		return ret;
	}


	EnchantResult LightReinforceEnchanter::operator()(const Database::Info::ItemInfo& info, Database::Enchant::EnchantBitfields source, uint32 catalystHash, uint16 numberOfCatalysts) const
	{
		Database::Enchant::EnchantTableManager& tables=Database::DatabaseInstance().InfoCollections.EnchantTables;

		uint32 minCatalystCount = 0;

		if ( Constants::IsAccessory( info.EquipParts ) )
		{
			minCatalystCount = tables.GetCatalystMinCountForLightReinforceOfAccessory(catalystHash, std::max<int>(info.Limits.Level, info.Limits.StoredLevel));
		}
		else
		{
			minCatalystCount = tables.GetCatalystMinCountForLightReinforce(catalystHash, std::max<int>(info.Limits.Level, info.Limits.StoredLevel));
		}

		const Database::Enchant::EnchantTableManager::ProbabilityInfo* probabilityInfo;

		if( minCatalystCount <= numberOfCatalysts )
		{
			probabilityInfo = tables.GetLightReinforcementProb(catalystHash);
		}
		else
		{
			probabilityInfo =  tables.GetLightReinforcementProb(0);
		}

		if( probabilityInfo == NULL ) 
		{
			return EnchantResult(0,EnchantResult::States::Error);
		}

		float distroyPorb=probabilityInfo->DestroyTable.at( source.LightLevel );
			//tables.GetLightDestroyProb()[ source.LightLevel ];
		if (Math::RandomByRate(distroyPorb)) /* be destroyed */
		{
			source.DestroyedByLightReinforcement=1;
		}
		else /* no destroy */
		{
			float successProb=probabilityInfo->SuccessTable.at( source.LightLevel );
			if( minCatalystCount <= numberOfCatalysts )
			{
					//tables.GetLightSuccessProb()[ source.LightLevel ];
				float catalystProb
					= tables.GetCatalystProbPerLevel(catalystHash, info.Limits.Level) // (10-(info.Limits.Level-1)/10)
					* tables.GetCatalystProbPerCnt(catalystHash, numberOfCatalysts); // (numberOfCatalysts/2.0f)
				
				successProb += catalystProb; /* applying catalyst */	
			}
			
			if (Math::RandomByRate(successProb)) /* successed in reinforcement */
			{
				++source.LightLevel;
			}
			else /* failed in reinforcement */
			{
				float gradeDownProb=probabilityInfo->GradeDownTable.at( source.LightLevel );
					//tables.GetLightGradeDownProb()[ source.LightLevel ];
				if (Math::RandomByRate(gradeDownProb) && source.LightLevel) /* be graded down */
				{
					--source.LightLevel;
				} /* else no change */
			}
		}

		return EnchantResult(source,EnchantResult::States::Compleate);
	}	


} } }
