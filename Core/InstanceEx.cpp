#pragma once
#include "InstanceEx.h"

namespace Lunia {
	namespace XRated {
        const DateTime InstanceEx::NoExpiration(L"2050-12-31 00:00:00");
        bool InstanceEx::IsExpired(const DateTime& now) const
        {
            if (ExpireDate == NoExpiration)
                return false;
            if (ExpireDate.GetDate().GetYear(0) == 0)
                return false;
            if (ExpireDate > now)
                return false;
            return true;
        }
        void InstanceEx::ForceExpiration()
        {
            DateTime newExpireDate(DateTime::Now());
            newExpireDate.GetDate().SetYear(newExpireDate.GetDate().GetYear() - 1);
            ExpireDate = newExpireDate;
        }

        void InstanceEx::MakeUnlimitedPeriod()
        {
            ExpireDate = NoExpiration;
        }
        DateTime InstanceEx::GetExpiredDate() const
        {
            return ExpireDate;
        }
        DateTime InstanceEx::ExtensionExpiredDay(uint32 day)
        {
            if (*this == NoExpiration)
                return DateTime::Infinite;

            DateTime nowExpiredDate = DateTime::Now(); //NormalBitfields::GetExpiredDate();

            nowExpiredDate = nowExpiredDate.Add(DateTime::Unit::Day, day);

            return nowExpiredDate;
        }
        void InstanceEx::Serialize(Serializer::StreamWriter& out) const
        {
            //out.Begin(L"InstanceEx");
            out.Write(Instance);

            ExpireDateFormat tmp;
            tmp.Year = ExpireDate.GetDate().GetYear();
            tmp.Month = ExpireDate.GetDate().GetMonth();
            tmp.Day = ExpireDate.GetDate().GetDay();
            tmp.Hour = ExpireDate.GetTime().GetHour();
            tmp.Minute = ExpireDate.GetTime().GetMinute();

            out.Write(reinterpret_cast<const uint32&>(tmp));

        }
        void InstanceEx::Deserialize(Serializer::StreamReader& in)
        {
            //in.Begin(L"InstanceEx");
            in.Read(Instance);

            ExpireDateFormat tmp;
            in.Read(reinterpret_cast<uint32&>(tmp));
            if (reinterpret_cast<uint32&>(tmp) == 0)
                ExpireDate = DateTime::Infinite;
            else
                ExpireDate = DateTime(tmp.Year, tmp.Month, tmp.Day, tmp.Hour, tmp.Minute, 0);
        }
	}
}