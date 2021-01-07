#include "TimeSpan.h"
namespace Lunia {
	namespace XRated {
		TimeSpan::TimeSpan()
		{

		}

		void TimeSpan::SetStatus(TimeSpanStatus::type status)
		{
			m_status = status;
		}

		TimeSpan::TimeSpanStatus::type TimeSpan::GetStatus() const
		{
			return m_status;
		}

		void TimeSpan::SetTimeSpan(long days, int hours, int minutes, int seconds,
			int millis, int micros, int nanos)
			// Milli, Micro & nano, default = 0
		{

			int hundredsNanos;

			if (nanos >= 0)
				hundredsNanos = (nanos + 50) / 100;
			else
				hundredsNanos = (nanos - 50) / 100;

			micros += hundredsNanos / 10;
			hundredsNanos %= 10;
			millis += micros / 1000;
			micros %= 1000;
			seconds += millis / 1000;
			millis %= 1000;
			minutes += seconds / 60;
			seconds %= 60;
			hours += minutes / 60;
			minutes %= 60;
			days += hours / 24;
			hours %= 24;

			LiSpan = days;
			LiSpan *= 86400L;
			LiSpan += (hours * 3600L) +
				(minutes * 60) +
				seconds;
			LiSpan *= 10000000L;
			LiSpan += (millis * 10000L) +
				(micros * 10L) +
				hundredsNanos;

			SetStatus(TimeSpanStatus::type::Valid);
		} // TimeSpan::SetTimeSpan()

		void TimeSpan::SetTimeSpan(DateTime dateTime)
		{
			SetTimeSpan(dateTime.GetDate().GetCumulatedDay(), dateTime.Get(DateTime::Unit::Hour), dateTime.Get(DateTime::Unit::Minute), dateTime.Get(DateTime::Unit::Second), dateTime.Get(DateTime::Unit::MilliSecond));
		} // TimeSpan::SetTimeSpan()


		int64 TimeSpan::GetTotalDays() const    // span in days (about -3.65e6 to 3.65e6)
		{
			int64 liTemp;
			liTemp = LiSpan / 10000000L;
			liTemp /= 86400L;
			return liTemp;
		} // TimeSpan::GetTotalDays()

		int64 TimeSpan::GetTotalHours() const   // span in hours (about -8.77e7 to 8.77e6)
		{
			int64 liTemp;
			liTemp = LiSpan / 10000000L;
			liTemp /= 3600L;
			return liTemp;
		} // TimeSpan::GetTotalHours()

		int64 TimeSpan::GetTotalMinutes() const // span in minutes (about -5.26e9 to 5.26e9)
		{
			int64 liTemp;
			liTemp = LiSpan / 10000000L;
			liTemp /= 60L;
			return liTemp;
		} // TimeSpan::GetTotalMinutes()

		int64 TimeSpan::GetTotalSeconds() const // span in seconds (about -3.16e11 to 3.16e11)
		{
			int64 liTemp;
			liTemp = LiSpan / 10000000L;
			return liTemp;
		} // TimeSpan::GetTotalSeconds()

		int64 TimeSpan::GetTotalMilliSeconds() const // span in milliseconds
		{
			int64 liTemp;
			liTemp = LiSpan / 10000L;
			return liTemp;
		} // TimeSpan::GetTotalMilliSeconds()

		int64 TimeSpan::GetTotalMicroSeconds() const // span in microseconds
		{
			int64 liTemp;
			liTemp = LiSpan / 10L;
			return liTemp;
		} // TimeSpan::GetTotalMicroSeconds()

		int64 TimeSpan::GetTotalNanoSeconds() const // span in nanoseconds
		{
			int64 liTemp;
			liTemp = LiSpan * 100L;
			return liTemp;
		} // TimeSpan::GetTotalNanoSeconds()

		int TimeSpan::GetDays() const       // component days in span
		{
			int64 liTemp;
			liTemp = LiSpan / 10000000L;
			liTemp = (liTemp / 86400L);
			return (int)liTemp;
		} // TimeSpan::GetDays()

		int TimeSpan::GetHours() const      // component hours in span (-23 to 23)
		{
			int64 liTemp;
			liTemp = LiSpan / 10000000L;
			liTemp = (liTemp % 86400L) / 3600;
			return (int)liTemp;
		} // TimeSpan::GetHours()

		int TimeSpan::GetMinutes() const    // component minutes in span (-59 to 59)
		{
			int64 liTemp;
			liTemp = LiSpan / 10000000L;
			liTemp = (liTemp % 3600L) / 60;
			return (int)liTemp;
		} // TimeSpan::GetMinutes()

		int TimeSpan::GetSeconds() const    // component seconds in span (-59 to 59)
		{
			int64 liTemp;
			liTemp = LiSpan / 10000000L;
			liTemp = (liTemp % 60L);
			return (int)liTemp;
		} // TimeSpan::GetSeconds()

		int TimeSpan::GetMilliSeconds() const // component Milliseconds in span (-999 to 999)
		{
			int64 liTemp;
			liTemp = (LiSpan % 10000000L) / 10000L;
			return (int)liTemp;
		} // TimeSpan::GetMilliSeconds()

		int TimeSpan::GetMicroSeconds() const // component Microseconds in span (-999 to 999)
		{
			int64 liTemp;
			liTemp = (LiSpan % 10000L) / 10L;
			return (int)liTemp;
		} // TimeSpan::GetMicroSeconds()

		int TimeSpan::GetNanoSeconds() const  // component Nanoseconds in span (-900 to 900)
		{
			int64 liTemp;
			liTemp = (LiSpan % 10) * 100L;
			return (int)liTemp;
		} // TimeSpan::GetNanoSeconds()


		// TimeSpan math
		TimeSpan TimeSpan::operator+(const TimeSpan& dateSpan) const
		{
			TimeSpan dateSpanTemp;

			// If either operand Null, result Null
			if (GetStatus() == TimeSpanStatus::Null || dateSpan.GetStatus() == TimeSpanStatus::Null)
			{
				dateSpanTemp.SetStatus(TimeSpanStatus::type::Null);
				return dateSpanTemp;
			}

			// If either operand Invalid, result Invalid
			if (GetStatus() == TimeSpanStatus::type::Invalid || dateSpan.GetStatus() == TimeSpanStatus::type::Invalid)
			{
				dateSpanTemp.SetStatus(TimeSpanStatus::type::Invalid);
				return dateSpanTemp;
			}

			// Add spans and validate within legal range
			dateSpanTemp.LiSpan = LiSpan + dateSpan.LiSpan;

			return dateSpanTemp;
		} // TimeSpan::operator+()

		TimeSpan TimeSpan::operator-(const TimeSpan& dateSpan) const
		{
			TimeSpan dateSpanTemp;

			// If either operand Null, result Null
			if (GetStatus() == TimeSpanStatus::Null || dateSpan.GetStatus() == TimeSpanStatus::Null)
			{
				dateSpanTemp.SetStatus(TimeSpanStatus::Null);
				return dateSpanTemp;
			}

			// If either operand Invalid, result Invalid
			if (GetStatus() == TimeSpanStatus::type::Invalid || dateSpan.GetStatus() == TimeSpanStatus::type::Invalid)
			{
				dateSpanTemp.SetStatus(TimeSpanStatus::type::Invalid);
				return dateSpanTemp;
			}

			// Add spans and validate within legal range
			dateSpanTemp.LiSpan = LiSpan - dateSpan.LiSpan;

			return dateSpanTemp;
		} // TimeSpan::operator-()

		TimeSpan::operator int64() const
		{
			return LiSpan;
		}

		bool TimeSpan::operator<=(const TimeSpan& dateSpan) const
		{
			// If either operand Null, result Null
			if (GetStatus() == TimeSpanStatus::Null || dateSpan.GetStatus() == TimeSpanStatus::Null)
				return false;

			// If either operand Invalid, result Invalid
			if (GetStatus() == TimeSpanStatus::type::Invalid || dateSpan.GetStatus() == TimeSpanStatus::type::Invalid)
				return false;

			if (LiSpan <= dateSpan.LiSpan)
				return true;

			return false;
		}

		bool TimeSpan::operator<(const TimeSpan& dateSpan) const
		{
			// If either operand Null, result Null
			if (GetStatus() == TimeSpanStatus::Null || dateSpan.GetStatus() == TimeSpanStatus::Null)
				return false;

			// If either operand Invalid, result Invalid
			if (GetStatus() == TimeSpanStatus::type::Invalid || dateSpan.GetStatus() == TimeSpanStatus::type::Invalid)
				return false;

			if (LiSpan < dateSpan.LiSpan)
				return true;

			return false;
		}

		bool TimeSpan::operator>=(const TimeSpan& dateSpan) const
		{
			// If either operand Null, result Null
			if (GetStatus() == TimeSpanStatus::Null || dateSpan.GetStatus() == TimeSpanStatus::Null)
				return false;

			// If either operand Invalid, result Invalid
			if (GetStatus() == TimeSpanStatus::type::Invalid || dateSpan.GetStatus() == TimeSpanStatus::type::Invalid)
				return false;

			if (LiSpan >= dateSpan.LiSpan)
				return true;

			return false;
		}

		bool TimeSpan::operator>(const TimeSpan& dateSpan) const
		{
			// If either operand Null, result Null
			if (GetStatus() == TimeSpanStatus::Null || dateSpan.GetStatus() == TimeSpanStatus::Null)
				return false;

			// If either operand Invalid, result Invalid
			if (GetStatus() == TimeSpanStatus::type::Invalid || dateSpan.GetStatus() == TimeSpanStatus::type::Invalid)
				return false;

			if (LiSpan > dateSpan.LiSpan)
				return true;

			return false;
		}

		bool TimeSpan::operator==(const TimeSpan& dateSpan) const
		{
			// If either operand Null, result Null
			if (GetStatus() == TimeSpanStatus::Null || dateSpan.GetStatus() == TimeSpanStatus::Null)
				return false;

			// If either operand Invalid, result Invalid
			if (GetStatus() == TimeSpanStatus::type::Invalid || dateSpan.GetStatus() == TimeSpanStatus::type::Invalid)
				return false;

			if (LiSpan == dateSpan.LiSpan)
				return true;

			return false;
		}

		bool TimeSpan::operator!=(const TimeSpan& dateSpan) const
		{
			// If either operand Null, result Null
			if (GetStatus() == TimeSpanStatus::Null || dateSpan.GetStatus() == TimeSpanStatus::Null)
				return false;

			// If either operand Invalid, result Invalid
			if (GetStatus() == TimeSpanStatus::type::Invalid || dateSpan.GetStatus() == TimeSpanStatus::type::Invalid)
				return false;

			if (LiSpan != dateSpan.LiSpan)
				return true;

			return false;
		}
	}
}