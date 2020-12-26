#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class EventUnit : public Serializer::ISerializable
				{
				public:
					typedef enum
					{
						Unknown,
						Once,
						Daily,
						Weekly,
						Monthly,
						MonthlySpecific,
						Immediately
					} ReserveType;

					typedef enum
					{
						NoEvent,
						BeginStageEvent,
						EndStageEvent,
						BeginScriptEvent,
						EndScriptEvent,
						BeginLevelUpPresentEvent,
						EndLevelUpPresentEvent,
						BeginPlayTimePresentEvent,
						EndPlayTimePresentEvent,
						BeginQuestEvent,
						EndQuestEvent,
						AlreadyBeginScriptEvent,
						BeginDungeonStageEvent,
						EndDungeonStageEvent,
						BeginNpcDropEvent,
						EndNpcDropEvent,
					} EventType;

					typedef std::map<std::wstring, int> StringTable;
					typedef std::vector<std::wstring> ParamContainer;
					static StringTable sharedStringTable;

					EventUnit();
					ReserveType GetReserveType();
					void SetReserveType(EventUnit::ReserveType eventType);
					EventType GetEventType() const;
					void SetEventType(EventUnit::EventType type);

					const DateTime& GetEventTime() const;
					void GetLastEventTime(DateTime& dateTime) const;
					void SetEventTime(const DateTime& dateTime);
					void SetFirstEventTime();
					void SetFirstEventTimeAfter(const DateTime& serverTime);
					bool SetNextEventTime();
					bool IsValid();
					const ParamContainer& GetEventInfo() const;
					void SetEventInfo(const ParamContainer& eventInfo);

					void InitStringTable();
					bool GetDataFromString(std::wstring& in, int& out);

					void ParseSelectedWeek(std::wstring selectedWeekStr);
					void ParseSelectedMonth(std::wstring selectedMonthStr);

					bool GetNextSelectedMonth(int month, int& nextMonth);
					bool GetNextSelectedWeek(int week, int& nextWeek);

					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);

					void operator =(const EventUnit& rhs);

				private:
					static const int monthCount = 12;

				protected:
					ReserveType reserveType;
					std::wstring reserveTypeStr;

					EventType eventType;
					std::wstring eventTypeStr;

					// the latest reserved event time.
					DateTime eventTime;

					// in specified day of week, and that should be number of passed 'day of week'.
					int specifiedWeek;
					std::wstring specifiedWeekStr;

					DateTime::Week::type specifiedDayOfWeek;
					std::wstring specifiedDayOfWeekStr;

					// in selected month.
					char selectedMonth[monthCount + 1];
					std::wstring selectedMonthStr;

					// in selected week.
					char selectedDayOfWeek[DateTime::Week::NumberOfWeek];
					std::wstring selectedDayOfWeekStr;

					// daily interval
					int dailyInterval;

					// weekly interval
					int weeklyInterval;

					// day (for once, for month)
					int day;

					// month (for once)
					int month;

					int year;

					// term of the next event time.
					DateTime::DateTerm eventTerm;

					// eventInfo
					ParamContainer eventInfo;
				};
			}
		}
	}
}
