#include "GuildInfoManager.h"
#include <Core/Resource/Resource.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void GuildInfoManager::Load(bool xml)
				{
					Resource::SerializerStreamReader reader;
					if (xml == true) {
						reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Guild/GuildInfo.xml");
					}
					else {
						reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(L"Database/GuildInfo.b");
					}
					reader->Read(L"General", general);
					reader->Read(L"LevelInfos", levelInfos);
					reader->Read(L"RankInfos", rankInfos);
				}
				void GuildInfoManager::Save(bool xml)
				{
					Resource::SerializerStreamWriter writer;
					if (xml == true) {
						writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/Guild/GuildInfo.xml");
					}
					else {
						writer = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(L"Database/GuildInfo.b");
					}
					writer->Write(L"General", general);
					writer->Write(L"LevelInfos", levelInfos);
					writer->Write(L"RankInfos", rankInfos);
				}

				void GuildInfoManager::Init(bool xml)
				{
					Load(xml);
				}

				const GuildInfo::Level* GuildInfoManager::GetLevelInfo(uint8 level) const
				{
					if (level - 1 < 0)
					{
						LoggerInstance().Exception(L"Invalid Guild Level - Under 1");
					}

					if (levelInfos.size() > level - 1) {
						return &(levelInfos.at(level - 1));
					}
					return NULL;
				}
				const GuildInfo::Rank* GuildInfoManager::GetRankInfo(uint32 rank) const
				{
					if (rankInfos.empty()) return NULL;

					RankInfoList::const_iterator iter = rankInfos.find(rank);
					if (iter != rankInfos.end())
					{
						return &iter->second;
					}

					return NULL;
				}
				const GuildInfo::General& GuildInfoManager::GetGeneral() const
				{
					return general;
				}
				uint32 GuildInfoManager::GetMaxGuildPoint(uint8 currentLevel) const
				{
					if (currentLevel - 1 < 0)
					{
						LoggerInstance().Exception(L"Invalid Guild Level - Under 1");
						return 0;
					}

					return general.MaxGuildPoints.at(static_cast<int>(currentLevel) - 1);
				}
				uint32 GuildInfoManager::GetMaxGuildUserPerLevel(uint8 currentLevel) const
				{
					if (static_cast<int>(currentLevel) - 1 < 0)
					{
						LoggerInstance().Exception(L"Invalid Guild Level - Under 1");
						return 0;
					}
					if (static_cast<uint32>(currentLevel) > general.MaxGuildUser.size())
					{
						LoggerInstance().Exception(L"Invalid Guild Level - {0}", currentLevel);
						return 0;
					}
					return general.MaxGuildUser.at(static_cast<int>(currentLevel) - 1);
				}
				std::pair<bool, uint32> GuildInfoManager::GetNextLevelExp(uint8 currentLevel) const
				{
					if (currentLevel - 1 < 0)
					{
						LoggerInstance().Exception(L"Invalid Guild Level - Under 1");
						return std::pair<bool, uint32>(false, 0);
					}

					std::pair<bool, uint32> returnValue(false, 0);
					if (general.LevelUpExps.size() > currentLevel - 1) {
						returnValue.first = true;
						returnValue.second = general.LevelUpExps.at(currentLevel - 1);
						return returnValue;
					}
					return returnValue;
				}
				DateTime GuildInfoManager::GetGuildUserExpAddTime(DateTime startTime, float userExp) const
				{
					if (general.MinuteForExp > userExp) {
						uint32 needMinute = general.MinuteForExp - static_cast<uint32>(userExp);
						startTime.Add(DateTime::Unit::Second, needMinute * 60);
						return startTime;
					}
					return DateTime::Now();
				}

				float GuildInfoManager::GetShopDiscountRate(uint16 rank) const
				{
					if (rank < 1 || rank > general.DiscountRate.size())
						return 1.0f;
					return general.DiscountRate.at(rank - 1);
				}

				std::pair<float, uint32> GuildInfoManager::CalculdateCurrentUserExp(DateTime start, DateTime currentTime, float oldPlayTime) const
				{
					static uint32 secondForExp = general.MinuteForExp * 60;
					float playTime = CalculatePlayTime(start, currentTime, oldPlayTime);
					std::pair<float, uint32> result;

					result.first = float((uint64)playTime % secondForExp);
					result.second = (uint32)(playTime / secondForExp);
					return result;
				}

				float GuildInfoManager::CalculatePlayTime(DateTime start, DateTime now, float oldPlayTime)
				{
					const static int secondOfDay = 60 * 60 * 24;
					oldPlayTime += (float)now.GetTime().GetCumulatedSec() - (float)start.GetTime().GetCumulatedSec();
					oldPlayTime += (float)(now.GetDate().GetCumulatedDay() - start.GetDate().GetCumulatedDay()) * secondOfDay;
					return oldPlayTime;
				}
				void GuildInfoManager::SortSellItemList()
				{
					{
						//Level
						levelToSellItems.clear();
						levelToSellItems.resize(levelInfos.size());

						LevelToSellItemList::iterator levelToSellItemIter = levelToSellItems.begin();

						LevelInfoList::const_iterator iter = levelInfos.begin();
						LevelInfoList::const_iterator end = levelInfos.end();
						while (iter != end) {

							SellItemLists& items = *levelToSellItemIter;

							ShopInfo::CategoryList::const_iterator categoryIter = (*iter).ShopCategorys.begin();
							ShopInfo::CategoryList::const_iterator categoryEnd = (*iter).ShopCategorys.end();
							while (categoryIter != categoryEnd) {
								ShopInfo::Category::ItemList::const_iterator itemIter = (*categoryIter).Items.begin();
								ShopInfo::Category::ItemList::const_iterator itemEnd = (*categoryIter).Items.end();
								while (itemIter != itemEnd) {
									items.push_back((*itemIter).ItemHash);
									++itemIter;
								}
								++categoryIter;
							}
							++iter;
							++levelToSellItemIter;
						}
					}

					{
						//Rank
						rankToSellItems.clear();

						RankInfoList::const_iterator rankIter = rankInfos.begin();
						RankInfoList::const_iterator rankEnd = rankInfos.end();
						while (rankIter != rankEnd)
						{
							SellItemLists items;

							ShopInfo::CategoryList::const_iterator categoryIter = rankIter->second.ShopCategorys.begin();
							ShopInfo::CategoryList::const_iterator categoryEnd = rankIter->second.ShopCategorys.end();
							while (categoryIter != categoryEnd) {
								ShopInfo::Category::ItemList::const_iterator itemIter = (*categoryIter).Items.begin();
								ShopInfo::Category::ItemList::const_iterator itemEnd = (*categoryIter).Items.end();
								while (itemIter != itemEnd) {
									items.push_back((*itemIter).ItemHash);
									++itemIter;
								}
								++categoryIter;
							}

							rankToSellItems[rankIter->first] = items;

							++rankIter;
						}
					}
				}

				bool GuildInfoManager::IsShopItem(uint8 guildLevel, uint32 guildRank, uint32 hash) const
				{
					if (levelToSellItems.size() > guildLevel - 1) {
						const SellItemLists& sellItems = (levelToSellItems.at(guildLevel - 1));
						SellItemLists::const_iterator iter = std::find(sellItems.begin(), sellItems.end(), hash);
						if (iter != sellItems.end()) {
							return true;
						}
					}

					if (rankToSellItems.empty()) return false;

					RankToSellItemList::const_iterator iter = rankToSellItems.find(guildRank);
					if (iter != rankToSellItems.end())
					{
						SellItemLists::const_iterator itemIter = std::find(iter->second.begin(), iter->second.end(), hash);
						if (itemIter != iter->second.end()) {
							return true;
						}
					}

					return false;
				}
			}
		}
	}
}
