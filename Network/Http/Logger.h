#pragma once

#include <Core/Utils/StringUtil.h>
#include <Core/Runnable.h>
#include <Core/Service.h>

namespace Lunia {
	namespace XRated {
	namespace Http {

			/**
				@brief		HTTP(Web) based logging client.
							protocol makes
							XRated service uses HTTP(Web) based middleware as external database connector.

				@code

					AllM::XRated::Service::Http::Logger logger("222.122.133.84", 80, "test", XRated::Service::LobbyService);

					logger << "asdad" << L"Adada" << 123132 << 0.1231f;
					logger.Flush("cmd", "accountName", "characterName");

					logger.Write("cmd2", "acountName", "characterName", StringUtil::Format("%d,%s", 123, "asds"));

					AllM::XRated::Service::Http::Logger::Parameter param;
					param << L"_" << 123 << "asda";
					logger.Write("cmd3", "accountName2", "account3", param);

				@endocde

			*/
			class Logger
			{
			public:
				class Parameter
				{
				public:
					Parameter() {}
					Parameter(const char* msg) : param(msg) {}
					template <typename T> inline void Append(const T& str) { Append(StringUtil::ToASCII(str)); }
					template <> inline void Append(const std::string& str) { param += ","; param += str; }
					template <typename T> inline Parameter& operator<< (const T& param) { Append(param); return *this; }
					//inline Parameter& operator<< (const XRated::InstanceEx& param) { Append(param.Instance); Append(param.ExpireDate.ToString()); return *this; }


					const std::string& ToString() const { return param; }
					void Clear() { param.clear(); }

				private:
					std::string param;
				};
				enum Methods { GET = 0, POST, };

				Logger(const char* ip, uint16 port, const std::string& serverName, Service::TypeFlags serviceType, uint32 bufferSize = 8);
				void Write(const char* operation, const char* accountName, const char* characterName, Parameter& parameter, Methods methods = GET);
				void Write(const char* operation, const char* accountName, const char* characterName, const std::string& parameter, Methods methods = GET);

				template <typename T> Logger& operator <<(const T& param) { ThreadContext<Parameter>::Instance() << param; return *this; }
				void Flush(const char* operation, const char* accountName, const char* characterName)
				{
					Write(operation, accountName, characterName, ThreadContext<Parameter>::Instance()); ThreadContext<Parameter>::Instance().Clear();
				}

			private:
				std::string urlBase;
				std::string serverName;
				std::string serverType;

			};

		}
	}
}
