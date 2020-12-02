#pragma once
#include "../Serializer/Serializer.h"

struct DateTime {
public:
	struct Unit
	{
		enum type
		{
			Year, Month, Day, Hour, Minute, Second, MilliSecond
		};
	};
	class Date {
	private:
		struct
		{
			uint16 Year;
			uint8 Month;
			uint8 Day;
		} dateValue;
	public:
		Date();
		//Date(String rhs);
		Date(uint16 year, uint8 month, uint8 day);
		//Date(time_t time);
		uint16 GetYear(bool checkValid = true) const { return dateValue.Year; }
		uint16 GetMonth(bool checkValid = true) const { return dateValue.Month; }
		uint16 GetDay(bool checkValid = true) const { return dateValue.Day; }
		void SetYear(uint16 year) { dateValue.Year = year; }
		void SetMonth(uint8 month) { dateValue.Month = month; }
		void SetDay(uint8 day) { dateValue.Day = day; }
		
	public:
		void Deserialize(Serializer::StreamReader& in);
		void Serialize(Serializer::StreamWriter& out) const;
	};
	class Time {
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
		//Time(String rhs);
		Time(uint8 hh, uint8 mm, uint16 ss, uint16 milsec);
		//Time(time_t time);
		uint8 GetHour(bool checkValid = true) const { return timeValue.Hour; }
		uint8 GetMinute(bool checkValid = true) const { return timeValue.Minute; }
		uint16 GetSecond(bool checkValid = true) const { return timeValue.Second; }
		uint16 GetMilliSec(bool checkValid = true) const { return timeValue.MilliSec; }
		void SetHour(uint8 hour) { timeValue.Hour = hour; }
		void SetMinute(uint8 minute) { timeValue.Minute = minute; }
		void SetSecond(uint16 second) { timeValue.Second = second; }
		void SetMilliSec(uint16 milsec) { timeValue.MilliSec = milsec; }
	public:
		void Deserialize(Serializer::StreamReader& in);
		void Serialize(Serializer::StreamWriter& out) const;
	};

	DateTime();
	//DateTime(const wchar_t* rhs);
	DateTime(uint16 year, uint8 month, uint8 day, uint8 hour, uint8 min, uint16 sec, uint16 milisec = 0);
	//DateTime(time_t time);
	DateTime(const Date& rhs);
	DateTime(const Time& rhs);
	DateTime(const Date& lhs, const Time& rhs);
	static DateTime Now();
	Date GetDate() const { return this->dateData; }
	Time GetTime() const { return this->timeData; }
	void SetTime(const Time& time) { this->timeData = time; }
	void SetDate(const Date& time) { this->dateData = time; }
	void Set(Unit::type unit, uint16 value);
	
private:
	Time timeData;
	Date dateData;
public:
	void Deserialize(Serializer::StreamReader& in);
	void Serialize(Serializer::StreamWriter& out) const;
};