#include "fwPacketListener.h"
#include "../Core/Utils/InitFunction.h"
#include <Core/ErrorDefinition.h>
#include <StageServer/User/UserManager.h>
#include <StageServer/Protocol/FromServer.h>
#include <StageServer/Protocol/ToServer.h>
#include <Network/Api/Api.h>
#include <timeapi.h>
#pragma comment(lib, "winmm.lib")

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			void InitHandlers() {
				fwPacketListener::GetInstance().Connect(
					[](UserSharedPtr user, StageServer::Protocol::ToServer::Stage& packet)
					{
						Logger::GetInstance().Info("fwPacketListener::protocol@Stage");
						if (packet.Version != Lunia::Constants::Version) {
							Protocol::FromServer::Error sendPacket;
							sendPacket.errorcode = XRated::Errors::InvalidClientVersion;
							user->Send(sendPacket);
							user->Terminate();
							UserManagerInstance().RemoveUser(user->GetSerial());
							return;
						}

						user->m_SecuKey = packet.secukey;
						user->m_UsingLocale = packet.Locale.c_str();

						{
							Logger::GetInstance().Info("A connection with (Ip: {0}) is authorizing", user->GetPeerAddress());
							Net::Api api("AuthConnection");
							api << user->m_SecuKey;
							api.GetAsync(&UserManagerInstance(),&UserManager::AuthedConnection, user);
						}

					});
				fwPacketListener::GetInstance().Connect(
					[](UserSharedPtr user, StageServer::Protocol::ToServer::Join& packet)
					{
						Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@Join", user->GetSerial());
						user->Dispatch(packet);
					});
				fwPacketListener::GetInstance().Connect(
					[](UserSharedPtr user, StageServer::Protocol::ToServer::Alive& packet)
					{
						Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@Alive", user->GetSerial());
						user->m_AliveTime = timeGetTime();
						user->Send(packet);
					});
				fwPacketListener::GetInstance().Connect(
					[](UserSharedPtr user, StageServer::Protocol::ToServer::LoadEnd& packet)
					{
						Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@LoadEnd", user->GetSerial());
						user->Dispatch(packet);
					});
				fwPacketListener::GetInstance().Connect(
					[](UserSharedPtr user, StageServer::Protocol::ToServer::Family::RefreshInfo& packet) {
						Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@Family::RefreshInfo", user->GetSerial());
						user->Dispatch(packet);
					}
				);
				fwPacketListener::GetInstance().Connect(
					[](UserSharedPtr user, StageServer::Protocol::ToServer::ListItem& packet)
					{
						//this does literally nothing but i'm handling it so it doesnt bother me.
						Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@ListItem", user->GetId());
					});
				fwPacketListener::GetInstance().Connect(
					[](UserSharedPtr user, StageServer::Protocol::ToServer::ListQuickSlot& packet)
					{
						//this does literally nothing but i'm handling it so it doesnt bother me.
						Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@ListQuickSlot", user->GetId());
					});
			}
		}
	}
}
