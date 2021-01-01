#ifndef FamilyInfo_H
#define FamilyInfo_H
#pragma once
#ifndef Serializer_H
#include <Core/Serializer/Serializer.h>
#endif // !Serializer_H

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct FamilyBasicInfo : public Serializer::ISerializable
				{
				public:
					uint16 MaxRegularMember;
					uint16 MaxGuestMember;
					uint16 KickableHourPerDay;
					uint16 KickableDaysAfterLastLoggedIn;

				public:
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};

				struct FamilyPresentConditionInfo : public Serializer::ISerializable
				{
				public:
					uint16						PersonalPlayTimeUnit;
					uint8						PlayerNumberForGroupPlay;
					uint8						GroupPlayTime;
					float						GroupPlayTimeRate;
					uint32						FishingItemHash;
					//uint32						MemorialDayPerDay;	
				public:
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
				struct FamilyPassvieRewardInfo : public Serializer::ISerializable
				{
					uint16 EnableToOnlineRegularMemberCount;
					uint16 EnableToOnlineGeustMemberCount;	
					float	AddExpFactor;
				public:
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};


				struct FamilyPresentInfo
				{
					struct MailInfo
					{
						std::wstring	Title;
						std::wstring	Text;
						std::wstring	Sender;
					public:
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
					struct Present
					{
						struct Item {
							uint32	Hash;
							uint32	ExpireHour;
						public:
							virtual void Serialize(Serializer::IStreamWriter& out) const;
							virtual void Deserialize(Serializer::IStreamReader& in);
						};
						typedef std::vector< Item > ItemList;
						ItemList		Items;
						uint32			Money;
					public:
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
						Present() : Money(0) {}

						bool IsPresent() const {
							if (Money > 0) {
								return true;
							}
							if (Items.empty() == false) {
								return true;
							}
							return false;
						}
					};
					MailInfo		MailInfo;
					bool			IsOnlyOnlineUser;

					Present			SenderMemberPresentWithRegular;
					Present			SenderMemberPresentWithGuest;

					Present			OtherMemberPresentWithRegular;
					Present			OtherMemberPresentWithGuest;
				public:
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};

				struct MemorialDayPresentInfo
				{
					uint32				Day;
					FamilyPresentInfo	Present;
				public:
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};

			}
		}
	}
}
#endif // ! FamilyInfo_H
