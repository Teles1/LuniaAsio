#include <Info/Info/GuildFamily/GuildInfo.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void GuildInfo::General::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::GuildInfo::General");
					out.Write(L"MinuteForExp", MinuteForExp);
					out.Write(L"LevelUpExpList", LevelUpExps);
					out.Write(L"MaxGuildPointList", MaxGuildPoints);
					out.Write(L"DiscountRate", DiscountRate);
					out.Write(L"MaxGuildUser", MaxGuildUser);
					out.Write(L"GuildCashItemTabInfo", GuildCashItemTabInfo);
					out.Write(L"CreationFee", CreationFee);
					out.Write(L"MinimalCreationPlayerLevel", MinimalCreationPlayerLevel);
					out.Write(L"LeaveFee", LeaveFee);
				}
				void GuildInfo::General::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::GuildInfo::General");
					in.Read(L"MinuteForExp", MinuteForExp);
					in.Read(L"LevelUpExpList", LevelUpExps);
					in.Read(L"MaxGuildPointList", MaxGuildPoints);
					in.Read(L"DiscountRate", DiscountRate, std::vector<float>(1, 1.0f));
					in.Read(L"MaxGuildUser", MaxGuildUser);
					in.Read(L"GuildCashItemTabInfo", GuildCashItemTabInfo);
					in.Read(L"CreationFee", CreationFee);
					in.Read(L"MinimalCreationPlayerLevel", MinimalCreationPlayerLevel);
					in.Read(L"LeaveFee", LeaveFee, static_cast<uint32>(50000));
				}

				void GuildInfo::Level::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::GuildInfo::Level");
					out.Write(L"AddExpFactor", AddExpFactor);
					out.Write(L"ShopCategorys", ShopCategorys);
					out.Write(L"GuildPassives", GuildPassives);
				}
				void GuildInfo::Level::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::GuildInfo::Level");
					in.Read(L"AddExpFactor", AddExpFactor);
					in.Read(L"ShopCategorys", ShopCategorys);
					in.Read(L"GuildPassives", GuildPassives);
				}

				void GuildInfo::Rank::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::GuildInfo::Rank");
					out.Write(L"ShopCategorys", ShopCategorys);
				}
				void GuildInfo::Rank::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::GuildInfo::Rank");
					in.Read(L"ShopCategorys", ShopCategorys);
				}
			}
		}
	}
}