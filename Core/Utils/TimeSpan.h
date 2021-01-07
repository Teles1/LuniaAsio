#pragma once
#include <Core/Utils/DateTime.h>
namespace Lunia {
	namespace XRated {
		class TimeSpanException : public Exception
		{
		public:
			TimeSpanException(const wchar_t* message) : Exception(message) {}
		};

		class TimeSpan
		{
		public:
			struct TimeSpanStatus
			{
				enum type
				{
					Valid = 0,
					Invalid = 1,
					Null = 2
				};
			};

			TimeSpan();
			//		TimeSpan(const int64 dateSrcSpan);
			//		TimeSpan(const LARGE_INTEGER dateSrcSpan);
			//		TimeSpan(const TimeSpan &dateSrcSpan);
			//		TimeSpan(long lDays, int nHours, int nMins, int nSecs,
			//					int nMilli = 0, int nMicro = 0, int nNano = 0);

					// Operations
		public:
			void SetStatus(TimeSpanStatus::type status);
			TimeSpanStatus::type GetStatus() const;

			void SetTimeSpan(long days, int hours, int mins, int secs,
				int millis = 0, int micros = 0, int nanos = 0);
			void SetTimeSpan(DateTime dateTime);

			int64 GetTotalDays() const;    // span in days (about -3.65e6 to 3.65e6)
			int64 GetTotalHours() const;   // span in hours (about -8.77e7 to 8.77e6)
			int64 GetTotalMinutes() const; // span in minutes (about -5.26e9 to 5.26e9)
			int64 GetTotalSeconds() const; // span in seconds (about -3.16e11 to 3.16e11)
			int64 GetTotalMilliSeconds() const; // span in milliseconds
			int64 GetTotalMicroSeconds() const; // span in microseconds
			int64 GetTotalNanoSeconds() const; // span in nanoseconds

			int GetDays() const;       // component days in span
			int GetHours() const;      // component hours in span (-23 to 23)
			int GetMinutes() const;    // component minutes in span (-59 to 59)
			int GetSeconds() const;    // component seconds in span (-59 to 59)
			int GetMilliSeconds() const; // component Milliseconds in span (-999 to 999)
			int GetMicroSeconds() const; // component Microseconds in span (-999 to 999)
			int GetNanoSeconds() const;  // component Nanoseconds in span (-900 to 900)

			//bool IsNegative() const;

			// formatting
	//		LPSTR Format(LPSTR pszBuffer, int iBufferLen, LPCTSTR pFormat) const;

		public:
			// operators
	//		const TimeSpan& operator=(LARGE_INTEGER dateSpanSrc);
	//		const TimeSpan& operator=(int64 dateSpanSrc);
	//		const TimeSpan& operator=(const TimeSpan& dateSpanSrc);

			bool operator==(const TimeSpan& dateSpan) const;
			bool operator!=(const TimeSpan& dateSpan) const;
			bool operator<(const TimeSpan& dateSpan) const;
			bool operator>(const TimeSpan& dateSpan) const;
			bool operator<=(const TimeSpan& dateSpan) const;
			bool operator>=(const TimeSpan& dateSpan) const;

			// TimeSpan math
			TimeSpan operator+(const TimeSpan& dateSpan) const;
			TimeSpan operator-(const TimeSpan& dateSpan) const;
			//		const TimeSpan& operator+=(const TimeSpan dateSpan);
			//		const TimeSpan& operator-=(const TimeSpan dateSpan);
			//		TimeSpan operator-() const;

			operator int64() const;

		public:
			TimeSpanStatus::type m_status;
			int64      LiSpan;
		private:
			friend class DateTime;
		}; // class TimeSpan
	}
}