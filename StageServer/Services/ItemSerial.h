#pragma once
#include <mutex>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class ItemSerial
			{
			public:
				ItemSerial();
				~ItemSerial();

				uint64 MakeItemSerial();
				int GetServerNumber() const { return m_ServerNumber; }
				static inline ItemSerial& GetInstance() {
					static ItemSerial m_Instance;
					return m_Instance;
				}
			private:
				void MakeServerNumber();
				int ServerNumber();

				PSYSTEMTIME stSTime;

				int m_ServerNumber;
				uint64 m_PreviousSerial;

				std::mutex m_Mtx;
				
			};
			ItemSerial& ItemSerialInstance();
		}
	}
}