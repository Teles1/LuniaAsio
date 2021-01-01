#include <Info/Info/GuildFamily/FamilyInfo.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void FamilyBasicInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"Database::Info::FamilyBasicInfo");
					out.Write(L"MaxRegularMember", MaxRegularMember);
					out.Write(L"MaxGuestMember", MaxGuestMember);
					out.Write(L"Kickable_HourPerDay", KickableHourPerDay);
					out.Write(L"Kickable_DaysAfterLastLoggedIn", KickableDaysAfterLastLoggedIn);
				}

				void FamilyBasicInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"Database::Info::FamilyBasicInfo");
					in.Read(L"MaxRegularMember", MaxRegularMember);
					in.Read(L"MaxGuestMember", MaxGuestMember);
					in.Read(L"Kickable_HourPerDay", KickableHourPerDay);
					in.Read(L"Kickable_DaysAfterLastLoggedIn", KickableDaysAfterLastLoggedIn);
				}

				void FamilyPresentConditionInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"Database::Info::FamilyPresentConditionInfo");
					out.Write(L"PersonalPlayTimeUnit", PersonalPlayTimeUnit);
					out.Write(L"PlayerNumberForGroupPlay", PlayerNumberForGroupPlay);
					out.Write(L"GroupPlayTime", GroupPlayTime);
					out.Write(L"GroupPlayTimeRate", GroupPlayTimeRate);
					out.Write(L"FishingItemHash", FishingItemHash);
					//out.Write(L"MemorialDayPerDay",MemorialDayPerDay);		
				}
				void FamilyPresentConditionInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"Database::Info::FamilyPresentConditionInfo");
					in.Read(L"PersonalPlayTimeUnit", PersonalPlayTimeUnit);
					in.Read(L"PlayerNumberForGroupPlay", PlayerNumberForGroupPlay);
					in.Read(L"GroupPlayTime", GroupPlayTime);
					in.Read(L"GroupPlayTimeRate", GroupPlayTimeRate);
					in.Read(L"FishingItemHash", FishingItemHash);
					//in.Read(L"MemorialDayPerDay",MemorialDayPerDay);
				}

				void FamilyPassvieRewardInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"Database::Info::FamilyPassvieRewardInfo");
					out.Write(L"EnableToOnlineRegularMemberCount", EnableToOnlineRegularMemberCount);
					out.Write(L"EnableToOnlineGeustMemberCount", EnableToOnlineGeustMemberCount);
					out.Write(L"AddExpFactor", AddExpFactor);
				}
				void FamilyPassvieRewardInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"Database::Info::FamilyPassvieRewardInfo");
					in.Read(L"EnableToOnlineRegularMemberCount", EnableToOnlineRegularMemberCount);
					in.Read(L"EnableToOnlineGeustMemberCount", EnableToOnlineGeustMemberCount);
					in.Read(L"AddExpFactor", AddExpFactor);
				}

				//MailInfo
				void FamilyPresentInfo::MailInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"Database::Info::FamilyPresentInfo::MailInfo");
					out.Write(L"Title", Title);
					out.Write(L"Text", Text);
					out.Write(L"Sender", Sender);
				}
				void FamilyPresentInfo::MailInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"Database::Info::FamilyPresentInfo::MailInfo");
					in.Read(L"Title", Title);
					in.Read(L"Text", Text);
					in.Read(L"Sender", Sender, std::wstring(L""));
				}

				//FamilyPresentInfo::Present::Item
				void FamilyPresentInfo::Present::Item::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"Database::Info::FamilyPresentInfo::Present::Item");
					out.Write(L"Hash", Hash);
					out.Write(L"ExpireHour", ExpireHour);
				}
				void FamilyPresentInfo::Present::Item::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"Database::Info::FamilyPresentInfo::Present::Item");
					in.Read(L"Hash", Hash);
					in.Read(L"ExpireHour", ExpireHour, static_cast<uint32>(0));
				}

				//Present
				void FamilyPresentInfo::Present::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"Database::Info::FamilyPresentInfo::Present");
					out.Write(L"ItemList", Items);
					out.Write(L"Money", Money);
				}
				void FamilyPresentInfo::Present::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"Database::Info::FamilyPresentInfo::Present");
					in.Read(L"ItemList", Items, ItemList());
					in.Read(L"Money", Money, static_cast<uint32>(0));
				}

				//FamilyPresentInfo
				void FamilyPresentInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"Database::Info::FamilyPresentInfo");
					out.Write(L"MailInfo", MailInfo);
					out.Write(L"IsOnlyOnlineUser", IsOnlyOnlineUser);
					out.Write(L"SenderMemberPresentWithRegular", SenderMemberPresentWithRegular);
					out.Write(L"SenderMemberPresentWithGuest", SenderMemberPresentWithGuest);
					out.Write(L"OtherMemberPresentWithRegular", OtherMemberPresentWithRegular);
					out.Write(L"OtherMemberPresentWithGuest", OtherMemberPresentWithGuest);
				}
				void FamilyPresentInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"Database::Info::FamilyPresentInfo");
					in.Read(L"MailInfo", MailInfo);
					in.Read(L"IsOnlyOnlineUser", IsOnlyOnlineUser);
					in.Read(L"SenderMemberPresentWithRegular", SenderMemberPresentWithRegular, Present());
					in.Read(L"SenderMemberPresentWithGuest", SenderMemberPresentWithGuest, Present());
					in.Read(L"OtherMemberPresentWithRegular", OtherMemberPresentWithRegular, Present());
					in.Read(L"OtherMemberPresentWithGuest", OtherMemberPresentWithGuest, Present());
				}

				void MemorialDayPresentInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"Database::Info::MemorialDayPresentInfo");
					out.Write(L"Day", Day);
					out.Write(L"Present", Present);
				}
				void MemorialDayPresentInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"Database::Info::MemorialDayPresentInfo");
					in.Read(L"Day", Day);
					in.Read(L"Present", Present);
				}
			}
		}
	}
}