#include "fwPacketListener.h"
#include "../Core/Utils/InitFunction.h"
#include <Core/ErrorDefinition.h>
#include <StageServer/User/UserRegistry.h>
#include <Network/Api/Api.h>
#include <StageServer/User/UserManager.h>

namespace Lunia {
	namespace StageServer {
		void InitHandlers() {
			fwPacketListener::GetInstance().Connect(
				[](StageServer::UserSharedPtr& user, StageServer::Protocol::Stage& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@Stage", user->GetId());
					if (packet.Version != Constants::Version) {
						Protocol::Error sendPacket;
						sendPacket.errorcode = XRated::Errors::InvalidClientVersion;
						user->Send(sendPacket);
						user->Terminate();
						UserRegistry::GetInstance().RemoveUser(user);
						return;
					}

					user->m_SecuKey = packet.secukey;
					user->m_UsingLocale = packet.Locale.c_str();

					{
						UserRegistry::GetInstance().AuthenticateUser(user);
						Logger::GetInstance().Info("A connection with (Id: {0} - Ip: {1}) is authorizing", user->GetId(), user->GetPeerAddress());
						Net::Api api("AuthConnection");
						api << user->m_SecuKey;
						api.GetAsync(
							[&](const Net::Answer& result) {
								if (result.errorCode == 0) {
									if (!result.resultObject.is_null()) {
										user->SetCharacterName(StringUtil::ToUnicode(result.resultObject["characterName"].get<std::string>()));
										AutoLock _l(user->mtx);
										user->m_RoomIndex = result.resultObject["roomNumber"].get<int>();
										user->m_CurrentStage.StageGroupHash = result.resultObject["stageGroupHash"].get<uint32>();
										user->m_CurrentStage.Level = result.resultObject["accessLevel"].get<uint16>();
										user->m_CurrentStage.Difficulty = result.resultObject["difficulty"].get<uint8>();
										user->m_RoomPass = result.resultObject["password"].get<std::string>();
										user->m_RoomFullCount = result.resultObject["capacity"].get<uint32>();
										user->m_TeamNumber = result.resultObject["teamNumber"].get<int>();
										user->m_StageStateFlags = result.resultObject["stageStates"].get<int>();
										user->m_LastStage.StageGroupHash = result.resultObject["lastPlayedStageGroupHash"].get<uint32>();
										user->m_LastStage.Level = result.resultObject["lastPlayedStageLevel"].get<uint16>();
										user->m_RoomActivateSerial = result.resultObject["uniqueKey"].get<int64>();
										Logger::GetInstance().Info("Authed {0},{1},{2}", user->m_CurrentStage.StageGroupHash, user->m_CurrentStage.Level, user->m_CurrentStage.Difficulty);

										Net::Api api("Auth");
										api << user->GetCharacterName();
										api.GetAsync(
											[&](const Net::Answer& result) {
												if (result.errorCode == 0) {
													if (!result.resultObject.is_null()) {
														if (UserManager().Auth(user, result.resultObject))
														{

														}
													}
													else
														Logger::GetInstance().Warn(L"Api responded succefuly but the data was empty Auth user = {0}", user->GetId());
												}
												else
													Logger::GetInstance().Warn(L"Could not handle the call api to Auth user = {0}", user->GetId());
											});
									}
									else
										Logger::GetInstance().Warn("Result code is 0 but the data is empty.");
								}
								else {
									user->Terminate();
								}
							});
					}

				});
			fwPacketListener::GetInstance().Connect(
				[](StageServer::UserSharedPtr& user, StageServer::Protocol::Join& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@Join", user->GetId());
					//Check if the user is fully authenticated and loaded if not send a big crashadao.
					/*
					if( !IsLoadedStage(currentStage) )
					{
						Net::Protocol::Stage::FromServer::Error error;
						error.errorcode   = Errors::InvalidStageCode;
						DirectSend( error );
						return;
					}
					*/
				});

			fwPacketListener::GetInstance().Connect(
				[](StageServer::UserSharedPtr& user, StageServer::Protocol::ListItem& packet)
				{
					//this does literally nothing but i'm handling it so it doesnt bother me.
					//Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@ListItem", user->GetId());
				});
			fwPacketListener::GetInstance().Connect(
				[](StageServer::UserSharedPtr& user, StageServer::Protocol::ListQuickSlot& packet)
				{
					//this does literally nothing but i'm handling it so it doesnt bother me.
					//Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@ListQuickSlot", user->GetId());
				});
		}
	}
}
