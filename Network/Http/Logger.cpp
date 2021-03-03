#include <Network/AspApi/AspApi.h>
#include <Network/Http/Logger.h>
namespace Lunia {
	namespace XRated {
		namespace Http {
			Logger::Logger(const char* ip, uint16 port, const std::string& serverName, Service::TypeFlags serviceType, uint32 bufferSize)
				: serverName(serverName)
			{
				if (ip == NULL || *ip == '\0') return; // disable

				urlBase = StringUtil::Format((char*)"http://%s:%d/", ip, port);
				switch (serviceType)
				{
				case Service::LobbyService:
					serverType = "lobby"; break;
				case Service::SquareService:
					serverType = "square"; break;
				case Service::StageService:
					serverType = "stage"; break;
				default:
					serverType = "unknown"; break;
				}
			}

			void Logger::Write(const char* operation, const char* accountName, const char* characterName, Parameter& parameter, Methods methods)
			{
				Write(operation, accountName, characterName, parameter.ToString(), methods);
			}

			void Logger::Write(const char* operation, const char* accountName, const char* characterName, const std::string& parameter, Methods methods)
			{
				Http::Request request;

				request.Url = urlBase;
				request.Url += operation ? operation : "";
				request.Url += "?,";
				request.Url += serverType;
				request.Url += ",";
				request.Url += serverName;
				request.Url += ",";
				request.Url += operation ? operation : "";
				request.Url += ",";
				request.Url += accountName ? accountName : "";
				request.Url += ",";
				request.Url += characterName ? characterName : "";

				/*
				if (parameter.empty()==false)
				{
					if (parameter.size() > 256)
						request.Content=parameter;
					else
					{
						if (parameter[0]!=',')
							request.Url+=",";
						request.Url+=parameter;
					}
				}
				*/
				switch (methods)
				{
				case Methods::GET:
					if (parameter[0] != ',')
						request.Url += ",";
					request.Url += parameter;
					break;
				case Methods::POST:
					request.Content = parameter;
					break;
				}

				if (urlBase.empty())
				{ // http logging is disalbed, default logging is used.
					//LoggerInstance().Warn("{}", request.Url.c_str());
				}
				else
				{
					
				}
				LoggerInstance().Warn("{}", request.Url.c_str());
			}
		}
	}
}