#include <StageServer/User/Services/FamilyManager.h>
#include <StageServer/User/User.h>
#include <StageServer/Services/ItemSerial.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			const static std::string guestStageLevelUpLogName("family-gift-levelup-g");
			const static std::string memberStageLevelUpLogName("family-gift-levelup-m");
			const static std::string guestStoredLevelUpLogName("family-gift-slevelup-g");
			const static std::string memberStoredLevelUpLogName("family-gift-slevelup-m");
			const static std::string rebirthLogName("family-gift-rebirth");
			const static std::string contributeLogName("family-gift-contribute");
			const static std::string groupPlayTimeLogName("family-gift-playtime");
			const static std::string fishingLogName("family-gift-fishing");
			const static std::string memorialDayLogName("family-gift-memorialday");
			const static std::string stateClearLogName("family-gift-stageclear");

			FamilyManager::FamilyManager(User& user)
				: owner(user)
				, isFamily(false)
			{
				isFamilyInfoWait = false;
				isFamily = false;
				inviteFamilyId = 0;
				inviteOwnerName.clear();
				members.clear();
				joinedDate = DateTime::Now();
				connectedDate = joinedDate;
				onlineRegularCount = 0;
				onlineGuestCount = 0;

				myInfo.MemberSerial = 0;
				myInfo.Class = XRated::Constants::ClassType::Knight;
				myInfo.StageLevel = 0;
				myInfo.PvpLevel = 0;
				myInfo.IsGuest = false;
				myInfo.IsOnline = false;
				myInfo.PlayTime = 0;
				DateTime defaultDate = DateTime::Infinite;
				joinedDate = defaultDate;
				connectedDate = defaultDate;

				info.Serial = 0;
				info.CreatedTime = defaultDate;
				condition.NextPlayTimeForPersonalPlay = 0xffffffffffffffff;
				condition.ReceivedDateGroupPlay = defaultDate;
				condition.MemorialDay = defaultDate;
			}

			FamilyManager::~FamilyManager()
			{
			}

			void FamilyManager::Init(const std::wstring& characterName)
			{
				AutoLock lock(cs);

				Clear();
				myInfo.CharacterName = characterName;
				connectedDate = DateTime::Now();
				RequestDBFamilyInfoForInit();
			}

			void FamilyManager::Release()
			{
				AutoLock lock(cs);

				RequestDBTerminate();
				Clear();
				myInfo.CharacterName.clear();
			}

			Protocol::FromServer::Family::InviteResult::Type FamilyManager::Invite(const std::wstring& inviter)
			{
				AutoLock lock(cs);
				if ((IsFamily() == true) && (myInfo.IsGuest == false))
				{
					return Protocol::FromServer::Family::InviteResult::Type::Failed_JoinedFamilyMember;
				}
				if (IsInvited() == false)
				{
					inviteOwnerName = inviter;
					inviteFamilyId = 0;
					inviteFamilyCreatedDate = DateTime::Infinite;
					Protocol::FromServer::Family::RequestToInvite packet;
					packet.FromOwner = inviteOwnerName;
					owner.Send(packet);
					return Protocol::FromServer::Family::InviteResult::Type::Success;
				}
				return Protocol::FromServer::Family::InviteResult::Type::Failed_NowOtherFamilyDecisionWait;
			}

			Protocol::FromServer::Family::InviteResult::Type FamilyManager::Invite(const std::wstring& inviter, XRated::Family::FamilySerial familySerial, DateTime createdDate)
			{
				AutoLock lock(cs);
				if ((IsFamily() == true) && (myInfo.IsGuest == false))
				{
					return Protocol::FromServer::Family::InviteResult::Type::Failed_JoinedFamilyMember;
				}
				if (IsInvited() == false)
				{
					inviteOwnerName = inviter;
					inviteFamilyId = familySerial;
					inviteFamilyCreatedDate = createdDate;
					Protocol::FromServer::Family::RequestToInvite packet;
					packet.FromOwner = inviteOwnerName;
					owner.Send(packet);
					return Protocol::FromServer::Family::InviteResult::Type::Success;
				}
				return Protocol::FromServer::Family::InviteResult::Type::Failed_NowOtherFamilyDecisionWait;
			}

			void FamilyManager::LevelUp(uint16 level)
			{
				if (IsFamily() == false)
				{
					return;
				}
				//level
				const Database::Info::FamilyPresentInfo* present = Database::DatabaseInstance().InfoCollections.FamilyInfos.GetLevelUpPresent(level, myInfo.IsGuest);
				if (present != NULL)
				{
					Protocol::FromServer::Family::TakePresentResult result;
					result.Type = XRated::Family::PresentType::LevelUp;
					result.Param = level;
					result.IsSuccess = true;
					{
						AutoLock lock(cs);
						if (myInfo.IsGuest == true)
						{
							SendPresentMail(guestStageLevelUpLogName, *present, result.receiveMembers);
						}
						else
						{
							SendPresentMail(memberStageLevelUpLogName, *present, result.receiveMembers);
						}
						result.Condition = condition;
					}
					owner.Send(result);
				}
			}

			void FamilyManager::AcquiredFishItem(uint32 itemHash)
			{
				if (IsFamily() == false)
				{
					return;
				}
				if (Database::DatabaseInstance().InfoCollections.FamilyInfos.IsRewardFromFishingItem(itemHash) == true)
				{
					Protocol::FromServer::Family::TakePresentResult result;
					result.Param = itemHash;
					result.Type = XRated::Family::PresentType::Fishing;
					result.IsSuccess = true;
					{
						AutoLock lock(cs);
						SendPresentMail(fishingLogName, Database::DatabaseInstance().InfoCollections.FamilyInfos.GetFishingPresent(), result.receiveMembers);
						result.Condition = condition;
					}
					owner.Send(result);
				}
			}

			void FamilyManager::Rebirthed()
			{
				if (IsFamily() == false)
				{
					return;
				}
				Protocol::FromServer::Family::TakePresentResult result;
				result.Param = 0;
				result.Type = XRated::Family::PresentType::Rebirth;
				result.IsSuccess = true;
				{
					AutoLock lock(cs);
					SendPresentMail(rebirthLogName, Database::DatabaseInstance().InfoCollections.FamilyInfos.GetRebirthPresent(), result.receiveMembers);
					result.Condition = condition;
				}
				owner.Send(result);
			}

			void FamilyManager::StoredLevelUp(uint32 storedLevel)
			{
				//storedLevel
				if (IsFamily() == false)
				{
					return;
				}
				const Database::Info::FamilyPresentInfo* present = Database::DatabaseInstance().InfoCollections.FamilyInfos.GetStoredLevelUpPresent(storedLevel, myInfo.IsGuest);
				if (present != NULL)
				{
					Protocol::FromServer::Family::TakePresentResult result;
					result.Param = storedLevel;
					result.Type = XRated::Family::PresentType::StoredLevelUp;
					result.IsSuccess = true;
					{
						AutoLock lock(cs);
						if (myInfo.IsGuest == true)
						{
							SendPresentMail(guestStoredLevelUpLogName, *present, result.receiveMembers);
						}
						else
						{
							SendPresentMail(memberStoredLevelUpLogName, *present, result.receiveMembers);
						}
						result.Condition = condition;
					}
					owner.Send(result);
				}
			}

			void FamilyManager::MissionClear(bool success, XRated::Database::Info::StageGroup* group, uint16 accessLevel)
			{
			}

			bool FamilyManager::IsFamily() const
			{
				return isFamily;
			}

			bool FamilyManager::IsInvited() const
			{
				return (inviteOwnerName.empty() == false);
			}

			bool FamilyManager::IsGuest() const
			{
				if (!IsFamily())
				{
					return false;
				}
				return myInfo.IsGuest;
			}

			void FamilyManager::Clear()
			{
				{
					AutoLock lock(cs);
					DateTime defaultDate = DateTime::Infinite;

					isFamilyInfoWait = false;
					isFamily = false;
					inviteFamilyId = 0;
					inviteFamilyCreatedDate = defaultDate;

					inviteOwnerName.clear();
					members.clear();
					joinedDate = DateTime::Now();
					connectedDate = joinedDate;
					onlineRegularCount = 0;
					onlineGuestCount = 0;

					myInfo.MemberSerial = 0;
					myInfo.Class = XRated::Constants::ClassType::Knight;
					myInfo.StageLevel = 0;
					myInfo.PvpLevel = 0;
					myInfo.IsGuest = false;
					myInfo.IsOnline = false;
					myInfo.PlayTime = 0;
					joinedDate = defaultDate;
					connectedDate = defaultDate;

					info.Serial = 0;
					info.CreatedTime = defaultDate;
					condition.NextPlayTimeForPersonalPlay = 0;
					condition.ReceivedDateGroupPlay = defaultDate;
					condition.MemorialDay = defaultDate;

				}
				owner.ClearFamilyExpFactor();
			}

			void FamilyManager::ResetFamilyPassive()
			{
				const XRated::Database::Info::FamilyPassvieRewardInfo* info = XRated::Database::DatabaseInstance().InfoCollections.FamilyInfos.GetPassiveReward(onlineRegularCount, onlineGuestCount);
				if (info != NULL)
				{
					owner.SetFamilyExpFactor(info->AddExpFactor);
				}
				else
				{
					owner.ClearFamilyExpFactor();
				}
			}

			bool FamilyManager::SendPlayTimePresent(DateTime now, std::vector< XRated::Family::FamilyMemberSerial >& receiveMembers)
			{
				AutoLock lock(cs);
				if (IsFamily() == true)
				{
					if (connectedDate == DateTime::Infinite)
					{
						return false;
					}
					if (connectedDate > now)
					{
						return false;
					}
					double second = now.GetTime().GetCumulatedSec() - connectedDate.GetTime().GetCumulatedSec();
					int64 minute = ((now.GetDate().GetCumulatedDay() - connectedDate.GetDate().GetCumulatedDay()) * 24 * 60) + static_cast<int64>(second / 60);
					connectedDate = now;
					myInfo.PlayTime += minute;

					bool sended = false;
					for (size_t i = 0; i < 3; ++i)
					{
						//while( myInfo.PlayTime >= condition.NextPlayTimeForPersonalPlay ) {
						if (myInfo.PlayTime < condition.NextPlayTimeForPersonalPlay)
						{
							return sended;
						}
						const Database::Info::FamilyPresentInfo* info = Database::DatabaseInstance().InfoCollections.FamilyInfos.GetPersonalPlayPresent(condition.NextPlayTimeForPersonalPlay);
						if (info != NULL)
						{
							SendPresentMail(contributeLogName, *info, receiveMembers);
							sended = true;
						}
						else
						{
							LoggerInstance().Warn("can not found Family PlayTime Present : playTime {}", condition.NextPlayTimeForPersonalPlay);
						}
						condition.NextPlayTimeForPersonalPlay = XRated::Database::DatabaseInstance().InfoCollections.FamilyInfos.GetNextPersonalPlayPresent(condition.NextPlayTimeForPersonalPlay);
					}
					return sended;
				}
				return false;
			}

			uint32 FamilyManager::SendMemorialDayPresent(DateTime now, std::vector<XRated::Family::FamilyMemberSerial>& receiveMembers)
			{
				AutoLock lock(cs);
				if (IsFamily() == true)
				{
					uint32 dayTemp = info.CreatedTime.GetDate().GetCumulatedDay();
					uint32 count = 0;
					uint32 recievedMemorialDay = 0;
					while (now.GetDate() > condition.MemorialDay.GetDate())
					{
						uint32 day = condition.MemorialDay.GetDate().GetCumulatedDay() - dayTemp;

						const Database::Info::FamilyPresentInfo* presentInfo = Database::DatabaseInstance().InfoCollections.FamilyInfos.GetMemorialDayPresent(day);
						if (presentInfo != NULL)
						{
							receiveMembers.clear();
							SendPresentMail(memorialDayLogName, *presentInfo, receiveMembers);
							recievedMemorialDay = day;
						}
						else
						{
							LoggerInstance().Warn("can not found family memorial day present, day : {}", day);
						}

						condition.MemorialDay = Database::DatabaseInstance().InfoCollections.FamilyInfos.GetNextMemorialDayPresentDate(info.CreatedTime, condition.MemorialDay);
						if (count > 3)
						{
							break;
						}
						++count;
					}
					//memorial day update.
					if (recievedMemorialDay > 0)
					{
						RequestDBUpdateMemorialDay();
					}
					return recievedMemorialDay;
				}
				return 0;
			}

			void FamilyManager::SendPresentMail(const std::string& logName, const Database::Info::FamilyPresentInfo& info, std::vector< XRated::Family::FamilyMemberSerial >& sendedMembers)
			{
				DateTime now = DateTime::Now();
				std::wstring sender = info.MailInfo.Sender;
				if (sender.empty() == true)
				{
					sender = myInfo.CharacterName;
				}

				AutoLock lock(cs);
				if (myInfo.IsGuest == true)
				{
					if (SendPresentMail(logName, myInfo.MemberSerial, myInfo.CharacterName, sender, info.MailInfo.Title, info.MailInfo.Text, info.SenderMemberPresentWithGuest, now, myInfo.IsGuest) == true)
					{
						sendedMembers.push_back(myInfo.MemberSerial);
					}
				}
				else
				{
					if (SendPresentMail(logName, myInfo.MemberSerial, myInfo.CharacterName, sender, info.MailInfo.Title, info.MailInfo.Text, info.SenderMemberPresentWithRegular, now, myInfo.IsGuest) == true)
					{
						sendedMembers.push_back(myInfo.MemberSerial);
					}
				}

				FamilyMemberList::iterator iter = members.begin();
				FamilyMemberList::iterator end = members.end();
				while (iter != end)
				{
					if (info.IsOnlyOnlineUser == true)
					{
						if (iter->IsOnline == false)
						{
							++iter;
							continue;
						}
					}
					if (iter->MemberSerial != myInfo.MemberSerial)
					{
						bool sended = false;
						if (iter->IsGuest == true)
						{
							sended = SendPresentMail(logName, iter->MemberSerial, iter->CharacterName, sender, info.MailInfo.Title, info.MailInfo.Text, info.OtherMemberPresentWithGuest, now, iter->IsGuest);
						}
						else
						{
							sended = SendPresentMail(logName, iter->MemberSerial, iter->CharacterName, sender, info.MailInfo.Title, info.MailInfo.Text, info.OtherMemberPresentWithRegular, now, iter->IsGuest);
						}
						if (sended == true)
						{
							sendedMembers.push_back(iter->MemberSerial);
						}
					}
					++iter;
				}
			}

			bool FamilyManager::SendPresentMail(const std::string& logName, const XRated::Family::FamilyMemberSerial receiverMemberSerial, const std::wstring& receiver, const std::wstring& sender, const std::wstring& title, const std::wstring& text, const Database::Info::FamilyPresentInfo::Present& present, const DateTime current, bool isGuest)
			{
				if ((present.Money == 0) && (present.Items.empty() == true))
				{
					return false;
				}
				std::vector< XRated::ItemBasicInfo > sendItems;
				sendItems.reserve(present.Items.size());

				Database::Info::FamilyPresentInfo::Present::ItemList::const_iterator iter = present.Items.begin();
				Database::Info::FamilyPresentInfo::Present::ItemList::const_iterator end = present.Items.end();

				std::wstring strLoggingItem = L"|";

				while (iter != end)
				{
					const Database::Info::ItemInfo* itemInfo = Database::DatabaseInstance().InfoCollections.Items.Retrieve(iter->Hash);
					if (itemInfo != NULL)
					{
						XRated::ItemBasicInfo item;
						if (Database::Enchant::IsEnchantable(itemInfo) == false)
						{
							Database::Enchant::NormalBitfields field(0);

							if (iter->ExpireHour > 0)
							{
								static uint32 minuteOfHour = 60;
								DateTime expireDate(current);
								int minSum = expireDate.Get(DateTime::Unit::Minute) + (iter->ExpireHour * minuteOfHour);
								if (minSum / DateTime::Constant::MinutesInAnHour != 0)
								{
									expireDate.Add(DateTime::Unit::Hour, minSum / DateTime::Constant::MinutesInAnHour);
								}
								expireDate.Set(DateTime::Unit::Minute, minSum % DateTime::Constant::MinutesInAnHour);
								/* field.ExpiredYear   = expireDate.Get( DateTime::Unit::Year );
								field.ExpiredMonth  = expireDate.Get( DateTime::Unit::Month );
								field.ExpiredDay    = expireDate.Get( DateTime::Unit::Day );
								field.ExpiredHour   = expireDate.Get( DateTime::Unit::Hour );
								field.ExpiredMinute = expireDate.Get( DateTime::Unit::Minute );
								field.ExpiredSecond = expireDate.Get( DateTime::Unit::Second ); */
							}
							item.InstanceEx = field;
						}
						else
						{
							item.InstanceEx = 0;
						}
						item.ItemHash = iter->Hash;
						item.ItemSerial = ItemSerialInstance().MakeItemSerial();
						item.StackCount = 1;
						sendItems.push_back(item);

						strLoggingItem += fmt::format(L"{}:{}:{}:{}|"
							, item.ItemHash
							, item.ItemSerial
							, item.StackCount
							, item.InstanceEx.ToString());
					}
					else
					{
						LoggerInstance().Error("SendPresentMail Failed : can not found item info : {}", iter->Hash);
					}
					++iter;
				}

				owner.SendSystemMail(receiver, title, text, present.Money, sendItems, sender);
				//Logger().Write(IAdmLogger::ImportantLogger, logName.c_str(), receiver, Service::Http::Logger::Parameter() << info.Serial << receiverMemberSerial << (isGuest ? 1 : 0) << strLoggingItem);
				return true;
			}

			//recv from client(dispatch)
			void FamilyManager::Dispatch(UserSharedPtr, Protocol::ToServer::Family::Invite& packet)
			{
				XRated::Family::FamilySerial serial = 0;
				DateTime createdDate = DateTime::Now();
				Protocol::FromServer::Family::InviteResult result;
				{
					AutoLock lock(cs);
					if (IsFamily() == true)
					{
						if (myInfo.IsGuest == true)
						{
							owner.CriticalError("Family::Invite Failed : this user is guest", false);
							return;
						}
						serial = info.Serial;
						createdDate = info.CreatedTime;
					}
				}

				if (serial != 0)
				{
					result.Result = owner.RequestInviteFamily(serial, createdDate, packet.TargetSerial, packet.Target);
				}
				else
				{
					result.Result = owner.RequestInviteFamily(packet.TargetSerial, packet.Target);
				}
				owner.Send(result);
			}

			void FamilyManager::Dispatch(UserSharedPtr, Protocol::ToServer::Family::Leave& packet)
			{
				AutoLock lock(cs);
				if (IsFamily() == true)
				{
					RequestDBLeave(true);
				}
			}

			void FamilyManager::Dispatch(UserSharedPtr, Protocol::ToServer::Family::InviteAnwer& packet)
			{
				if ((packet.IsJoin == true) && ((IsFamily() == false) || (myInfo.IsGuest == true)))
				{		
					RequestDBJoin();
					return;
				}
				else
				{
					AutoLock lock(cs);
					Protocol::FromServer::Family::JoineResult result;
					result.Result = Protocol::FromServer::Family::JoineResult::Type::Cancel;
					result.InviteOwner = inviteOwnerName;
					owner.Send(result);

					inviteOwnerName.clear();
					inviteFamilyId = 0;
					inviteFamilyCreatedDate = DateTime::Infinite;
				}
			}

			void FamilyManager::Dispatch(UserSharedPtr, Protocol::ToServer::Family::RefreshInfo& packet)
			{
				RequestDBFamilyInfoForRefresh();
			}

			void FamilyManager::Dispatch(UserSharedPtr, Protocol::ToServer::Family::RequestPresent& packet)
			{

				Protocol::FromServer::Family::TakePresentResult result;
				result.Param = 0;
				result.Type = packet.Type;

				if (IsFamily() == false)
				{
					return;
				}
				DateTime now = DateTime::Now();


				switch (packet.Type)
				{
				case XRated::Family::GroupPlay:
				{
					AutoLock lock(cs);
					if (condition.ReceivedDateGroupPlay.GetDate() != now.GetDate())
					{
						condition.ReceivedDateGroupPlay = now;
						Net::Api expireUpdatePacket("Family.UpdateExpire_Gift1");
						expireUpdatePacket << myInfo.CharacterName << myInfo.MemberSerial << condition.ReceivedDateGroupPlay.ToString();
						expireUpdatePacket.GetAsync();
						SendPresentMail(groupPlayTimeLogName, Database::DatabaseInstance().InfoCollections.FamilyInfos.GetGroupPlayPresent(), result.receiveMembers);
						result.IsSuccess = true;

					}
					else
					{
						result.IsSuccess = false;
					}
				}
				break;
				case XRated::Family::PersonalPlay:
					result.IsSuccess = SendPlayTimePresent(now, result.receiveMembers);
					break;
				case XRated::Family::FamilyMemorialDay:
					result.Param = SendMemorialDayPresent(now, result.receiveMembers);
					result.IsSuccess = result.Param ? true : false;
					break;
				default:
					owner.CriticalError(fmt::format("Wrong Request : RequestPresent : {}", static_cast<int>(packet.Type)).c_str());
					return;
				}
				result.Condition = condition;
				owner.Send(result);
			}

			void FamilyManager::Dispatch(UserSharedPtr, Protocol::ToServer::Family::Kick& packet)
			{
				if (IsFamily() == false)
				{
					LoggerInstance().Warn(L"Failed Family::Kick : Not Joined Family User.");
					return;
				}

				FamilyMemberList::const_iterator iter = std::find_if(members.begin(), members.end(), XRated::Family::MemberInfo::FindByCharacterName(packet.TargetCharacterName));
				if (iter != members.end())
				{
					if (iter->IsOnline == true)
					{
						Protocol::FromServer::Family::KickResult result;
						result.Result = Protocol::FromServer::Family::KickResult::Failed_IsOnlineState;
						owner.Send(result);

						return;
					}

					Net::Api kickPacket("Family.Kick");
					kickPacket << myInfo.CharacterName << myInfo.MemberSerial << iter->CharacterName << iter->MemberSerial;
					kickPacket.GetAsync(this, &FamilyManager::DBKicked, owner.shared_from_this());
				}
				else
				{
					Protocol::FromServer::Family::KickResult result;
					result.Result = Protocol::FromServer::Family::KickResult::Failed_NotEntredFamily;
					owner.Send(result);
				}
			}

			//db request.
			void FamilyManager::RequestDBFamilyInfoForInit()
			{
				AutoLock lock(cs);
				Net::Api packet("Family.JoinedInfo");
				packet << myInfo.CharacterName;
				packet.GetAsync(this, &FamilyManager::DBFamilyInfoForInit, owner.shared_from_this());
			}

			void FamilyManager::RequestDBFamilyInfoForRefresh()
			{
				AutoLock lock(cs);
				if (myInfo.CharacterName.empty() == true)
				{
					return;
				}
				//not joined family to info clear
				if (isFamilyInfoWait == false)
				{
					if (IsFamily() == false)
					{
						Clear();
					}
					Net::Api packet("Family.JoinedInfo");
					packet << myInfo.CharacterName;
					packet.GetAsync(this, &FamilyManager::DBFamilyInfoForRefresh, owner.shared_from_this());
					isFamilyInfoWait = true;
				}
			}

			void FamilyManager::RequestDBJoin()
			{
				{
					AutoLock lock(cs);
					if (myInfo.CharacterName.empty() == true)
					{
						LoggerInstance().Warn("RequestDBJoin : not inited familyManager : {}", owner.GetSerial());
						return;
					}
					if ((IsFamily() == false) || (myInfo.IsGuest == true))
					{
						DateTime now = DateTime::Now();

						if (inviteFamilyId != 0)
						{
							if (inviteFamilyCreatedDate > now)
							{
								LoggerInstance().Warn(L"Family Join : wrong created family date [{}]", inviteFamilyCreatedDate.ToString().c_str());
								inviteFamilyCreatedDate = now;
							}
							DateTime memorialDate = XRated::Database::DatabaseInstance().InfoCollections.FamilyInfos.GetNextMemorialDayPresentDate(inviteFamilyCreatedDate, now);
							Net::Api packet("Family.Join");
							packet << myInfo.CharacterName << inviteFamilyId << memorialDate.ToString();
							packet.GetAsync(this, &FamilyManager::DBJoined, owner.shared_from_this());
							return;
						}
						else
						{
							if (inviteOwnerName.empty() == false)
							{
								DateTime memorialDate = XRated::Database::DatabaseInstance().InfoCollections.FamilyInfos.GetNextMemorialDayPresentDate(now, now);
								Net::Api packet("Family.Create");
								packet << myInfo.CharacterName << inviteOwnerName << memorialDate.ToString();
								packet.GetAsync(this, &FamilyManager::DBCreated, owner.shared_from_this());
								return;
							}
						}
					}
					else
					{
						LoggerInstance().Warn(L"RequestDBJoin : is joined family user : {}", myInfo.CharacterName.c_str());
						return;
					}
				}
				owner.CriticalError("Wrong Family InviteAnwer : not invited", false);
			}

			void FamilyManager::RequestDBLeave(bool isPenalty)
			{
				AutoLock lock(cs);
				if (IsFamily() == true)
				{
					Net::Api packet("Family.Leave");
					int temp = isPenalty ? 1 : 0;
					packet << myInfo.CharacterName << myInfo.MemberSerial << temp;
					packet.GetAsync(this, &FamilyManager::DBLeaved, owner.shared_from_this());
				}
				else
				{
					LoggerInstance().Warn(L"RequestDBLeave : not joined family user : {}", myInfo.CharacterName.c_str());
				}
			}

			void FamilyManager::RequestDBTerminate()
			{
				AutoLock lock(cs);

				if (IsFamily() == true) {
					Net::Api packet("Family.Terminate");
					DateTime now = DateTime::Now();
					std::vector< XRated::Family::FamilyMemberSerial > receiveMembers;
					SendPlayTimePresent(now, receiveMembers);
					packet << myInfo.CharacterName << myInfo.PlayTime;
					packet.GetAsync();
				}
			}

			//db response
			void FamilyManager::DBFamilyInfoForInit(const UserSharedPtr& user, Net::Answer& answer)
			{
				SetFamilyInfo(answer, false);
				Protocol::FromServer::Family::TakePresentResult result;
				result.Param = SendMemorialDayPresent(DateTime::Now(), result.receiveMembers);
				if (result.Param != 0)
				{
					result.IsSuccess = true;
					result.Type = XRated::Family::PresentType::FamilyMemorialDay;
					result.Condition = condition;
					owner.Send(result);
				}
				
			}

			void FamilyManager::DBFamilyInfoForRefresh(const UserSharedPtr& user, Net::Answer& answer)
			{
				isFamilyInfoWait = false;
				if (answer.errorCode != 0) {
					Clear();
					Protocol::FromServer::Family::RefreshFailed result;
					result.Result = Protocol::FromServer::Family::RefreshFailed::Type::NotEntredFamily;
					owner.Send(result);
				}
				if (!answer)
				{
					// nothing to do
					LoggerInstance().Error(L"DBFamilyInfo Failed : Error Number : {}", answer.errorCode);
					owner.CriticalError("DBFamilyInfoForRefresh Failed", false);
					return;
				}
				SetFamilyInfo(answer, true);
				
			}

			void FamilyManager::RequestDBUpdateMemorialDay()
			{
				AutoLock lock(cs);

				if (IsFamily() == true)
				{
					Net::Api packet("Family.UpdateMemorialDay");
					packet << myInfo.CharacterName << myInfo.MemberSerial << condition.MemorialDay.ToString();
					packet.GetAsync();
				}
			}

			void FamilyManager::SetFamilyInfo(Net::Answer& answer, bool isRefresh)
			{
				// familyId,createDate,familyMemberId,isGuest,isOnline,playTime,expire_gift1,memorialDay,joinedDate,
				// {familyMemberId,characterName,classsNumber,stageLevel,pvpLevel,isGuest,isOnline,playTime,joinedDate,lastLoggedDate}
				Protocol::FromServer::Family::Info result;
				{
					AutoLock lock(cs);
					isFamilyInfoWait = false;


					DateTime now = DateTime::Now();
					if (isFamily == false)
					{
						connectedDate = now;
						answer.get_to("playTime", myInfo.PlayTime);
						condition.NextPlayTimeForPersonalPlay = XRated::Database::DatabaseInstance().InfoCollections.FamilyInfos.GetNextPersonalPlayPresent(myInfo.PlayTime);

						answer.get_to("memorialDay", condition.MemorialDay);
					}
					else
					{
						Protocol::FromServer::Family::TakePresentResult result;
						result.Param = 0;
						result.Type = XRated::Family::PresentType::PersonalPlay;
						result.IsSuccess = SendPlayTimePresent(now, result.receiveMembers);

						if (result.IsSuccess == true)
						{
							result.Condition = condition;
							owner.Send(result);
						}
					}

					isFamily = true;
					myInfo.CharacterName = owner.GetName();

					if (owner.GetPlayer() == NULL)
					{
						myInfo.Class = owner.GetInitialPlayerData()->type;
						myInfo.StageLevel = owner.GetInitialPlayerData()->level;
						myInfo.PvpLevel = owner.GetInitialPlayerData()->pvpLevel;
					}
					else
					{
						myInfo.Class = owner.GetClassType();
						myInfo.StageLevel = owner.GetLevel();
						myInfo.PvpLevel = owner.GetPvpLevel();
					}

					//familyId,createDate,familyMemberId,isGuest,isOnline,playTime,expire_gift1,memorialDay,joinedDate
					//,{familyMemberId,characterName,classsNumber,stageLevel,pvpLevel,isGuest,isOnline,playTime}
					answer.get_to("familyId", info.Serial);
					answer.get_to("createDate", info.CreatedTime);
					answer.get_to("id", myInfo.MemberSerial);
					answer.get_to("isGuest", myInfo.IsGuest);
					answer.get_to("isOnline", myInfo.IsOnline);
					answer.get_to("expireGroupGift", condition.ReceivedDateGroupPlay);
					answer.get_to("joinedDate", joinedDate);

					if (condition.MemorialDay < joinedDate)
					{
						condition.MemorialDay = Database::DatabaseInstance().InfoCollections.FamilyInfos.GetNextMemorialDayPresentDate(info.CreatedTime, joinedDate);
						RequestDBUpdateMemorialDay();
					}

					{
						// { familyMemberId, characterName, classsNumber, stageLevel, pvpLevel, isGuest, isOnline, playTime, joinedDate, lastLoggedDate }
						for (auto& member : answer.at("members")) {
							{
								members.push_back(Family::MemberInfo());
								auto& info = members.back();

								member.at("id").get_to(info.MemberSerial); // familyMember ID NOT CharacterSerial !!!!!!!!!!!!!!!!!!!
								member.at("characterName").get_to(info.CharacterName);
								info.Class = static_cast<XRated::Constants::ClassType>(member.at("classNumber").get<uint8>());
								member.at("stageLevel").get_to(info.StageLevel);
								member.at("pvpLevel").get_to(info.PvpLevel);
								member.at("lastLoggedDate").get_to(info.LastLoggedDate);
								member.at("isGuest").get_to(info.IsGuest);
								member.at("isOnline").get_to(info.IsOnline);
								member.at("playTime").get_to(info.PlayTime);
								member.at("joinedDate").get_to(info.JoinedDate);
								//populate the counts.
								if (info.IsOnline) {
									if (info.IsGuest)
										onlineGuestCount++;
								}
								else
									onlineRegularCount++;

							}
						}
					}
					if (members.size() < 2)
					{
						RequestDBLeave(false);
						return;
					}

					result.FamilyInfo = info;
					result.Members = members;
					result.Condition = condition;
				}

				ResetFamilyPassive();

				//family packet send.		
				owner.Send(result);
			}

			void FamilyManager::DBCreated(const UserSharedPtr& user, Net::Answer& answer)
			{
				Protocol::FromServer::Family::JoineResult result;
				if (!answer)
				{
					LoggerInstance().Error("DBCreated Failed : Error Number : {}", answer.errorCode);

					switch (answer.errorCode)
					{
					case 1:
						result.Result = Protocol::FromServer::Family::JoineResult::Type::Failed_JoinedOtherFamilyFromInviteUser;
						break;
					case 2:
						result.Result = Protocol::FromServer::Family::JoineResult::Type::Failed_NotReadyJoinFromInviteUser;
						break;
					case 3:
						result.Result = Protocol::FromServer::Family::JoineResult::Type::Failed_JoinedOtherFamily;
						break;
					case 4:
						result.Result = Protocol::FromServer::Family::JoineResult::Type::Failed_NotReadyJoin;
						break;
					default:
						result.Result = Protocol::FromServer::Family::JoineResult::Type::Failed_Unknown;
						break;
					}
				}
				else
				{
					{
						AutoLock lock(cs);
						answer.get_to("id", info.Serial);
						XRated::Family::FamilyMemberSerial inviterMemberSerial = answer.at("inviter").get<uint32>();
						XRated::Family::FamilyMemberSerial confirmMemberSerial = answer.at("member").get<uint32>();
						

						//Logger().Write(IAdmLogger::ImportantLogger, "family-create", inviteOwnerName, Service::Http::Logger::Parameter() << info.Serial);
						//Logger().Write(IAdmLogger::ImportantLogger, "family-joined", inviteOwnerName, Service::Http::Logger::Parameter() << info.Serial << inviterMemberSerial << 0);
						//Logger().Write(IAdmLogger::ImportantLogger, "family-joined", myInfo.CharacterName, Service::Http::Logger::Parameter() << info.Serial << confirmMemberSerial << 0);

						isFamily = false;
						//db reqyest family info init.
						RequestDBFamilyInfoForInit();
						/*UserSharedPtr player1 = UserManagerInstance().GetUser(inviteOwnerName);
						if (player1->achievement.IsConnected()) {
							Net::Protocol::Achievement::ServerProtocol::FamilyActionChanged packetone;
							packetone.characterName = player1->GetName();
							packetone.Action = Net::Protocol::Achievement::ServerProtocol::FamilyActionChanged::Action::Family_Joined;
							player1->achievement.Send(packetone);
						}
						UserSharedPtr player2 = UserManagerInstance().GetUser(myInfo.CharacterName);
						if (player2->achievement.IsConnected()) {
							Net::Protocol::Achievement::ServerProtocol::FamilyActionChanged packettwo;
							packettwo.characterName = player2->GetName();
							packettwo.Action = Net::Protocol::Achievement::ServerProtocol::FamilyActionChanged::Action::Family_Joined;
							player2->achievement.Send(packettwo);
						}*/
					}
					result.Result = Protocol::FromServer::Family::JoineResult::Type::Success;
				}
				{
					AutoLock lock(cs);
					result.InviteOwner = inviteOwnerName;
					inviteOwnerName.clear();
					inviteFamilyId = 0;
					inviteFamilyCreatedDate = DateTime::Infinite;
				}
				owner.Send(result);
			}

			void FamilyManager::DBJoined(const UserSharedPtr& user, Net::Answer& answer)
			{
				Protocol::FromServer::Family::JoineResult result;
				if (!answer)
				{
					LoggerInstance().Error(L"DBJoined Failed : Error Number : {}", answer.errorCode);

					switch (answer.errorCode)
					{
					case 1:
						result.Result = Protocol::FromServer::Family::JoineResult::Type::Failed_NotExistFamily;
						break;
					case 2:
						result.Result = Protocol::FromServer::Family::JoineResult::Type::Failed_OverToFamilyCount;
						break;
					case 3:
						result.Result = Protocol::FromServer::Family::JoineResult::Type::Failed_JoinedOtherFamily;
						break;
					case 4:
						result.Result = Protocol::FromServer::Family::JoineResult::Type::Failed_NotReadyJoin;
						break;
					default:
						result.Result = Protocol::FromServer::Family::JoineResult::Type::Failed_Unknown;
						break;
					}
				}
				else
				{
					result.Result = Protocol::FromServer::Family::JoineResult::Type::Success;
					{
						AutoLock lock(cs);
						answer.get_to("id", info.Serial);
						XRated::Family::FamilyMemberSerial inviterMemberSerial = answer.at("memberSerial").get<uint32>();
						result.InviteOwner = inviteOwnerName;
						inviteOwnerName.clear();
						inviteFamilyId = 0;
						inviteFamilyCreatedDate = DateTime::Infinite;

						//Logger().Write(IAdmLogger::ImportantLogger, "family-joined", myInfo.CharacterName, Service::Http::Logger::Parameter() << info.Serial << myInfo.MemberSerial << 0);

						isFamily = false;
						//db reqyest family info.
						RequestDBFamilyInfoForInit();
					}
				}
				owner.Send(result);
			}

			void FamilyManager::DBLeaved(const UserSharedPtr& user, Net::Answer& answer)
			{
				Protocol::FromServer::Family::LeavedResult result;
				if (!answer)
				{
					LoggerInstance().Error("DBLeaved Failed : Error Number : {}", answer.errorCode);

					Protocol::FromServer::Family::LeavedResult packet;
					switch (answer.errorCode)
					{
					case 1:
						result.Result = Protocol::FromServer::Family::LeavedResult::Type::Failed_NotMemberOfFamily;
						break;
					default:
						result.Result = Protocol::FromServer::Family::LeavedResult::Type::Failed_Unknown;
						break;
					}
				}
				else
				{
					{
						AutoLock lock(cs);
						Clear();
					}
					//response : characterName, familyId, familyMemberId, isGuest
					std::wstring characterName = answer.at("characterName").get<std::wstring>();
					XRated::Family::FamilySerial serial = answer.at("id").get<uint32>();
					XRated::Family::FamilyMemberSerial memberSerial = answer.at("memberId").get<uint32>();
					int isGuest = answer.at("isGuest").get<bool>();
					//Logger().Write(IAdmLogger::ImportantLogger, "family-leave", characterName, Service::Http::Logger::Parameter() << serial << memberSerial << isGuest);

					owner.ClearFamilyExpFactor();
					result.Result = Protocol::FromServer::Family::LeavedResult::Type::Success;
				}
				owner.Send(result);
			}

			void FamilyManager::DBKicked(const UserSharedPtr& user, Net::Answer& answer)
			{
				Protocol::FromServer::Family::KickResult result;
				if (!answer)
				{
					//ALLM_ERROR((L"DBKicked Failed : Error Number : {}", errorNumber));
					switch (answer.errorCode)
					{
					case 1:
						result.Result = Protocol::FromServer::Family::KickResult::Type::Failed_NotEntredFamily;
						break;

					default:
						result.Result = Protocol::FromServer::Family::KickResult::Type::Failed_NotEntredFamily;
						break;
					}
				}
				else
				{
					//response : characterName_target, familyID_target, familyMemberID_target, IsGutest_target

					String target = answer.at("targetCharacterName").get<std::wstring>();
					XRated::Family::FamilyMemberSerial targetSerial = answer.at("targetSerial").get<uint32>();
					XRated::Family::FamilyMemberSerial targetMemberSerial = answer.at("targetMemberSerial").get<uint32>();
					bool targetIsGuest = answer.at("targetIsGuest").get<bool>();
					result.Result = (targetIsGuest == 1)
						? Protocol::FromServer::Family::KickResult::Type::GuestKickSuccess
						: Protocol::FromServer::Family::KickResult::Type::FixedMemberKickSuccess;

					{
						AutoLock lock(cs);
						FamilyMemberList::iterator iter = std::find_if(members.begin(), members.end(), XRated::Family::MemberInfo::FindByCharacterName(target));
						if (iter != members.end())
						{
							members.erase(iter);
						}
					}
					result.Target = target;
					//Logger().Write(IAdmLogger::ImportantLogger, "family-kick", target, Service::Http::Logger::Parameter() << targetSerial << targetMemberSerial << targetIsGuest);
				}
				owner.Send(result);
			}
		}
	}
}