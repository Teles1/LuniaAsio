#pragma once
#include <Core/Utils/ThreadPool.h>
#include <Network/Http/UrlEncoder.h>
#pragma warning(disable : 4996)
#include <cpr/cpr.h>
#include <Network/TextPacket/TextPacket.h>
#include <set>
namespace Lunia {
	namespace Net {
		namespace Http {
			struct Request
			{
				enum Methods {
					GET,
					POST
				};
				std::string Url;
				cpr::Header Header;
				std::string Content;
				void* PassToResponse; // reserved
				Methods Method;
			};

			struct Response
			{
				std::string Url;
				std::string Status;
				std::string Header;
				std::string Content;
				void* PassedFromRequest; // reserved
			};
			/**
			@brief		dynamic binding database connector via HTTP.
			@code

			class Listener
			{
			public:
				void Process(User* sender, int16 errorNumber, TextPacket* packte)
				{
					...
				}
			};


			class Host
			{
			public:
				Host : db("url", "requesterId") {}

				void Update(const T& sender)
				{
					TextPacket packet("auth");
					packet<<"param";
					db.Request(sender, packet, l, Listener::Process);
				}

			pirvate:
				DynamicDBConnector db;
				Listener l;
			};

			@emndcode

			@remarks you should use external synchronize guard that controlls the channel(T) and use carefully to avoid dead-lock problem. response event will be araised after locking.
			*/
			template <typename T> // should implement 'unsigned int GetUniqueSerial() const'
			class DynamicDBConnector
			{
			public:
				DynamicDBConnector()
				{
				}
				DynamicDBConnector(const std::string& baseUrl, const std::string& serviceName)
					: baseUrl(baseUrl), serviceName(serviceName)
				{
					if (this->baseUrl[this->baseUrl.size() - 1] != '/')
						this->baseUrl += '/';
				}

				void Initialize(const std::string& baseUrl, const std::string& serviceName)
				{
					this->baseUrl = baseUrl;
					if (this->baseUrl[this->baseUrl.size() - 1] != '/')
						this->baseUrl += '/';

					this->serviceName = serviceName;
				}

				template <typename ListenerType>
				void Request(const T& sender, const std::string& toGetAreaPacketString, const TextPacket& toPostAreaPacket, ListenerType& listener, void (ListenerType::* fp)(const T&, int16, TextPacket*, unsigned int), bool cancelByDuplication = false, bool disableWrongSenderEvent = true)
				{
					Caller<ListenerType>* caller = new Caller<ListenerType>(sender, &listener, fp, disableWrongSenderEvent);
					Request(toGetAreaPacketString, toPostAreaPacket, caller, cancelByDuplication);
				}

				template <typename ListenerType>
				void Request(const T& sender, const TextPacket& packet, ListenerType& listener, void (ListenerType::* fp)(const T&, int16, TextPacket*, unsigned int), bool cancelByDuplication = false, bool disableWrongSenderEvent = true)
				{
					Caller<ListenerType>* caller = new Caller<ListenerType>(sender, &listener, fp, disableWrongSenderEvent);
					Request(packet, caller, cancelByDuplication);
				}

				void Request(const TextPacket& packet)
				{
					Request(packet, NULL);
				}

				// cancel request has possiblity of 00000000() call

			private:
				class ICaller
				{
				public:
					virtual ~ICaller() {}
					virtual void Call(int16 errorCode, TextPacket* param) = 0;
					virtual unsigned int GetSenderSerial() const = 0;
				};

				template <typename U>
				class Caller : public ICaller
				{
				public:
					Caller(const T& sender, U* listener, void (U::* fp)(const T&, int16, TextPacket*, unsigned int), bool disableWrongSenderEvent)
						: sender(sender), listener(listener), fp(fp), uniqueSerial(sender ? sender->GetUniqueSerial() : 0) // class T must implement GetUniqueSerial()
						,
						disableWrongSenderEvent(disableWrongSenderEvent)
					{
					}
					void Call(int16 errorCode, TextPacket* param)
					{
						if (fp == NULL || listener == NULL)
							throw;
						if (disableWrongSenderEvent == false || sender == NULL || (sender && sender->GetUniqueSerial() == uniqueSerial))
							(listener->*fp)(sender, errorCode, param, uniqueSerial);
					}
					unsigned int GetSenderSerial() const { return uniqueSerial; }

				private:
					const T sender;
					U* listener;
					unsigned int uniqueSerial;
					bool disableWrongSenderEvent;
					void (U::* fp)(const T&, int16, TextPacket*, unsigned int);
				};

				void Request(const std::string& toGetAreaPacketString, const TextPacket& toPostAreaPacket, ICaller* caller, bool cancelByDuplication = false)
				{
					if (baseUrl.empty())
					{
						LoggerInstance().Info("DynamicDBConnector is not iniitalized; [{}] operation is cancled", toPostAreaPacket.GetOperation().c_str());
						return; // not initialized yet;
					}

					if (caller != NULL)
					{
						AutoLock lock(cs);
						if (cancelByDuplication && ref.find(caller->GetSenderSerial()) != ref.end())
						{
							LoggerInstance().Warn("http requesting is ignored by duplication ({})", toPostAreaPacket.GetOperation().c_str());
							delete caller;
							return;
						}
						ref.insert(caller->GetSenderSerial());
					}

					/* increase reference count */
					Http::Request request;
					request.Header.emplace("REQUESTER", serviceName);

					request.Url = baseUrl;
					request.Url += toPostAreaPacket.GetOperation();
					request.Url += ".asp";
					request.PassToResponse = caller; // attach listener
					request.Url += "?";
					request.Url += toGetAreaPacketString;
					request.Content = toPostAreaPacket.ToString();

					request.Method = Request::Methods::GET;

					Open(request);
				}

				void Request(const TextPacket& packet, ICaller* caller, bool cancelByDuplication = false)
				{
					if (baseUrl.empty())
					{
						LoggerInstance().Info("DynamicDBConnector is not iniitalized; [{}] operation is cancled", packet.GetOperation().c_str());
						return; // not initialized yet;
					}

					if (caller != NULL)
					{
						AutoLock lock(cs);
						if (cancelByDuplication && ref.find(caller->GetSenderSerial()) != ref.end())
						{
							LoggerInstance().Warn("http requesting is ignored by duplication ({})", packet.GetOperation().c_str());
							delete caller;
							return;
						}
						ref.insert(caller->GetSenderSerial());
					}

					/* increase reference count */
					Http::Request request;
					request.Header.emplace("REQUESTER", serviceName);

					request.Url = baseUrl;
					request.Url += packet.GetOperation();
					request.Url += ".asp";
					request.PassToResponse = caller; // attach listener

					switch (packet.GetMethod())
					{
					case TextPacket::Methods::GET:
						request.Url += "?";
						request.Url += UrlEncoder::Encode(packet.ToString());
						request.Method = Request::Methods::GET;
						break;
					case TextPacket::Methods::POST:
						request.Content = packet.ToString();
						request.Method = Request::Methods::POST;
						break;
					}

					Open(request);
				}
				
				void Open(Http::Request& request) {
					std::string encodedUrl = request.Url;
					cpr::Url url(encodedUrl);
					cpr::Response r;
					if (request.Method == Request::Methods::GET)
						r = cpr::Get(url, request.Header, timeout);
					else
						r = cpr::Post(url, request.Header, cpr::Body{request.Content}, timeout);

					Utils::thread_pool::GetInstance().push(
					[&](const int& id, cpr::Response response, void* passToResponse) {
						// Sending response.
						ICaller* event = reinterpret_cast<ICaller*>(passToResponse);
						if (event == NULL)
							return; // nothing to be reported

						std::string operation(ExtractOperation(response.url));
						TextPacket packet(operation, TextPacket::Methods::POST);

						bool eventWork = false;

						{
							AutoLock lock(cs);
							std::multiset<unsigned int>::iterator i = ref.find(event->GetSenderSerial());
							if (i != ref.end())
							{
								eventWork = true;
								ref.erase(i);
							}
						}

						// TODO: make sure 'event' is valid while working. 
						if (eventWork == true)
						{
							uint8 errorNumber;
							std::vector<std::string> parameters;

							if (response.status_code != 200)
							{
								LoggerInstance().Error("Http response : {}\n{}\n{}", response.url.c_str(), response.status_code, response.text.c_str());
							}

							// error porcessing
							const int errorCode = static_cast<int>(response.status_code / 100);

							if (2 == errorCode)
							{
								errorNumber = 0; // 0 means 'ok'
								packet.Parse(response.text.c_str());
								event->Call(errorNumber, &packet);
							}
							else if (6 <= errorCode)
							{
								// XRated protocol level error (db level)
								errorNumber = static_cast<uint8>(response.status_code - 600); // 600 + error code
								event->Call(errorNumber, &packet);
							}
							else
							{
								// iis level error
								// TODO : error handling on iis error/
								errorNumber = 0xff; // NoResponse
								event->Call(errorNumber, &packet);
							}
						}
						delete event;

					}, r, request.PassToResponse);
				}

				std::mutex cs;

				std::string serviceName;
				std::string baseUrl;
				uint16 timeout = 5000;

				std::multiset<unsigned int> ref;
			};

		}
	}
}