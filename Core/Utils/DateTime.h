#pragma once
#include "../Serializer/Serializer.h"

struct DateTime {
public:
	
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
private:
	Time timeData;
	Date dateData;
public:
	void Deserialize(Serializer::StreamReader& in);
	void Serialize(Serializer::StreamWriter& out) const;
};