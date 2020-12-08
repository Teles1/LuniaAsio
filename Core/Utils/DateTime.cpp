#include "./DateTime.h"
#include "../Core.h"
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
void DateTime::Date::Serialize(Serializer::StreamWriter& out) {
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
void DateTime::Time::Serialize(Serializer::StreamWriter& out){
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
DateTime DateTime::Now()
{
	DateTime nowDateTime;

	time_t now = time(0); 
	struct tm timeinfo;
	localtime_s(&timeinfo, &now);
	// set current date
	nowDateTime.Set(Unit::Year, (uint16)timeinfo.tm_year + 1900);
	nowDateTime.Set(Unit::Month, (uint16)timeinfo.tm_mon + 1);
	nowDateTime.Set(Unit::Day, (uint16)timeinfo.tm_mday);

	// set current time
	nowDateTime.Set(Unit::Hour, (uint16)timeinfo.tm_hour);
	nowDateTime.Set(Unit::Minute, (uint16)timeinfo.tm_min);
	nowDateTime.Set(Unit::Second, (uint16)timeinfo.tm_sec);

	return nowDateTime;
}
void DateTime::Set(Unit::type unit, uint16 value)
{
	switch (unit)
	{
	case Unit::Year:
		return dateData.SetYear(value);
	case Unit::Month:
		return dateData.SetMonth(static_cast<uint8>(value));
	case Unit::Day:
		return dateData.SetDay(static_cast<uint8>(value));
	case Unit::Hour:
		return timeData.SetHour(static_cast<uint8>(value));
	case Unit::Minute:
		return timeData.SetMinute(static_cast<uint8>(value));
	case Unit::Second:
		return timeData.SetSecond(value);
	case Unit::MilliSecond:
		return timeData.SetMilliSec(value);
	}
}
void DateTime::Deserialize(Serializer::StreamReader& in) {
	//in.Begin(L"AllM::DateTime");
	uint16 tmp;
	in.Read(tmp); dateData.SetYear(tmp);
	in.Read(tmp); dateData.SetMonth(static_cast<uint8>(tmp));
	in.Read(tmp); dateData.SetDay(static_cast<uint8>(tmp));
	in.Read(tmp); timeData.SetHour(static_cast<uint8>(tmp));
	in.Read(tmp); timeData.SetMinute(static_cast<uint8>(tmp));
	in.Read(tmp); timeData.SetSecond(tmp);
	in.Read(tmp); timeData.SetMilliSec(tmp);
}

void DateTime::Serialize(Serializer::StreamWriter& out){
	out.Begin((uint16) StringUtil::Hash(L"AllM::DateTime"));
	uint16 tmp;
	tmp = dateData.GetYear();
	out.Write( tmp);
	tmp = dateData.GetMonth();
	out.Write(tmp);
	tmp = dateData.GetDay();
	out.Write(tmp);

	tmp = timeData.GetHour();
	out.Write(tmp);
	tmp = timeData.GetMinute();
	out.Write(tmp);
	tmp = timeData.GetSecond();
	out.Write(tmp);
	tmp = timeData.GetMilliSec();
	out.Write(tmp);
}
#pragma endregion