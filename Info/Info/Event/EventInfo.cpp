#pragma once
#include "EventInfo.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				EventUnit::StringTable EventUnit::sharedStringTable;

				EventUnit::EventUnit() :
					eventType(NoEvent),
					reserveType(Unknown),
					specifiedWeek(DateTime::Unknown),
					specifiedDayOfWeek(const_cast<DateTime::Week::type>(DateTime::Week::NumberOfWeek)),
					dailyInterval(1),
					weeklyInterval(1),
					day(0),
					month(0)
				{
					memset(selectedMonth, 0, sizeof(int) * (monthCount + 1));
					memset(selectedDayOfWeek, 0, sizeof(int) * DateTime::Week::NumberOfWeek);
					InitStringTable();
				}

				EventUnit::ReserveType EventUnit::GetReserveType()
				{
					return reserveType;
				}

				void EventUnit::SetReserveType(EventUnit::ReserveType type)
				{
					reserveType = type;
				}

				EventUnit::EventType EventUnit::GetEventType() const
				{
					return eventType;
				}

				void EventUnit::SetEventType(EventUnit::EventType type)
				{
					eventType = type;
				}

				void EventUnit::SetEventInfo(const ParamContainer& info)
				{
					eventInfo = info;
				}

				void EventUnit::operator =(const EventUnit& rhs)
				{
					reserveType = rhs.reserveType;
					reserveTypeStr = rhs.reserveTypeStr;
					eventTime = rhs.eventTime;
					eventType = rhs.eventType;
					eventTypeStr = rhs.eventTypeStr;
					specifiedWeek = rhs.specifiedWeek;
					specifiedWeekStr = rhs.specifiedWeekStr;
					specifiedDayOfWeek = rhs.specifiedDayOfWeek;
					specifiedDayOfWeekStr = rhs.specifiedDayOfWeekStr;

					selectedMonthStr = rhs.selectedMonthStr;
					memcpy(selectedMonth, (void*)rhs.selectedMonth, sizeof(char) * (monthCount + 1));

					selectedDayOfWeekStr = rhs.selectedDayOfWeekStr;
					memcpy(selectedDayOfWeek, (void*)rhs.selectedDayOfWeek, sizeof(char) * DateTime::Week::NumberOfWeek);

					dailyInterval = rhs.dailyInterval;
					weeklyInterval = rhs.weeklyInterval;
					day = rhs.day;
					month = rhs.month;
					year = rhs.year;

					eventTerm = rhs.eventTerm;
					eventInfo = rhs.eventInfo;

				}

				void EventUnit::InitStringTable()
				{
					// EventType
					sharedStringTable.insert(std::make_pair(L"stageevent", 1));
					sharedStringTable.insert(std::make_pair(L"beginstageevent", 1));
					sharedStringTable.insert(std::make_pair(L"endstageevent", 2));
					sharedStringTable.insert(std::make_pair(L"beginscriptevent", 3));
					sharedStringTable.insert(std::make_pair(L"endscriptevent", 4));
					sharedStringTable.insert(std::make_pair(L"beginpresentevent", 5));		
					sharedStringTable.insert(std::make_pair(L"beginleveluppresentevent", 5));
					sharedStringTable.insert(std::make_pair(L"endpresentevent", 6));		
					sharedStringTable.insert(std::make_pair(L"endleveluppresentevent", 6));	
					sharedStringTable.insert(std::make_pair(L"beginplaytimepresentevent", 7));
					sharedStringTable.insert(std::make_pair(L"endplaytimepresentevent", 8));
					sharedStringTable.insert(std::make_pair(L"beginquestevent", 9));
					sharedStringTable.insert(std::make_pair(L"endquestevent", 10));
					sharedStringTable.insert(std::make_pair(L"begindungeonstageevent", 12));
					sharedStringTable.insert(std::make_pair(L"enddungeonstageevent", 13));
					sharedStringTable.insert(std::make_pair(L"beginnpcdropevent", 14));
					sharedStringTable.insert(std::make_pair(L"endnpcdropevent", 15));

					// ReserveType
					sharedStringTable.insert(std::make_pair(L"once", 1));
					sharedStringTable.insert(std::make_pair(L"daily", 2));
					sharedStringTable.insert(std::make_pair(L"weekly", 3));
					sharedStringTable.insert(std::make_pair(L"monthly", 4));
					sharedStringTable.insert(std::make_pair(L"monthlyspecific", 5));

					sharedStringTable.insert(std::make_pair(L"first", 1));
					sharedStringTable.insert(std::make_pair(L"second", 2));
					sharedStringTable.insert(std::make_pair(L"third", 3));
					sharedStringTable.insert(std::make_pair(L"fourth", 4));
					sharedStringTable.insert(std::make_pair(L"last", 5));

					// Day of week
					sharedStringTable.insert(std::make_pair(L"sun", 0));
					sharedStringTable.insert(std::make_pair(L"sunday", 0));
					sharedStringTable.insert(std::make_pair(L"mon", 1));
					sharedStringTable.insert(std::make_pair(L"monday", 1));
					sharedStringTable.insert(std::make_pair(L"tue", 2));
					sharedStringTable.insert(std::make_pair(L"tuesday", 2));
					sharedStringTable.insert(std::make_pair(L"wed", 3));
					sharedStringTable.insert(std::make_pair(L"wedneday", 3));
					sharedStringTable.insert(std::make_pair(L"thur", 4));
					sharedStringTable.insert(std::make_pair(L"thursday", 4));
					sharedStringTable.insert(std::make_pair(L"fri", 5));
					sharedStringTable.insert(std::make_pair(L"friday", 5));
					sharedStringTable.insert(std::make_pair(L"sat", 6));
					sharedStringTable.insert(std::make_pair(L"saturday", 6));
				}

				bool EventUnit::GetDataFromString(std::wstring& in, int& out)
				{
					StringUtil::Trim(in);
					StringUtil::ToLowerCase(in);
					StringTable::iterator itr = sharedStringTable.find(in);
					if (itr == sharedStringTable.end())
					{
						return false;
					}

					out = itr->second;
					return true;
				}

				const DateTime& EventUnit::GetEventTime() const
				{
					return eventTime;
				}

				void EventUnit::GetLastEventTime(DateTime& dateTime) const
				{
					dateTime = GetEventTime();

					switch (reserveType)
					{
					case Daily:
						dateTime.Add(DateTime::Unit::Day, -1);
						break;
					case Weekly:
						dateTime.Add(DateTime::Unit::Day, -7);
						break;
					case Monthly:
					case MonthlySpecific:
						dateTime.Add(DateTime::Unit::Year, -1);
						break;
					}
				}

				bool EventUnit::SetNextEventTime()
				{
					DateTime finalTime = eventTime;
					DateTime now = DateTime::Now();
					DateTime diff;

					switch (reserveType)
					{
					case Once:
					case Immediately:
					{
						return false;
					}
					case Daily:
					{
						// set next day.
						eventTerm.termUnit = DateTime::Unit::Day;
						eventTerm.term = dailyInterval;
					} break;
					case Weekly:
					{
						int nextWeek = 0;
						int thisWeek = eventTime.GetDayOfWeek();

						if (GetNextSelectedWeek(thisWeek, nextWeek))
						{
							// set next selected week.
							int dayDiff = nextWeek - eventTime.GetDayOfWeek();
							assert(dayDiff > 0);
							eventTerm.termUnit = DateTime::Unit::Day;
							eventTerm.term = dayDiff;
						}
						else
						{
							// set first selected day of week and strides interval of week.
							int dayDiff = nextWeek - eventTime.GetDayOfWeek();
							assert(dayDiff <= 0);
							eventTerm.termUnit = DateTime::Unit::Day;
							eventTerm.term = dayDiff + weeklyInterval * 7;
						}
					} break;
					case Monthly:
					case MonthlySpecific:
					{
						int thisMonth = eventTime.GetDate().GetMonth();
						if (!GetNextSelectedMonth(thisMonth, month))
						{
							eventTime.Add(DateTime::Unit::Year, 1);
						}
						eventTime.Set(DateTime::Unit::Month, static_cast<uint16>(month));
						if (reserveType == MonthlySpecific)
						{
							day = DateTime::GetSpecificDayOfWeek(static_cast<DateTime::SpecificType>(specifiedWeek), specifiedDayOfWeek, eventTime);
							Logger::GetInstance().Info(L"GetSpecificDayOfWeek, Month : {0}, Day : {1}", month, day);
						}

						eventTime.Set(DateTime::Unit::Day, day);
					} break;
					}

					// it needs assignment?
					eventTime = eventTime.Add(eventTerm);
					Logger::GetInstance().Info(L"ReserveType : {0}, NextEventTime : {1}", GetReserveType(), eventTime.ToString().c_str());

					return IsValid();
				}

				void EventUnit::SetFirstEventTime()
				{
					DateTime finalTime = eventTime;
					DateTime now = DateTime::Now();
					DateTime diff;

					switch (reserveType)
					{
					case Immediately:
					{
						break;
					}
					case Once:
					{
						eventTime.Set(DateTime::Unit::Year, year);
						eventTime.Set(DateTime::Unit::Month, month);
						eventTime.Set(DateTime::Unit::Day, day);
						break;
					}
					case Daily:
					{
						eventTime.Set(DateTime::Unit::Year, now.GetDate().GetYear());
						eventTime.Set(DateTime::Unit::Month, now.GetDate().GetMonth());
						eventTime.Set(DateTime::Unit::Day, now.GetDate().GetDay());
					} break;
					case Weekly:
					{
						eventTime.Set(DateTime::Unit::Year, now.GetDate().GetYear());
						eventTime.Set(DateTime::Unit::Month, now.GetDate().GetMonth());
						eventTime.Set(DateTime::Unit::Day, now.GetDate().GetDay());

						int nextWeek = 0;
						int thisWeek = now.GetDate().GetDayOfWeek();
						if (selectedDayOfWeek[thisWeek] == 0)
						{
							if (GetNextSelectedWeek(thisWeek, nextWeek))
							{
								// set next selected week.
								int dayDiff = nextWeek - now.GetDayOfWeek();
								eventTerm.termUnit = DateTime::Unit::Day;
								eventTerm.term = dayDiff;
							}
							else
							{
								// set first selected day of week and strides interval of week.
								int dayDiff = nextWeek - thisWeek;
								assert(dayDiff <= 0);
								eventTerm.termUnit = DateTime::Unit::Day;
								eventTerm.term = dayDiff + weeklyInterval * 7;
							}
							eventTime = eventTime.Add(eventTerm);
						}
					} break;
					case Monthly:
					case MonthlySpecific:
					{
						eventTime.Set(DateTime::Unit::Year, now.GetDate().GetYear());
						eventTime.Set(DateTime::Unit::Day, 1);
						int thisMonth = now.GetDate().GetMonth();

						if (selectedMonth[thisMonth] == 0)
						{
							if (!GetNextSelectedMonth(thisMonth, month))
							{
								eventTime.Add(DateTime::Unit::Year, 1);
							}
						}
						else
						{
							month = thisMonth;
						}

						eventTime.Set(DateTime::Unit::Month, static_cast<uint16>(month));
						if (reserveType == MonthlySpecific)
						{
							day = DateTime::GetSpecificDayOfWeek(static_cast<DateTime::SpecificType>(specifiedWeek), specifiedDayOfWeek, eventTime);
							Logger::GetInstance().Info(L"GetSpecificDayOfWeek, Month : {0}, Day : {1}", month, day);
						}

						eventTime.Set(DateTime::Unit::Day, day);
					} break;
					}

					Logger::GetInstance().Info(L"ReserveType : {0}, SetFirstEventTime : {1}", GetReserveType(), eventTime.ToString().c_str());
					if (now > eventTime)
					{
						Logger::GetInstance().Info(L"FirstTime is Already Passed !!!");
						SetNextEventTime();
					}
				}

				void EventUnit::SetFirstEventTimeAfter(const DateTime& serverTime)
				{
					DateTime finalTime = eventTime;
					DateTime now = serverTime;
					DateTime diff;

					switch (reserveType)
					{
					case Immediately:
					{
						break;
					}
					case Once:
					{
						eventTime.Set(DateTime::Unit::Year, year);
						eventTime.Set(DateTime::Unit::Month, month);
						eventTime.Set(DateTime::Unit::Day, day);
						break;
					}
					case Daily:
					{
						eventTime.Set(DateTime::Unit::Year, now.GetDate().GetYear());
						eventTime.Set(DateTime::Unit::Month, now.GetDate().GetMonth());
						eventTime.Set(DateTime::Unit::Day, now.GetDate().GetDay());
					} break;
					case Weekly:
					{
						eventTime.Set(DateTime::Unit::Year, now.GetDate().GetYear());
						eventTime.Set(DateTime::Unit::Month, now.GetDate().GetMonth());
						eventTime.Set(DateTime::Unit::Day, now.GetDate().GetDay());

						int nextWeek = 0;
						int thisWeek = now.GetDate().GetDayOfWeek();
						if (selectedDayOfWeek[thisWeek] == 0)
						{
							if (GetNextSelectedWeek(thisWeek, nextWeek))
							{
								// set next selected week.
								int dayDiff = nextWeek - now.GetDayOfWeek();
								eventTerm.termUnit = DateTime::Unit::Day;
								eventTerm.term = dayDiff;
							}
							else
							{
								// set first selected day of week and strides interval of week.
								int dayDiff = nextWeek - thisWeek;
								assert(dayDiff <= 0);
								eventTerm.termUnit = DateTime::Unit::Day;
								eventTerm.term = dayDiff + weeklyInterval * 7;
							}
							eventTime = eventTime.Add(eventTerm);
						}
					} break;
					case Monthly:
					case MonthlySpecific:
					{
						eventTime.Set(DateTime::Unit::Year, now.GetDate().GetYear());
						eventTime.Set(DateTime::Unit::Day, 1);
						int thisMonth = now.GetDate().GetMonth();

						if (selectedMonth[thisMonth] == 0)
						{
							if (!GetNextSelectedMonth(thisMonth, month))
							{
								eventTime.Add(DateTime::Unit::Year, 1);
							}
						}
						else
						{
							month = thisMonth;
						}

						eventTime.Set(DateTime::Unit::Month, static_cast<uint16>(month));
						if (reserveType == MonthlySpecific)
						{
							day = DateTime::GetSpecificDayOfWeek(static_cast<DateTime::SpecificType>(specifiedWeek), specifiedDayOfWeek, eventTime);
							Logger::GetInstance().Info(L"GetSpecificDayOfWeek, Month : {0}, Day : {1}", month, day);
						}

						eventTime.Set(DateTime::Unit::Day, day);
					} break;
					}

					Logger::GetInstance().Info(L"ReserveType : {0}, SetFirstEventTime : {1}", GetReserveType(), eventTime.ToString().c_str());
					if (now > eventTime)
					{
						Logger::GetInstance().Info(L"FirstTime is Already Passed !!!");
						SetNextEventTime();
					}
				}

				const EventUnit::ParamContainer& EventUnit::GetEventInfo() const
				{
					return eventInfo;
				}

				void EventUnit::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"Stage::EventUnit");

					// data reverse.
					out.Write(L"EventType", eventTypeStr);
					out.Write(L"EventInfo", eventInfo);
					out.Write(L"ReserveType", reserveTypeStr);

					std::wstring startAt = eventTime.GetTime().ToString();
					out.Write(L"StartAt", startAt);

					out.Write(L"Year", year);
					out.Write(L"Month", month);
					out.Write(L"Day", day);

					out.Write(L"SelectedMonth", selectedMonthStr);
					out.Write(L"SelectedDayOfWeek", selectedDayOfWeekStr);

					out.Write(L"SpecifiedWeek", specifiedWeekStr);
					out.Write(L"SpecifiedDayOfWeek", specifiedDayOfWeekStr);
					out.Write(L"WeeklyInterval", weeklyInterval);
					out.Write(L"DailyInterval", dailyInterval);
				}

				void EventUnit::Deserialize(Serializer::IStreamReader& in)
				{
					std::wstring startTime;

					in.Begin(L"Stage::EventUnit");
					in.Read(L"EventType", eventTypeStr);
					if (!GetDataFromString(eventTypeStr, reinterpret_cast<int&>(eventType)))
					{
						eventType = NoEvent;
					}

					// Event Infos (vector)
					in.Read(L"EventInfo", eventInfo, std::vector<std::wstring>());
					in.Read(L"ReserveType", reserveTypeStr);
					if (!GetDataFromString(reserveTypeStr, reinterpret_cast<int&>(reserveType)))
					{
						reserveType = Unknown;
					}

					in.Read(L"StartAt", startTime);	// essential
					in.Read(L"Year", year, 0);
					in.Read(L"Month", month, 0);
					in.Read(L"Day", day, 0);		// essential in most of all.

					DateTime::Time time(startTime.c_str());
					eventTime.SetTime(time);

					in.Read(L"SelectedMonth", selectedMonthStr, std::wstring(L""));		// for monthly (optional)
					ParseSelectedMonth(selectedMonthStr);

					in.Read(L"SelectedDayOfWeek", selectedDayOfWeekStr, std::wstring(L""));	// for weekly (optional)
					ParseSelectedWeek(selectedDayOfWeekStr);

					in.Read(L"SpecifiedWeek", specifiedWeekStr, std::wstring(L""));	// for monthly specific
					GetDataFromString(specifiedWeekStr, reinterpret_cast<int&>(specifiedWeek));

					in.Read(L"SpecifiedDayOfWeek", specifiedDayOfWeekStr, std::wstring(L""));	// for monthly specific
					GetDataFromString(specifiedDayOfWeekStr, reinterpret_cast<int&>(specifiedDayOfWeek));

					in.Read(L"WeeklyInterval", weeklyInterval, 1);	// for weekly
					in.Read(L"DailyInterval", dailyInterval, 1);	// for daily
				}

				void EventUnit::SetEventTime(const DateTime& dateTime)
				{
					eventTime = dateTime;
					year = dateTime.GetDate().GetYear();
					month = dateTime.GetDate().GetMonth();
					day = dateTime.GetDate().GetDay();
				}

				bool EventUnit::IsValid()
				{
					// we need validation.
					bool isValid = false;
					switch (reserveType)
					{
					case Immediately:
					{
						isValid = true;
					}
					break;
					case Once:
					{
						// eventTime.IsValid() [day, month]
						isValid = eventTime.IsValid();
					}
					break;
					case Daily:
					{
						// eventTime.GetTime().IsValid() [just need time]
						// dailyInterval limitation(365)
						isValid = eventTime.GetTime().IsValid();
						if (dailyInterval <= 0 || dailyInterval >= 365)
						{
							Logger::GetInstance().Info(L"Invalid dailyInterval Range : {0}", dailyInterval);
							isValid = false;
							break;
						}
					}
					break;
					case Weekly:
					{
						// weeklyInterval limitation(54) => one year limit(54 weeks!)
						// eventTime.GetTime().IsValid() [just need time]
						// at least one selected week.
						isValid = eventTime.GetTime().IsValid();
						if (!isValid)
						{
							Logger::GetInstance().Info(L"Invalid StartTime : {0}", eventTime.GetTime().ToString());
							break;
						}

						if (weeklyInterval <= 0 || weeklyInterval >= 54)
						{
							Logger::GetInstance().Info(L"Invalid weeklyInterval Range : {0}", weeklyInterval);
							isValid = false;
							break;
						}

						bool findSelectedDayOfweek = false;
						for (int index = 0; index < DateTime::Week::NumberOfWeek; ++index)
						{
							if (selectedDayOfWeek[index])
							{
								findSelectedDayOfweek = true;
								isValid = true;
								break;
							}
						}

						if (!findSelectedDayOfweek)
						{
							Logger::GetInstance().Info(L"No Selected Day Of Week!");
							isValid = false;
						}
					}
					break;
					case Monthly:
					{
						// eventTime.GetTime().IsValid()
						// day, and at least one selected month. [day and selected months]
						isValid = eventTime.GetTime().IsValid();
						if (!isValid || day <= 0)
						{
							isValid = false;
							break;
						}

						bool findSelectedMonth = false;
						for (int index = 1; index <= monthCount; ++index)
						{
							if (selectedMonth[index])
							{
								findSelectedMonth = true;
								break;
							}
						}

						if (!findSelectedMonth)
						{
							Logger::GetInstance().Info(L"No Selected Day Of Month!");
							isValid = false;
						}
					}
					break;
					case MonthlySpecific:
					{
						// eventTime.GetTime().IsValid()
						// specifiedWeek is default first, but have to selected.
						// specifiedDayOfWeek has to selected.
						isValid = eventTime.GetTime().IsValid();
						if (!isValid || specifiedDayOfWeek == DateTime::Week::NumberOfWeek)
						{
							Logger::GetInstance().Info(L"No settings: specifiedDayOfWeek");
							isValid = false;
							break;
						}

						if (specifiedWeek == DateTime::Unknown)
						{
							Logger::GetInstance().Info(L"No settings: specifiedWeek");
							isValid = false;
							break;
						}
					} break;
					default:
					{
						isValid = false;
					} break;
					}

					if (GetReserveType() == EventUnit::Once &&
						DateTime::Now() > GetEventTime())
					{
						isValid = false;
					}

					return isValid;
				}

				void EventUnit::ParseSelectedMonth(std::wstring selectedMonths)
				{
					if (selectedMonths.empty())
					{
						return;
					}

					typedef std::vector<std::wstring> TokenContainer;
					TokenContainer tokens;
					StringUtil::ToLowerCase(selectedMonths);
					StringUtil::Trim(selectedMonths);
					StringUtil::Split(selectedMonths.begin(), selectedMonths.end(), ',', tokens);

					int index = 0;
					for (TokenContainer::iterator itr = tokens.begin();
						itr != tokens.end();
						++itr)
					{
						// 1~12
						index = StringUtil::ToInt(itr->c_str());
						if (index < 1 && index > monthCount)
						{
							continue;
						}

						//ALLM_IMPORTANT((L"Selected Month : %d", index));
						selectedMonth[index] = 1;
					}
				}

				bool EventUnit::GetNextSelectedMonth(int month, int& nextMonth)
				{
					if (month < 1)
					{
						return false;
					}

					bool findNextMonth = false;
					while (++month <= EventUnit::monthCount)
					{
						if (selectedMonth[month] != 0)
						{
							findNextMonth = true;
							nextMonth = month;
							break;
						}
					}

					if (!findNextMonth)
					{
						// month starts at index one.
						int index = 1;
						while (!selectedMonth[index] && index <= monthCount)
						{
							++index;
						}
						nextMonth = index;
					}

					return findNextMonth;
				}

				void EventUnit::ParseSelectedWeek(std::wstring selectedWeeks)
				{
					if (selectedWeeks.empty())
					{
						return;
					}

					typedef std::vector<std::wstring> TokenContainer;
					TokenContainer tokens;
					StringUtil::ToLowerCase(selectedWeeks);
					StringUtil::Trim(selectedWeeks);
					StringUtil::Split(selectedWeeks.begin(), selectedWeeks.end(), ',', tokens);

					int index = 0;
					for (TokenContainer::iterator itr = tokens.begin();
						itr != tokens.end();
						++itr)
					{
						if (GetDataFromString(*itr, index))
						{
							assert(index >= DateTime::Week::Sunday && index < DateTime::Week::NumberOfWeek);
							if (index < 0 || index >= DateTime::Week::NumberOfWeek)
							{
								Logger::GetInstance().Info(L"Invalid Week Index: {0}", index);
								continue;
							}
							selectedDayOfWeek[index] = 1;
						}
					}
				}

				bool EventUnit::GetNextSelectedWeek(int week, int& nextWeek)
				{
					if (week < 0)
					{
						return false;
					}

					bool findNextWeek = false;
					while (++week < DateTime::Week::NumberOfWeek)
					{
						if (selectedDayOfWeek[week] != 0)
						{
							findNextWeek = true;
							nextWeek = week;
							break;
						}
					}

					if (!findNextWeek)
					{
						int index = 0;
						while (!selectedDayOfWeek[index])
						{
							++index;
						}
						nextWeek = index;
					}
					return findNextWeek;
				}
			}
		}
	}
}
