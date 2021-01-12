#pragma once
#include <Network/Api/Api.h>
#include <StageServer/Protocol/ToServer.h>
#include <Info/Info.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;
			typedef std::shared_ptr<User> UserSharedPtr;
			class MailboxManager
			{
			public:
				enum WaitingStateType
				{
					Idle = 0
					, WaitDisposeMail
					, WaitDisposeReadMails
					, WaitReadMail
					, WaitRequestMailList
					, WaitRetrieveAttached
					, WaitSendMail
					, WaitSendMailAndItemAttached
					, WaitRollback
				};

				enum RetrieveFlag
				{
					None = 0			// not use.
					, Money = 1
					, Item = 2
				};


			private:
				mutable std::mutex			criticalSection;
				WaitingStateType			waitingState;

				std::map<uint64, XRated::Mail::ContentsInfo>	readBufferContentsInfo;
				XRated::Mail::ContentsInfo						sendBufferContentsInfo;
				std::list< XRated::ItemPosition >				sendBufferAttachedItemPositions;
				XRated::ItemPosition							cashStampPosition;
				uint32											cashStampHash;

				bool											forcedFreeCashMailSend;

				std::list<uint64>								newMails;

				bool IsStageItem(const XRated::ItemBasicInfo& attachedItem);

			public:
				static std::wstring MakeTitle(const std::wstring& message);

			public:

				// Packet handlers (Client-Server)
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Mail::DisposeMail& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Mail::DisposeReadMails& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Mail::ReadMail& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Mail::RequestMailList& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Mail::RetrieveAttached& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Mail::SendMail& packet);

				// Packet handlers (Server-DB)
				void DBDisposeMail(const UserSharedPtr& user, const Net::Answer& answer);
				void DBDisposeReadMails(const UserSharedPtr& user, const Net::Answer& answer);
				void DBReadMail(const UserSharedPtr& user, const Net::Answer& answer);
				void DBRequestMailList(const UserSharedPtr& user, const Net::Answer& answer);
				void DBRetrieveAttached(const UserSharedPtr& user, const Net::Answer& answer);
				void DBSendMail(const UserSharedPtr& user, const Net::Answer& answer);
				void DBRollback(const UserSharedPtr& user, const Net::Answer& answer);

				void SendMailSelf(UserSharedPtr user, const std::wstring& title, const std::wstring& text, const uint32 attachedMoney, const std::vector<XRated::ItemBasicInfo>& sendItems, const std::wstring sender = L"", const XRated::Mail::HeaderInfo::FlagType flag = XRated::Mail::HeaderInfo::FlagType::None); 

				void SendSystemMail(const std::wstring& receiver, const std::wstring& title, const std::wstring& text, const uint32 attachedMoney, const std::vector<XRated::ItemBasicInfo>& sendItems, const std::wstring& sender, const XRated::Mail::HeaderInfo::FlagType flag = XRated::Mail::HeaderInfo::FlagType::None);

				// TODO : Reserved
				//void MailAlarm();
				void NotifyMailDisposition(UserSharedPtr user, const std::wstring& to, const std::wstring& title);
				void DBNotifyMailDispoition(const UserSharedPtr& user, const Net::Answer& answer);

				// Constuctor/Destructor
				MailboxManager();
				~MailboxManager();

				void Init();
				uint16 GetMailCnt() const { return newMails.size(); }
				void MailAlarmed() { if (newMails.empty() == true) newMails.push_back(0); }
			};
		}
	}
}