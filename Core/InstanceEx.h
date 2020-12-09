#ifndef InstanceEx_GUARD
#define InstanceEx_GUARD
#pragma once
#include "Utils/DateTime.h"

namespace Lunia
{

	struct InstanceEx : public Serializer::ISerializable
	{
		static const DateTime NoExpiration;

		int64 Instance;
		DateTime ExpireDate;

		InstanceEx()
			: Instance(0)
			, ExpireDate(NoExpiration)
		{}

		InstanceEx(int64 instance) // 2.6 Compatibility constructor
			: Instance(instance)
			, ExpireDate(NoExpiration)
		{}

		InstanceEx(int64 instance, DateTime expireDate)
			: Instance(instance),
			ExpireDate(expireDate)
		{}
		InstanceEx(int64 instance, String expireDate)
			: Instance(instance)
		{
			ExpireDate.Parse(expireDate.c_str());
		}
		bool IsExpired(const DateTime& now = DateTime::Now()) const;
		void ForceExpiration();
		void MakeUnlimitedPeriod();
		DateTime GetExpiredDate() const;
		DateTime ExtensionExpiredDay(uint32 day);

		void Serialize(Serializer::StreamWriter& out) const;
		void Deserialize(Serializer::StreamReader& in);

		bool operator==(const InstanceEx& rhs) const
		{
			return (Instance == rhs.Instance) && (ExpireDate == rhs.ExpireDate);
		}
		bool operator==(const DateTime& rhs) const
		{
			return (ExpireDate == rhs);
		}
		void operator=(const int64& b)
		{
			Instance = b;
		}
		void operator=(const Lunia::DateTime& b) {
			try {
				if (b.IsValid()) {
					ExpireDate = b;
				}
			}
			catch (Exception& e) {
				//ALLM_EXCEPTIONCAUGHT((L"an exception was caught during audio system init, %s", e.What()));
			}

		}
		/* operator int64 () const
		{
			return Instance;
		} */

		operator const int64& () {
			return Instance;
		}
		operator int64 () const {
			return Instance;
		}
		operator Lunia::DateTime() const {
			return ExpireDate;
		}
		operator std::wstring() const {
			return std::wstring(L"Instance " + StringUtil::ToUnicode(Instance) + L", ExpireDate " + ExpireDate.ToString());
		}
		operator std::string() {
			return std::string(StringUtil::ToASCII(Instance) + "," + StringUtil::ToASCII(ExpireDate.ToString()));
		}

		/* operator AllM::XRated::Database::Enchant::NormalBitfields& (){
			AllM::XRated::Database::Enchant::NormalBitfields normalinstance(Instance);
			return normalinstance;
		} */

		/*
			In a nutshell AllM said that the format for their DateTime in uint32 would follow the standart bellow
			12 bits Year
			4 bits month
			5 bits day
			5 bits hours
			6 bit minutes

			2050-12-31 00:00:00 -- > 02 C8 1F 00 (Little Endian)
		*/
		struct ExpireDateFormat {
			uint32 Year : 12, Month : 4, Day : 5, Hour : 5, Minute : 6;
		};

		struct NormalBitfields
		{
			uint64
				/* item expiration information */
				ExpiredYear : 12,	/* 0-4095 ,0 means no expiration. */
				ExpiredMonth : 4,	/* 1-12 in 0-15 */
				ExpiredDay : 5,	/* 1-31 in 0 - 31 */
				ExpiredHour : 5,	/* 0-23 in 0 - 31 */
				ExpiredMinute : 6,	/* 0-59 in 0 - 64 */
				ExpiredSecond : 6,	/* 0-59 in 0 - 64 */
				Reserved : 26;

			operator int64 () const { return reinterpret_cast<const int64&>(*this); }
			NormalBitfields& operator =(int64 rhs) { reinterpret_cast<int64&>(*this) = rhs; return *this; }
			NormalBitfields() { *this = 0; }
			NormalBitfields(int64 rhs) { *this = rhs; }
		};
	};
}
#endif // !InstanceEx_GUARD