#include "QuestManager.h"
#include <StageServer/Protocol/FromServer.h>
#include <StageServer/User/User.h>
#include <StageServer/User/UserManager.h>
#include <StageServer/User/IUserRoom.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class QuestException : public Exception
			{
			public:
				virtual bool IsCritical() { return false; }
				QuestException(const wchar_t* message) : Exception(message) {}
				QuestException(const String& message) : Exception(message) {}
			};
			class QuestCriticalException : public QuestException
			{
			public:
				bool IsCritical() { return true; }
				QuestCriticalException(const wchar_t* message) : QuestException(message) {}
				QuestCriticalException(const String& message) : QuestException(message) {}
			};
			void WriteQuestInfo(const QuestManager::Quest& quest)
			{
				LoggerInstance().Error(L"Quest Hash {} , Current State {}", quest.Id, quest.CurrentState);
				LoggerInstance().Error(L"ExpiredDate : {}", quest.ExpiredDate.ToString().c_str());
				size_t max = quest.Params.size();
				for (size_t i = 0; i < max; ++i)
				{
					LoggerInstance().Error("Param .{} : {}", i, quest.Params[i]);
				}
			}

			bool IsDuplicateQuestList(const std::vector<QuestManager::Quest>& quests)
			{
				bool duplicated = false;
				std::vector<uint32>	duplicatedHashs;

				std::vector<QuestManager::Quest>::const_iterator iter = quests.begin();
				std::vector<QuestManager::Quest>::const_iterator end = quests.end();
				while (iter != end) {
					uint32 hash = iter->Id;
					if (duplicatedHashs.end() != std::find(duplicatedHashs.begin(), duplicatedHashs.end(), hash)) {
						++iter;
						continue;
					}
					std::vector<QuestManager::Quest>::const_iterator iterTemp = iter;
					int count = 0;
					++iterTemp;
					while (iterTemp != quests.end()) {
						iterTemp = std::find_if(iterTemp, quests.end(), QuestManager::Quest::FindByHash(hash));
						if (iterTemp != quests.end()) {
							WriteQuestInfo(*iterTemp);
							++iterTemp;
							++count;
						}
					}
					if (count > 0) {
						//duplicated
						WriteQuestInfo(*iter);
						duplicated = true;
						duplicatedHashs.push_back(hash);
						LoggerInstance().Error("Duplicate Quest Hash : {} , Count : {}", hash, (count + 1));
					}
					++iter;
				}
				return duplicated;
			}
			bool IsDuplicateQuestList(const std::vector<QuestManager::Quest>& quests, uint32 hash)
			{
				bool duplicated = false;


				std::vector<QuestManager::Quest>::const_iterator iter = std::find_if(quests.begin(), quests.end(), QuestManager::Quest::FindByHash(hash));
				if (iter == quests.end()) {
					return false;
				}
				std::vector<QuestManager::Quest>::const_iterator iterTemp = iter;
				++iterTemp;
				int count = 0;
				while (iterTemp != quests.end()) {
					iterTemp = std::find_if(iterTemp, quests.end(), QuestManager::Quest::FindByHash(hash));
					if (iterTemp != quests.end()) {
						WriteQuestInfo(*iterTemp);
						++iterTemp;
						++count;
					}
				}
				if (count > 0) {
					WriteQuestInfo(*iter);
					duplicated = true;
					LoggerInstance().Error("Duplicate Quest Hash : {} , Count : {}", hash, (count + 1));
				}

				return duplicated;
			}



			QuestManager::Quest::Quest(uint32 questHash, const Database::Info::QuestInfo* info, const DateTime::Date& expiredDate, bool parameterChanged)
				: XRated::Quest(questHash, expiredDate)
				, Info(info)
				, ParameterChanged(parameterChanged)
			{
				if (!info)
					throw QuestCriticalException(fmt::format(L"invalid or unknown quest information : {}", questHash).c_str());
			}

			QuestManager::Quest::Quest(uint32 questHash, const Database::Info::QuestInfo* info, const DateTime::Date& expiredDate)
				: XRated::Quest(questHash, expiredDate)
				, Info(info)
				, ParameterChanged(false)
			{
				if (!info)
					throw QuestCriticalException(fmt::format(L"invalid or unknown quest information : {}", questHash).c_str());
			}

			QuestManager::Quest::Quest(uint32 questHash)
				: XRated::Quest(questHash, XRated::Quest::NotUsedDate)
				, Info(Database::DatabaseInstance().InfoCollections.Quests.Retrieve(questHash))
				, ParameterChanged(false)
			{
				if (Info == NULL)
					throw QuestCriticalException(fmt::format(L"invalid or unknown quest information : {}", questHash).c_str());
			}

			uint8 QuestManager::GetQuestState(const UserSharedPtr user, uint32 questHash) const
			{
				AutoLock lock(cs);
				{

					std::vector<Quest>::const_iterator i = std::find_if(workingQuests.begin(), workingQuests.end(), XRated::Quest::FindByHash(questHash));
					if (i != workingQuests.end())
					{
						/* because Quest::CurrentState is used as current objective state internally, it should be converted as real current state */
						if (i->CurrentState == XRated::Quest::State::Succeeded && GetValidResult(user, i->Info->CompleteCondition, false) != ValidQuestConditionResult::Success)
							return XRated::Quest::State::Working;
						return i->CurrentState;
					}
				}

				{
					std::map<uint32, uint32>::const_iterator i = completedQuests.find(questHash);
					if ((i != completedQuests.end()) && (i->second > 0)) return XRated::Quest::State::Completed;
				}

				return XRated::Quest::State::NotAccepted;
			}

			uint8 QuestManager::SetQuestState(UserSharedPtr user, uint32 questHash, uint8 newState)
			{
				AutoLock lock(cs);
				uint8 oldState(XRated::Quest::State::NotAccepted);
				std::vector<Quest>::iterator i = std::find_if(workingQuests.begin(), workingQuests.end(), XRated::Quest::FindByHash(questHash));
				if (i != workingQuests.end())
				{
					oldState = i->CurrentState;
					if (oldState == newState) // nothing has been changed
						return oldState;

					switch (newState) // special case
					{
					case XRated::Quest::State::Offered: // ignorible
						return oldState;
					case XRated::Quest::State::NotAccepted: // drop the quest
						if (DropQuest(user, questHash) == true) {
							//Logger().Write(IAdmLogger::ImportantLogger, "questdrop-setqueststate", user->GetName(), Http::Logger::Parameter() << questHash);
						}
						return oldState;
					}

					i->CurrentState = newState;
				}
				else
				{
					switch (newState)
					{
					case XRated::Quest::State::Succeeded:
					case XRated::Quest::State::Failed:
					case XRated::Quest::State::Completed:
					case XRated::Quest::State::NotAccepted: // no-working quest cannot be dropped
						LoggerInstance().Error("quest state can be changed in working quest list");
						return XRated::Quest::State::NotAccepted;
					}

					if (newState != XRated::Quest::State::Offered) // 'Offered' state doesn't affect server-side, is just an event to client.
					{
						workingQuests.push_back(Quest(questHash));
						workingQuests.back().CurrentState = newState;

						//Logger().Write(IAdmLogger::ImportantLogger, "questchange", user->GetName(), Http::Logger::Parameter()
						//	<< questHash << newState );
					}
				}

				/* notice the new state to user */
				Protocol::FromServer::Quest::ChangeState response;
				response.QuestHash = questHash;
				response.NewState = newState;
				if (newState == XRated::Quest::State::Completed)
				{
					response.CompletedCount = completedQuests[questHash];
				}
				else
				{
					response.CompletedCount = 0;
				}
				user->Send(response);

				ChangeActiveQuestState(questHash, newState);

				return oldState;
			}

			uint32 QuestManager::GetQuestParameter(const UserSharedPtr UserSharedPtr, uint32 questHash, uint8 paramIndex) const
			{
				AutoLock lock(cs);
				std::vector<Quest>::const_iterator i = std::find_if(workingQuests.begin(), workingQuests.end(), XRated::Quest::FindByHash(questHash));
				if (i != workingQuests.end() && i->Params.size() > paramIndex)
				{
					return i->Params[paramIndex];
				}

				LoggerInstance().Error("invalid quest({}) or parameter({}) requested", questHash, paramIndex);
				return 0; // unalbe to find the result
			}

			uint32 QuestManager::SetQuestParameter(UserSharedPtr user, uint32 questHash, uint8 paramIndex, uint32 newValue)
			{
				AutoLock lock(cs);
				uint32 oldParam(0);

				std::vector<Quest>::iterator i = std::find_if(workingQuests.begin(), workingQuests.end(), XRated::Quest::FindByHash(questHash));
				if (i != workingQuests.end() && i->Params.size() > paramIndex)
				{
					oldParam = i->Params[paramIndex];
					i->Params[paramIndex] = newValue;

					/* notice the new parameter to user */
					if (oldParam != newValue)
					{
						Protocol::FromServer::Quest::Update response;
						response.Quest = *i;
						user->Send(response);

						i->ParameterChanged = true;
					}

					/* update quest state by parameter change */
					switch (i->CurrentState)
					{
					case XRated::Quest::State::CompletedAndDropWaiting:
					case XRated::Quest::State::FailedAndDropWaiting:
						return oldParam;
						break;
					default:
					{
						uint8 oldState = i->CurrentState;
						i->CurrentState = i->Info->UpdateParameter(L""/*should be "script"?*/, 0, 0, false, i->Params);
						if (oldState != i->CurrentState)
						{
							Protocol::FromServer::Quest::ChangeState response;
							response.QuestHash = questHash;
							response.NewState = i->CurrentState;
							if (i->CurrentState == XRated::Quest::State::Completed)
							{
								response.CompletedCount = completedQuests[questHash];
							}
							else
							{
								response.CompletedCount = 0;
							}

							user->Send(response);

							ChangeActiveQuestState(questHash, i->CurrentState);

							i->ParameterChanged = true;
						}

						return oldParam;
					}
					break;
					}
				}


				LoggerInstance().Error("invalid quest({}) or parameter({}) requested", questHash, paramIndex);
				return oldParam;
			}



			void QuestManager::Dispatch(UserSharedPtr user, Protocol::ToServer::Quest::Accept& packet)
			{
				AcceptQuest(user, packet.QuestHash, packet.SelectRewardIndex, true);

				user->AcceptedQuest(packet.QuestHash);
			}
			void QuestManager::Dispatch(UserSharedPtr user, Protocol::ToServer::Quest::SharedAnswer& packet)
			{
				AcceptQuest(user, packet.QuestHash, packet.SelectRewardIndex, false);

				user->AcceptedQuest(packet.QuestHash);
			}
			void QuestManager::AcceptQuest(UserSharedPtr user, uint32 questHash, uint32 SelectRewardIndex, bool checkLocation)
			{
				AutoLock userLock(user->GetSyncObject());

				const Database::Info::QuestInfo* info(Database::DatabaseInstance().InfoCollections.Quests.Retrieve(questHash));

				try {
					do // dummy do to validate quest accept request - make sure 'break' for CRITICAL invalid tag like something client can validate first
					{
						AutoLock lock(cs);

						DateTime now = DateTime::Now();
						if (dbRequested.find(user) != dbRequested.end()) return; // already doing something( accept or complete
						if (!user->GetPlayer()) return; // invalid character state
						if (!info)
						{
							LoggerInstance().Error("invalid quest({}) accept info==NULL", questHash);
							// unable to find the quest information on server
							throw QuestCriticalException(fmt::format(L"quest({}) info==NULL", questHash).c_str());
						}

						if (info->IsEventQuest() && !UserManagerInstance().IsEventQuestPeriod(info->GetEventType()))
						{
							LoggerInstance().Error("not event period quest({}) accept", questHash);
							throw QuestException(fmt::format(L"not event period quest"));
						}

						if (checkLocation == true) {
							if (info->AcceptLocation.StageGroupHash && info->AcceptLocation != user->GetCurrentStage() && !IsItemActivityQuest(info)) // check accept location
							{
								LoggerInstance().Error("invalid quest({}) accept location({},{}) - user location({},{})", questHash
									, info->AcceptLocation.StageGroupHash, info->AcceptLocation.Level
									, user->GetCurrentStage().StageGroupHash, user->GetCurrentStage().Level);
									return; // could be invalid quest data set
							}
							/* TODO : client side npc validation - currently, only the condition works in a stage */
							if (info->AcceptLocation.StageGroupHash && info->AcceptSourceHash && !(user->GetRoom()->GetCurrentStageGroupInfo()->GameType & XRated::Constants::PeacefulGameTypeMask) && !IsItemActivityQuest(info))
							{
								float distance = user->GetRoom()->GetObjectDistance(user, info->AcceptSourceHash);
								if (distance < 0/*not exist*/ || distance>240)
								{
									/*std::string message = fmt::format("invalid quest({}) target({}) accept distance({})", questHash, info->AcceptSourceHash, distance);
									Logger().Write(IAdmLogger::NormalLogger, "critical", user->GetName(), message.c_str();
									ALLM_ERROR((message.c_str());*/
									return; // just cancel the command
								}
							}
						}
						std::vector<Quest>::iterator iter = std::find_if(workingQuests.begin(), workingQuests.end(), XRated::Quest::FindByHash(questHash));
						if (iter != workingQuests.end()) {
							if (info->IsHaveResetDate() == false) {
								LoggerInstance().Error("invalid quest({}) accept workingQuest : have working quest list(this quest is not ReSetQuest)", questHash);

								if (IsDuplicateQuestList(workingQuests) == true) {
									LoggerInstance().Error("this user have Duplicated WorkingQuestList : {} [IsHaveResetDate Wrong]", user->GetSerial());
								}
								WriteQuestInfo(*iter);
								user->CriticalError(fmt::format("invalid quest({})-accept requested, already have quest", questHash).c_str(), false);
								return;

								//break; // a quest cannot work concurrently.					
							}
							if (((XRated::Quest::State::CompletedAndDropWaiting == iter->CurrentState) || (XRated::Quest::State::FailedAndDropWaiting == iter->CurrentState)) == false) {
								LoggerInstance().Error("invalid quest({}) accept workingQuest: have working quest list, current state is not accept ready : state[{}](this quest is ReSetQuest)", questHash, iter->CurrentState);
								if (IsDuplicateQuestList(workingQuests) == true) {
									LoggerInstance().Error("this user have Duplicated WorkingQuestList :{} [ReSetQuestState Wrong]", user->GetSerial());
								}
								WriteQuestInfo(*iter);

								user->CriticalError(fmt::format("invalid quest({})-accept requested, already have quest", questHash).c_str(), false);
								return;
							}
							if ((now.GetDate() <= iter->ExpiredDate)) {
								now.Add(DateTime::Unit::Second, 300);
								if ((now.GetDate() <= iter->ExpiredDate)) {
									LoggerInstance().Error("invalid quest({}) accept workingQuest: have working quest list, this quest is not reset ready:(this quest is ReSetQuest)", questHash);
									if (IsDuplicateQuestList(workingQuests) == true) {
										LoggerInstance().Error("this user have Duplicated WorkingQuestList : {} [ReSetQuestExpired Wrong]", user->GetSerial());
									}
									WriteQuestInfo(*iter);

									// a quest cannot work concurrently.
									throw QuestCriticalException(L"have working quest list, this quest is not reset ready");
								}
								LoggerInstance().Warn(L"Quest request accept to fast(reset quest) : {}, {}", user->GetSerial(), questHash);
							}

							workingQuests.erase(iter);
						}

						//if ( std::find_if(workingQuests.begin(), workingQuests.end(), XRated::Quest::FindByHash(questHash))!=workingQuests.end() ) 
						//{
						//	LoggerInstance().Error("invalid quest({}) accept workingQuest", questHash);
						//	break; // a quest cannot work concurrently.
						//}
						if (GetValidQuestCount() >= XRated::Quest::MaxCount)
						{
							LoggerInstance().Error("invalid quest({}) accept out of range", questHash);
							// out of range
							throw QuestCriticalException(fmt::format(L"accept out of range"));
						}
						{
							std::map<uint32, uint32>::const_iterator iter = completedQuests.find(questHash);
							if ((info->IsRepeatQuest() == false) && (iter != completedQuests.end()) && (iter->second > 0))
							{
								LoggerInstance().Error("invalid quest({}) accept already complete", questHash);
								// an unrepeatable quest is requested again.
								throw QuestCriticalException(fmt::format(L"accept already complete"));
							}
						}
						if (info->IsRepeatQuest() == true && IsRepeatableQuest(*info, questHash) == false)
						{
							LoggerInstance().Error("invalid quest({}) not repeatable quest - completeCount({}) repeatableCount({})", questHash
								, completedQuests.find(questHash)->second, info->MaximumCompleteableCount);

							// over complete count quest is requested again.
							throw QuestCriticalException(fmt::format(L"not repeatable quest - completeCount({}) repeatableCount({})", completedQuests.find(questHash)->second, info->MaximumCompleteableCount));
						}
						if (info->AcceptCondition.IsGuildQuest() == true) {
							if ((user->IsPartOfGuild() == false)) {
								if (user->GetRequestedInitGuildInfo() == true) {
									LoggerInstance().Error("Not Recved Init GuildInfo From DB");
								}
								user->CriticalError(fmt::format("AcceptCondition::Quest::IsValid Failed, not joined guild user - character({})", StringUtil::To<std::string>(user->GetName()).c_str()).c_str(), false);
								return;
							}
							if (user->GetGuildInfo().GuildLevel < info->AcceptCondition.GuildLevel) {
								user->CriticalError(fmt::format("AcceptCondition::Quest::IsValid Failed, invalid guildLevel condition - character({})", StringUtil::To<std::string>(user->GetName()).c_str()).c_str(), false);
								return;
							}
						}
						ValidQuestConditionResult result = GetValidResult(user, info->AcceptCondition, true);
						if (result != ValidQuestConditionResult::Success) {
							LoggerInstance().Error("invalid quest({}) accept quest condition", questHash);

							const wchar_t* errorMsg = GetValidQuestConditionErrorMsg(result);
							if (errorMsg != NULL) {
								throw QuestCriticalException(fmt::format(L"Vaild : {}", errorMsg));
							}
							throw QuestCriticalException(fmt::format(L"Vaild : Unknown"));
						}
						//edit by kds218
						if (ValidateInventory(user, info->AcceptReward, SelectRewardIndex, questHash, 1) == false) return; // invalid inventory states

						DateTime::Date expiredDate(info->GetExpiredDate(now).GetDate());

						if (GiveReward(user, info->AcceptReward, SelectRewardIndex, 1) == false)
						{
							/* just return, because this is not that serious. client could not check the item slot validation */
							return;
						}
						workingQuests.push_back(Quest(questHash, info, expiredDate, true));

						/* notice the success of quest accept */
						Protocol::FromServer::Quest::Accept response;
						response.QuestHash = questHash;
						response.NewState = XRated::Quest::State::Working;
						response.ExpiredDate = expiredDate;
						user->Send(response);

						ChangeActiveQuestState(questHash, XRated::Quest::State::Working);

						/*Logger().Write(IAdmLogger::ImportantLogger, "questaccept", user->GetName(), Http::Logger::Parameter()
							<< questHash << user->GetLevel();*/

						return;

					} while (false);
				}
				catch (QuestException& e) {
					user->CriticalError(fmt::format("invalid accept quest({})  : {} {}", questHash, e.what()).c_str(), e.IsCritical());
					return;
				}
				//default...
				user->CriticalError(fmt::format("invalid quest({})-accept requested", questHash).c_str(), true);
			}

			void QuestManager::Dispatch(UserSharedPtr user, Protocol::ToServer::Quest::Complete& packet)
			{
				AutoLock userLock(user->GetSyncObject());
				const Database::Info::QuestInfo* info(Database::DatabaseInstance().InfoCollections.Quests.Retrieve(packet.QuestHash));

				try {
					do // dummy do to validate quest accept request - make sure 'break' for CRITICAL invalid tag like something client can validate first
					{
						{
							AutoLock lock(cs);

							if (dbRequested.find(user) != dbRequested.end()) return; // already doing something( accept or complete
							if (user->GetPlayer() == NULL) return; // invalid character state
							if (info == NULL) {
								LoggerInstance().Error("invalid quest({}) complete info==NULL", packet.QuestHash);
								throw QuestCriticalException(fmt::format(L"complete info==NULL"));
								// unable to find the quest information on server					
							}

							if (info->IsEventQuest() && !UserManagerInstance().IsEventQuestPeriod(info->GetEventType()))
							{
								LoggerInstance().Error("not event period quest({}) complete", packet.QuestHash);
								throw QuestException(fmt::format(L"not event period complete quest"));
							}

							std::vector<Quest>::iterator quest = std::find_if(workingQuests.begin(), workingQuests.end(), XRated::Quest::FindByHash(packet.QuestHash));
							if (quest == workingQuests.end()) {
								LoggerInstance().Error("invalid quest({}) not accepted quest", packet.QuestHash);
								user->CriticalError(fmt::format("invalid quest({})-complete requested, not exsist quest", packet.QuestHash).c_str(), false);
								return;
							}
							else if ((info->IsHaveResetDate() == true)) {
								if ((quest->CurrentState == XRated::Quest::State::CompletedAndDropWaiting) || (quest->CurrentState == XRated::Quest::State::FailedAndDropWaiting)) {
									user->CriticalError(fmt::format("invalid quest({})-complete requested, complete or failed and drop waiting quest", packet.QuestHash).c_str(), false);
									return;
								}
							}
							if (IsDuplicateQuestList(workingQuests, packet.QuestHash) == true) {
								LoggerInstance().Error("this user have Duplicated WorkingQuestList user : {} [CompleteQuestChecker]", user->GetSerial());
								user->CriticalError(fmt::format("Dulpicated WorkingQuestList - Request Complete Quest").c_str(), true);
								return;
							}

							//if ( !user->IsAdmitable( 6500 ) ) { //FloodCheck. Quest has a high factor
							if (!user->IsAdmitable(500)) { //FloodCheck. Quest has a high factor
								user->CriticalError(fmt::format("Packet Flood - Quest::Complete [{}/{}]", packet.QuestHash, packet.SelectRewardIndex).c_str(), true);
								return;
							}

							/* TODO : client side npc validation - currently, only the condition works in a stage */
							if (info->AcceptLocation.StageGroupHash && info->CompleteTarget.empty() == false && !(user->GetRoom()->GetCurrentStageGroupInfo()->GameType & XRated::Constants::PeacefulGameTypeMask))
							{
								float distance(-1);
								for (std::vector<uint32>::const_iterator i = info->CompleteTarget.begin(), end = info->CompleteTarget.end(); i != end; ++i)
								{
									distance = user->GetRoom()->GetObjectDistance(user, *i);
									if (distance > 0 && distance < 240) { distance = 0; break; } // right npc or character
									distance = -1;
								}
								if (distance < 0)
								{
									std::string message = fmt::format("valid npc target({} or more) not exist(distance:{})", info->CompleteTarget[0], distance);
									/*Logger().Write(IAdmLogger::NormalLogger, "critical", user->GetName(), message.c_str());*/
									LoggerInstance().Error(message.c_str());

									Protocol::FromServer::Quest::Error response;
									response.QuestHash = packet.QuestHash;
									response.Result = XRated::QuestErrorType::FarDistance;
									user->Send(response);

									return; // just cancel the command
								}
							}
							/* validate complete objectives */
							if (quest->Info->UpdateParameter(L"", 0, 0, false, quest->Params) != Quest::State::Succeeded &&
								quest->CurrentState != XRated::Quest::State::Succeeded) {

								if (IsDuplicateQuestList(workingQuests) == true) {
									LoggerInstance().Error("this user have Duplicated WorkingQuestList user : {} [CompleteQuest::UpdateParameter Wrong]", user->GetSerial());
								}
								LoggerInstance().Error("validate quest({}) condition Failed", packet.QuestHash);
								WriteQuestInfo(*quest);
								throw QuestCriticalException(fmt::format(L"validate condition Failed"));
							}

							/* NPC cannot be validate because of client object (quest can be completed by client object which is unknown by server) */

							if (info->CompleteCondition.IsGuildQuest() == true) {
								if ((user->IsPartOfGuild() == false)) {
									if (user->GetRequestedInitGuildInfo() == true) {
										LoggerInstance().Error("Not Recved Init GuildInfo From DB");
									}
									user->CriticalError(fmt::format("CompleteCondition::Quest::IsValid Failed, not joined guild user - character({})", StringUtil::To<std::string>(user->GetName()).c_str()).c_str(), false);
									return;
								}
								if (user->GetGuildInfo().GuildLevel < info->CompleteCondition.GuildLevel) {
									user->CriticalError(fmt::format("CompleteCondition::Quest::IsValid Failed, invalid guildLevel condition - character({})", StringUtil::To<std::string>(user->GetName()).c_str()).c_str(), false);
									return;
								}
							}
							ValidQuestConditionResult result = GetValidResult(user, info->CompleteCondition, true, packet.count);
							if (result != ValidQuestConditionResult::Success) {
								LoggerInstance().Error("invalid quest({}) complete quest condition", packet.QuestHash);
								const wchar_t* errorMsg = GetValidQuestConditionErrorMsg(result);
								if (errorMsg != NULL) {
									throw QuestCriticalException(fmt::format(L"Vaild : {}", errorMsg));
								}
								throw QuestCriticalException(fmt::format(L"Vaild : Unknown"));
							}
							if (ValidateInventory(user, info->CompleteReward, packet.SelectRewardIndex, packet.QuestHash, packet.count) == false) return; // invalid inventory states

							int deleteWorkingQuest = 1;
							/* finally */
							DateTime::Date now = DateTime::Date::Now();
							if ((info->IsHaveResetDate() == true) && (quest->ExpiredDate >= now)) {
								//' request : name, quest hash, quest current state.
								quest->CurrentState = XRated::Quest::State::CompletedAndDropWaiting;
								//ChangeActiveQuestState(quest->Id, quest->CurrentState);
								deleteWorkingQuest = 0;

							}



							if (GiveReward(user, info->CompleteReward, packet.SelectRewardIndex, packet.count) == false)
							{
								// just return, because this is not that serious. client could not check the item slot validation 
								return;
							}

							std::map<uint32, uint32>::iterator iter = completedQuests.find(packet.QuestHash);
							if (iter == completedQuests.end())
								completedQuests[packet.QuestHash] = packet.count;
							else
								iter->second += packet.count;


							// notice the success of quest complete 
							Protocol::FromServer::Quest::ChangeState response;
							response.QuestHash = packet.QuestHash;
							response.NewState = XRated::Quest::State::Completed;
							response.Parameter = packet.SelectRewardIndex;
							response.CompletedCount = packet.count;


							switch (quest->CurrentState)
							{
							case Quest::State::CompletedAndDropWaiting:
								if (IsRepeatableQuest(*(quest->Info), packet.QuestHash) == false) {
									workingQuests.erase(remove_if(workingQuests.begin(), workingQuests.end(), XRated::Quest::FindByHash(packet.QuestHash)), workingQuests.end()); // should not be workingQuests.end()
								}
								response.NewState = XRated::Quest::State::CompletedAndDropWaiting;
								break;
							default:
								workingQuests.erase(remove_if(workingQuests.begin(), workingQuests.end(), XRated::Quest::FindByHash(packet.QuestHash)), workingQuests.end()); // should not be workingQuests.end()					
								break;
							}


							user->Send(response);
							ChangeActiveQuestState(packet.QuestHash, XRated::Quest::State::Completed);
							//if (user->achievement.IsConnected()) {
							//	Net::Protocol::Achievement::ServerProtocol::QuestCompleted packet;
							//	packet.characterName = user->GetName();
							//	packet.count = response.CompletedCount;
							//	packet.questHash = response.QuestHash;
							//	packet.questType = quest->Info->GetEventType(); // dunno what this is yet;
							//	DateTime::Date date;
							//	packet.date = date.Now().ToString();
							//	user->achievement.Send(packet);
							//}
							//Logger().Write(IAdmLogger::ImportantLogger, "questcomplete", user->GetName(), Http::Logger::Parameter()
							//	<< packet.QuestHash << packet.SelectRewardIndex << user->GetLevel();
						}
						user->CompletedQuest(packet.QuestHash);
						return;
					} while (false);
				}
				catch (QuestException& e) {
					user->CriticalError(fmt::format("invalid complete quest({})  : {}", packet.QuestHash, e.what()).c_str(), e.IsCritical());
					return;
				}

				user->CriticalError(fmt::format("invalid quest({})-complete requested", packet.QuestHash).c_str(), true);
			}

			void QuestManager::Dispatch(UserSharedPtr user, Protocol::ToServer::Quest::Drop& packet)
			{
				AutoLock userLock(user->GetSyncObject());
				AutoLock lock(cs);

				if (DropQuest(user, packet.QuestHash) == true) {
					/*Logger().Write(IAdmLogger::ImportantLogger, "questdrop", user->GetName(), Http::Logger::Parameter()
						<< packet.QuestHash);*/
				}
				else {
					/*Logger().Write(IAdmLogger::ImportantLogger, "questdropfailed", user->GetName(), Http::Logger::Parameter()
						<< packet.QuestHash);*/
				}

			}
			void QuestManager::Dispatch(UserSharedPtr user, Protocol::ToServer::Quest::ShareToPartyPlayer& packet)
			{
				const Database::Info::QuestInfo* info = NULL;
				{
					AutoLock userLock(user->GetSyncObject());
					AutoLock lock(cs);
					try {
						info = XRated::Database::DatabaseInstance().InfoCollections.Quests.Retrieve(packet.QuestHash);
						if (!info) {
							throw QuestCriticalException(fmt::format(L"can not found quest info"));
						}
						std::vector<Quest>::iterator iter = std::find_if(workingQuests.begin(), workingQuests.end(), Quest::FindByHash(packet.QuestHash));
						if (iter == workingQuests.end()) {
							throw QuestCriticalException(fmt::format(L"not exist workingQuests"));
						}
						if (iter->Info->IsShareQuest() == false) {
							throw QuestCriticalException(fmt::format(L"Imposible share to quest"));
						}
					}
					catch (QuestException& e) {
						user->CriticalError(fmt::format("Wrong ShareToPartyPlayer, Quest({}) : {}", packet.QuestHash, e.what()).c_str(), e.IsCritical());
						return;
					}
				}
				user->ShareQuestToParty(*info, packet.QuestHash);

			}

			bool QuestManager::DropQuest(UserSharedPtr user, uint32 questHash)
			{
				/* drop the quest */
				std::vector<Quest>::iterator i = std::find_if(workingQuests.begin(), workingQuests.end(), XRated::Quest::FindByHash(questHash));
				if (i == workingQuests.end())
				{
					/* invalid quest to drop */
					return false;
				}

				const Database::Info::QuestInfo* info(Database::DatabaseInstance().InfoCollections.Quests.Retrieve(questHash));
				if (info == NULL) {
					return false;
				}
				DateTime::Date now(DateTime::Date::Now());
				if ((info->IsHaveResetDate() == true) && (i->ExpiredDate >= now)) {
					//' request : name, quest hash, quest current state.
					i->CurrentState = XRated::Quest::State::FailedAndDropWaiting;

					Protocol::FromServer::Quest::ChangeState response;
					response.QuestHash = i->Id;
					response.NewState = i->CurrentState;
					response.CompletedCount = 0;


					user->Send(response);
				}
				else {
					workingQuests.erase(i);

					/* notice to user */
					Protocol::FromServer::Quest::Drop response;
					response.QuestHash = questHash;
					user->Send(response);
				}
				ChangeActiveQuestState(questHash, XRated::Quest::State::NotAccepted);
				return true;
			}


			void QuestManager::DropGuildQuest(UserSharedPtr user)
			{
				std::vector<Quest>::iterator i = workingQuests.begin();
				for (; i != workingQuests.end(); ) {
					if ((*i).Info->IsGuildQuest()) {
						DateTime::Date now(DateTime::Date::Now());
						uint32 questHash = (*i).Id;
						LoggerInstance().Info("[QuestManager::DropGuildQuest] Guild quest found. ({})", (int)questHash);
						if (((*i).Info->IsHaveResetDate() == true) && (i->ExpiredDate >= now)) {
							i->CurrentState = XRated::Quest::State::FailedAndDropWaiting;

							Protocol::FromServer::Quest::ChangeState response;
							response.QuestHash = i->Id;
							response.NewState = i->CurrentState;
							response.CompletedCount = 0;
							user->Send(response);
							++i;
							ChangeActiveQuestState(questHash, XRated::Quest::State::NotAccepted);
							LoggerInstance().Info("[QuestManager::DropGuildQuest] Guild quest Set to FailedAndDropWaiting. ({})", (int)questHash);
						}
						else {
							i = workingQuests.erase(i);

							/* notice to user */
							Protocol::FromServer::Quest::Drop response;
							response.QuestHash = questHash;
							user->Send(response);

							LoggerInstance().Info("[QuestManager::DropGuildQuest] Guild quest Removed. ({})", (int)questHash);
						}
					}
					else {
						++i;
					}
				}
			}


			void QuestManager::RefreshCompletedQuests(UserSharedPtr user, const StageLocation& location)
			{
				//' request  : character name, stage group hash, stage level
				Net::Api request("Quest/CompletedList");
				request << user->GetSerial() << location.StageGroupHash << location.Level;
				request.GetAsync(this, &QuestManager::CompletedQuestList, user);
			}

			void QuestManager::RefreshWorkingQuests(UserSharedPtr user)
			{
				//' request  : character name		
				Net::Api request("Quest/WorkingList");
				request << user->GetSerial();
				request.GetAsync(this, &QuestManager::WorkingQuestList, user);
			}

			void QuestManager::Release(UserSharedPtr user)
			{
				AutoLock lock(cs);

				completedQuests.clear();

				// origin completedQuests Count backup table clear. 2008-02-19 by kwind.
				originCompletedQuests.clear();
				//insertCompletedQuests.clear();
				//updateCompletedQuests.clear();

				/* need to update runtime-parameters */
				workingQuests.clear();
				itemQuests.clear();

				// origin workingQuests backup table clear. 2008-02-19 by kwind.
				originWorkingQuests.clear();

				//insertWorkingQuests.clear();
				//updateWorkingQuests.clear();
				//dropWorkingQuests.clear();
			}

			void QuestManager::ObjectDestroyed(UserSharedPtr user, uint32 id, uint8 team)
			{
				AutoLock lock(cs);
				for (std::vector<Quest>::iterator i = std::find_if(workingQuests.begin(), workingQuests.end(), XRated::Quest::FindByState(XRated::Quest::State::Working))
					; i != workingQuests.end()
					; i = std::find_if(i + 1, workingQuests.end(), XRated::Quest::FindByState(XRated::Quest::State::Working)))
				{
					uint8 state = i->Info->UpdateParameter(L"DestroyObject", id, 1/*count*/, (user->GetTeamNumber() == team), i->Params);
					i->ParameterChanged = true;

					if (state == i->CurrentState) continue;

					i->CurrentState = state;
					switch (i->CurrentState)
					{
					case XRated::Quest::State::Failed:
					case XRated::Quest::State::Succeeded:
						if (i->CurrentState == XRated::Quest::State::Succeeded && GetValidResult(user, i->Info->CompleteCondition, false) != ValidQuestConditionResult::Success) // working ; invalid condition
							break;

						Protocol::FromServer::Quest::ChangeState packet;
						packet.QuestHash = i->Id;
						packet.NewState = i->CurrentState;

						if (i->CurrentState == XRated::Quest::State::Completed)
						{
							packet.CompletedCount = completedQuests[i->Id];
						}
						else
						{
							packet.CompletedCount = 0;
						}

						user->Send(packet);

						ChangeActiveQuestState(i->Id, i->CurrentState);
						break;
					}
				}
			}

			void QuestManager::CompletedQuestList(const UserSharedPtr& user, const Net::Answer& answer)
			{
				if (!user->IsConnected()) return;

				//' response : quest hashes
				if (answer)
				{
					/* completed quests which is related for this stage are listed like, previous quests of relay quest in this stage. */
					AutoLock lock(cs);

					completedQuests.clear();
					uint16 i(0);
					for(auto& quest : answer.at("quests").get<json>())
					{
						uint32 questHash = quest.at("id").get<uint32>();
						if (!Database::DatabaseInstance().InfoCollections.Quests.IsExistQuest(questHash))
						{
							LoggerInstance().Error("CompletedQuestList, Not Exist QuestInfo : {}", questHash);
							++i;

							Net::Api request("DeleteQuest");
							request << user->GetSerial() << questHash;
							request.GetAsync();
							continue;
						}
						else
						{
							const Database::Info::QuestInfo* questInfo = Database::DatabaseInstance().InfoCollections.Quests.Retrieve(questHash);
							if (questInfo)
							{
								if (questInfo->IsEventQuest() && !UserManagerInstance().IsEventQuestPeriod(questInfo->GetEventType()))
								{
									LoggerInstance().Error("CompletedQuestList, Not Period EventQuestInfo : {}", questHash);
									++i;
									Net::Api request("DeleteQuest");
									request << user->GetSerial() << questHash;
									request.GetAsync();
									continue;
								}

							}
						}

						uint32 completeCount = quest.at("completeCount").get<uint32>();
						completedQuests[questHash] = completeCount;
					}

					// origin list backup.
					originCompletedQuests = completedQuests;
					//insertCompletedQuests.clear();
					//updateCompletedQuests.clear();
					dbRequestCompletCount.clear();

					Protocol::FromServer::Quest::CompletedList response;
					response.Quests = completedQuests;
					user->Send(response);
				}
				else
				{
					//user->CriticalError(fmt::format("CompletedQuestList Error DB Response Quest List").c_str());
					return;
				}
			}

			void QuestManager::WorkingQuestList(const UserSharedPtr& user, const Net::Answer& answer)
			{
				if (!user->IsConnected()) return;

				//' response : quests(separated : quest hash, state, param1, param2, param3)
				if (answer)
				{
					AutoLock lock(cs);

					bool sendQuestEventNotice = false;
					workingQuests.clear();
					uint16 i(0);
					//assert( packet->GetCount() % (1/*hash*/ + 1/*state*/ + XRated::Quest::MaxParameterCount) == 0 ); /// .asp should be updated if this fails

					originWorkingQuests.clear();
					std::map< uint32, Quest > originWorkingQuestFinder;
					//std::map< uint32 , Quest > duplicatedList;

					DateTime::Date now = DateTime::Date::Now();


					try {
						for(auto& questdb : answer.at("quests").get<json>())
						{
							uint32 questHash = questdb.at("id").get<uint32>();
							if (!Database::DatabaseInstance().InfoCollections.Quests.IsExistQuest(questHash))
							{
								LoggerInstance().Error("WorkingQuest, Not Exist QuestInfo : {}", questHash);
								i += 5; 

								Net::Api request("DeleteQuest");
								request << user->GetSerial() << questHash;
								request.GetAsync();
								continue;
							}

							originWorkingQuests.push_back(Quest(questHash));

							Quest& quest(originWorkingQuests.back());

							//workingQuests.push_back( Quest(StringUtil::ToInt( (*packet)[i++])) );

							//Quest& quest(workingQuests.back();
							questdb.at("currentState").get_to(quest.CurrentState);
							for (std::vector<uint32>::iterator j = quest.Params.begin(); j != quest.Params.end(); ++j)
							{
								questdb.at("params").get_to(*j);
							}
							// fixed by kwind, 2008-01-22
							questdb.at("expiredDate").get_to(quest.ExpiredDate);

							std::map< uint32, Quest >::iterator finder = originWorkingQuestFinder.find(questHash);
							if (finder != originWorkingQuestFinder.end())
							{
								duplicatedList.insert(std::make_pair(finder->first, finder->second));
								originWorkingQuests.pop_back();

								std::list<Quest>::iterator originIter = std::find_if(originWorkingQuests.begin(), originWorkingQuests.end(), XRated::Quest::FindByHash(finder->first));
								if (originIter != originWorkingQuests.end())
								{
									originWorkingQuests.erase(originIter);
								}

								LoggerInstance().Warn("asdf originWorkingQuests delete questHash={} because duplicated!!", questHash);

								continue;
							}

							originWorkingQuestFinder.insert(std::make_pair(questHash, quest));

							if ((XRated::Quest::State::CompletedAndDropWaiting == quest.CurrentState) || (XRated::Quest::State::FailedAndDropWaiting == quest.CurrentState)) {
								if (quest.Info->IsHaveResetDate() == true) {
									if ((now > quest.ExpiredDate)) {
										continue;
									}
								}
							}

							if (quest.Info->IsEventQuest() && !UserManagerInstance().IsEventQuestPeriod(quest.Info->GetEventType()))
							{
								sendQuestEventNotice = true;
								continue;
							}

							workingQuests.push_back(quest);
						}
					}
					catch (QuestException& e) {
						LoggerInstance().Error("WorkingQuest, CriticalError : {}", e.what());
						originWorkingQuests.clear();
						workingQuests.clear();
						user->CriticalError("SetWorkingQuestList Failed", e.IsCritical());
						return;
					}

					//user->RemoveDuplicatedQuest( duplicatedList );

					//originWorkingQuests.assign( workingQuests.begin(), workingQuests.end() );
					//insertWorkingQuests.clear();
					//updateWorkingQuests.clear();
					//dropWorkingQuests.clear();			

					Protocol::FromServer::Quest::WorkingList response;
					for (std::vector<Quest>::const_iterator i = workingQuests.begin(); i != workingQuests.end(); ++i)
					{
						response.Quests.push_back(*i);
					}

					user->Send(response);

					if (sendQuestEventNotice)
					{
						Protocol::FromServer::QuestEvent::NoticeQuestEvent sendPacket;
						sendPacket.EventNotice = Protocol::FromServer::QuestEvent::NoticeQuestEvent::Notice::DeleteWorkingQuestBecauseNotEventPeriod;
						user->Send(sendPacket);
					}
				}
				else
				{
					user->CriticalError(fmt::format("WorkingQuestList Error DB Response Quest List").c_str());
					return;
				}
			}

			bool QuestManager::IsRepeatableQuest(const Database::Info::QuestInfo& info, uint32 questHash)
			{
				std::map<uint32/*Quest hash*/, uint32/*Completed Count*/>::const_iterator iter = completedQuests.find(questHash);
				if (iter != completedQuests.end()) {
					return info.IsRepeatQuest(iter->second);
				}


				return true;
			}

			void QuestManager::ShareQuest(UserSharedPtr user, const std::wstring& shareOwner, const Database::Info::QuestInfo& info, uint32 questHash)
			{
				AutoLock lock(cs);
				do {
					std::map<uint32/*Quest hash*/, uint32/*Completed Count*/>::const_iterator iter = completedQuests.find(questHash);
					if (iter == completedQuests.end()) {
						if (info.AcceptLocation != user->GetCurrentStage()) {
							if (info.Next.second != nullptr) {
								if ((info.Next.second->AcceptLocation) != user->GetCurrentStage()) {
									break;
								}
							}
							else {
								break;
							}
						}
					}
					Protocol::FromServer::Quest::Share packet;
					packet.ShareOwner = shareOwner;
					packet.QuestHash = questHash;
					user->Send(packet);
					return;
				} while (false);
				std::vector< std::pair<uint32, std::wstring> >::iterator iter = dbRequestCompletCount.begin();
				std::vector< std::pair<uint32, std::wstring> >::iterator end = dbRequestCompletCount.end();
				while (iter != end) {
					if (iter->first == questHash) {
						return;
					}
					++iter;
				}

				dbRequestCompletCount.push_back(std::pair<uint32, std::wstring>(questHash, shareOwner));
				Net::Api request("Quest/GetCompletedCount");
				request << user->GetSerial() << questHash;
				request.GetAsync(this, &QuestManager::CompletedQuestCountWIthShare, user);
			}
			void QuestManager::CompletedQuestCountWIthShare(const UserSharedPtr& user, const Net::Answer& answer)
			{
				if (answer && user->IsConnected())
				{
					uint32 questHash = answer.at("hash").get<uint32>();
					uint32 completeCount = answer.at("completeCount").get<uint32>();
					const Database::Info::QuestInfo* info(Database::DatabaseInstance().InfoCollections.Quests.Retrieve(questHash));
					if (!info) {
						LoggerInstance().Error("can not found quest info :{}", questHash);
						return;
					}
					{
						AutoLock lock(cs);
						{
							std::map<uint32, uint32>::iterator iter = completedQuests.find(questHash);
							if (iter != completedQuests.end()) {
								LoggerInstance().Error("QuestShare Failed : have completed(current list) count : Quest({})", questHash);
								return;
							}
						}
						{
							std::map<uint32, uint32>::iterator iter = originCompletedQuests.find(questHash);
							if (iter != originCompletedQuests.end()) {
								LoggerInstance().Error("QuestShare Failed : have completed(origin list) count : Quest({})", questHash);
								return;
							}
						}
						completedQuests[questHash] = completeCount;
						originCompletedQuests[questHash] = completeCount;

						{
							if (completeCount > 0) {
								Protocol::FromServer::Quest::CompletedCount packet;
								packet.QuestHash = questHash;
								packet.Count = completeCount;
								user->Send(packet);
							}
						}


						{
							std::vector< std::pair<uint32, std::wstring> >::iterator iter = dbRequestCompletCount.begin();
							std::vector< std::pair<uint32, std::wstring> >::iterator end = dbRequestCompletCount.end();
							while (iter != end) {
								if (iter->first == questHash) {

									Protocol::FromServer::Quest::Share packet;
									packet.ShareOwner = iter->second;
									packet.QuestHash = questHash;
									user->Send(packet);

									dbRequestCompletCount.erase(iter);
									break;
								}
								++iter;
							}
						}
					}
				}
			}


			bool QuestManager::IsItemActivityQuest(const Database::Info::QuestInfo* info)
			{
				if (!info) return false;

				if (info->AcceptLocation.StageGroupHash == XRated::Constants::HiddenQuestStageGroupHash &&
					info->AcceptLocation.Level == XRated::Constants::HiddenQuestStageGroupLevel)
					return true;

				return false;
			}

			const wchar_t* QuestManager::GetValidQuestConditionErrorMsg(ValidQuestConditionResult result) const
			{
				switch (result)
				{
				case Failed_PlayerIsNull:
					return L"Failed_PlayerIsNull";
				case Failed_ScriptOnlyQuest:
					return L"Failed_ScriptOnlyQuest";
				case Failed_NotEnoughCharacterStatus:
					return L"Failed_NotEnoughCharacterStatus";
				case Failed_NotJoinedGuild:
					return L"Failed_NotJoinedGuild";
				case Failed_NotEnoughGuildLevel:
					return L"Failed_NotEnoughGuildLevel";
				case Failed_NotEnoughItems:
					return L"Failed_NotEnoughItems";
				case Failed_HaveNotLicenses:
					return L"Failed_HaveNotLicenses";
				case Failed_HaveNotCompleteQuests:
					return L"Failed_HaveNotCompleteQuests";
				case Success:
					return L"Success";
				}
				return L"Failed_Unknown";
			}

			QuestManager::ValidQuestConditionResult QuestManager::GetValidResult(const UserSharedPtr user, const Database::Info::QuestInfo::Condition& condition, bool wirteLog, uint32 count) const
			{
				Logic::Player* player(user->GetPlayer());
				if (player == NULL)
				{
					LoggerInstance().Error("invalid character({}) state for a quest", user->GetSerial());
					return ValidQuestConditionResult::Failed_PlayerIsNull;
				}

				/* state can be changed by only script */
				if (condition.ScriptOnly)
				{
					if (wirteLog == true) {
						LoggerInstance().Error("Quest::IsValid Failed, only for script - character({})", user->GetSerial());

					}
					return ValidQuestConditionResult::Failed_ScriptOnlyQuest;
				}

				/* character validation */
				if (condition.IsValid(player->GetPlayerData()) == false)
				{
					if (wirteLog == true) {
						LoggerInstance().Error("Quest::IsValid Failed, not enough character status- character({})", user->GetSerial());

					}
					return ValidQuestConditionResult::Failed_NotEnoughCharacterStatus;
				}
				/* guild quest validateion */
				if (condition.IsGuildQuest() == true) {
					if ((user->IsPartOfGuild() == false)) {
						if (wirteLog == true) {
							LoggerInstance().Error("Quest::IsValid Failed, not joined guild user - character({})", user->GetSerial());

						}
						return ValidQuestConditionResult::Failed_NotJoinedGuild;
					}
					if (user->GetGuildInfo().GuildLevel < condition.GuildLevel) {
						if (wirteLog == true) {
							LoggerInstance().Error("Quest::IsValid Failed, invalid guildLevel condition - character({})", user->GetSerial());

						}
						return ValidQuestConditionResult::Failed_NotEnoughGuildLevel;
					}
				}

				/* item validation */
				if (!condition.Items.empty())
				{
					struct Condition : public Database::Info::ItemInfo::IFindCondition
					{
						uint32 Hash;
						Condition(uint32 hash) : Hash(hash) {}
						bool operator()(const Database::Info::ItemInfo& info) const { return (info.Hash == Hash); }
					};

					int availablequestcount = 0;

					std::vector< Database::Info::QuestInfo::Condition::Item >::const_iterator i;
					for (i = condition.Items.begin(); i != condition.Items.end(); ++i)
					{
						int itemcount = user->GetItemCount(Condition(i->ItemHash), true);
						int temp = itemcount / i->Count;

						if (availablequestcount == 0)
						{
							availablequestcount = temp;
						}
						else if (temp < availablequestcount)
						{
							availablequestcount = temp;
						}

						if (itemcount < i->Count)
						{
							if (wirteLog == true) {
								LoggerInstance().Error("not enough items({} of {} required) of a user({})", i->ItemHash, i->Count, user->GetSerial());

							}
							else {
								LoggerInstance().Info("not enough items({} of {} required) of a user({})", i->ItemHash, i->Count, user->GetSerial());
							}
							return ValidQuestConditionResult::Failed_NotEnoughItems;
						}
					}
					if (availablequestcount < count)
					{
						if (wirteLog == true)
						{
							LoggerInstance().Error("not enough items({} of {} required) of a user({})", i->ItemHash, i->Count, user->GetSerial());

						}
						else
						{
							LoggerInstance().Info("not enough items({} of {} required) of a user({})", i->ItemHash, i->Count, user->GetSerial());
						}
						return ValidQuestConditionResult::Failed_NotEnoughItems;
					}

				}

				/* license validation */
				if (!condition.Licenses.empty())
				{
					for (std::vector< StageLocation >::const_iterator i = condition.Licenses.begin(); i != condition.Licenses.end(); ++i)
					{
						if (user->IsAbleToJoinStage(*i) == false)
						{
							if (wirteLog == true) {
								LoggerInstance().Error("Quest::IsValid Failed, wrong licenses condition- character({})", user->GetSerial());

							}
							return ValidQuestConditionResult::Failed_HaveNotLicenses;
						}
					}
				}

				/* quest validation */
				if (!condition.Quests.empty())
				{
					for (std::vector< uint32 >::const_iterator i = condition.Quests.begin(); i != condition.Quests.end(); ++i)
					{
						std::map<uint32, uint32>::const_iterator completedIter = completedQuests.find(*i);
						if ((completedIter == completedQuests.end()) || (completedIter->second < 1))
						{
							if (wirteLog == true) {
								LoggerInstance().Error("Quest::IsValid Failed, wrong completedQuests condition- character({})", user->GetSerial());

							}
							return ValidQuestConditionResult::Failed_HaveNotCompleteQuests;
						}
					}
				}

				/* finally */
				return ValidQuestConditionResult::Success;
			}

			bool QuestManager::ValidateInventory(UserSharedPtr user, const Database::Info::QuestInfo::Reward& reward, uint32 selectItemIndex, uint32 questHash, uint32 count) const
			{
				/* calculate reward items */
				typedef std::vector< std::pair<XRated::Item, uint16/*count*/> > RewardItems;
				RewardItems toDrop;
				RewardItems toGet;
				std::vector< std::pair<uint32/*item hash*/, uint32/*item count*/> > vR, vA; // to validate slot count

				if (!reward.SelectableItems.empty())
				{
					if (reward.SelectableItems.size() < selectItemIndex) // out of index range - kind of ciritical
					{
						LoggerInstance().Error("out of select item index({} size:{})", selectItemIndex, reward.SelectableItems.size());
						user->Close(); // critical
						return false;
					}
					RewardItems& target(reward.SelectableItems[selectItemIndex].TakeAway ? toDrop : toGet);
					target.push_back(std::pair(XRated::Item(reward.SelectableItems[selectItemIndex].ItemHash, reward.SelectableItems[selectItemIndex].Instance), reward.SelectableItems[selectItemIndex].Count));

					std::vector< std::pair<uint32, uint32> >& validateInfo(reward.SelectableItems[selectItemIndex].TakeAway ? vR : vA);
					validateInfo.push_back(std::pair<uint32, uint32>(reward.SelectableItems[selectItemIndex].ItemHash, reward.SelectableItems[selectItemIndex].Count));
				}
				for (auto i = reward.Items.begin(); i != reward.Items.end(); ++i)
				{
					RewardItems& target(i->TakeAway ? toDrop : toGet);
					target.push_back(std::pair(XRated::Item(i->ItemHash, i->Instance), i->Count));

					std::vector< std::pair<uint32, uint32> >& validateInfo(i->TakeAway ? vR : vA);
					validateInfo.push_back(std::pair<uint32, uint32>(i->ItemHash, i->Count));
				}

				/* item validation */
				int requiredSlotCount(user->GetRequiredSlotCount(vR, vA, count));
				if (static_cast<int>(user->GetBlankSlotCountOfNormalBag()) < requiredSlotCount) // not enough item slots to put-in
				{
					Protocol::FromServer::Quest::Error packet;
					packet.QuestHash = questHash;
					packet.Result = XRated::QuestErrorType::Type::NotEnoughInventory;
					user->Send(packet);
					return false;
				}
				return true;
			}

			bool QuestManager::GiveReward(UserSharedPtr user, const Database::Info::QuestInfo::Reward& reward, uint32 selectItemIndex, uint32 count) const
			{
				/* money validation */
				if (reward.Money < 0 && user->GetMoney() < (static_cast<uint32>(std::abs(reward.Money))) * count)
				{
					Protocol::FromServer::Quest::Error packet;
					packet.Result = XRated::QuestErrorType::Type::NotEnoughMoney;
					user->Send(packet);
					LoggerInstance().Info(L"not enough money");
					return false; // not enough money
				}
				/* calculate reward items */
				typedef std::vector< std::pair<XRated::Item, uint16/*count*/> > RewardItems;
				RewardItems toDrop;
				RewardItems toGet;
				std::vector< std::pair<uint32/*item hash*/, uint32/*item count*/> > vR, vA; // to validate slot count

				if (!reward.SelectableItems.empty())
				{
					if (reward.SelectableItems.size() < selectItemIndex) // out of index range - kind of ciritical
					{
						LoggerInstance().Error("out of select item index({} size:{})", selectItemIndex, reward.SelectableItems.size());
						user->Close(); // critical
						return false;
					}
					RewardItems& target(reward.SelectableItems[selectItemIndex].TakeAway ? toDrop : toGet);
					target.push_back(std::pair(XRated::Item(reward.SelectableItems[selectItemIndex].ItemHash, reward.SelectableItems[selectItemIndex].Instance), reward.SelectableItems[selectItemIndex].Count));

					std::vector< std::pair<uint32, uint32> >& validateInfo(reward.SelectableItems[selectItemIndex].TakeAway ? vR : vA);
					validateInfo.push_back(std::pair<uint32, uint32>(reward.SelectableItems[selectItemIndex].ItemHash, reward.SelectableItems[selectItemIndex].Count));
				}
				for (std::vector<Database::Info::QuestInfo::Reward::Item>::const_iterator i = reward.Items.begin(); i != reward.Items.end(); ++i)
				{
					RewardItems& target(i->TakeAway ? toDrop : toGet);
					target.push_back(std::pair(XRated::Item(i->ItemHash, i->Instance), i->Count));

					std::vector< std::pair<uint32, uint32> >& validateInfo(i->TakeAway ? vR : vA);
					validateInfo.push_back(std::pair<uint32, uint32>(i->ItemHash, i->Count));
				}

				/* item validation */
				int requiredSlotCount(user->GetRequiredSlotCount(vR, vA, count));
				if (static_cast<int>(user->GetBlankSlotCountOfNormalBag()) < requiredSlotCount) // not enough item slots to put-in
				{
					Protocol::FromServer::Quest::Error packet;
					packet.Result = XRated::QuestErrorType::Type::NotEnoughInventory;
					user->Send(packet);
					return false;
				}

				// Money OverFlow Validation
				if (reward.Money > 0 && (user->GetMoney() > user->GetMoney() + (reward.Money * count) || user->GetMoney() + (reward.Money * count) > XRated::Constants::GoldMaxLimit))
				{
					LoggerInstance().Warn("QUEST REWARD GOLD OVER FLOW!");
					Protocol::FromServer::Quest::Error packet;
					packet.Result = XRated::QuestErrorType::Type::GoldOverflow;
					user->Send(packet);
					return false;
				}
				/* give money */
				user->SetMoney(user->GetMoney() + (reward.Money * count));

				//if (user->achievement.IsConnected()) {
				//	// send
				//	Net::Protocol::Achievement::ServerProtocol::GoldGainChanged packet;
				//	packet.characterName = user->GetName();
				//	packet.checkAchieve = uint8(0);
				//	packet.goldGainType = Net::Protocol::Achievement::ServerProtocol::GoldGainChanged::goldGainType::Mission;
				//	packet.gold = uint64(reward.Money * count);
				//	user->achievement.Send(packet);
				//	// check only
				//	packet.characterName = user->GetName();
				//	packet.checkAchieve = uint8(1);
				//	packet.goldGainType = Net::Protocol::Achievement::ServerProtocol::GoldGainChanged::goldGainType::Mission;
				//	packet.gold = uint64(0);
				//	user->achievement.Send(packet);
				//}

				/* get rid of items */
				if (!toDrop.empty())
				{
					for (RewardItems::const_iterator i = toDrop.begin(); i != toDrop.end(); ++i)
						user->ItemRemove(i->first.Id, i->second * count); // User::ItemRemove() sends packet automatically. instance(like enchant bitfields) is ignored at removing item
				}

				/* give items */
				if (!toGet.empty())
				{

					for (RewardItems::const_iterator i = toGet.begin(); i != toGet.end(); ++i)
					{
						user->ItemFill(i->first.Id, (i->second * count));
						//user->ItemAdd(i->first.Id, 0/*unused*/, i->second); // item instance of reward is ignored, and this method work auto-identify and item raise sending AcquireItem packet

						// arcrus. Log if acquired item is LuniChip.								
						uint32 chipItemCredit = 0;

						if (i->first.Id == XRated::Database::DatabaseInstance().InfoCollections.LuniaChipItems.GetChipHash(XRated::Gamble::ChipType::Pink))
							chipItemCredit = i->second * count;
						else if (i->first.Id == XRated::Database::DatabaseInstance().InfoCollections.LuniaChipItems.GetChipHash(XRated::Gamble::ChipType::Green))
							chipItemCredit = i->second * 100 * count;
						else if (i->first.Id == XRated::Database::DatabaseInstance().InfoCollections.LuniaChipItems.GetChipHash(XRated::Gamble::ChipType::Yellow))
							chipItemCredit = i->second * 10000 * count;

						if (chipItemCredit > 0) // at least 1 chip acquired.
						{
							// Logging : race-buychip
							/*Logger().Write(IAdmLogger::ImportantLogger, "race-buychip", user->GetName(), Http::Logger::Parameter()
								<< chipItemCredit
								<< (reward.Money * count)
							);*/
						}
					}

				}

				/* give licenses */
				if (!reward.Licenses.empty())
				{
					for (std::vector<StageLicense>::const_iterator i = reward.Licenses.begin(); i != reward.Licenses.end(); ++i)
					{
						if (user->AddStageLicense(*i) == true) {
							Protocol::FromServer::AcquireLicense packet;
							packet.serial = user->GetSerial();
							packet.stageLicense = *i;
							packet.sharedOtherPlayers = 0; // 3.1 by ultimate

							user->SendToAll(packet);
							//user->Send(packet);
						}
					}
				}

				if (reward.Exp)
					user->AddExp(XRated::Constants::ExpAcquiredType::ExpQuest, static_cast<uint32>(reward.Exp * ConfigInstance().Get("BaseExp",1.0f) * count), false); /* quest exp reward should not be affected by PC room */

				if (reward.WarExp)
					user->AddWarExp(XRated::Constants::ExpAcquiredType::ExpQuest, static_cast<uint32>(reward.WarExp * ConfigInstance().Get("BaseExp", 1.0f) * count));

				if (reward.StateBundleHash)
					user->AddStateBundle(reward.StateBundleHash);

				/* finally */
				return true;
			}

			uint32 QuestManager::GetValidQuestCount() const
			{
				AutoLock lock(cs);

				uint32 count(0);
				for (std::vector<Quest>::const_iterator i = workingQuests.begin(); i != workingQuests.end(); ++i)
				{
					switch (i->CurrentState)
					{
					case XRated::Quest::State::Completed:
					case XRated::Quest::State::CompletedAndDropWaiting:
					case XRated::Quest::State::FailedAndDropWaiting:
						break;
					default:
						++count;
						break;
					}
				}
				return count;
			}



			//--------------------------------------------------------------------------------------------------------------------------------------------
			//	Active Quest Item
			//--------------------------------------------------------------------------------------------------------------------------------------------

			void QuestManager::Dispatch(UserSharedPtr user, Protocol::ToServer::Quest::ActivityItem& packet)
			{
				AutoLock userLock(user->GetSyncObject());
				AutoLock lock(cs);


				const Database::Info::QuestInfo* info(Database::DatabaseInstance().InfoCollections.Quests.Retrieve(packet.QuestHash));

				//------------------------------------------------------------------------------------------------------------------------------
				if (std::find_if(workingQuests.begin(), workingQuests.end(), XRated::Quest::FindByHash(packet.QuestHash)) != workingQuests.end())
				{
					user->CriticalError("already working quest by activityItem");
					return;
				}

				std::map<uint32, uint32>::const_iterator iter = completedQuests.find(packet.QuestHash);
				if ((info->IsRepeatQuest() == false) && ((iter != completedQuests.end()) && (iter->second > 0)))
				{
					user->CriticalError("already completed quest by activityItem");
					return;
				}
				//------------------------------------------------------------------------------------------------------------------------------

				std::vector<Quest>::const_iterator i = std::find_if(itemQuests.begin(), itemQuests.end(), XRated::Quest::FindByHash(packet.QuestHash));
				if (i != itemQuests.end())
				{
					Protocol::FromServer::Quest::ActivityItem packet;
					user->Send(packet);

					LoggerInstance().Info(L"Send To Client Active Item Result");
					return;
				}

				do // dummy do to validate quest UseActivityItem request - make sure 'break' for CRITICAL invalid tag
				{
					if (dbRequested.find(user) != dbRequested.end()) break; // already doing something( accept or complete or activityItem
					if (!info) break; // unable to find the quest information on server

					dbRequested.insert(user);
					//' request  : character name, quest hashs
					Net::Api request("Quest/ActivityItem");

					//Add Basic ActiveItemQuestInfo
					LoggerInstance().Info("Request active item({}) info to DB", packet.QuestHash);

					AddItemQuest(packet.QuestHash, XRated::Quest::State::NotAccepted);
					request << user->GetSerial() << packet.QuestHash;

					if (!info->AcceptCondition.Quests.empty())
					{
						for (auto i = info->AcceptCondition.Quests.begin(); i != info->AcceptCondition.Quests.end(); ++i)
						{
							LoggerInstance().Info("Request active item({}) info to DB", *i);

							AddItemQuest(*i, XRated::Quest::State::NotAccepted);
							request << *i;
						}
					}

					request.GetAsync(this, &QuestManager::ActiveItemQuestInfo, user);
					return;

				} while (false);

				/* TODO : ciritical policy */
				LoggerInstance().Error("invalid quest({})-activityitem requested", packet.QuestHash);
			}

			void QuestManager::ActiveItemQuestInfo(const UserSharedPtr& user, const Net::Answer& answer)
			{
				AutoLock lock(cs);
				dbRequested.erase(user);

				//' response : quest hashes
				if (answer)
				{
					for(auto& quest : answer.at("quests").get<json>())
					{
						uint32 questHash = quest.at("hash").get<uint32>();
						auto ai = std::find_if(itemQuests.begin(), itemQuests.end(), XRated::Quest::FindByHash(questHash));
						if (ai != itemQuests.end())
							ai->CurrentState = XRated::Quest::State::Completed;
						else
							AddItemQuest(questHash, XRated::Quest::State::Completed);

						//ActivityQuestItem Update
						LoggerInstance().Info("Recieve active item({}) info from DB", questHash);
					}
				}

				LoggerInstance().Info(L"Send To Client Active Item Result");

				Protocol::FromServer::Quest::ActivityItem sendPacket;
				user->Send(sendPacket);
			}

			void QuestManager::AddItemQuest(uint32 questHash, uint8 currentState)
			{
				AutoLock lock(cs);

				std::vector<Quest>::iterator i = std::find_if(itemQuests.begin(), itemQuests.end(), XRated::Quest::FindByHash(questHash));
				if (i == itemQuests.end())
				{
					itemQuests.push_back(Quest(questHash));
					itemQuests.back().CurrentState = currentState;
				}
				else
					ChangeActiveQuestState(questHash, currentState);
			}

			void QuestManager::ChangeActiveQuestState(uint32 questHash, uint8 newState)
			{
				//AutoLock lock(cs);	
				std::vector<Quest>::iterator i = std::find_if(itemQuests.begin(), itemQuests.end(), XRated::Quest::FindByHash(questHash));
				if (i != itemQuests.end())
				{
					if (i->CurrentState == newState) return;
					if (newState == XRated::Quest::State::Completed) i->CurrentState = newState;
					else if (i->CurrentState != XRated::Quest::State::Completed) i->CurrentState = newState; 
				}
			}

			bool QuestManager::ClearQuestList()
			{
				AutoLock lock(cs);

				workingQuests.clear();
				completedQuests.clear();

				originWorkingQuests.clear();
				originCompletedQuests.clear();

				return true;
			}

			void QuestManager::ValidActiveItem(UserSharedPtr user, uint32 questHash)
			{
				AutoLock lock(cs);

				auto i = std::find_if(itemQuests.begin(), itemQuests.end(), XRated::Quest::FindByHash(questHash));
				if (i != itemQuests.end())
				{
					/* notice to client */
					Protocol::FromServer::Quest::ShowDetailWindow packet;
					packet.QuestHash = questHash;
					packet.Result = static_cast<XRated::QuestErrorType::Type>(IsValidItemQuest(user, *i, questHash));
					user->Send(packet);
				}
				else
				{
					Protocol::FromServer::Quest::Error response;
					response.QuestHash = questHash;
					response.Result = XRated::QuestErrorType::Type::NoError;
					user->Send(response);

					LoggerInstance().Error("unexpected : quest({}) not found at itemQuests", questHash); 
					return; // should not be happened
				}
			}

			uint8 QuestManager::IsValidItemQuest(const UserSharedPtr user, const Quest& questInfo, uint32 questHash) const
			{
				AutoLock lock(cs);

				if (GetValidQuestCount() >= XRated::Quest::MaxCount)
				{
					LoggerInstance().Error("invalid quest({}) accept out of range", questHash);
					return XRated::QuestErrorType::Type::AlreadyFullQuestCount;
				}

				/* working quests validation */
				if (std::find_if(workingQuests.begin(), workingQuests.end(), XRated::Quest::FindByHash(questHash)) != workingQuests.end()) // a quest cannot work concurrently.
				{
					LoggerInstance().Error("already working quest");
					return XRated::QuestErrorType::Type::AlreadyWorking;
				}

				/* completed quests validation */
				//if ( questInfo.Info->IsRepeatQuest()==false &&
				//	 (completedQuests.find(questHash)!=completedQuests.end() || questInfo.CurrentState == XRated::Quest::State::Completed) )
				//{
				//	ALLM_INFO((L"already completed quest");
				//	return XRated::QuestErrorType::Type::AlreadyCompletedQuest;
				//}

				std::map<uint32, uint32>::const_iterator iter = completedQuests.find(questHash);
				if (questInfo.Info->IsRepeatQuest() == false && iter != completedQuests.end() && iter->second > 0)
				{
					LoggerInstance().Error("invalid quest({}) accept already complete", questHash);
					return XRated::QuestErrorType::Type::AlreadyCompletedQuest;
				}
				if (questInfo.Info->IsRepeatQuest() == true)
				{
					if (iter != completedQuests.end()) {
						if (questInfo.Info->IsRepeatQuest(iter->second) == false) {
							LoggerInstance().Error("invalid quest({}) not repeatable quest - completeCount({}) repeatableCount({})", questHash
								, completedQuests.find(questHash)->second, questInfo.Info->MaximumCompleteableCount);
							return XRated::QuestErrorType::Type::CompletedTooManyTimes;
						}
					}
				}

				const Database::Info::QuestInfo::Condition& condition = questInfo.Info->AcceptCondition;

				Logic::Player* player(user->GetPlayer());
				if (player == NULL)
				{
					return XRated::QuestErrorType::Type::InvalidUser;
				}

				/* state can be changed by only script */
				if (condition.ScriptOnly)
				{
					LoggerInstance().Error("only for script");
					return XRated::QuestErrorType::Type::OnlyScript;
				}

				/* character validation */
				if (condition.IsValid(player->GetPlayerData()) == false)
				{
					LoggerInstance().Error("not enough character status");
					return XRated::QuestErrorType::Type::MoreCharacterStatusRequired;
				}
				/* guild quest validateion */
				if (condition.IsGuildQuest() == true) {
					if ((user->IsPartOfGuild() == false)) {
						return false;
					}
					if (user->GetGuildInfo().GuildLevel < condition.GuildLevel) {
						return false;
					}
				}

				/* item validation */
				if (!condition.Items.empty())
				{
					struct Condition : public Database::Info::ItemInfo::IFindCondition
					{
						uint32 Hash;
						Condition(uint32 hash) : Hash(hash) {}
						bool operator()(const Database::Info::ItemInfo& info) const { return (info.Hash == Hash); }
					};

					for (std::vector< Database::Info::QuestInfo::Condition::Item >::const_iterator i = condition.Items.begin(); i != condition.Items.end(); ++i)
					{
						if (user->GetItemCount(Condition(i->ItemHash), true) < i->Count)
						{
							LoggerInstance().Info(L"not enough items");
							return XRated::QuestErrorType::Type::NotEnoughItems;
						}
					}
				}

				/* license validation */
				if (!condition.Licenses.empty())
				{
					for (std::vector< StageLocation >::const_iterator i = condition.Licenses.begin(); i != condition.Licenses.end(); ++i)
					{
						if (!user->IsAbleToJoinStage(*i))
						{
							LoggerInstance().Info("not enough stage licenses");
							return XRated::QuestErrorType::Type::NotEnoughLicense;
						}
					}
				}

				/* quest validation */
				if (!condition.Quests.empty())
				{
					for (std::vector< uint32 >::const_iterator i = condition.Quests.begin(); i != condition.Quests.end(); ++i)
					{
						std::map<uint32, uint32>::const_iterator completedIter = completedQuests.find(*i);
						if ((completedIter == completedQuests.end()) || completedIter->second < 1)
						{
							auto ai = std::find_if(itemQuests.begin(), itemQuests.end(), XRated::Quest::FindByHash(*i));
							if (ai != itemQuests.end() && ai->CurrentState == XRated::Quest::State::Completed)
							{
							}
							else
							{
								LoggerInstance().Info("not enough preceded quest");
								return XRated::QuestErrorType::Type::PrecededQuestRequired;
							}
						}
					}
				}

				/* finally */
				return XRated::QuestErrorType::Type::NoError;
			}

			void QuestManager::GetChangedQuestInfoAndOriginBackup(ChangeQuestInfo& info)
			{
				AutoLock lock(cs);
				{
					std::vector<Quest>::iterator iter = workingQuests.begin();
					std::vector<Quest>::iterator end = workingQuests.end();

					while (iter != end) {
						std::list<Quest>::iterator originIter = std::find_if(originWorkingQuests.begin(), originWorkingQuests.end(), XRated::Quest::FindByHash((*iter).Id));
						if (originIter != originWorkingQuests.end()) {
							const Quest& now((*iter));
							const Quest& origin((*originIter));
							if ((now.ParameterChanged == true) || (now.CurrentState != origin.CurrentState) || (now.ExpiredDate != origin.ExpiredDate)) {
								info.workingQuestsToUpdate.push_back((*iter));
							}

							originWorkingQuests.erase(originIter);
						}
						else {
							info.workingQuestsToInsert.push_back((*iter));
						}
						++iter;
					}
					info.workingQuestsToDelete.assign(originWorkingQuests.begin(), originWorkingQuests.end());

					{
						std::map<uint32, Quest>::iterator iter = duplicatedList.begin();
						std::map<uint32, Quest>::iterator end = duplicatedList.end();

						while (iter != end)
						{
							LoggerInstance().Warn("asdf workingQuestsToDelete push questHash={} from duplicatedList!!", iter->first);
							info.workingQuestsToDelete.push_back(iter->second);
							++iter;
						}
					}
				}

				{
					std::map<uint32/*Quest hash*/, uint32/*Completed Count*/>::const_iterator iter = completedQuests.begin();
					std::map<uint32, uint32>::const_iterator end = completedQuests.end();

					while (iter != end) {
						std::map<uint32, uint32>::iterator originIter = originCompletedQuests.find(iter->first);
						if (originIter != originCompletedQuests.end()) {
							//update
							if (iter->second != originIter->second) {
								const Database::Info::QuestInfo* questInfo(Database::DatabaseInstance().InfoCollections.Quests.Retrieve(iter->first));
								info.completedQuestsToUpdate.push_back(ComplatedQuest(iter->first, questInfo, iter->second));
							}
						}
						else {
							//insert
							const Database::Info::QuestInfo* questInfo(Database::DatabaseInstance().InfoCollections.Quests.Retrieve(iter->first));
							info.completedQuestsToInsert.push_back(ComplatedQuest(iter->first, questInfo, iter->second));
						}
						++iter;
					}

				}

				//back up
				originWorkingQuests.clear();
				originWorkingQuests.assign(workingQuests.begin(), workingQuests.end());

				originCompletedQuests = completedQuests;


			}


			void QuestManager::ChangeQuestInfo::Clear()
			{
				workingQuestsToInsert.clear();
				workingQuestsToUpdate.clear();
				workingQuestsToDelete.clear();

				completedQuestsToInsert.clear();
				completedQuestsToUpdate.clear();
			}

			void QuestManager::DropEventQuests(uint8 eventType)
			{
				std::vector<Quest>::iterator i = workingQuests.begin();
				for (; i != workingQuests.end(); )
				{
					if ((*i).Info->EventQuestType == eventType)
					{
						i = workingQuests.erase(i);
					}
					else
					{
						++i;
					}
				}
			}
		}
	}
}