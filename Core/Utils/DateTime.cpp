#pragma once
#include "./DateTime.h"
#pragma region Date
DateTime::Date::Date() {
	this->dateValue.Year = 0;
	this->dateValue.Month = 0;
	this->dateValue.Day = 0;
}
DateTime::Date::Date(uint16 year, uint8 month, uint8 day) {
	this->dateValue.Year = 0;
	this->dateValue.Month = 0;
	this->dateValue.Day = 0;
}
void DateTime::Date::Deserialize(Serializer::StreamReader& in) {
	uint16 aux = 0;
	in.Read(aux);
	dateValue.Year = aux;
	in.Read(aux);
	dateValue.Month = static_cast<uint8>(aux);
	in.Read(aux);
	dateValue.Day = static_cast<uint8>(aux);
}
void DateTime::Date::Serialize(Serializer::StreamWriter& out) const {
	out.Write(static_cast<uint16>(dateValue.Year));
	out.Write(static_cast<uint16>(dateValue.Month));
	out.Write(static_cast<uint16>(dateValue.Day));
}
#pragma endregion
#pragma region Time
DateTime::Time::Time() {
	timeValue.Hour = uint8(0);
	timeValue.Minute = uint8(0);
	timeValue.Second = uint16(0);
	timeValue.MilliSec = uint16(0);
}
DateTime::Time::Time(uint8 hh, uint8 mm, uint16 ss, uint16 milisec) {
	timeValue.Hour = hh;
	timeValue.Minute = mm;
	timeValue.Second = ss;
	timeValue.MilliSec = milisec;
}
void DateTime::Time::Deserialize(Serializer::StreamReader& in) {
	uint16 tmp;
	in.Read(tmp); 
	timeValue.Hour = static_cast<uint8>(tmp);
	in.Read(tmp); 
	timeValue.Minute = static_cast<uint8>(tmp);
	in.Read(tmp); 
	timeValue.Second = tmp;
	in.Read(tmp); 
	timeValue.MilliSec = tmp;
}
void DateTime::Time::Serialize(Serializer::StreamWriter& out) const{
	uint16 tmp;
	tmp = timeValue.Hour;
	out.Write( tmp);
	tmp = timeValue.Minute;
	out.Write(tmp);
	tmp = timeValue.Second;
	out.Write(tmp);
	tmp = timeValue.MilliSec;
	out.Write(tmp);
}
#pragma endregion
#pragma region DateTime
DateTime::DateTime() : timeData(), dateData(){}
DateTime::DateTime(uint16 year, uint8 month, uint8 day, uint8 hour, uint8 min, uint16 sec, uint16 milisec)
	: timeData(hour, min, sec, milisec), dateData(year, month, day){}
DateTime::DateTime(const Date& rhs) 
	: timeData(), dateData(rhs){}
DateTime::DateTime(const Time& rhs) 
	: timeData(rhs), dateData(){}
DateTime::DateTime(const Date& lhs, const Time& rhs) 
	: timeData(rhs), dateData(lhs){}
void DateTime::Deserialize(Serializer::StreamReader& in) {
	dateData.Deserialize(in);
	timeData.Deserialize(in);
}

void DateTime::Serialize(Serializer::StreamWriter& out) const{
	dateData.Serialize(out);
	timeData.Serialize(out);
}
#pragma endregion