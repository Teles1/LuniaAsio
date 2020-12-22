#ifndef DateTime_HEADER_GUARD
#define DateTime_HEADER_GUARD

#pragma once
#include "../Serializer/Serializer.h"
namespace Lunia {
	class DateTime : public Serializer::ISerializable {
	public:

		#pragma region Constants
		struct Unit{
			enum type{
				Year, Month, Day, Hour, Minute, Second, MilliSecond
			};
		};

		struct Format{
			enum type{
				Default,		/* YYYY-MM-DD HH:MM:SS */
				Unseparated,	/* YYYYMMDDHHMMSS */
			};
		};

		struct Week{
			enum type{
				Sunday = 0, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, NumberOfWeek
			};
		};

		typedef enum{
			Unknown = 0,
			First,
			Second,
			Third,
			Fourth,
			Last
		} SpecificType;

		struct Constant{
			static const int MilliSecondsInASecond = 1000;
			static const int SecondsInAMinute = 60;
			static const int HoursInADay = 24;
			static const int MonthsInAYear = 12;
			static const int MinutesInAnHour = 60;
			static const int SecondsInAnHour = SecondsInAMinute * MinutesInAnHour;
			static const int SecondsInADay = SecondsInAnHour * HoursInADay;
		};

		struct DateTerm{
			DateTerm() : term(0){}
			Unit::type termUnit;
			uint32 term;
		};
		#pragma endregion

		class Date : public Serializer::ISerializable {
		private:
			struct
			{
				uint16 Year;
				uint8 Month;
				uint8 Day;
			} dateValue;
			int32 DateConvertToJulianDay();
		public:
			Date();
			Date(String rhs);
			Date(uint16 year, uint8 month, uint8 day);
			Date(time_t time);

			bool Parse(const std::wstring& timeString);
			std::wstring ToString(Format::type format = Format::Default) const;

			uint32 GetCumulatedDay(const Date& origin = Date(0, 0, 0)) const;
			uint16 GetYear(bool checkValid = true)const { return dateValue.Year; }
			uint16 GetMonth(bool checkValid = true)const { return dateValue.Month; }
			uint16 GetDay(bool checkValid = true)const { return dateValue.Day; }

			void SetYear(uint16 year) { dateValue.Year = year; }
			void SetMonth(uint8 month) { dateValue.Month = month; }
			void SetDay(uint8 day) { dateValue.Day = day; }
			void SetCumulatedDay(uint32 day, bool isValid = true, const Date& origin = Date(0, 0, 0));

			Week::type GetDayOfWeek() const;

			bool IsValid() const;

			static Date Now();
			static bool IsValid(const std::wstring& date);
			static uint16 GetLastDayOfMonth(uint16 year, uint16 month);
		public:
			/* Operations */
			bool operator==(const Date& rhs) const;
			bool operator!=(const Date& rhs) const;
			bool operator<(const Date& rhs) const;
			bool operator<=(const Date& rhs) const;
			bool operator>(const Date& rhs) const;
			bool operator>=(const Date& rhs) const;

			Date& Add(Unit::type unit, int32 value);
		public:
			void Deserialize(Serializer::IStreamReader& in);
			void Serialize(Serializer::IStreamWriter& out) const;
		};
		
		class Time : public Serializer::ISerializable {
		private:
			struct
			{
				uint8 Hour;
				uint8 Minute;
				uint16 Second;
				uint16 MilliSec;
			} timeValue;
		public:
			Time();
			Time(String rhs);
			Time(uint8 hh, uint8 mm, uint16 ss, uint16 milsec);
			Time(time_t time);

			bool Parse(const std::wstring& timeString);
			std::wstring ToString(Format::type format = Format::Default) const;

			uint8 GetHour(bool checkValid = true) const { return timeValue.Hour; }
			uint8 GetMinute(bool checkValid = true) const { return timeValue.Minute; }
			uint16 GetSecond(bool checkValid = true) const { return timeValue.Second; }
			uint16 GetMilliSec(bool checkValid = true) const { return timeValue.MilliSec; }
			double GetCumulatedSec(const Time& origin = Time(0, 0, 0, 0)) const;

			void SetHour(uint8 hour) { timeValue.Hour = hour; }
			void SetMinute(uint8 minute) { timeValue.Minute = minute; }
			void SetSecond(uint16 second) { timeValue.Second = second; }
			void SetMilliSec(uint16 milsec) { timeValue.MilliSec = milsec; }
			void SetCumulatedSec(double sec, const Time& origin = Time(0, 0, 0, 0));

			bool IsValid() const;

			Time& Add(Unit::type unit, int32 value);
			Time& AddEx(Unit::type unit, int32 value, Date& date);

			static Time Now();
			static bool IsValid(const std::wstring& time);
		public:
			/* Operations */
			bool operator==(const Time& rhs) const;
			bool operator!=(const Time& rhs) const;
			bool operator<(const Time& rhs) const;
			bool operator<=(const Time& rhs) const;
			bool operator>(const Time& rhs) const;
			bool operator>=(const Time& rhs) const;
		public:
			void Deserialize(Serializer::IStreamReader& in);
			void Serialize(Serializer::IStreamWriter& out) const;
		};

		DateTime();
		DateTime(const std::wstring rhs);
		DateTime(uint16 year, uint8 month, uint8 day, uint8 hour, uint8 min, uint16 sec, uint16 milisec = 0);
		DateTime(time_t time);
		DateTime(const Date& rhs);
		DateTime(const Time& rhs);
		DateTime(const Date& lhs, const Time& rhs);
		bool Parse(const String& timeString);
		bool Parse(const std::string& timeString);

		std::wstring ToString(Format::type format = Format::Default) const;
		uint16 Get(const Unit::type& unit, bool checkValid = true) const;
		Date GetDate() const { return this->dateData; }
		Time GetTime() const { return this->timeData; }
		void SetTime(const Time& time) { this->timeData = time; }
		void SetDate(const Date& time) { this->dateData = time; }
		void Set(Unit::type unit, uint16 value);

		void SetFromSecondToDate(uint32 sec);

		Week::type GetDayOfWeek() const;

		bool IsValid() const;
	public:
		/* static tools */
		static DateTime Now();
		static bool IsValid(const std::wstring& dateTime);
		static DateTime Infinite;
		static bool IsSpecificDayOfWeek(SpecificType specific, Week::type dayOfWeek, const DateTime& dateTime = DateTime::Now());
		static uint16 GetSpecificDayOfWeek(SpecificType specific, Week::type dayOfWeek, const DateTime& dateTime = DateTime::Now());
	public:
		/* Operations */
		bool operator==(const DateTime& rhs) const;
		bool operator!=(const DateTime& rhs) const;
		bool operator<(const DateTime& rhs) const;
		bool operator<=(const DateTime& rhs) const;
		bool operator>(const DateTime& rhs) const;
		bool operator>=(const DateTime& rhs) const;

		//	DateTime math routines
		DateTime& Add(Unit::type unit, int32 value);
		DateTime& Add(DateTime dateTime);
		DateTime& AddEx(Unit::type unit, int32 value);
		DateTime& Add(const DateTerm& dateTerm);

		static DateTime Minus(const DateTime& lhs, const DateTime& rhs);
		static DateTime Plus(const DateTime& lhs, const DateTime& rhs);
	private:
		Time timeData;
		Date dateData;
	public:
		void Deserialize(Serializer::IStreamReader& in) override;
		void Serialize(Serializer::IStreamWriter& out)  const;
	};

	DateTime::Time operator+(const DateTime::Time& lhs, const DateTime::Time& rhs);
	DateTime::Time operator-(const DateTime::Time& lhs, const DateTime::Time& rhs);


	DateTime::Date operator+(const DateTime::Date& lhs, const DateTime::Date& rhs);
	DateTime::Date operator-(const DateTime::Date& lhs, const DateTime::Date& rhs);


	DateTime operator+(const DateTime& lhs, const DateTime& rhs);
	DateTime operator-(const DateTime& lhs, const DateTime& rhs);
}
#endif // !DateTime_HEADER_GUARD