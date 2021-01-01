#include <StageServer/Common.h>
#include <StageServer/User/User.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			namespace Common {
				void ItemEx::operator=(const ItemEx& rhs) 
				{ 
					Info = rhs.Info; 
					Serial = rhs.Serial; 
					InstanceEx = rhs.InstanceEx;
					StackedCount = rhs.StackedCount;
				}
				bool ItemEx::operator==(const ItemEx& rhs) const
				{
					return (Info == rhs.Info && Serial == rhs.Serial 
						&& InstanceEx == rhs.InstanceEx 
						&& StackedCount == rhs.StackedCount); 
				}
				bool ItemEx::operator!=(const ItemEx& rhs) const
				{
					return !(*this == rhs);
				}
				bool ItemEx::IsTradable(UserSharedPtr user) const
				{
					if (!(Info->Attributes.Attr & XRated::Database::Info::ItemInfo::ATTRIBUTES::TRADE)) {
						if (Info->Attributes.Attr & XRated::Database::Info::ItemInfo::ATTRIBUTES::TRADEWHENEXPIRED)
						{
							if (Database::Enchant::IsEnchantable(Info) == false
								&& InstanceEx.IsExpired() == true) {
								if (user != NULL) {
									if (Database::DatabaseInstance().InfoCollections.Pets.IsPetItem(Info->Hash) == true)
									{
										Logger::GetInstance().Exception("Missing implementation.");
									}
									return true;
								}
								return false;
							}
						}
						return false;
					}
					else {
						if (!(Info->Attributes.Attr & XRated::Database::Info::ItemInfo::ATTRIBUTES::TRADEAFTERUSE)) {
							if (Database::Enchant::IsEnchantable(Info) == true
								&& static_cast<Database::Enchant::EnchantBitfields>(InstanceEx).HasEquipped == 1)
								return false;
							else if (Database::Enchant::IsCashEnchantable(*Info) == true
								&& static_cast<Database::Enchant::NormalBitfields>(InstanceEx).HasEquipped == 1)
								return false;
						}
					}
					return true;
				}

				GuildInfo& GuildInfo::operator =(const AllMGuildInfo& info)
				{
					GuildId = info.GuildId;
					GuildAlias = info.GuildAlias;
					GuildExp = info.GuildExp;
					Point = info.Point;
					ProfitStartDate = info.ShopOpenDate;
					ProfitEndDate = info.ShopCloseDate;
					GuildName = info.GuildName;
					Message = info.Message;
					MasterName = info.MasterName;
					GuildLevel = info.GuildLevel;
					MemberCount = info.MemberCount;
					CreatedDate = info.CreatedDate;
					return *this;
				}
				bool stItemSlotEx::operator==(const stItemSlotEx& rhs) const
				{
					return (ItemInfo == rhs.ItemInfo 
						&& serial == rhs.serial 
						&& IsBank == rhs.IsBank);
				}
				bool stItemSlotEx::operator!=(const stItemSlotEx& rhs) const
				{
					return (ItemInfo != rhs.ItemInfo 
						|| serial != rhs.serial 
						|| IsBank != rhs.IsBank 
						|| Id != rhs.Id 
						|| Position != rhs.Position 
						|| InstanceEx != rhs.InstanceEx 
						|| Stacked != rhs.Stacked);
				}
				bool ItemEx::FindCondition::operator()(const Common::ItemEx& rhs) const
				{
					return (rhs.Info != NULL 
						&& rhs.Info->Hash == hash) ? true : false; 
				}
				uint32 ItemEx::CountCondition::operator()(const Common::ItemEx& rhs) const
				{
					return (rhs.Info != NULL
						&& rhs.Info->Hash == hash) ? rhs.StackedCount : 0;
				}
			}
		}
	}
}