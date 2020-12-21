#ifndef ErrorDefinition_H_Guard
#define ErrorDefinition_H_Guard
#pragma once
namespace Lunia {
	namespace XRated {
		struct Errors
		{
			// glboal erros
			static const int NotEnoughParameters = 1;
			static const int NotSupported = 98;
			static const int Unexpected = 99;

			// normal (lobby and stage) erros
			static const int InvalidAccount = 101;
			static const int InvalidPassword = 102;
			static const int AlreadyConnectedAccount = 103;
			static const int NotConnectedAccount = 104;
			static const int NotSelectedCharacter = 105;
			static const int AlreadySelectedCharacter = 106;
			static const int AlreadyPlaying = 107;
			static const int NotInStage = 108;
			static const int AlreadyExistingCharacter = 109;
			static const int InvalidCharacterName = 110;
			static const int InvalidClass = 111;
			static const int InvalidGuildCharacterName = 112;
			static const int AlreadyVoting = 113;
			static const int SetSkillPointFaild = 114;
			static const int InvalidDropItem = 115;
			static const int UnableToUseItem = 117;
			static const int ItemCountMismatched = 118;
			static const int UnableToJoinSquare = 119;
			static const int UnableToJoinStage = 120;
			static const int NotPlayerWhisper = 121;
			static const int InvalidRoomNumber = 201;
			static const int StageFulled = 202;
			static const int AlreadyStageExists = 203;
			static const int InvalidStageCode = 204;
			static const int JoinDenied = 205;
			static const int StageLocked = 206;
			static const int ServerIsFull = 301;
			static const int UnableSellItem = 401;
			static const int InventoryFull = 402;
			static const int NotEnoughMoney = 403;
			static const int NotHaveReagents = 404;
			static const int PacketMisMatch = 405;
			static const int InvalidVotingParam = 406;
			static const int AlreadyParty = 407;
			static const int CashItemMoveFailed = 408;
			static const int ItemNoPermissions = 410;
			static const int NoResponse = 411;
			static const int GuildCreateFailed = 412;
			static const int GuildInvalidUser = 413;
			static const int AlreadyExistingGuildId = 414;
			static const int AlreadyExistingGuildName = 415;
			static const int JoinedAnotherGuild = 416;
			static const int GuildUnknown = 417;
			static const int GuildNoRighT = 418;
			static const int GuildJoinFailed = 419;
			static const int GuildInvalidAccount = 420;
			static const int GuildAlreadyJoined = 421;
			static const int InvalidGuildId = 422;
			static const int InvalidGuildName = 423;
			static const int InvalidMemberGrade = 424;
			static const int GuildLeaveFailed = 425;
			static const int GuildKickedFailed = 426;
			static const int GuildRemoveFailed = 427;
			static const int GuildNotExisting = 428;
			static const int GuildMemberExist = 429;
			static const int GuildMemberInWaitingList = 430;
			static const int GuildGetListMembersFailed = 431;
			static const int AlreadyCompletedQuest = 432;
			static const int NotEnoughQuestInventory = 433;
			static const int PrecededQuestRequired = 434;
			static const int TargetAlreadyJoinedGuild = 435;
			static const int MismatchedMemberCount = 436; // when pvp re-match is requested by that the number of team members are mismatched
			static const int GuildOutOfMembers = 437;
			static const int GuildUnableToChangeMaster = 438;
			static const int CompletedTooManyTimes = 439;
			static const int AlreadyWorking = 440;
			static const int GoldOverflow = 441;
			static const int PetCreateFailed_ExsistName = 442;
			static const int PetCreateFailed_InvalidItem = 443;
			static const int RebirthFailedNeedMoreDay = 444;
			static const int EnterShopFailed = 445;
			static const int NotJoinedGuild = 446;
			static const int BuyGuildShopFailed_HaveNotItem = 447;
			static const int GuildRankExpired = 448;
			static const int NotHaveSkillItem = 449;
			static const int NotUseToRebirthSkill = 450;
			static const int AlreadyUseUltimateSkill = 451;  ///< 
			static const int NotEnoughLadderPoint = 452;
			static const int NotEnoughNeedItemCount_BuyBarter = 453; // 3.1 by ultimate


			// square specific errors
			static const int InvalidSquareNumber = 501;
			static const int TooManyPlayerStores = 502;
			static const int TooClosePlayerStore = 503;

			//
			static const int InvalidClientVersion = 601;

			static const int TooManyFriends = 701;

			static const wchar_t* ToString(const int errorNumber)
			{
				switch (errorNumber)
				{
				case AlreadyConnectedAccount:	return L"AlreadyConnectedAccount";
				case AlreadyExistingCharacter:	return L"AlreadyExistingCharacter";
				case AlreadyPlaying:			return L"AlreadyPlaying";
				case AlreadySelectedCharacter:	return L"AlreadySelectedCharacter";
				case AlreadyStageExists:		return L"AlreadyStageExists";
				case AlreadyVoting:				return L"AlreadyVoting";
				case JoinDenied:				return L"JoinDenied";
				case NotConnectedAccount:		return L"NotConnectedAccount";
				case NotEnoughParameters:		return L"NotEnoughParameters";
				case NotInStage:				return L"NotInStage";
				case NotSelectedCharacter:		return L"NotSelectedCharacter";
				case NotSupported:				return L"NotSupported";
				case InvalidAccount:			return L"InvalidAccount";
				case InvalidCharacterName:		return L"InvalidCharacterName";
				case InvalidClass:				return L"InvalidClass";
				case InvalidGuildCharacterName:	return L"InvalidGuildCharacterName";
				case InvalidClientVersion:		return L"InvalidClientVersion";
				case InvalidDropItem:			return L"InvalidDropItem";
				case InvalidPassword:			return L"InvalidPassword";
				case InvalidRoomNumber:			return L"InvalidRoomNumber";
				case InvalidStageCode:			return L"InvalidStageCode";
				case StageLocked:				return L"StageLocked";
				case ItemCountMismatched:		return L"ItemCountMismatched";
				case StageFulled:				return L"StageFulled";
				case UnableToJoinSquare:		return L"UnableToJoinSquare";
				case UnableToJoinStage:			return L"UnableToJoinStage";
				case UnableToUseItem:			return L"UnableToUseItem";
				case SetSkillPointFaild:		return L"SetSkillPointFaild";
				case UnableSellItem:			return L"UnableSellItem";
				case Unexpected:				return L"Unexpected";
				case InventoryFull:				return L"InventoryFull";
				case NotEnoughMoney:			return L"NotEnoughMoney";
				case NotPlayerWhisper:			return L"NotPlayerWhisper";
				case InvalidSquareNumber:		return L"InvalidSquareNumber";
				case ServerIsFull:				return L"ServerIsFull";
				case NotHaveReagents:			return L"NotHaveReagents";
				case NotHaveSkillItem:			return L"NotHaveSkillItem";
				case NotUseToRebirthSkill:		return L"NotUseToRebirthSkill";
				case AlreadyUseUltimateSkill:	return L"AlreadyUseUltimateSkill";
				case PacketMisMatch:			return L"PacketMisMatch";
				case InvalidVotingParam:		return L"InvalidVotingParam";
				case AlreadyParty:				return L"AlreadyParty";
				case CashItemMoveFailed:		return L"CashItemMoveFailed";
				case ItemNoPermissions:			return L"ItemNoPermissions";
				case NoResponse:				return L"NoResponse";
				case GuildCreateFailed:			return L"GuildCreateFailed";
				case AlreadyExistingGuildId:	return L"AlreadyExistingGuildId";
				case AlreadyExistingGuildName:	return L"AlreadyExistingGuildName";
				case JoinedAnotherGuild:		return L"JoinedAnotherGuild";
				case GuildUnknown:				return L"GuildCreateFailed";
				case GuildNoRighT:				return L"GuildNoRighT";
				case GuildJoinFailed:			return L"GuildJoinFailed";
				case GuildInvalidAccount:		return L"GuildInvalidAccount";
				case GuildAlreadyJoined:		return L"GuildAlreadyJoined";
				case InvalidGuildId:			return L"InvalidGuildId";
				case InvalidGuildName:			return L"InvalidGuildName";
				case InvalidMemberGrade:		return L"InvalidMemberGrade";
				case GuildKickedFailed:			return L"GuildKickedFailed";
				case GuildLeaveFailed:			return L"GuildLeaveFailed";
				case GuildRemoveFailed:			return L"GuildRemoveFailed";
				case GuildNotExisting:			return L"GuildNotExisting";
				case GuildMemberExist:			return L"GuildMemberExist";
				case GuildMemberInWaitingList:	return L"GuildMemberInWaitingList";
				case GuildInvalidUser:			return L"GuildInvalidUser";
				case GuildGetListMembersFailed: return L"GuildGetListMembersFailed";
				case TooManyPlayerStores:		return L"TooManyPlayerStores";
				case TooClosePlayerStore:		return L"TooClosePlayerStore";
				case AlreadyCompletedQuest:		return L"AlreadyCompletedQuest";
				case NotEnoughQuestInventory:	return L"NotEnoughQuestInventory";
				case PrecededQuestRequired:		return L"PrecededQuestRequired";
				case TargetAlreadyJoinedGuild:  return L"TargetAlreadyJoinedGuild";
				case MismatchedMemberCount:		return L"MismatchedMemberCount";
				case GuildOutOfMembers:			return L"GuildOutOfMembers";
				case GuildUnableToChangeMaster:	return L"GuildUnableToChangeMaster";
				case CompletedTooManyTimes:		return L"CompletedTooManyTimes";
				case AlreadyWorking:			return L"AlreadyWorking";
				case TooManyFriends:			return L"TooManyFriends";
				case PetCreateFailed_ExsistName:return L"PetCreateFailed_ExsistName";
				case PetCreateFailed_InvalidItem:return L"PetCreateFailed_InvalidItem";
				case RebirthFailedNeedMoreDay:	return L"RebirthFailedNeedMoreDay";
				case EnterShopFailed: return L"EnterShopFailed";
				case NotJoinedGuild: return L"NotJoinedGuild";
				case BuyGuildShopFailed_HaveNotItem: return L"BuyGuildShopFailed_HaveNotItem";
				case GuildRankExpired: return L"GuildRankExpired";
				case NotEnoughLadderPoint:		return L"NotEnoughLadderPoint";
				case NotEnoughNeedItemCount_BuyBarter:		return L"NotEnoughNeedItemCount_BuyBarter";
				}

				Logger::GetInstance().Exception(L"Unknown error number : {0}", errorNumber);
				return L"UnIdentifiedError";
			}
		};
	}
}


#endif //! ErrorDefinition_H_Guard