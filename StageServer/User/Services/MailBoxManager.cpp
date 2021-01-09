#include "MailBoxManager.h"
#include <StageServer/User/User.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {

			// Packet handlers (Client-Server)
			void MailboxManager::Dispatch(UserSharedPtr user, Protocol::ToServer::Mail::DisposeMail& packet)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());

				// Request Dispose Mail.
				{
					Net::Api request("Mail.Delete");

					request
						<< packet.Index
						<< user->GetSerial();
					request.GetAsync(this, &MailboxManager::DBDisposeMail, user);
					waitingState = WaitDisposeMail;
				}
			}

			void MailboxManager::Dispatch(UserSharedPtr user, Protocol::ToServer::Mail::DisposeReadMails& packet)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());

				// Request Dispose Read Mails.
				{
					Net::Api request("Mail.DeleteAll");

					request
						<< user->GetSerial();
					request.GetAsync(this, &MailboxManager::DBDisposeReadMails, user);
					waitingState = WaitDisposeReadMails;
				}
			}

			void MailboxManager::Dispatch(UserSharedPtr user, Protocol::ToServer::Mail::ReadMail& packet)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (waitingState != Idle) {
					Protocol::FromServer::Mail::ReadMail		response;
					// not enough empty slots.
					response.Result = Protocol::FromServer::Mail::ReadMail::Results::Busy;
					user->Send(response);
					return;
				}

				//AutoLock lock( user->GetSyncObject() );

				// Request Read Mail.
				{
					Net::Api request("Mail.Read");

					request
						<< packet.Index
						<< user->GetSerial();
					request.GetAsync(this, &MailboxManager::DBReadMail, user);
					waitingState = WaitReadMail;
				}
			}

			void MailboxManager::Dispatch(UserSharedPtr user, Protocol::ToServer::Mail::RequestMailList& /*packet*/)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());

				// Request MailList.
				{
					Net::Api request("Mail.List");

					request
						<< user->GetSerial();
					request.GetAsync(this, &MailboxManager::DBRequestMailList, user);
					waitingState = WaitRequestMailList;
				}
			}

			void MailboxManager::Dispatch(UserSharedPtr user, Protocol::ToServer::Mail::RetrieveAttached& packet)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (waitingState != Idle) {
					Protocol::FromServer::Mail::RetrieveAttached		response;
					// not enough empty slots.
					response.Result = Protocol::FromServer::Mail::RetrieveAttached::Results::Busy;
					user->Send(response);
					return;
				}

				AutoLock lock(user->GetSyncObject());

				std::map<uint64, XRated::Mail::ContentsInfo>::iterator i = readBufferContentsInfo.find(packet.Index);
				if (i == readBufferContentsInfo.end()) {
					//maybe packet hacking.
					Protocol::FromServer::Mail::RetrieveAttached		response;
					response.Result = Protocol::FromServer::Mail::RetrieveAttached::Results::NotExistMail;
					user->Send(response);

					// TODO : Block user
					return;
				}

				XRated::Mail::ContentsInfo& contentsInfo = i->second;
				// Check inventory, money upperbound.

				// Inventory must have enough empty slots to Retrieve attached items.
				if (contentsInfo.AttachedItems.size() > user->GetEmptyItemSlotCount())
				{
					Protocol::FromServer::Mail::RetrieveAttached response;
					// not enough empty slots.
					response.Result = Protocol::FromServer::Mail::RetrieveAttached::Results::NotEnoughInventory;
					user->Send(response);
					return;
				}

				{
					std::map< uint8 /*position*/, XRated::ItemBasicInfo /*itemInfo*/ >::const_iterator itemIt = contentsInfo.AttachedItems.begin();
					std::map< uint8 /*position*/, XRated::ItemBasicInfo /*itemInfo*/ >::const_iterator itemEndIt = contentsInfo.AttachedItems.end();
					for (; itemIt != itemEndIt; ++itemIt)
					{
						if (IsStageItem(itemIt->second))
						{
							Protocol::FromServer::Mail::RetrieveAttached response;
							response.Result = Protocol::FromServer::Mail::RetrieveAttached::Results::RetrieveStageItem;
							user->Send(response);
							return;
						}
						if (Database::DatabaseInstance().InfoCollections.Pets.IsPetItem(itemIt->second.ItemHash))
						{
							AutoLock cs(user->GetPetDatas().GetSyncObject());
							const XRated::Pet* pet = user->GetPetDatas().GetPetData(itemIt->second.ItemSerial);
							if (!pet)
							{
								Protocol::FromServer::Mail::RetrieveAttached response;
								response.Result = Protocol::FromServer::Mail::RetrieveAttached::Results::PetInfoNotLoaded;
								user->Send(response);
								return;
							}
						}
					}
				}

				// User must have 'money capacity' at least attached money size.
				// Money Capacity Upperbound = XRated::Constants::GoldMaxLimit
				if ((user->GetMoney() + contentsInfo.AttachedMoney) > XRated::Constants::GoldMaxLimit)
				{
					Protocol::FromServer::Mail::RetrieveAttached		response;
					// Money upperbound overflow.
					response.Result = Protocol::FromServer::Mail::RetrieveAttached::Results::OverMaxGold;
					user->Send(response);
					return;
				}

				int retrieveFlag = RetrieveFlag::None;
				if (contentsInfo.AttachedMoney > 0)
					retrieveFlag = RetrieveFlag::Money;
				if (contentsInfo.AttachedItems.empty() != true)
					retrieveFlag = (retrieveFlag == RetrieveFlag::None) ? RetrieveFlag::Item : retrieveFlag | RetrieveFlag::Item;


				// Request DB Retrieve Attached Items.
				{
					Net::Api request("Mail.PopItems");

					request
						<< packet.Index
						<< user->GetSerial()
						<< retrieveFlag;
					request.GetAsync(this, &MailboxManager::DBRetrieveAttached, user);
					waitingState = WaitRetrieveAttached;

					// lock inventory
					user->LockItems(true);
				}
			}

			void MailboxManager::Dispatch(UserSharedPtr user, Protocol::ToServer::Mail::SendMail& packet)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (waitingState != Idle) {
					Protocol::FromServer::Mail::SendMail	response;
					response.Result = Protocol::FromServer::Mail::SendMail::Results::Busy;
					user->Send(response);
					return;
				}

				AutoLock lock(user->GetSyncObject());

				cashStampPosition = packet.StampItemPosition;
				cashStampHash = 0;
				if (cashStampPosition != XRated::ItemPosition::Invalid) {
					const Common::ItemEx* itemSlot = user->GetItem(cashStampPosition);
					if (itemSlot == NULL) {
						Protocol::FromServer::Mail::SendMail	response;
						response.Result = Protocol::FromServer::Mail::SendMail::Results::InvalidStampInfo;
						user->Send(response);
						return;
					}
					else {
						if (Database::DatabaseInstance().InfoCollections.CashStamps.Get(itemSlot->Info->Hash) != NULL) {
							cashStampHash = itemSlot->Info->Hash;
						}
						else {
							Protocol::FromServer::Mail::SendMail	response;
							response.Result = Protocol::FromServer::Mail::SendMail::Results::InvalidStampInfo;
							user->Send(response);
							return;
						}
					}
				}
				if (user->IsItemLocked() == true) {
					LoggerInstance().Error("[SendMail] Inven is locked. ({})", user->GetSerial());
					Protocol::FromServer::Mail::SendMail	response;
					response.Result = Protocol::FromServer::Mail::SendMail::Results::InvenIsLocked;
					user->Send(response);
					return;
				}
				String tmpUser(user->GetName()), tmpSender(packet.Receiver);
				StringUtil::ToLowerCase(tmpUser);
				StringUtil::ToLowerCase(tmpSender);
				StringUtil::Trim(tmpSender);
				if (tmpUser == tmpSender) {
					LoggerInstance().Exception("[SendMail] Cannot send to myself. ({})", user->GetSerial());
					Protocol::FromServer::Mail::SendMail	response;
					response.Result = Protocol::FromServer::Mail::SendMail::Results::Error;
					user->Send(response);
					return;
				}


				// Generate Mail Contents Info.
				XRated::Mail::ContentsInfo		contents;

				contents.Message = packet.Message;
				contents.AttachedMoney = packet.AttachedMoney;
				contents.Receiver = packet.Receiver;
				contents.StampItemHash = cashStampHash;

				// lock invontory
				user->LockItems(true);

				// attach items.
				sendBufferAttachedItemPositions.clear();

				if (packet.AttachedItems.empty() != true)
				{
					if (packet.AttachedItems.size() > 5) {
						Protocol::FromServer::Mail::SendMail	response;
						response.Result = Protocol::FromServer::Mail::SendMail::Results::ExceedItemLimit;
						user->Send(response);
						user->LockItems(false);
						return;
					}

					int attachedItemCount = 0;

					for (std::map< uint8, XRated::ItemPosition >::iterator iter = packet.AttachedItems.begin();
						iter != packet.AttachedItems.end();
						++iter)
					{
						attachedItemCount += 1;
						// check attached item limit
						if (contents.StampItemHash == 0)
						{
							if (attachedItemCount > 1)
							{
								// TODO : exception - Non stamped mail overflow attached item count upperbound violation detected.
								LoggerInstance().Exception("[SendMail] MailboxManager::Dispatch() - Non stamped mail overflow attached item count upperbound violation detected.");

								Protocol::FromServer::Mail::SendMail	response;
								response.Result = Protocol::FromServer::Mail::SendMail::Results::ExceedItemLimit;
								user->Send(response);
								user->LockItems(false);
								return;
							}
						}

						const Common::ItemEx* tItem = user->GetItem(iter->second);

						XRated::ItemBasicInfo		basicItemInfo;

						if (tItem && tItem->IsTradable(NULL) == true && tItem->Info
							&& (Database::DatabaseInstance().InfoCollections.Pets.IsPetItem(tItem->Info->Hash) == false))
						{
							basicItemInfo.ItemHash = tItem->Info->Hash;
							basicItemInfo.InstanceEx = tItem->InstanceEx;
							basicItemInfo.ItemSerial = tItem->Serial;
							basicItemInfo.StackCount = tItem->StackedCount;

							if (IsStageItem(basicItemInfo))
							{
								LoggerInstance().Exception("[SendMail] Trying to send stage item. ({}/{})", user->GetSerial(), tItem->Info->Hash);
								Protocol::FromServer::Mail::SendMail	response;
								response.Result = Protocol::FromServer::Mail::SendMail::Results::ForbiddenStageItem;
								user->Send(response);
								user->LockItems(false);
								return;
							}

							contents.AttachedItems.insert(std::pair< uint8, XRated::ItemBasicInfo >(iter->first, basicItemInfo));

							if (std::find(sendBufferAttachedItemPositions.begin(), sendBufferAttachedItemPositions.end(), iter->second) == sendBufferAttachedItemPositions.end()) {
								sendBufferAttachedItemPositions.push_back(iter->second);
							}
							else {
								// TODO : Block character
								LoggerInstance().Exception("[SendMail] Trying to send same item twice. ({}/{})", user->GetSerial(), tItem->Info->Hash);
								Protocol::FromServer::Mail::SendMail	response;
								response.Result = Protocol::FromServer::Mail::SendMail::Results::Error;
								user->Send(response);
								user->LockItems(false);
								return;
							}
						}
						else
						{
							// TODO : exception - Tradable item can not be attached to mail.
							if (tItem && tItem->Info)
								LoggerInstance().Exception("[SendMail] Untradable item. ({}/{})", user->GetSerial(), tItem->Info->Hash);
							else
								LoggerInstance().Exception("[SendMail] Untradable item. ({}/{},{})", user->GetSerial(), (int)iter->second.Bag, (int)iter->second.Position);
							Protocol::FromServer::Mail::SendMail	response;
							response.Result = Protocol::FromServer::Mail::SendMail::Results::Error;
							user->Send(response);
							user->LockItems(false);
							return;
						}
					}

				}

				// Check postal charge. If not enough, cancel request, if not, balancing postal charge.
				if (contents.StampItemHash == 0)
				{
					if (user->GetMoney() < Mail::CalculatePostalCharge(contents.AttachedMoney, contents.AttachedItems.size()) + contents.AttachedMoney)
					{
						// Send result - not enough postal charge.
						Protocol::FromServer::Mail::SendMail		response;
						response.Result =
							Protocol::FromServer::Mail::SendMail::Results::NotEnoughFee;

						user->Send(response);
						user->LockItems(false);
						return;
					}
					if (contents.AttachedMoney > XRated::Mail::Limit::GoldWithoutStamp) {
						LoggerInstance().Exception("[SendMail] Gold limit exceeded. ({}/{})", XRated::Mail::Limit::GoldWithoutStamp, contents.AttachedMoney);
						Protocol::FromServer::Mail::SendMail		response;
						response.Result =
							Protocol::FromServer::Mail::SendMail::Results::Error;

						user->Send(response);
						user->LockItems(false);
						return;
					}
				}
				else {
					if (contents.AttachedMoney > user->GetMoney()) {
						LoggerInstance().Exception("[SendMail] Not enough money. ({}/{})", user->GetMoney(), contents.AttachedMoney);
						Protocol::FromServer::Mail::SendMail		response;
						response.Result =
							Protocol::FromServer::Mail::SendMail::Results::Error;
						user->Send(response);
						user->LockItems(false);
						return;
					}
				}

				// Generate Mail Header Info.
				XRated::Mail::HeaderInfo		header;
				header.Title = MakeTitle(contents.Message); // ( contents.Message.size() >= 20 ) ? contents.Message.substr( 0, 20 ) : contents.Message;

				header.Flag = XRated::Mail::HeaderInfo::FlagType::None;
				if (contents.StampItemHash != 0)
					header.Flag |= XRated::Mail::HeaderInfo::FlagType::Cash;

				header.Sender = user->GetName();


				// Buffer.
				sendBufferContentsInfo = contents;

				// Request SendMail.
				{
					Net::Api request("Mail.Send", Net::Api::Methods::POST);
					json j = {
						{"receiver",contents.Receiver},
						{"sender",header.Sender},
						{"title",header.Title},
						{"message",contents.Message},
						{"attachedMoney",contents.AttachedMoney},
						{"stampItemHash",contents.StampItemHash},
						{"flag",header.Flag}
					};
					// Attached items.
					std::vector<json> itemList;
					for (auto& itr : contents.AttachedItems)
					{
						itemList.push_back({
							{"itemHash", itr.second.ItemHash},
							{"stackCount",itr.second.StackCount},
							{"itemSerial",itr.second.ItemSerial},
							{"instanceEx",itr.second.InstanceEx}
						});
					}
					j["items"] = itemList;
					request.GetAsync(this, &MailboxManager::DBSendMail, user, j);
					waitingState = WaitSendMail;
				}
			}


			// Packet handlers (Server-DB)
			void MailboxManager::DBDisposeMail(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				waitingState = Idle;

				// Verify packet.
				if (!answer)
				{
					LoggerInstance().Warn("[DisposeMail] MailboxManager::DBDisposeMail() - Invalid Packet received.");
					return;
				}

				// Verify user.

				// Send result.
				Protocol::FromServer::Mail::DisposeMail		response;


				// Checking Error code.
				if (!answer)
				{
					// TODO : error log
					//ALLM_IMPORTANT(( L"[DisposeMail] MailboxManager::DBDisposeMail() - Error [{}]", errorNumber ));			
					response.Result =
						static_cast<Protocol::FromServer::Mail::DisposeMail::Results>(answer.errorCode);
				}
				else
				{
					response.Result = Protocol::FromServer::Mail::DisposeMail::Results::Ok;
				}

				if (user->GetSerial() != answer.at("senderUniqueSerial").get<uint64>())
				{
					LoggerInstance().Warn("[DisposeMail] MailboxManager::DBDisposeMail() - User unique serials are mismatched.");
					return;
				}
				switch (response.Result)
				{
				case Protocol::FromServer::Mail::DisposeMail::Results::Ok:
					answer.get_to("index", response.Index);

					/*Logger().Write(IAdmLogger::ImportantLogger, "mail-delete", user->GetName(), Http::Logger::Parameter()
						<< response.Index);*/
					break;
				case Protocol::FromServer::Mail::DisposeMail::Results::NotExistMail:
				case Protocol::FromServer::Mail::DisposeMail::Results::InvalidUser:
				default:
					break;
				}

				user->Send(response);
			}

			void MailboxManager::DBDisposeReadMails(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				waitingState = Idle;

				// Verify packet.
				/*if (!answer)
				{
					LoggerInstance().Warn("[DBDisposeReadMails] MailboxManager::DBDisposeReadMails() - Invalid Packet received.");
					return;
				}*/

				// Verify user.
				if (user->GetSerial() != answer.at("senderUniqueSerial").get<uint64>())
				{
					LoggerInstance().Warn("[DBDisposeReadMails] MailboxManager::DBDisposeReadMails() - User unique serials are mismatched.");
					return;
				}

				// Send result.
				Protocol::FromServer::Mail::DisposeReadMails		response;


				// Checking Error code.
				if (!answer)
				{
					// TODO : error log
					//ALLM_IMPORTANT(( L"[DisposeMail] MailboxManager::DBDisposeMail() - Error [{}]", errorNumber ));			
					response.Result = static_cast<Protocol::FromServer::Mail::DisposeReadMails::Results>(answer.errorCode);
				}
				else
				{
					response.Result = Protocol::FromServer::Mail::DisposeReadMails::Results::Ok;
				}

				switch (response.Result)
				{
				case Protocol::FromServer::Mail::DisposeReadMails::Results::Ok:
				{
					/*uint16 count = packet->GetCount();
					Http::Logger::Parameter logParam;

					for (uint16 i = 0; i < count; ++i)
					{
						response.Indexes.push_back(StringUtil::ToInt64((*packet)[i]));
						logParam << StringUtil::ToInt64((*packet)[i]);
					}*/

					//Logger().Write( IAdmLogger::ImportantLogger, "mail-deleteAll", user->GetName(), logParam );
				}
				break;
				case Protocol::FromServer::Mail::DisposeReadMails::Results::NotExistMail:
				case Protocol::FromServer::Mail::DisposeReadMails::Results::InvalidUser:
				default:
					break;
				}

				user->Send(response);
			}

			void MailboxManager::DBReadMail(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				waitingState = Idle;

				// Verify packet.
				/*if (!answer)
				{
					ALLM_IMPORTANT((L"[ReadMail] MailboxManager::DBReadMail() - Invalid Packet received."));
					return;
				}*/

				// Verify user.
				if (user->GetSerial() != answer.at("senderUniqueSerial").get<uint64>())
				{
					LoggerInstance().Warn(L"[ReadMail] MailboxManager::DBReadMail() - User unique serials are mismatched.");
					// TODO : if errorNumber == 0 send return mail to sender
					return;
				}

				// Send result
				Protocol::FromServer::Mail::ReadMail	response;


				// Checking Error code.
				if (!answer)
				{
					// TODO : error log
					//ALLM_IMPORTANT(( L"[ReadMail] MailboxManager::DBReadMail() - Error [{}]", errorNumber ));
					response.Result =
						static_cast<Protocol::FromServer::Mail::ReadMail::Results>(answer.errorCode);
				}
				else
				{
					response.Result = Protocol::FromServer::Mail::ReadMail::Results::Ok;
				}

				// Receiver check
				//if ( StringUtil::ToUnicode( (*packet)[1] ) != user->GetName() )
				//{
				//	response.Result = Protocol::FromServer::Mail::ReadMail::Results::InvalidUser;
				//}


				if (response.Result == Protocol::FromServer::Mail::ReadMail::Results::Ok)
				{
					std::wstring Sender{};
					bool isRead = false;
					answer.get_to("index", response.MailContents.Index);
					answer.get_to("receiver", response.MailContents.Receiver);
					answer.get_to("sender", Sender);
					answer.get_to("message", response.MailContents.Message);
					answer.get_to("attachedMoney", response.MailContents.AttachedMoney);
					answer.get_to("stampItemHash", response.MailContents.StampItemHash);
					answer.get_to("stampItemHash", response.MailContents.StampItemHash);
					answer.get_to("isRead", isRead);

					// Parse subpacket.
					uint16 iter = 7;
					int slotIndex = 0;
					bool expiredItemCheck = true;

					for(auto& item : answer.at("items"))
					{
						uint32 itemHash = item.at("hash").get<uint32>();
						InstanceEx instance = item.at("instance").get<InstanceEx>();
						if (expiredItemCheck == true)
						{

							XRated::Database::Info::ItemInfo* itemInfo = XRated::Database::DatabaseInstance().InfoCollections.Items.Retrieve(itemHash);

							if (itemInfo != NULL &&
								((itemInfo->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::EXISTWHENEXPIRED) ||
									Database::Enchant::IsExpired(itemInfo, instance) == false))
							{
								expiredItemCheck = false;
							}
						}

						response.MailContents.AttachedItems.insert(

							std::pair< uint8, XRated::ItemBasicInfo >(
								slotIndex++
								, XRated::ItemBasicInfo(
									itemHash
									, instance
									, item.at("serial").get<int64>()
									, item.at("stackedCount").get<uint16>()
								))
						);
					}

					// buffer.
					readBufferContentsInfo[response.MailContents.Index] = response.MailContents;

					if (expiredItemCheck == true)
					{
						do {

							XRated::Mail::ContentsInfo& contentsInfo = response.MailContents;
							if (contentsInfo.AttachedItems.empty() == true) {
								break;
							}

							AutoLock lock(user->GetSyncObject());
							// Check inventory, money upperbound.

							// Inventory must have enough empty slots to Retrieve attached items.
							if (contentsInfo.AttachedItems.size() > user->GetEmptyItemSlotCount()) {
								break;;
							}

							int retrieveFlag = RetrieveFlag::Item;


							// Request DB Retrieve Attached Items.
							{
								Net::Api request("Mail.PopItems");

								request
									<< response.MailContents.Index
									<< user->GetSerial()
									<< retrieveFlag;
								request.GetAsync(this, &MailboxManager::DBRetrieveAttached, user);
								waitingState = WaitRetrieveAttached;

								// lock inventory
								user->LockItems(true);
							}
						} while (false);
					}

					// if current read mail has stamp item, send Response mail(Notify Mail Disposition)
					if (response.MailContents.StampItemHash != 0 && Sender != L"" && isRead == 0)
					{
						NotifyMailDisposition(user, Sender, MakeTitle(response.MailContents.Message));
					}

					std::list<uint64>::iterator i = std::find(newMails.begin(), newMails.end(), response.MailContents.Index);
					if (i != newMails.end()) {
						newMails.erase(i);
					}
				}

				user->Send(response);
			}

			void MailboxManager::DBRequestMailList(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				waitingState = Idle;

				// Verify packet.
				/*if (!answer)
				{
					LoggerInstance().Warn(L"[RequestMailList] MailboxManager::DBRequestMailList() - Invalid Packet received.");
					return;
				}*/

				// Verify user.
				if (user->GetSerial() != answer.at("senderUniqueSerial").get<uint64>())
				{
					LoggerInstance().Warn("[RequestMailList] MailboxManager::DBRequestMailList() - User unique serials are mismatched.");
					return;
				}


				// Send result.
				Protocol::FromServer::Mail::RequestMailList		response;

				// Checking Error code.
				if (!answer)
				{
					// TODO : error log
					//ALLM_IMPORTANT(( L"[RequestMailList] MailboxManager::DBRequestMailList() - Error [{}]", errorNumber ));
					response.Result =
						static_cast<Protocol::FromServer::Mail::RequestMailList::Results>(answer.errorCode);
				}
				else
				{
					response.Result = Protocol::FromServer::Mail::RequestMailList::Ok;
				}

				// Parse packet.
				uint16 iter = 0;
				newMails.clear();
				for(auto& mail : answer.at("mails").get<json>())
				{
					XRated::Mail::HeaderInfo	header;
					mail.at("index").get_to(header.Index);
					mail.at("sender").get_to(header.Sender);
					mail.at("title").get_to(header.Title);
					mail.at("isRead").get_to(header.IsRead);
					mail.at("flag").get_to(header.Flag);
					mail.at("sentDate").get_to(header.SentDate);

					response.MailHeaderList.push_back(header);

					if (header.IsRead == false)
					{
						newMails.push_back(header.Index);
					}
				}

				user->Send(response);
			}

			void MailboxManager::DBRetrieveAttached(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				waitingState = Idle;

				// Verify packet.
				/*if (!answer)
				{
					ALLM_IMPORTANT((L"[RetrieveAttached] MailboxManager::DBRetrieveAttached() - Invalid Packet received."));
					return;
				}*/

				// Send result.
				Protocol::FromServer::Mail::RetrieveAttached		response;
				response.Result = static_cast<Protocol::FromServer::Mail::RetrieveAttached::Results>(answer.errorCode);

				std::map<uint64, XRated::Mail::ContentsInfo>::iterator iterContentsInfo;
				uint64 mailIndex = 0;

				AutoLock lock(user->GetSyncObject());
				switch (response.Result) {
				case Protocol::FromServer::Mail::RetrieveAttached::Results::Ok:
					answer.get_to("index", mailIndex);
					iterContentsInfo = readBufferContentsInfo.find(mailIndex);
					if (iterContentsInfo == readBufferContentsInfo.end()) { //Critical Error
						/*Logger().Write(IAdmLogger::ImportantLogger, "mail-rollbackfail", user->IsConnected() ? user->GetName() : L"Unknown", Http::Logger::Parameter()
							<< mailIndex);*/

						if (user->IsConnected() == false || user->GetSerial() != answer.at("senderUniqueSerial").get<uint64>())
							return;

						response.Result = Protocol::FromServer::Mail::RetrieveAttached::Results::NotExistMail;
						break;
					}
					// Verify user.
					if (user->IsConnected() == false || user->GetSerial() != answer.at("senderUniqueSerial").get<uint64>())
					{
						LoggerInstance().Warn("[RetrieveAttached] MailboxManager::DBRetrieveAttached() - User Disconnected. Should be rollback.");
						// TODO : Logging here.

						waitingState = WaitRollback;
						response.Result = Protocol::FromServer::Mail::RetrieveAttached::Results::RollbackNeeded;

						// TODOO : Call rollback
						Net::Api request("Mail.RollbackItems", Net::Api::Methods::POST);
						
						request.GetAsync(this, &MailboxManager::DBRollback, user, answer.resultObject);
						waitingState = WaitRollback;

						readBufferContentsInfo.erase(iterContentsInfo); //verify Index for retrieve attach.
					}
					else {
						answer.get_to("index", response.Index);
						int retrieveFlag = 0;
						answer.get_to("retrieveFlag", retrieveFlag);

						//Add items and money to user.
						uint32 attachedMoney = 0;
						if (retrieveFlag & RetrieveFlag::Money) {
							if (iterContentsInfo->second.AttachedMoney > 0) {
								user->SetMoney(user->GetMoney() + iterContentsInfo->second.AttachedMoney);
								attachedMoney = iterContentsInfo->second.AttachedMoney;
							}
						}


						std::wstring strLoggingItem(L"|");

						// Retrieve attached items.
						if (retrieveFlag & RetrieveFlag::Item) {
							if (iterContentsInfo->second.AttachedItems.empty() != true)
							{
								for (std::map< uint8, XRated::ItemBasicInfo >::iterator iter = iterContentsInfo->second.AttachedItems.begin();
									iter != iterContentsInfo->second.AttachedItems.end();
									++iter)
								{
									Common::ItemEx		itemEx;
									XRated::Database::Info::ItemInfo* itemInfo =
										XRated::Database::DatabaseInstance().InfoCollections.Items.Retrieve(iter->second.ItemHash);
									if (itemInfo == NULL) {
										std::string failedAttachItems;
										while (iter != iterContentsInfo->second.AttachedItems.end()) {
											if (failedAttachItems.empty() == false) {
												failedAttachItems += ",";
											}
											failedAttachItems += StringUtil::To<std::string>(iter->second.ItemHash);
											++iter;
										}
										user->CriticalError(fmt::format("MailAttachItem Failed(cannot found item) : {}", failedAttachItems.c_str()).c_str(), true);
										return;
									}

									itemEx.Info = itemInfo;
									itemEx.InstanceEx = iter->second.InstanceEx;
									itemEx.Serial = iter->second.ItemSerial;
									itemEx.StackedCount = iter->second.StackCount;

									if (itemInfo != NULL &&
										(itemInfo->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::EXISTWHENEXPIRED ||
											Database::Enchant::IsExpired(itemEx.Info, itemEx.InstanceEx) == false))
									{
										XRated::ItemPosition position = user->ItemAdd(itemEx);

										/*Logger().Write(IAdmLogger::ImportantLogger, "mailitem-receive", user->GetName(), Http::Logger::Parameter()
											<< itemEx.Info->Hash
											<< itemEx.Serial
											<< itemEx.StackedCount
											<< itemEx.InstanceEx.Instance
											<< itemEx.InstanceEx.ExpireDate.ToString()
											<< response.Index);*/

										Protocol::FromServer::AcquireItem	packet;
										packet.ItemId = itemEx.Info->Hash;
										packet.Bag = position.Bag;
										packet.Pos = position.Position;
										packet.StackedCount = itemEx.StackedCount;
										packet.instanceEx = itemEx.InstanceEx;
										packet.ItemSerial = itemEx.Serial;

										user->Send(packet);
									}
									else
									{
										response.Result = Protocol::FromServer::Mail::RetrieveAttached::Results::SomeItemExpired;
									}

									strLoggingItem += fmt::format(L"{}:{}:{}:{}:{}|"
										, StringUtil::ToUnicode(itemEx.Info->Hash).c_str()
										, StringUtil::ToUnicode(itemEx.Serial).c_str()
										, StringUtil::ToUnicode(itemEx.StackedCount).c_str()
										, StringUtil::ToUnicode(itemEx.InstanceEx.Instance).c_str()
										, itemEx.InstanceEx.ExpireDate.ToString());
								}

								if (strLoggingItem == L"|")
								{
									response.Result = Protocol::FromServer::Mail::RetrieveAttached::Results::AllItemExpired;
								}
							}
						}

						/*Logger().Write(IAdmLogger::ImportantLogger, "mail-popItems", user->GetName(), Http::Logger::Parameter()
							<< response.Index
							<< attachedMoney
							<< strLoggingItem);*/

						if (retrieveFlag & RetrieveFlag::Money) {
							iterContentsInfo->second.AttachedMoney = 0;
						}
						if (retrieveFlag & RetrieveFlag::Item) {
							iterContentsInfo->second.AttachedItems.clear();
						}
						if ((iterContentsInfo->second.AttachedMoney == 0) && (iterContentsInfo->second.AttachedItems.empty() == true)) {
							readBufferContentsInfo.erase(iterContentsInfo); //verify Index for retrieve attach.
						}
					}
					break;
				}

				user->Send(response);


				// unlock invontory
				user->LockItems(false);
			}

			void MailboxManager::DBSendMail(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				waitingState = Idle;

				// Verify packet.
				//if (!answer)
				//{
				//	ALLM_IMPORTANT((L"[SendMail] MailboxManager::DBSendMail() - Invalid Packet received."));
				//	user->LockItems(false);
				//	return;
				//}

				// Send result.
				Protocol::FromServer::Mail::SendMail		response;

				// Checking Error code.
				if (!answer)
				{
					//ALLM_IMPORTANT(( L"[SendMail] MailboxManager::DBSendMail() - Error [{}]", errorNumber ));
					response.Result =
						static_cast<Protocol::FromServer::Mail::SendMail::Results>(answer.errorCode);
				}
				else
				{
					response.Result = Protocol::FromServer::Mail::SendMail::Results::Ok;
					//static_cast< Protocol::FromServer::Mail::SendMail::Results >( StringUtil::ToInt( (*packet)[0] ) );

					AutoLock lock(user->GetSyncObject());

					// Verify user.
					if (user->IsConnected() == false || user->GetSerial() != answer.at("senderUniqueSerial").get<uint64>())
					{
						LoggerInstance().Warn(L"[SendMail] MailboxManager::DBSendMail() - Disconnected User. Delete send mail.");
						//Logging here

						Net::Api request("Mail.RollbackSend");
						request << answer.at("sender").get<uint64>();
						request << user->GetSerial();
						request.GetAsync(this, &MailboxManager::DBRollback, user);

						response.Result = Protocol::FromServer::Mail::SendMail::Results::RollbackNeeded;

						waitingState = WaitRollback;

						/*Logger().Write(IAdmLogger::ImportantLogger, "mail-rollback", L"Unknown", Http::Logger::Parameter()
							<< (*packet)[1]);*/
					}
					else {
						//Delete cash stamp
						if (cashStampPosition != XRated::ItemPosition::Invalid && user->ItemRemove(cashStampPosition, cashStampHash, 1, false) != 1) {
							//Unable to remove cashstamp from user. call Rollback
							LoggerInstance().Exception("[SendMail] MailboxManager::DBSendMail() - Unable to remove cash stamp from user. Delete send mail");
							Net::Api request("Mail.RollbackSend");
							request << answer.at("sender").get<uint64>();
							request << user->GetSerial();
							request.GetAsync(this, &MailboxManager::DBRollback, user);

							response.Result = Protocol::FromServer::Mail::SendMail::Results::RollbackNeeded;

							waitingState = WaitRollback;

							/*Logger().Write(IAdmLogger::ImportantLogger, "mail-rollback", user->GetName(), Http::Logger::Parameter()
								<< (*packet)[1]);*/

						}
						else {
							std::wstring strLoggingItem(L"|");
							//Remove items and money from inventory.
							std::list< XRated::ItemPosition >::iterator i = sendBufferAttachedItemPositions.begin();
							for (; i != sendBufferAttachedItemPositions.end(); ++i) {
								const Common::ItemEx* itemInfo = user->GetItem(*i);
								if (itemInfo) {
									strLoggingItem += fmt::format(L"{}:{}:{}:{}:{}|"
										, StringUtil::ToUnicode(itemInfo->Info ? itemInfo->Info->Hash : 0).c_str()
										, StringUtil::ToUnicode(itemInfo->Serial).c_str()
										, StringUtil::ToUnicode(itemInfo->StackedCount).c_str()
										, StringUtil::ToUnicode(itemInfo->InstanceEx.Instance).c_str()
										, itemInfo->InstanceEx.ExpireDate.ToString());

									/*Logger().Write(IAdmLogger::ImportantLogger, "mailitem-send", user->GetName(), Http::Logger::Parameter()
										<< (itemInfo->Info ? itemInfo->Info->Hash : 0)
										<< itemInfo->Serial
										<< itemInfo->StackedCount
										<< itemInfo->InstanceEx.Instance
										<< itemInfo->InstanceEx.ExpireDate.ToString()
										<< (*packet)[1]);*/
								}
								user->ItemDrop(*i);

							}
							uint32 postalCharge = cashStampPosition == XRated::ItemPosition::Invalid ?
								Mail::CalculatePostalCharge(sendBufferContentsInfo.AttachedMoney, sendBufferContentsInfo.AttachedItems.size()) : 0;

							if (forcedFreeCashMailSend == true)
							{
								postalCharge = 0;
								LoggerInstance().Info("[SendMail] MailboxManager::DBSendMail() - postal charge nullified.");
							}
							if (user->GetMoney() >= (postalCharge + sendBufferContentsInfo.AttachedMoney)) {
								if (forcedFreeCashMailSend == true)
								{
									forcedFreeCashMailSend = false;
								}
								else
								{
									user->SetMoney(user->GetMoney() - (postalCharge + sendBufferContentsInfo.AttachedMoney));
								}
							}
							else {
								LoggerInstance().Exception("[SendMail] MailboxManager::DBSendMail() - Not enough money. ({}/{}/{}/{})"
									, user->GetSerial()
									, user->GetMoney()
									, postalCharge + sendBufferContentsInfo.AttachedMoney
									, answer.at("id").get<uint64>() // TODO : check if 64bit int is available or not.
									);
								// TODO : Logging here
							}

							/*Logger().Write(IAdmLogger::ImportantLogger, "mail-send", user->GetName(), Http::Logger::Parameter()
								<< (*packet)[1]
								<< sendBufferContentsInfo.Receiver
								<< sendBufferContentsInfo.AttachedMoney
								<< sendBufferContentsInfo.StampItemHash
								<< strLoggingItem);*/
						}
					}
				}

				//switch ( response.Result )
				//{
				//case Protocol::FromServer::Mail::SendMail::Results::Ok:

				//	// TODO : clear buffer sendBufferContentsInfo

				//	break;
				//case Protocol::FromServer::Mail::SendMail::Results::NotExistReceiver:
				//case Protocol::FromServer::Mail::SendMail::Results::MailBoxIsFull:
				//case Protocol::FromServer::Mail::SendMail::Results::NotExistItem:
				//case Protocol::FromServer::Mail::SendMail::Results::NotEnoughFee: // already checked.
				//default:
				//	break;
				//}

				sendBufferAttachedItemPositions.clear();
				cashStampPosition = XRated::ItemPosition::Invalid;

				if (sendBufferContentsInfo.Receiver != user->GetName())
					user->Send(response);

				// unlock invontory
				user->LockItems(false);
			}

			void MailboxManager::DBRollback(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				waitingState = Idle;

				// Verify packet.
				//if (!answer)
				//{
				//	LoggerInstance().Warn("[SendMail] MailboxManager::DBRollback() - Invalid Packet received.");
				//	return;
				//}

				Protocol::FromServer::Mail::RollbackFinished		response;

				if (!answer) {
					LoggerInstance().Error("[SendMail] MailboxManager::DBRollback() - CriticalError. ({})", answer.errorCode);
					response.Result = static_cast<Protocol::FromServer::Mail::RollbackFinished::Results>(answer.errorCode);

					//Logging here
				}
				else {
					response.Result = Protocol::FromServer::Mail::RollbackFinished::Results::Ok;
				}

				if (user->IsConnected() == true && user->GetSerial() == answer.at("senderUniqueSerial").get<uint64>())
					user->Send(response);
			}

			// Send Mail(Self)
			void MailboxManager::SendMailSelf(UserSharedPtr user, const std::wstring& title, const std::wstring& text, const uint32 attachedMoney, const std::vector<XRated::ItemBasicInfo>& sendItems, const std::wstring sender, const XRated::Mail::HeaderInfo::FlagType flag)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());

				if (user->IsItemLocked() == true) {
					LoggerInstance().Error("[SendMail] Inven is locked. ({})", user->GetSerial());
				}

				// Generate Mail Contents Info.
				XRated::Mail::ContentsInfo		contents;

				contents.Message = text;
				contents.AttachedMoney = attachedMoney;
				contents.Receiver = user->GetName();
				contents.StampItemHash = 0; //36359512; // HACK : arcrus. Stamp Item Hash

				for (uint32 i = 0; i < sendItems.size(); ++i)
				{
					contents.AttachedItems[i] = sendItems[i];
				}

				// lock inventory
				user->LockItems(true);

				// Generate Mail Header Info.
				XRated::Mail::HeaderInfo		header;
				//header.Title = MakeTitle(contents.Message);
				header.Title = title; // HACK : title must shorter than size (20)

				header.Flag = flag;
				header.Flag |= XRated::Mail::HeaderInfo::FlagType::Cash;

				if (sender == L"")
					header.Sender = user->GetName();
				else
					header.Sender = sender;

				// Buffer.
				sendBufferContentsInfo = contents;

				// Request SendMail.
				{
					Net::Api request("Mail.Send", Net::Api::Methods::POST);
					json j = {
						{"receiver",contents.Receiver},
						{"sender",header.Sender},
						{"title",header.Title},
						{"message",contents.Message},
						{"attachedMoney",contents.AttachedMoney},
						{"stampItemHash",contents.StampItemHash},
						{"flag",header.Flag}
					};
					// Attached items.
					std::vector<json> itemList;
					for (auto& itr : contents.AttachedItems)
					{
						itemList.push_back({
							{"itemHash", itr.second.ItemHash},
							{"stackCount",itr.second.StackCount},
							{"itemSerial",itr.second.ItemSerial},
							{"instanceEx",itr.second.InstanceEx}
							});
					}
					j["items"] = itemList;
					
					request.GetAsync(this, &MailboxManager::DBSendMail, user, j);
					waitingState = WaitSendMail;

					forcedFreeCashMailSend = true;
				}
			}
			
			void MailboxManager::SendSystemMail(const std::wstring& receiver, const std::wstring& title, const std::wstring& text, const uint32 attachedMoney, const std::vector<XRated::ItemBasicInfo>& sendItems, const std::wstring& sender, const XRated::Mail::HeaderInfo::FlagType flag)
			{
				/** 
				if( user->GetCharacterStateFlags().IsGuestID )
				{
					return;
				}
				*/
				// Generate Mail Contents Info.
				XRated::Mail::ContentsInfo		contents;

				contents.Message = text;
				contents.AttachedMoney = attachedMoney;
				contents.Receiver = receiver;
				contents.StampItemHash = 0; //36359512; // HACK : arcrus. Stamp Item Hash

				for (uint32 i = 0; i < sendItems.size(); ++i)
				{
					contents.AttachedItems[i] = sendItems[i];
				}
				// Generate Mail Header Info.
				XRated::Mail::HeaderInfo		header;
				//header.Title = MakeTitle(contents.Message);
				header.Title = title; // HACK : title must shorter than size (20)

				header.Flag = flag;
				header.Flag |= XRated::Mail::HeaderInfo::FlagType::SystemMail;

				header.Sender = sender;
				// Buffer.
				sendBufferContentsInfo = contents;
				// Request SendMail.
				{
					Net::Api request("Mail.Send", Net::Api::Methods::POST);
					json j = {
						{"receiver",contents.Receiver},
						{"sender",header.Sender},
						{"title",header.Title},
						{"message",contents.Message},
						{"attachedMoney",contents.AttachedMoney},
						{"stampItemHash",contents.StampItemHash},
						{"flag",header.Flag}
					};
					// Attached items.
					std::vector<json> itemList;
					for (auto& itr : contents.AttachedItems)
					{
						itemList.push_back({
							{"itemHash", itr.second.ItemHash},
							{"stackCount",itr.second.StackCount},
							{"itemSerial",itr.second.ItemSerial},
							{"instanceEx",itr.second.InstanceEx}
							});
					}
					j["items"] = itemList;

					request.GetAsync(j);

					/*Logger().Write(IAdmLogger::ImportantLogger, "mail-send", L"system", Http::Logger::Parameter()
						<< 0
						<< receiver
						<< attachedMoney
						<< 0
						<< strLoggingItem);*/
				}
			}

			// Alarm mail
			void MailboxManager::NotifyMailDisposition(UserSharedPtr user, const std::wstring& to, const std::wstring& title)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());

				// Generate Mail Contents Info.
				XRated::Mail::ContentsInfo		contents;

				// TODO : locale
				contents.Message = L"returnMail";
				contents.AttachedMoney = 0;
				contents.Receiver = to.c_str();
				contents.StampItemHash = 0;

				

				// Generate Mail Header Info.
				XRated::Mail::HeaderInfo		header;

				// TODO : locale
				header.Title = title;
				header.Flag = XRated::Mail::HeaderInfo::FlagType::Responsed;
				header.Sender = user->GetName();


				// Request SendMail.
				{
					Net::Api request("Mail.Send", Net::Api::Methods::POST);

					json j = {
						{"receiver",contents.Receiver},
						{"sender",header.Sender},
						{"title",header.Title},
						{"message",contents.Message},
						{"attachedMoney",contents.AttachedMoney},
						{"stampItemHash",contents.StampItemHash},
						{"flag",header.Flag}
					};

					request.GetAsync(this, &MailboxManager::DBNotifyMailDispoition, user, j);
					//waitingState = WaitSendMail;
				}
			}

			void MailboxManager::DBNotifyMailDispoition(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				// Checking Error code.
				/*if (!answer)
				{
					LoggerInstance().Warn((L"[DBNotifyMailDispoition] MailboxManager::DBNotifyMailDispoition() - Error [{}]", errorNumber));
					return;
				}*/

				// Verify packet.
				if (!answer)
				{
					LoggerInstance().Error("[DBNotifyMailDispoition] MailboxManager::DBNotifyMailDispoition() - Invalid Packet received.");
					return;
				}

				// Verify user.
				if (user->GetSerial() != answer.at("senderUniqueSerial").get<uint64>())
				{
					LoggerInstance().Warn("[DBNotifyMailDispoition] MailboxManager::DBNotifyMailDispoition() - User unique serials are mismatched.");
					return;
				}
				// do nothing.
			}

			bool MailboxManager::IsStageItem(const XRated::ItemBasicInfo& itemBasicInfo)
			{
				const Database::Info::ItemInfo* info = Database::DatabaseInstance().InfoCollections.Items.Retrieve(itemBasicInfo.ItemHash);

				if (info != NULL)
				{
					if (info->Attributes.Attr & (1 << 8)/*Database::Info::ItemInfo::ATTRIBUTES::STAGEITEM*/)
					{
						return true;
					}
				}

				return false;
			}

			std::wstring MailboxManager::MakeTitle(const std::wstring& message)
			{
				std::string ret = StringUtil::ToASCII(message);
				ret = StringUtil::Replace(ret, "\r\n", "");
				ret = StringUtil::Replace(ret, "\r", "");
				ret = StringUtil::Replace(ret, "\n", "");
				ret = ret.substr(0, 20);
				if (ret.empty())
					ret = " ";



				return StringUtil::ToUnicode(ret);

				//char tempTitle[21]={0,};
		  //      std::string ascTitle = StringUtil::ToASCII(message);
				//strncpy(tempTitle, ascTitle.c_str(), 20);
				//size_t pos = strcspn(tempTitle, "\r\n");
				//if( pos < 20) {
				//	tempTitle[pos] = NULL;
				//}
				//return StringUtil::ToUnicode(tempTitle);
			}

			// Constuctor/Destructor
			MailboxManager::MailboxManager()
			{
			}

			MailboxManager::~MailboxManager()
			{
			}

			void MailboxManager::Init()
			{
				waitingState = Idle;
				cashStampHash = 0;
				cashStampPosition = XRated::ItemPosition::Invalid;
				sendBufferAttachedItemPositions.clear();
				readBufferContentsInfo.clear();
				sendBufferContentsInfo.Init();
				newMails.clear();
				newMails.push_back(0);
			}
		}
	}
}