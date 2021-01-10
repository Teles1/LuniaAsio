#include "ItemSerial.h"
#include <mmsystem.h>
#pragma comment( lib, "Winmm.lib" )
#include <Core/Utils/ConfigReader.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			struct Serial
			{
				unsigned TimeGetTime : 32;
				unsigned Hour : 5;
				unsigned Day : 5;
				unsigned Month : 4;
				unsigned Year : 7;
				unsigned ServerNumber : 10;
				unsigned ForceUnsigned : 1;


				operator uint64 () const { return reinterpret_cast<const uint64&>(*this); }
				Serial& operator =(uint64 rhs) { reinterpret_cast<uint64&>(*this) = rhs; return *this; }
				Serial() { *this = 0; }
				Serial(uint64 rhs) { *this = rhs; }
			};



			ItemSerial::ItemSerial() : m_PreviousSerial(0), m_ServerNumber(0), stSTime(new SYSTEMTIME)
			{
				MakeServerNumber();
			}


			ItemSerial::~ItemSerial()
			{
				delete stSTime;
			}


			uint64 ItemSerial::MakeItemSerial()
			{
				AutoLock lock(m_Mtx);

				GetLocalTime(stSTime);

				Serial serial;
				serial.ServerNumber = ServerNumber();
				serial.Year = stSTime->wYear % 100;
				serial.Month = stSTime->wMonth;
				serial.Day = stSTime->wDay;
				serial.Hour = stSTime->wHour;
				serial.TimeGetTime = timeGetTime();

				if (serial <= m_PreviousSerial)
				{
					serial = ++m_PreviousSerial;
				}

				m_PreviousSerial = serial;

				return serial;
			}


			void ItemSerial::MakeServerNumber()
			{
				std::string serverName = ConfigInstance().Settings.ServerName;
				std::string strNum;
				m_ServerNumber = ConfigInstance().GetKind() == SquareKind ? 500 : 0;

				for (size_t i = 0; i < serverName.size(); ++i)
					if ((serverName.at(i) >= '0') && (serverName.at(i) <= '9'))
						strNum += serverName.at(i);

				if (strNum.size() > 3)
					strNum.erase(0, strNum.size() - 3);

				m_ServerNumber += StringUtil::ToInt(strNum);

				Logger::GetInstance().Info("[ItemSerial::MakeServerNumber()] ServerNumber : {0}", m_ServerNumber);
			}


			int ItemSerial::ServerNumber()
			{
				return m_ServerNumber;
			}

			ItemSerial& ItemSerialInstance()
			{
				return ItemSerial::GetInstance();
			}
		}
	}
}

