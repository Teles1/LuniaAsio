#include "./DateTime.h"
#include "../Core.h"
namespace Lunia {

#pragma region Date
	DateTime::Date::Date() {
		this->dateValue.Year = 0;
		this->dateValue.Month = 0;
		this->dateValue.Day = 0;
	}
	DateTime::Date::Date(String rhs)
	{
		time_t now = time(0);
		struct tm timeinfo;
		localtime_s(&timeinfo, &now);

		SetYear(static_cast<uint16>(timeinfo.tm_year + 1900));
		SetMonth(static_cast<uint8>(timeinfo.tm_mon + 1));
		SetDay(static_cast<uint8>(timeinfo.tm_mday));
	}
	DateTime::Date::Date(uint16 year, uint8 month, uint8 day) {
		this->dateValue.Year = 0;
		this->dateValue.Month = 0;
		this->dateValue.Day = 0;
	}
	DateTime::Date::Date(time_t time)
	{
		struct tm timeinfo;
		localtime_s(&timeinfo, &time);


		SetYear(static_cast<uint16>(timeinfo.tm_year + 1900));
		SetMonth(static_cast<uint8>(timeinfo.tm_mon + 1));
		SetDay(static_cast<uint8>(timeinfo.tm_mday));
	}
	bool DateTime::Date::Parse(const std::wstring& timeString) {
		std::vector< std::wstring > extracted;
		std::wstring temp;


		uint16 year;
		uint8 month, day;
		for (size_t i = 0; i < timeString.size(); ++i)
		{
			if (timeString.at(i) >= '0' && timeString.at(i) <= '9')
			{
				temp.push_back(timeString.at(i));
			}
			else
			{
				if (!temp.empty())
				{
					extracted.push_back(temp);
					temp.clear();
				}
			}
		}

		if (!temp.empty())
		{
			extracted.push_back(temp);
			temp.clear();
		}

		if (extracted.size() == 1 && timeString.size() == 8)
		{
			year = static_cast<uint16>(StringUtil::ToInt32(std::wstring(extracted.at(0).c_str(), 0, 4)));
			month = static_cast<uint8>(StringUtil::ToInt32(std::wstring(extracted.at(0).c_str(), 4, 2)));
			day = static_cast<uint8>(StringUtil::ToInt32(std::wstring(extracted.at(0).c_str(), 6, 2)));
		}
		else if (extracted.size() == 1 && timeString.size() == 6)
		{  
			year = static_cast<uint16>(StringUtil::ToInt32(std::wstring(extracted.at(0).c_str(), 0, 2)));
			month = static_cast<uint8>(StringUtil::ToInt32(std::wstring(extracted.at(0).c_str(), 2, 2)));
			day = static_cast<uint8>(StringUtil::ToInt32(std::wstring(extracted.at(0).c_str(), 4, 2)));

			if (year >= 50)
				year += 1900; //ex)951104
			else
				year += 2000; //ex)060203
		}
		else if (extracted.size() >= 3)
		{ 
			year = static_cast<uint16>(StringUtil::ToInt32(extracted.at(0)));
			month = static_cast<uint8>(StringUtil::ToInt32(extracted.at(1)));
			day = static_cast<uint8>(StringUtil::ToInt32(extracted.at(2)));

			if (extracted.at(0).size() <= 2 && year >= 50)
				year += 1900;
			else if (extracted.at(0).size() <= 2 && year < 50)
				year += 2000;
		}
		else
		{
			Logger::GetInstance()->Warn("The number of numeric strings is invalid.");
			return false;
		}

		dateValue.Year = year;
		dateValue.Month = month;
		dateValue.Day = day;

		return true;
	}
	uint32 DateTime::Date::GetCumulatedDay(const Date& origin)const
	{
		if (dateValue.Year == 0 && dateValue.Month == 0 && dateValue.Day == 0)
			return 0;

		int days[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

		uint32 result = 0;

		result = (dateValue.Year - 1) * 365;

		result = result + (dateValue.Year - 1) / 4 - (dateValue.Year - 1) / 100 + (dateValue.Year - 1) / 400;


		if (dateValue.Year % 4 == 0)
		{
			days[2] = 29;

			if (dateValue.Year % 100 == 0 && dateValue.Year % 400 != 0)
				days[2] = 28;
		}

		for (int i = 1; i < dateValue.Month; i++)
			result += days[i];

		result += dateValue.Day;

		result -= origin.GetCumulatedDay();

		return result;
	}
	std::wstring DateTime::Date::ToString(Format::type format)const
	{
		if (IsValid())
		{
			switch (format)
			{
			case Format::Default:
				
				return fmt::format(L"{0:04d}-{1:02d}-{2:02d}", dateValue.Year, dateValue.Month, dateValue.Day);
			case Format::Unseparated:
				return fmt::format(L"{0:04d}{1:02d}{2:02d}", dateValue.Year, dateValue.Month, dateValue.Day);
			}
			Logger::GetInstance()->Warn("no format. format = {0}", format);
		}
		return L"";
	}
	void DateTime::Date::Deserialize(Serializer::IStreamReader& in) {
		in.Begin(L"AllM::Date");
		uint16 aux = 0;
		in.Read(L"Year", aux);
		dateValue.Year = aux;
		in.Read(L"Month", aux);
		dateValue.Month = static_cast<uint8>(aux);
		in.Read(L"Day", aux);
		dateValue.Day = static_cast<uint8>(aux);
	}
	void DateTime::Date::Serialize(Serializer::IStreamWriter& out)  const {
		out.Begin(L"AllM::Date");
		out.Write(L"Year", static_cast<uint16>(dateValue.Year));
		out.Write(L"Month", static_cast<uint16>(dateValue.Month));
		out.Write(L"Day", static_cast<uint16>(dateValue.Day));
	}
#pragma endregion

#pragma region Time
	DateTime::Time::Time() {
		timeValue.Hour = uint8(0);
		timeValue.Minute = uint8(0);
		timeValue.Second = uint16(0);
		timeValue.MilliSec = uint16(0);
	}
	DateTime::Time::Time(String rhs)
	{
		if (Parse(rhs))
			return;
		else {
			Logger::GetInstance()->Error("Arguments are invalid");
			throw "Arguments are invalid";
		}
	}
	DateTime::Time::Time(uint8 hh, uint8 mm, uint16 ss, uint16 milisec) {
		timeValue.Hour = hh;
		timeValue.Minute = mm;
		timeValue.Second = ss;
		timeValue.MilliSec = milisec;
	}
	DateTime::Time::Time(time_t time)
	{
		struct tm timeinfo;
		localtime_s(&timeinfo, &time);

		timeValue.Hour = static_cast<uint8>(timeinfo.tm_hour);
		timeValue.Minute = static_cast<uint8>(timeinfo.tm_min);
		timeValue.Second = static_cast<uint16>(timeinfo.tm_sec);
		timeValue.MilliSec = 0;
	}
	bool DateTime::Time::Parse(const std::wstring& timeString) {

		std::vector< std::wstring > extracted;
		std::wstring temp;


		uint8 hour, min;
		uint16 sec, millisec;


		for (size_t i = 0; i < timeString.size(); ++i)
		{
			//Ascii Code 48~57 -> 0~9
			if (timeString.at(i) >= 48 && timeString.at(i) <= 57)
			{
				temp.push_back(timeString.at(i));
			}
			else
			{
				if (!temp.empty())
				{
					extracted.push_back(temp);
					temp.clear();
				}
			}
		}

		if (!temp.empty())
		{
			extracted.push_back(temp);
			temp.clear();
		}


		if (extracted.size() == 1 && timeString.size() >= 4)
		{   
			
			hour = static_cast<uint8>(StringUtil::ToInt32(std::wstring(timeString.c_str(), 0, 2)));
			min = static_cast<uint8>(StringUtil::ToInt32(std::wstring(timeString.c_str(), 2, 2)));

			if (timeString.size() >= 6)
				sec = static_cast<uint16>(StringUtil::ToInt32(std::wstring(timeString.c_str(), 4, 2)));
			else sec = 0;

			if (timeString.size() >= 8)
				millisec = static_cast<uint16>(StringUtil::ToInt32(std::wstring(timeString.c_str(), 6, timeString.size() - 6)));
			else millisec = 0;
		}
		else if (extracted.size() >= 1)
		{ 
			hour = static_cast<uint8>(StringUtil::ToInt32(extracted.at(0)));
			if (extracted.size() >= 2)
				min = static_cast<uint8>(StringUtil::ToInt32(extracted.at(1)));
			else min = 0;

			if (extracted.size() >= 3)
				sec = static_cast<uint16>(StringUtil::ToInt32(extracted.at(2)));
			else sec = 0;

			if (extracted.size() >= 4)
				millisec = static_cast<uint16>(StringUtil::ToInt32(extracted.at(3)));
			else millisec = 0;
		}
		else
		{
			Logger::GetInstance()->Warn("The number of numeric strings is invalid.");
			return false;
		}

		timeValue.Hour = hour;
		timeValue.Minute = min;
		timeValue.Second = sec;
		timeValue.MilliSec = millisec;

		return true;

	}
	std::wstring DateTime::Time::ToString(Format::type format) const
	{
		if (IsValid())
		{
			switch (format)
			{
			case Format::Default:

				return fmt::format(L"{0:02d}:{1:02d}:{2:02d}", timeValue.Hour, timeValue.Minute, timeValue.Second);
			case Format::Unseparated:
				return fmt::format(L"{0:042}{1:02d}{2:02d}", timeValue.Hour, timeValue.Minute, timeValue.Second);
			}

			Logger::GetInstance()->Warn("no format. format = {0}", format);
		}
		return L"";
	}
	double DateTime::Time::GetCumulatedSec(const Time& origin) const
	{
		if (timeValue.Hour == 0 && timeValue.Minute == 0
			&& timeValue.Second == 0 && timeValue.MilliSec == 0)
			return 0;

		double sec = 0;

		sec += timeValue.Hour * 3600
			+ timeValue.Minute * 60
			+ timeValue.Second
			+ timeValue.MilliSec / Constant::MilliSecondsInASecond;

		sec -= origin.GetCumulatedSec();

		return sec;
	}
	void DateTime::Time::Deserialize(Serializer::IStreamReader& in) {
		in.Begin(L"AllM::Time");
		uint16 tmp;
		in.Read(L"Hour", tmp);
		timeValue.Hour = static_cast<uint8>(tmp);
		in.Read(L"Minute", tmp);
		timeValue.Minute = static_cast<uint8>(tmp);
		in.Read(L"Second", tmp);
		timeValue.Second = tmp;
		in.Read(L"MilliSec", tmp);
		timeValue.MilliSec = tmp;
	}
	void DateTime::Time::Serialize(Serializer::IStreamWriter& out)  const {
		uint16 tmp;
		tmp = timeValue.Hour;
		out.Write(L"Hour", tmp);
		tmp = timeValue.Minute;
		out.Write(L"Minute", tmp);
		tmp = timeValue.Second;
		out.Write(L"Second", tmp);
		tmp = timeValue.MilliSec;
		out.Write(L"MilliSec", tmp);
	}
#pragma endregion

#pragma region DateTime
	DateTime::DateTime() : timeData(), dateData() {}
	DateTime::DateTime(uint16 year, uint8 month, uint8 day, uint8 hour, uint8 min, uint16 sec, uint16 milisec)
		: timeData(hour, min, sec, milisec), dateData(year, month, day) {}
	DateTime::DateTime(const Date& rhs)
		: timeData(), dateData(rhs) {}
	DateTime::DateTime(const Time& rhs)
		: timeData(rhs), dateData() {}
	DateTime::DateTime(const Date& lhs, const Time& rhs)
		: timeData(rhs), dateData(lhs) {}
	bool DateTime::Parse(const String& timeString)
	{

		std::vector< std::wstring > extracted;
		std::wstring temp;


		uint16 year, second, milliSec;
		uint8 month, day, hour, minute;

		for (size_t i = 0; i < timeString.size(); ++i)
		{
			if (timeString.at(i) >= L'0' && timeString.at(i) <= L'9')
			{
				temp.push_back(timeString.at(i));
			}
			else
			{
				if (!temp.empty())
				{
					extracted.push_back(temp);
					temp.clear();
				}
			}
		}

		if (!temp.empty())
		{
			extracted.push_back(temp);
			temp.clear();
		}
		if (extracted.size() == 1 && extracted.at(0).size() == 14)
		{
			if (
				dateData.Parse(extracted.at(0).substr(0, 8))
				&& timeData.Parse(extracted.at(0).substr(8, 6)))
				return true;
		}
		else if (extracted.size() == 1 && extracted.at(0).size() == 12)
		{
			if (
				dateData.Parse(extracted.at(0).substr(0, 6))
				&& timeData.Parse(extracted.at(0).substr(6, 6)))
				return true;
		}
		else if (extracted.size() == 2)
		{ 
			if (dateData.Parse(extracted.at(0).c_str())
				&& timeData.Parse(extracted.at(1).c_str()))
				return true;
		}
		else if (extracted.size() >= 4)
		{
			year = static_cast<uint16>(StringUtil::ToInt32(extracted.at(0)));
			month = static_cast<uint8>(StringUtil::ToInt32(extracted.at(1)));
			day = static_cast<uint8>(StringUtil::ToInt32(extracted.at(2)));
			hour = static_cast<uint8>(StringUtil::ToInt32(extracted.at(3)));

			if (extracted.size() >= 5)
				minute = static_cast<uint8>(StringUtil::ToInt32(extracted.at(4)));
			else
				minute = 0;

			if (extracted.size() >= 6)
				second = static_cast<uint16>(StringUtil::ToInt32(extracted.at(5)));
			else
				second = 0;

			if (extracted.size() >= 7)
				milliSec = static_cast<uint16>(StringUtil::ToInt32(extracted.at(6)));
			else
				milliSec = 0;

			dateData = Date(year, month, day);
			timeData = Time(hour, minute, second, milliSec);
			return true;
		}

		Logger::GetInstance()->Error(" The number of numeric strings is invalid.");
		return false;
	}
	std::wstring DateTime::ToString(Format::type format) const
	{
		std::wstring data = L"";

		data += dateData.ToString(format);

		if (format == Format::Default)
			data += L" ";
		data += timeData.ToString(format);

		return data;
	}
	void DateTime::Deserialize(Serializer::IStreamReader& in) {
		in.Begin(L"AllM::DateTime");
		uint16 tmp;
		in.Read(L"Year", tmp); dateData.SetYear(tmp);
		in.Read(L"Month", tmp); dateData.SetMonth(static_cast<uint8>(tmp));
		in.Read(L"Day", tmp); dateData.SetDay(static_cast<uint8>(tmp));
		in.Read(L"Hour", tmp); timeData.SetHour(static_cast<uint8>(tmp));
		in.Read(L"Minute", tmp); timeData.SetMinute(static_cast<uint8>(tmp));
		in.Read(L"Second", tmp); timeData.SetSecond(tmp);
		in.Read(L"MilliSec", tmp); timeData.SetMilliSec(tmp);
	}
	void DateTime::Serialize(Serializer::IStreamWriter& out)  const {
		out.Begin(L"AllM::DateTime");
		uint16 tmp;
		tmp = dateData.GetYear();
		out.Write(L"Year", tmp);
		tmp = dateData.GetMonth();
		out.Write(L"Month", tmp);
		tmp = dateData.GetDay();
		out.Write(L"Day", tmp);

		tmp = timeData.GetHour();
		out.Write(L"Hour", tmp);
		tmp = timeData.GetMinute();
		out.Write(L"Minute", tmp);
		tmp = timeData.GetSecond();
		out.Write(L"Second", tmp);
		tmp = timeData.GetMilliSec();
		out.Write(L"MilliSec", tmp);
	}

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
	DateTime DateTime::Infinite(L"9999-01-01 00:00:00");
	bool DateTime::IsValid(const std::wstring& dateTime)
	{
		DateTime test;
		if (!(test.Parse(dateTime.c_str())))
			return false;
		else
		{
			if (test.IsValid())
				return true;
			else
				return false;
		}
	}
	bool DateTime::IsSpecificDayOfWeek(SpecificType specific, Week::type dayOfWeek, const DateTime& dateTime)
	{
		if (specific < First)
		{
			return false;
		}

		// It would not care about a month and a year.
		int passedWeek[Week::NumberOfWeek] = { 0 };
		if (dateTime.GetDayOfWeek() != dayOfWeek)
		{
			return false;
		}

		int today = dateTime.GetDate().GetDay();
		DateTime passedDate = dateTime;
		for (int dayIndex = 1; dayIndex <= today; ++dayIndex)
		{
			passedDate.Set(Unit::Day, dayIndex);
			++passedWeek[passedDate.GetDayOfWeek()];
		}

		if (specific != passedWeek[dayOfWeek])
		{
			return false;
		}

		return true;
	}
	uint16 DateTime::GetSpecificDayOfWeek(SpecificType specific, Week::type dayOfWeek, const DateTime& dateTime)
	{
		if (specific < First)
		{
			return false;
		}

		// It would not care about a month and a year.
		int passedWeek[Week::NumberOfWeek] = { 0 };
		int thisMonth = dateTime.GetDate().GetMonth();
		int thisYear = dateTime.GetDate().GetYear();
		int lastDayOfThisMonth = DateTime::Date::GetLastDayOfMonth(thisYear, thisMonth);
		DateTime passedDate = dateTime;
		for (int dayIndex = 1; dayIndex <= lastDayOfThisMonth; ++dayIndex)
		{
			passedDate.Set(Unit::Day, dayIndex);
			++passedWeek[passedDate.GetDayOfWeek()];

			if (passedWeek[dayOfWeek] == specific)
			{
				return dayIndex;
			}
		}

		return 0;
	}
	uint16 DateTime::Get(const Unit::type& unit, bool checkValid) const
	{
		switch (unit)
		{
		case Unit::Year:
			return dateData.GetYear(checkValid);
		case Unit::Month:
			return dateData.GetMonth(checkValid);
		case Unit::Day:
			return dateData.GetDay(checkValid);
		case Unit::Hour:
			return timeData.GetHour(checkValid);
		case Unit::Minute:
			return timeData.GetMinute(checkValid);
		case Unit::Second:
			return timeData.GetSecond(checkValid);
		case Unit::MilliSecond:
			return timeData.GetMilliSec(checkValid);
		}

		return 0;
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
	void DateTime::SetFromSecondToDate(uint32 sec)
	{
		if (sec <= 0)
			return;

		int day = 0;
		int hour = 0;
		int minute = 0;

		if (86400 >= sec)
		{
			day = sec / 86400;
			sec = sec % 86400;
			hour = sec / 3600;
			sec = sec % 3600;
			minute = sec / 60;
			sec = sec % 60;
		}
		if (sec != 0)
			Add(Unit::Second, sec);
		if (minute != 0)
			Add(Unit::Minute, minute);
		if (hour != 0)
			Add(Unit::Hour, hour);
		if (day != 0)
			Add(Unit::Day, day);
	}
	DateTime::Week::type DateTime::GetDayOfWeek() const
	{
		return dateData.GetDayOfWeek();
	}
	bool DateTime::IsValid() const
	{
		if (dateData.IsValid() && timeData.IsValid())
			return true;

		return false;
	}


	bool DateTime::operator==(const DateTime& rhs) const
	{
		return (dateData == rhs.dateData && timeData == rhs.timeData);
	}
	bool DateTime::operator!=(const DateTime& rhs) const
	{
		return !(dateData == rhs.dateData && timeData == rhs.timeData);
	}
	bool DateTime::operator<(const DateTime& rhs) const
	{
		if (dateData < rhs.dateData)
			return true;
		else if (dateData == rhs.dateData && timeData < rhs.timeData)
			return true;

		return false;
	}
	bool DateTime::operator<=(const DateTime& rhs) const
	{
		if (dateData < rhs.dateData)
			return true;
		else if (dateData == rhs.dateData && timeData <= rhs.timeData)
			return true;

		return false;
	}
	bool DateTime::operator>(const DateTime& rhs) const
	{
		if (dateData > rhs.dateData)
			return true;
		else if (dateData == rhs.dateData && timeData > rhs.timeData)
			return true;

		return false;
	}
	bool DateTime::operator>=(const DateTime& rhs) const
	{
		if (dateData > rhs.dateData)
			return true;
		else if (dateData == rhs.dateData && timeData >= rhs.timeData)
			return true;

		return false;
	}

	DateTime& DateTime::Add(Unit::type unit, int32 value)
	{
		if (value == 0)
		{
			Logger::GetInstance()->Warn("argument value is zero. value = 0");
			return *this;
		}
		else if (value > 0)
		{
			int32 sum = 0;
			switch (unit)
			{
			case Unit::Year:
				dateData.Add(unit, value);
				return *this;
			case Unit::Month:
				dateData.Add(unit, value);
				return *this;
			case Unit::Day:
				dateData.Add(unit, value);
				return *this;
			case Unit::Hour:
				sum = timeData.GetHour() + value;
				if (sum / Constant::HoursInADay != 0)
					dateData.Add(Unit::Day, sum / Constant::HoursInADay);

				timeData.Add(unit, value);
				return *this;
			case Unit::Minute:
				timeData.Add(unit, value);
				return *this;
			case Unit::Second:
				sum = int32(timeData.GetCumulatedSec()) + value;
				if (sum / Constant::SecondsInADay != 0)
					dateData.Add(Unit::Day, sum / Constant::SecondsInADay);
				timeData.Add(unit, value);
				return *this;
			}
		}
		else
		{
			int32 minus = 0;

			switch (unit)
			{
			case Unit::Year:
				dateData.Add(unit, value);
				return *this;
			case Unit::Month:
				dateData.Add(unit, value);
				return *this;
			case Unit::Day:
				dateData.Add(unit, value);
				return *this;
			case Unit::Hour:
				minus = timeData.GetHour() + value;
				if (minus < 0)
				{
					if (minus / Constant::HoursInADay != 0)
						dateData.Add(Unit::Day, -( minus / Constant::HoursInADay));
					else
						dateData.Add(Unit::Day, -1);
				}
				timeData.Add(unit, value);
				return *this;
			case Unit::Minute:
				timeData.Add(unit, value);
				return *this;
			case Unit::Second:
				minus = int32(timeData.GetCumulatedSec()) + value;
				if (minus < 0)
				{
					if (minus / Constant::SecondsInADay != 0)
						dateData.Add(Unit::Day, minus / Constant::SecondsInADay);
					else
						dateData.Add(Unit::Day, -1);
				}
				timeData.Add(unit, value);
				return *this;
			}
		}
		return *this;
	}
	DateTime& DateTime::Add(DateTime dateTime)
	{
		if (dateTime.IsValid())
		{
			dateData.Add(Unit::Year, dateTime.GetDate().GetYear());
			dateData.Add(Unit::Month, dateTime.GetDate().GetMonth());
			dateData.Add(Unit::Day, dateTime.GetDate().GetDay());
			dateData.Add(Unit::Hour, dateTime.GetTime().GetHour());
			dateData.Add(Unit::Minute, dateTime.GetTime().GetMinute());
			double sum(timeData.GetCumulatedSec() + dateTime.GetTime().GetSecond());
			if (sum / Constant::SecondsInADay != 0)
				dateData.Add(Unit::Day, int32(std::round(sum / Constant::SecondsInADay)) );
			timeData.Add(Unit::Second, dateTime.GetTime().GetSecond());
			return *this;
		}
		else
		{
			Logger::GetInstance()->Warn("argument value isnt valid. {0}", StringUtil::ToASCII(dateTime.ToString()));
		}
		return *this;
	}
	DateTime& DateTime::AddEx(Unit::type unit, int32 value)
	{
		if (value == 0)
		{
			Logger::GetInstance()->Warn("argument value is zero. value = 0");
			return *this;
		}
		else if (value > 0)
		{
			int32 sum = 0;
			switch (unit)
			{
			case Unit::Year:
				dateData.Add(unit, value);
				return *this;
			case Unit::Month:
				dateData.Add(unit, value);
				return *this;
			case Unit::Day:
				dateData.Add(unit, value);
				return *this;
			case Unit::Hour:
				timeData.AddEx(unit, value, dateData);
				return *this;
			case Unit::Minute:
				timeData.AddEx(unit, value, dateData);
				return *this;
			case Unit::Second:
				timeData.AddEx(unit, value, dateData);
				return *this;
			}
		}
		else
		{
			int32 minus = 0;

			switch (unit)
			{
			case Unit::Year:
				dateData.Add(unit, value);
				return *this;
			case Unit::Month:
				dateData.Add(unit, value);
				return *this;
			case Unit::Day:
				dateData.Add(unit, value);
				return *this;
			case Unit::Hour:
				timeData.AddEx(unit, value, dateData);
				return *this;
			case Unit::Minute:
				timeData.AddEx(unit, value, dateData);
				return *this;
			case Unit::Second:
				timeData.AddEx(unit, value, dateData);
				return *this;
			}
		}
		return *this;
	}
	DateTime& DateTime::Add(const DateTerm& dateTerm)
	{
		if (dateTerm.term == 0)
		{
			return *this;
		}
		return Add(dateTerm.termUnit, dateTerm.term);
	}
	DateTime::DateTime(const std::wstring rhs)
	{
		if (Parse(rhs))
			return;
		else 
		{
			Logger::GetInstance()->Exception("Arguments are invailid");
			throw "Arguments are invailid";
		}
	}
	DateTime operator+(const DateTime& lhs, const DateTime& rhs)
	{
		DateTime datetimeData(lhs);

		datetimeData.Add(DateTime::Unit::Second, rhs.Get(DateTime::Unit::Second));
		datetimeData.Add(DateTime::Unit::Minute, rhs.Get(DateTime::Unit::Minute));
		datetimeData.Add(DateTime::Unit::Hour, rhs.Get(DateTime::Unit::Hour));
		datetimeData.Add(DateTime::Unit::Day, rhs.Get(DateTime::Unit::Day));
		datetimeData.Add(DateTime::Unit::Month, rhs.Get(DateTime::Unit::Month));
		datetimeData.Add(DateTime::Unit::Year, rhs.Get(DateTime::Unit::Year));

		return datetimeData;
	}
	DateTime operator-(const DateTime& lhs, const DateTime& rhs)
	{
		DateTime datetimeData(lhs);

		datetimeData.Add(DateTime::Unit::Year, -rhs.Get(DateTime::Unit::Year));
		datetimeData.Add(DateTime::Unit::Month, -rhs.Get(DateTime::Unit::Month));
		datetimeData.Add(DateTime::Unit::Day, -rhs.Get(DateTime::Unit::Day));
		datetimeData.Add(DateTime::Unit::Hour, -rhs.Get(DateTime::Unit::Hour));
		datetimeData.Add(DateTime::Unit::Minute, -rhs.Get(DateTime::Unit::Minute));
		datetimeData.Add(DateTime::Unit::Second, -rhs.Get(DateTime::Unit::Second));

		return datetimeData;
	}
	DateTime DateTime::Plus(const DateTime& lhs, const DateTime& rhs)
	{
		double secs = lhs.GetTime().GetCumulatedSec() + rhs.GetTime().GetCumulatedSec();
		uint32 days = lhs.GetDate().GetCumulatedDay() + rhs.GetDate().GetCumulatedDay();

		days += static_cast<uint32>(secs / Constant::SecondsInADay);
		secs = static_cast<int>(secs) % Constant::SecondsInADay;

		Date date;		date.SetCumulatedDay(days, false);
		Time time;		time.SetCumulatedSec(secs);

		return DateTime(date, time);
	}
	DateTime DateTime::Minus(const DateTime& lhs, const DateTime& rhs)
	{
		assert(lhs.GetDate().GetCumulatedDay() - rhs.GetDate().GetCumulatedDay() >= 0);

		double secs = lhs.GetTime().GetCumulatedSec() - rhs.GetTime().GetCumulatedSec();
		uint32 days = lhs.GetDate().GetCumulatedDay() - rhs.GetDate().GetCumulatedDay();

		assert(days + secs / Constant::SecondsInADay >= 0);

		days += static_cast<uint32>(secs / Constant::SecondsInADay);
		secs = static_cast<int>(secs) % Constant::SecondsInADay;

		Date date;		date.SetCumulatedDay(days, false);
		Time time;		time.SetCumulatedSec(secs);

		return DateTime(date, time);
	}

	DateTime::DateTime(time_t time)
	{
		struct tm timeinfo;
		localtime_s(&timeinfo, &time);

		dateData.SetYear(static_cast<uint16>(timeinfo.tm_year + 1900));
		dateData.SetMonth(static_cast<uint8>(timeinfo.tm_mon + 1));
		dateData.SetDay(static_cast<uint8>(timeinfo.tm_mday));

		timeData.SetHour(static_cast<uint8>(timeinfo.tm_hour));
		timeData.SetMinute(static_cast<uint8>(timeinfo.tm_min));
		timeData.SetSecond(static_cast<uint16>(timeinfo.tm_sec));
	}
	void DateTime::Date::SetCumulatedDay(uint32 day, bool isValid, const Date& origin)
	{
		day += origin.GetCumulatedDay();

		uint8 days[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
		uint32 month = 1;
		uint32 year = 0;

		year = (day * 400 / (365 * 400 + 100 - 4 + 1));
		day -= year * 365 + year / 4 - year / 100 + year / 400;


		if (isValid == true)
		{
			year += 1;
			if (day == 0)
			{
				--year;
				if (year % 4 == 0)
				{
					day = 366;
					if (year % 100 == 0 && year % 400 != 0)
						day = 365;
				}
				else
					day = 365;
			}

			if (year % 4 == 0)
			{
				days[2] = 29;

				if (year % 100 == 0 && year % 400 != 0)
					days[2] = 28;
			}

			if (days[1] <= day)
			{
				for (month = 1; month <= 12 && day > days[month]; month++)
				{
					day -= days[month];
				}
			}
		}

		dateValue.Year = static_cast<uint16>(year);
		dateValue.Month = static_cast<uint8>(month);
		dateValue.Day = static_cast<uint8>(day);
	}
	int32 DateTime::Date::DateConvertToJulianDay()
	{
		if (dateValue.Month < 3)
		{
			dateValue.Month = dateValue.Month + 12;
			dateValue.Year = dateValue.Year - 1;
		}
		return dateValue.Day + (153 * dateValue.Month - 457) / 5 + 365
			* dateValue.Year + (dateValue.Year / 4) - (dateValue.Year / 100)
			+ (dateValue.Year / 400) + 1721119;
	}
	DateTime::Week::type DateTime::Date::GetDayOfWeek() const
	{
		if (!IsValid()) {
			Logger::GetInstance()->Warn("invalid value. Year = {0}, Month = {1}, Day = {2}"
				, dateValue.Year, dateValue.Month, dateValue.Day);
			throw fmt::format("invalid value. Year = {0}, Month = {1}, Day = {2}"
				, dateValue.Year, dateValue.Month, dateValue.Day);
		}

		struct Calculate {

			static bool IsLeapMonth(int year)
			{
				if (year % 400 == 0 || year % 100 != 0 && year % 4 == 0) {
					return true;
				}
				else {
					return false;
				}
			}

			static uint32 GetCumulatedDayOfNowYear(uint32 year, uint32 month, uint32 day)
			{
				static const uint32 Maxdays[12] = { 31
					, 59 // 31 + 28
					, 90 // 31 + 28 + 31
					, 120 // 31 + 28 + 30
					, 151 // 31 + 28 + 30 + 31
					, 181 // 31 + 28 + 30 + 31 + 30
					, 212 // 31 + 28 + 30 + 31 + 30 + 31
					, 243 // 31 + 28 + 30 + 31 + 30 + 31 + 31
					, 273 // 31 + 28 + 30 + 31 + 30 + 31 + 31 + 30
					, 304 // 31 + 28 + 30 + 31 + 30 + 31 + 31 + 30 + 31
					, 334 // 31 + 28 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30
					, 365 // 31 + 28 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31
				};
				uint32 JulianDay = day;

				if ((month > 1) && (month <= 12)) {
					JulianDay += Maxdays[month - 2];
				}

				if ((month > 2) && (IsLeapMonth(year) == true)) {
					JulianDay += 1;
				}
				return JulianDay;
			}
		};

		uint32 res = (Calculate::GetCumulatedDayOfNowYear(dateValue.Year, dateValue.Month, dateValue.Day)
			+ (dateValue.Year - 1)
			+ (dateValue.Year - 1) / 4
			- (dateValue.Year - 1) / 100
			+ (dateValue.Year - 1) / 400) % 7;

		switch (res)
		{
		case 0:
			return Week::Sunday;
		case 1:
			return Week::Monday;
		case 2:
			return Week::Tuesday;
		case 3:
			return Week::Wednesday;
		case 4:
			return Week::Thursday;
		case 5:
			return Week::Friday;
		case 6:
			return Week::Saturday;
		}

		Logger::GetInstance()->Warn("value error, res={0}", res);
		return Week::Sunday;
	}
	bool DateTime::Date::IsValid() const
	{
		if (dateValue.Month > 12 || dateValue.Month < 1)
		{
			Logger::GetInstance()->Error("error value. Month = {0}, Day = {1}", dateValue.Month, dateValue.Day);
			return false;
		}
		else if (dateValue.Day < 1 || dateValue.Day > GetLastDayOfMonth(dateValue.Year, dateValue.Month))
		{
			Logger::GetInstance()->Error("error value. Month = {0}, Day = {1}", dateValue.Month, dateValue.Day);
			return false;
		}
		return true;
	}
	DateTime::Date DateTime::Date::Now()
	{
		time_t ltime;
		time(&ltime);
		return Date(ltime);
	}
	bool DateTime::Date::IsValid(const std::wstring& date)
	{
		Date test;

		if (!(test.Parse(date.c_str())))
			return false;
		else
		{
			if (test.IsValid())
				return true;
			else
				return false;
		}
	}
	uint16 DateTime::Date::GetLastDayOfMonth(uint16 year, uint16 month)
	{
		static uint16 days[] = { 0/* no days in zeroth dateValue.Month*/, 31 /* January */,
			28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 /* December */ };

		if (month < 1 || month>12) {
			Logger::GetInstance()->Exception("invalid dateValue.Month. month = {0}", month);
			throw fmt::format("invalid dateValue.Month. month = {0}", month);
		}

		if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0))
			return 29;
		return days[month];
	}

	bool DateTime::Date::operator==(const Date& rhs) const
	{
		if (IsValid())
		{
			if (dateValue.Year == rhs.dateValue.Year && dateValue.Month == rhs.dateValue.Month && dateValue.Day == rhs.dateValue.Day)
				return true;
		}

		return false;
	}
	bool DateTime::Date::operator!=(const Date& rhs) const
	{
		if (IsValid())
		{
			return !(*this == rhs);
		}
		return false;
	}
	bool DateTime::Date::operator<(const Date& rhs) const
	{
		if (IsValid())
		{
			if (dateValue.Year < rhs.dateValue.Year)
				return true;
			else if (dateValue.Year == rhs.dateValue.Year)
			{
				if (dateValue.Month < rhs.dateValue.Month)
					return true;
				else if (dateValue.Month == rhs.dateValue.Month)
				{
					if (dateValue.Day < rhs.dateValue.Day)
						return true;
				}
			}
		}
		return false;
	}
	bool DateTime::Date::operator<=(const DateTime::Date& rhs) const
	{
		if (IsValid())
		{
			return (*this == rhs || *this < rhs);
		}
		return false;
	}
	bool DateTime::Date::operator>(const DateTime::Date& rhs) const
	{
		if (IsValid())
		{
			return !(*this < rhs || *this == rhs);
		}
		return false;
	}
	bool DateTime::Date::operator>=(const DateTime::Date& rhs) const
	{
		if (IsValid())
		{
			return !(*this < rhs);
		}
		return false;
	}
	DateTime::Date operator+(const DateTime::Date& lhs, const DateTime::Date& rhs)
	{
		DateTime::Date dateData(lhs);

		dateData.Add(DateTime::Unit::Day, rhs.GetDay(true));
		dateData.Add(DateTime::Unit::Month, rhs.GetMonth(true));
		dateData.Add(DateTime::Unit::Year, rhs.GetYear(true));

		return dateData;
	}
	DateTime::Date operator-(const DateTime::Date& lhs, const DateTime::Date& rhs)
	{
		DateTime::Date dateData(lhs);

		dateData.Add(DateTime::Unit::Day, -rhs.GetDay(true));
		dateData.Add(DateTime::Unit::Month, -rhs.GetMonth(true));
		dateData.Add(DateTime::Unit::Year, -rhs.GetYear(true));

		return dateData;
	}

	DateTime::Date& DateTime::Date::Add(Unit::type unit, int32 value)
	{
		if (unit >= Unit::Hour) {
			Logger::GetInstance()->Exception("argument is invalid, unit = {0}", unit);
			throw fmt::format("argument is invalid, unit = {0}", unit);
		}

		if (value == 0)
		{
			return *this;
		}
		else if (value > 0)
		{
			switch (unit)
			{
			case Unit::Year:
			{
				int32 year = dateValue.Year;
				year += value;
				dateValue.Year = static_cast<uint16>(year);
			}

			return *this;
			case Unit::Month:
			{
				int32 month = dateValue.Month;
				month += value;
				if (month > 12)
				{
					while (month > 12)
					{
						month -= 12;
						++dateValue.Year;
					}
				}
				dateValue.Month = static_cast<uint8>(month);
			}
			return *this;
			case Unit::Day:
				int32 totalDay = DateConvertToJulianDay() + value + 68569;
				int32 dateTempData[3];

				dateTempData[0] = ((4 * totalDay) / 146097);
				totalDay = totalDay - (((146097 * dateTempData[0] + 3) / 4));
				dateTempData[1] = ((4000 * (totalDay + 1) / 1461001));
				totalDay = totalDay - ((1461 * dateTempData[1]) / 4) + 31;
				dateTempData[2] = ((80 * totalDay) / 2447);

				dateValue.Day = static_cast<uint8>(totalDay - ((2447 * dateTempData[2]) / 80));

				totalDay = (dateTempData[2] / 11);
				dateValue.Month = static_cast<uint8>(dateTempData[2] + 2 - 12 * totalDay);

				dateValue.Year = static_cast<uint16>(100 * (dateTempData[0] - 49) + dateTempData[1] + totalDay);
				return *this;
			}
		}
		else
		{
			int32 number = 0;

			switch (unit)
			{
			case Unit::Year:
			{
				int32 year = dateValue.Year;
				year += value;
				dateValue.Year = static_cast<uint16>(year);
			}
			return *this;

			case Unit::Month:
				number = dateValue.Month;
				number += value;



				if (number % 12 == 0)
				{
					Add(DateTime::Unit::Year, number / 12);
					dateValue.Month = 12;
					return *this;
				}

				if (number <= -12 || number > 12)
				{
					Add(DateTime::Unit::Year, number / 12);
					number = number % 12;
				}

				if (number < 0)
				{
					Add(DateTime::Unit::Year, -1);
					number += 12;
				}

				dateValue.Month = static_cast<uint8>(number);
				return *this;

			case Unit::Day:
				int32 totalDay = DateConvertToJulianDay() + value + 68569;
				int32 dateTempData[3];
				dateTempData[0] = ((4 * totalDay) / 146097);
				totalDay = totalDay - (((146097 * dateTempData[0] + 3) / 4));
				dateTempData[1] = ((4000 * (totalDay + 1) / 1461001));
				totalDay = totalDay - ((1461 * dateTempData[1]) / 4) + 31;
				dateTempData[2] = ((80 * totalDay) / 2447);

				dateValue.Day = static_cast<uint8>(totalDay - ((2447 * dateTempData[2]) / 80));

				totalDay = (dateTempData[2] / 11);

				dateValue.Month = static_cast<uint8>(dateTempData[2] + 2 - 12 * totalDay);

				dateValue.Year = static_cast<uint16>(100 * (dateTempData[0] - 49) + dateTempData[1] + totalDay);
				return *this;
			}
		}

		return *this;
	}
	void DateTime::Time::SetCumulatedSec(double sec, const Time& origin)
	{
		sec += origin.GetCumulatedSec();

		if (sec > 24 * 60 * 60)
			sec -= 24 * 60 * 60; //86400 sec = 1 day
		else if (sec < 0)
			sec += 24 * 60 * 60;

		uint8 hour, minute;
		uint16 isec, msec;

		hour = static_cast<uint8>(sec / 3600);

		sec -= hour * 3600;
		minute = static_cast<uint8>(sec / 60);

		sec -= minute * 60;
		isec = static_cast<uint16>(sec);

		sec -= isec;
		msec = static_cast<uint16>(sec * Constant::MilliSecondsInASecond);

		timeValue.Hour = hour;
		timeValue.Minute = minute;
		timeValue.Second = isec;
		timeValue.MilliSec = msec;
	}
	bool DateTime::Time::IsValid() const
	{
		bool result = true;

		if (timeValue.Hour < 0 || timeValue.Hour >= Constant::HoursInADay)
		{
			Logger::GetInstance()->Warn("Invalid value, Unit::Hour = {0}. It can be only 0~{1}", timeValue.Hour, Constant::HoursInADay - 1);
			result = false;
		}
		if (timeValue.Minute < 0 || timeValue.Minute >= Constant::MinutesInAnHour)
		{
			Logger::GetInstance()->Warn("Invalid value, Unit::Minute = {0}. It can be only 0~{1}", timeValue.Minute, Constant::MinutesInAnHour - 1);
			result = false;
		}
		if (timeValue.Second < 0 || timeValue.Second >= Constant::SecondsInAMinute)
		{
			Logger::GetInstance()->Warn("Invalid value, Unit::Second = {0}. It can be only 0~{1}", timeValue.Second, Constant::SecondsInAMinute - 1);
			result = false;
		}
		if (timeValue.MilliSec < 0 || timeValue.MilliSec >= Constant::MilliSecondsInASecond)
		{
			Logger::GetInstance()->Warn("Invalid value, Unit::MilliSecond= {0}. It can be only 0~{1}", timeValue.MilliSec, Constant::MilliSecondsInASecond - 1);
			result = false;
		}

		return result;
	}
	DateTime::Time& DateTime::Time::Add(Unit::type unit, int32 value)
	{
		if (unit < Unit::Hour) {
			Logger::GetInstance()->Exception("argument is invalid. unit = {0}", unit);
			throw fmt::format("argument is invalid. unit = {0}", unit);
		}

		if (value == 0)
		{
			return *this;
		}
		/*
		else if(value > 0)
		{
			switch(unit)
			{
			case Unit::Hour:
				{
					int32 hour=timeValue.Hour;
					hour+=value;
					if(hour >= Constant::HoursInADay)
						hour = hour % Constant::HoursInADay;
					timeValue.Hour = static_cast<uint8>(hour);
				}
				return *this;
			case Unit::Minute:
				{
					int32 min=timeValue.Minute;
					min+=value;

					Add(Unit::Hour, min / Constant::MinutesInAnHour);

					if(min >= Constant::MinutesInAnHour)
						min =  min % Constant::MinutesInAnHour;
					timeValue.Minute = static_cast<uint8>(min);
				}
				return *this;
			case Unit::Second:
				{
					int32 sec=timeValue.Second;
					sec += value;

					Add(Unit::Minute, sec / Constant::SecondsInAMinute);

					if(sec >= Constant::MinutesInAnHour)
						sec = sec % Constant::SecondsInAMinute;

					timeValue.Second = static_cast<uint16>(sec);
				}
				return *this;
			case Unit::MilliSecond:
				{
					int32 msec=timeValue.MilliSec;
					msec += value;

					Add(Unit::Second, msec / Constant::MilliSecondsInASecond);

					if(msec >= Constant::MilliSecondsInASecond)
						msec = msec % Constant::MilliSecondsInASecond;
					timeValue.MilliSec = static_cast<uint16>(msec);
				}


				return *this;

			}
		}*/
		else
		{
			int32 number = 0;
			switch (unit)
			{
			case Unit::Hour:


				number += timeValue.Hour;
				number += value;

				if (number % Constant::HoursInADay == 0)
				{
					timeValue.Hour = 0;
					return *this;
				}

				if (number <= -(Constant::HoursInADay) || number >= Constant::HoursInADay)
					number = number % Constant::HoursInADay;

				if (number < 0)
					number += Constant::HoursInADay;

				timeValue.Hour = static_cast<uint8>(number);
				return *this;
			case Unit::Minute:
				number = timeValue.Minute;
				number += value;


				if (number % Constant::MinutesInAnHour == 0)
				{
					Add(DateTime::Unit::Hour, number / Constant::MinutesInAnHour);
					timeValue.Minute = 0;
					return *this;
				}

				if (number <= -(Constant::MinutesInAnHour) || number >= Constant::MinutesInAnHour)
				{
					Add(DateTime::Unit::Hour, number / Constant::MinutesInAnHour);
					number = number % Constant::MinutesInAnHour;
				}

				if (number < 0)
				{
					Add(DateTime::Unit::Hour, -1);
					number += Constant::MinutesInAnHour;
				}
				timeValue.Minute = static_cast<uint8>(number);
				return *this;

			case Unit::Second:
				number = timeValue.Second;
				number += value;

				if (number % Constant::SecondsInAMinute == 0)
				{
					Add(DateTime::Unit::Minute, number / Constant::SecondsInAMinute);
					timeValue.Second = 0;
					return *this;
				}

				if (number <= -(Constant::SecondsInAMinute) || number >= Constant::SecondsInAMinute)
				{
					Add(DateTime::Unit::Minute, number / Constant::SecondsInAMinute);
					number = number % Constant::SecondsInAMinute;
				}

				if (number < 0)
				{
					Add(DateTime::Unit::Minute, -1);
					number += Constant::SecondsInAMinute;
				}
				timeValue.Second = static_cast<uint16>(number);
				return *this;

			case Unit::MilliSecond:

				number = timeValue.MilliSec;
				number += value;

				if (number % Constant::MilliSecondsInASecond == 0)
				{
					Add(DateTime::Unit::Second, number / Constant::MilliSecondsInASecond);
					timeValue.MilliSec = 0;
					return *this;
				}

				if (number <= -(Constant::MilliSecondsInASecond) || number >= Constant::MilliSecondsInASecond)
				{
					Add(DateTime::Unit::Second, number / Constant::MilliSecondsInASecond);
					number = number % Constant::MilliSecondsInASecond;
				}

				if (number < 0)
				{
					Add(DateTime::Unit::Second, -1);
					number += Constant::MilliSecondsInASecond;
				}
				timeValue.MilliSec = static_cast<uint16>(number);
				return *this;
			}
		}
		return *this;
	}
	DateTime::Time& DateTime::Time::AddEx(Unit::type unit, int32 value, Date& date)
	{
		if (unit < Unit::Hour) 
		{
			Logger::GetInstance()->Exception("argument is invalid. unit = {0}", unit);
			throw fmt::format("argument is invalid. unit = {0}", unit);
		}

		if (value == 0)
		{
			return *this;
		}
		else
		{
			int32 number = 0;
			switch (unit)
			{
			case Unit::Hour:


				number += timeValue.Hour;
				number += value;

				if (number % Constant::HoursInADay == 0)
				{
					date.Add(Unit::Day, number / Constant::HoursInADay);

					timeValue.Hour = 0;
					return *this;
				}

				if (number <= -(Constant::HoursInADay) || number >= Constant::HoursInADay) {
					date.Add(Unit::Day, number / Constant::HoursInADay);

					number = number % Constant::HoursInADay;
				}

				if (number < 0) {
					date.Add(Unit::Day, -1);
					number += Constant::HoursInADay;
				}

				timeValue.Hour = static_cast<uint8>(number);
				return *this;
			case Unit::Minute:
				number = timeValue.Minute;
				number += value;

				// 2006.06.09 Caspian Changed these algorithm 

				if (number % Constant::MinutesInAnHour == 0)
				{
					AddEx(DateTime::Unit::Hour, number / Constant::MinutesInAnHour, date);
					timeValue.Minute = 0;
					return *this;
				}

				if (number <= -(Constant::MinutesInAnHour) || number >= Constant::MinutesInAnHour)
				{
					AddEx(DateTime::Unit::Hour, number / Constant::MinutesInAnHour, date);
					number = number % Constant::MinutesInAnHour;
				}

				if (number < 0)
				{
					AddEx(DateTime::Unit::Hour, -1, date);
					number += Constant::MinutesInAnHour;
				}
				timeValue.Minute = static_cast<uint8>(number);
				return *this;

			case Unit::Second:
				number = timeValue.Second;
				number += value;

				if (number % Constant::SecondsInAMinute == 0)
				{
					AddEx(DateTime::Unit::Minute, number / Constant::SecondsInAMinute, date);
					timeValue.Second = 0;
					return *this;
				}

				if (number <= -(Constant::SecondsInAMinute) || number >= Constant::SecondsInAMinute)
				{
					AddEx(DateTime::Unit::Minute, number / Constant::SecondsInAMinute, date);
					number = number % Constant::SecondsInAMinute;
				}

				if (number < 0)
				{
					AddEx(DateTime::Unit::Minute, -1, date);
					number += Constant::SecondsInAMinute;
				}
				timeValue.Second = static_cast<uint16>(number);
				return *this;

			case Unit::MilliSecond:

				number = timeValue.MilliSec;
				number += value;

				if (number % Constant::MilliSecondsInASecond == 0)
				{
					AddEx(DateTime::Unit::Second, number / Constant::MilliSecondsInASecond, date);
					timeValue.MilliSec = 0;
					return *this;
				}

				if (number <= -(Constant::MilliSecondsInASecond) || number >= Constant::MilliSecondsInASecond)
				{
					AddEx(DateTime::Unit::Second, number / Constant::MilliSecondsInASecond, date);
					number = number % Constant::MilliSecondsInASecond;
				}

				if (number < 0)
				{
					AddEx(DateTime::Unit::Second, -1, date);
					number += Constant::MilliSecondsInASecond;
				}
				timeValue.MilliSec = static_cast<uint16>(number);
				return *this;
			}
		}
		return *this;
	}
	DateTime::Time DateTime::Time::Now()
	{
		time_t now = time(0);
		return Time(now);
	}
	bool DateTime::Time::IsValid(const std::wstring& time)
	{
		Time test;
		if (!(test.Parse(time.c_str())))
			return false;
		else
		{
			if (test.IsValid())
				return true;
			else
				return false;
		}
	}

	bool DateTime::Time::operator==(const Time& rhs) const
	{
		if (IsValid())
		{
			if (timeValue.Hour == rhs.timeValue.Hour && timeValue.Minute == rhs.timeValue.Minute && timeValue.Second == rhs.timeValue.Second)
				return true;
		}
		return false;
	}
	bool DateTime::Time::operator!=(const Time& rhs) const
	{
		if (IsValid())
			return !(*this == rhs);

		return false;
	}
	bool DateTime::Time::operator<(const Time& rhs) const
	{
		if (IsValid())
		{
			if (timeValue.Hour < rhs.timeValue.Hour)
				return true;
			else if (timeValue.Hour == rhs.timeValue.Hour)
			{
				if (timeValue.Minute < rhs.timeValue.Minute)
					return true;
				else if (timeValue.Minute == rhs.timeValue.Minute)
				{
					if (timeValue.Second < rhs.timeValue.Second)
						return true;
				}
			}
		}
		return false;
	}
	bool DateTime::Time::operator<=(const Time& rhs) const
	{
		if (IsValid())
		{
			return (*this == rhs || *this < rhs);
		}
		return false;
	}
	bool DateTime::Time::operator>(const Time& rhs) const
	{
		if (IsValid())
		{
			return !(*this < rhs || *this == rhs);
		}
		return false;
	}
	bool DateTime::Time::operator>=(const Time& rhs) const
	{
		if (IsValid())
		{
			return !(*this < rhs);
		}
		return false;
	}
	DateTime::Time operator+(const DateTime::Time& lhs, const DateTime::Time& rhs)
	{
		DateTime::Time dateData(lhs);

		dateData.Add(DateTime::Unit::Hour, rhs.GetHour());
		dateData.Add(DateTime::Unit::Minute, rhs.GetMinute());
		dateData.Add(DateTime::Unit::Second, rhs.GetSecond());

		return dateData;
	}
	DateTime::Time operator-(const DateTime::Time& lhs, const DateTime::Time& rhs)
	{
		DateTime::Time timeData(lhs);

		timeData.Add(DateTime::Unit::Hour, -rhs.GetHour());
		timeData.Add(DateTime::Unit::Minute, -rhs.GetMinute());
		timeData.Add(DateTime::Unit::Second, -rhs.GetSecond());

		return timeData;
	}
#pragma endregion
}
