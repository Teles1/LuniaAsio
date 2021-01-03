#pragma once
#include <StageServer/Protocol/Protocol.h>
namespace Lunia
{
	namespace XRated
	{
		namespace StageServer
		{
			namespace Protocol
			{
				namespace ToServer
				{
					// Don't have to use these packets immediately. : by rapperkjm
					struct CharacterInfo : public IPacketSerializable
					{
						NET_SERIALIZABLE;
					};

					// Don't have to use these packets immediately. : by rapperkjm
					struct ListQuickSlot : public IPacketSerializable
					{
						NET_SERIALIZABLE;
					};

					// Don't have to use these packets immediately. : by rapperkjm
					struct ListSkill : public IPacketSerializable
					{
						NET_SERIALIZABLE;
					};

					// Don't have to use these packets immediately : by rapperkjm
					struct ListItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;
					};

					struct Stage : public IPacketSerializable		///< request authorization
					{
						NET_SERIALIZABLE;

						std::wstring secukey;
						uint32 Version;
						std::wstring Locale;
					};

					struct Chat : public IPacketSerializable
					{
						NET_SERIALIZABLE;


						XRated::Constants::ChatTypes chattype;
						std::wstring message;
					};

					struct Voice : public IPacketSerializable
					{
						NET_SERIALIZABLE;


						unsigned short chattype;
						unsigned short messageid;
					};

					struct Join : public IPacketSerializable
					{
						NET_SERIALIZABLE;

					};

					struct CheckJoinToSecretStage : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::wstring password;
					};

					struct LoadEnd : public IPacketSerializable		///< client notice join a stage after loading resource to server
					{
						NET_SERIALIZABLE;

						float progress; // 100% == 1.0f
					};

					struct Command : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						//std::wstring command;
						int command;
						XRated::Constants::Direction direction;

					};

					struct Cast : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 skillgroupid;
					};

					struct SetSkillLevel : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 skillgroupid;
						unsigned short delta;
					};

					struct Revive : public IPacketSerializable
					{
						NET_SERIALIZABLE;
					};

					struct DailyRevive : public IPacketSerializable
					{
						NET_SERIALIZABLE;
					};

					struct EquipSwap : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint8 Set;
					};

					struct CashEquipSwap : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint8 Set;
					};

					struct MoveItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint8 pbag;
						uint8 ppos;
						uint8 nbag;
						uint8 npos;
					};

					struct SortInventory : public IPacketSerializable
					{
						NET_SERIALIZABLE;
					};

					struct StackItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned short pbag;
						unsigned short ppos;
						unsigned short nbag;
						unsigned short npos;
						uint32 count;
					};

					struct GivePresentToPet : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition          PetItemPosition;
						std::vector<XRated::ItemPack> PresentItemPack;
					};

					struct MoveItemInventoryToPet : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						XRated::GlobalSerial PetSerial;
						XRated::ItemPosition Inventory;
						XRated::PetItemSlot::PositionType PetSlotType;
						uint8 PetSlotPosition;
					};

					struct MoveItemInPetInven : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::GlobalSerial PetSerial;

						XRated::PetItemSlot::PositionType SourcePositionType;
						uint8 SourcePosition;

						XRated::PetItemSlot::PositionType TargetPositionType;
						uint8 TargetPosition;
					};

					struct RequestTakeOutCaredPet : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						XRated::GlobalSerial PetSerial;
					};

					struct TakeOutAllPetItems : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						XRated::GlobalSerial PetSerial;
					};

					struct Use : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition itemposition;
					};

					struct UseItemInPetInven : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::GlobalSerial PetSerial;
						XRated::PetItemSlot::PositionType PositionType;
						uint8 Position;
					};

					struct DropItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition position;
						uint32 count;
					};

					struct AddMinimapPing : public IPacketSerializable
					{
						NET_SERIALIZABLE;


						float3 position;
					};

					struct MoveQuickSlot : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned short ppos;
						unsigned short npos;
					};

					struct RequestVoting : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::Constants::VoteTypes type;
						XRated::StageLicense targetStage;
						std::wstring charName; // or Room Password
					};

					struct AnswerPersonalVoting : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::Constants::PersonalVoteTypes type;
					};

					struct RequestPersonalVoting : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::Constants::PersonalVoteTypes type;
						std::wstring charName;
					};

					struct Vote : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::Constants::VoteResult result;
					};

					struct EnterShop : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::Constants::ShopType shopnumber;
						uint32				param;
					};


					struct LeaveShop : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned short shopnumber;
					};

					struct Buy : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 itemid;
						uint16 count;
					};

					struct BuyBack : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;

						uint32 itemid;
						uint16 count;
						InstanceEx instanceEx;
					};

					struct BuyBarterItem : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;

						uint32 barterItemId;
						uint16 count;
					};

					struct BuyToGuildShop : public Buy
					{
						NET_SERIALIZABLE;
					};
					struct Sell : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned short bag;
						unsigned short pos;
					};

					//struct Bank : public IPacketSerializable
					//{
					//	NET_SERIALIZABLE;

					//	XRated::ItemPosition invenposition;
					//	XRated::ItemPosition bankposition;
					//};

					struct StackBank : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned short invenbag;
						unsigned short invenpos;
						unsigned short bankbag;
						unsigned short bankpos;
						bool toBank;
						uint32 count;
					};

					struct RequestTrade : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int serial;
					};

					struct AddTradeItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition itemposition;
					};

					struct AddTradeMoney : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int money;
					};

					struct ReadyToTrade : public IPacketSerializable
					{
						NET_SERIALIZABLE;

					};

					struct CancelTrade : public IPacketSerializable
					{
						NET_SERIALIZABLE;

					};

					struct ConfirmTrade : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						bool conFirm;
					};

					struct DropBankItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition position;
					};

					struct BankMoneyIn : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 BankMoney;
					};

					struct BankMoneyOut : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 BankMoney;
					};

					struct QuickSlot : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned short number;
						uint32 id;
						bool isskill;
						InstanceEx instanceEx;
					};

					struct Alive : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						uint64	RequestTime;
						uint32 EncryptKey;
					};

					struct Identify : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition Position;
						//int IdentifyIndex; ///< used only in enchant identifying
					};

					struct Reinforce : public IPacketSerializable
					{
						NET_SERIALIZABLE;


						XRated::ItemPosition Position;
						std::vector<ItemPack> Stones;
						std::vector<ItemPack> Catalysts;
					};

					struct LightReinforce : public IPacketSerializable
					{
						NET_SERIALIZABLE;


						XRated::ItemPosition Position;
						std::vector<ItemPack> Jewels;
						std::vector<ItemPack> Catalysts;
					};

					struct AttachMagic : public IPacketSerializable
					{
						NET_SERIALIZABLE;


						XRated::ItemPosition Position;
						std::vector<ItemPack> Scrolls;
						std::vector<ItemPack> Components;
					};

					struct Extract : public IPacketSerializable
					{
						NET_SERIALIZABLE;


						XRated::ItemPosition Position;
					};

					struct RestoreBelonging : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition Position;
						std::vector<ItemPack> Scrolls; //
						std::vector<ItemPack> Components;
					};

					struct RecoverReinforcement : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition	Target;
						std::vector<ItemPack>	Stones;
					};

					struct RecoverLightReinforcement : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition	Target;
						std::vector<ItemPack>	Stones;
					};

					struct ResetIndentification : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition	Target;
						std::vector<ItemPack>	Scrolls;
					};

					struct Terminate : public IPacketSerializable
					{
						NET_SERIALIZABLE;
					};

					struct ListCashItems : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						int PageNumber; ///< must not be  0
						bool IsPresent;
						std::wstring Language;
					};

					struct PurchaseCashItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 ProductID;
					};

					struct CashItemMove : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						int OrderNumber;
						int ProductNumber;
						int Quantity;  ///< must be 1 currently, just reserved for extension
					};

					struct CashItemRefund : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						int OrderNumber;
					};

					struct CashItemView : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint16	Flag;
						uint16	WingType; // 3.1
					};

					struct UseItemEx : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition Position;
						std::vector<String> Parameters;
					};

					struct Alchemy : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition Position;
						std::vector<ItemPack> Components;
					};

					struct GiveUpRevive : public IPacketSerializable
					{
						NET_SERIALIZABLE;
					};

					struct InstantCoinRevive : public IPacketSerializable
					{
						NET_SERIALIZABLE;
					};

					struct InstantCoinReviveRollBack : public IPacketSerializable
					{
						NET_SERIALIZABLE;
					};

					struct Familiar
					{
						struct ChangeFormation : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							Constants::Familiar::Formation FormationTo;
						};

						struct Command : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint16         TargetIndex;
							XRated::Serial TargetSerial;
							Constants::Familiar::Command Sign; ///< you cannot use 'Command' as name because that is a struct name.
						};
					};

					struct Random : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						String RandomName; ///< something like "Dice"
						uint16 MinValue; ///< minimum random value to be choosen
						uint16 MaxValue; ///< maximum random value to be choosen
					};

					struct RequestReward : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						enum Types { MissionClear = 0, } RewardType;
						uint8 SelectType;
					};

					struct Upgrade : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition PotionPosition;
						XRated::ItemPosition TargetPosition;
					};

					struct Rename : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						enum Types { Character, Guild, }	TargetType;
						String								NewName;
						XRated::ItemPosition				RenamingItemPositoin;
					};

					struct AllMGuild
					{
						struct Create : public IPacketSerializable
						{
							NET_SERIALIZABLE;
							String Alias;
							String GuildName; // GuildName
							String Message; // Guild Message
						};

						struct Invite : public IPacketSerializable /// request to invite a member
						{
							NET_SERIALIZABLE;

							String TargetCharacterName;
						};

						struct InviteReply : public IPacketSerializable /// request to invite a member
						{
							NET_SERIALIZABLE;

							String TargetCharacterName;
							int ReplyCode;
						};

						struct Join : public IPacketSerializable /// request to join a guild
						{
							NET_SERIALIZABLE;

							int GuildId;	///< guild oid to join
							uint8 Grade;
						};

						struct Kick : public IPacketSerializable // request to kick a member
						{
							NET_SERIALIZABLE;

							String TargetCharacterName;
						};

						struct Leave : public IPacketSerializable /// request to leave the guild
						{
							NET_SERIALIZABLE;
						};

						struct ListMembers : public IPacketSerializable /// request list of all members of the guild
						{
							NET_SERIALIZABLE;
						};

						struct Remove : public IPacketSerializable /// destroy a guild
						{
							NET_SERIALIZABLE;
						};

						struct ValidateName : public IPacketSerializable /// test guild name validation
						{
							NET_SERIALIZABLE;

							String Name;
						};

						struct ValidateAlias : public IPacketSerializable /// test guild id validation
						{
							NET_SERIALIZABLE;

							String Alias;
						};
						struct AddExp : public IPacketSerializable /// guild exp up.
						{
							NET_SERIALIZABLE;
						};
						struct AddMaintenancePoint : public IPacketSerializable /// guild Maintenance point up.
						{
							NET_SERIALIZABLE;
							uint32 Point;
							uint32 ItemHash;
						};

						struct MessageChange : public IPacketSerializable /// Change Guild Messsage
						{
							NET_SERIALIZABLE;

							String Message;
						};

						struct ChangeGrade : public IPacketSerializable /// Change guild member's grade
						{
							NET_SERIALIZABLE;

							String TargetCharacterName;
							uint8 Grade;
						};

						struct ChangeGradeName : public IPacketSerializable /// Change guild grade name
						{
							NET_SERIALIZABLE;

							uint8 TargetGrade;
							String TargetGradeName;
						};

						struct ChangeGradeAuth : public IPacketSerializable /// Change guild grade Authority
						{
							NET_SERIALIZABLE;

							uint8 TargetGrade;
							uint8 Authority;
						};

						struct ChangeGuildMaster : public IPacketSerializable /// Change guild grade name
						{
							NET_SERIALIZABLE;

							String TargetCharacterName;
						};

						struct GetMyInfo : public IPacketSerializable /// request to refresh my information
						{
							NET_SERIALIZABLE;
						};

						struct RefreshMyInfo : public IPacketSerializable /// request to refresh my information. No reply from server.
						{
							NET_SERIALIZABLE;
						};

						struct LevelUp : public IPacketSerializable // guild level up
						{
							NET_SERIALIZABLE;

							uint8 level;
						};

						struct ReflashGuildLevel : public IPacketSerializable // guild level up
						{
							NET_SERIALIZABLE;
							uint8 level;
						};

						struct ReflashGuildProfitDate : public IPacketSerializable // guild profit date
						{
							NET_SERIALIZABLE;
							DateTime OpenDate;
							DateTime CloseDate;
						};


						struct ReflashGuildKicked : public IPacketSerializable // guild level up
						{
							NET_SERIALIZABLE;
						};

						struct GetTax : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32	tax;
						};
						struct ReflashGuildShopItem : public IPacketSerializable
						{
							NET_SERIALIZABLE;
							XRated::GuildShopItem	Item;
						};
					};


					struct Quest
					{
						struct Accept : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 QuestHash;
							uint32 SelectRewardIndex;
						};

						struct Complete : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 QuestHash;
							uint32 SelectRewardIndex;
							uint32 count;
						};

						struct Drop : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 QuestHash;
						};

						struct ActivityItem : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 QuestHash;
						};

						struct ShareToPartyPlayer : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 QuestHash;
						};

						struct SharedAnswer : public IPacketSerializable
						{
							NET_SERIALIZABLE;
							uint32 QuestHash;
							uint32 SelectRewardIndex;
							bool Accept;
						};


					}; // Quest


					struct PlayerStore
					{
						/* a player store opens by UseItemEx with a NOLOGIC state with "PlayerStore" type */

						/** request sore item list of the user */
						struct List : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 PlayerSerial;
						};

						/** request to buy */
						struct Buy : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 PlayerSerial;
							StoreSlot Item;
						};

						struct BuyPet : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 PlayerSerial;
							StoreSlot Item;
							XRated::GlobalSerial PetSerial;
						};

						/** close the store */
						struct Close : public IPacketSerializable
						{
							NET_SERIALIZABLE;
						};

					}; // PlayerStore

					struct Bank
					{
						struct List : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint8 BagNumber;
						};

						struct Push : public IPacketSerializable // request to push a cash item into storage
						{
							NET_SERIALIZABLE;

							ItemPosition SourceInventory;
							ItemPosition TargetStorage;
							uint16 Count; // 3.1
						};

						struct Pop : public IPacketSerializable // request to pop a cash item from storage
						{
							NET_SERIALIZABLE;

							ItemPosition SourceStorage;
							ItemPosition TargetInventory;
							uint32 ItemHash;
							uint16 Count; // 3.1
						};

						struct SwapItemInStorage : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							ItemPosition SourceStorage;
							ItemPosition TargetStorage;
							uint32 ItemHash;
							uint16 Count; // 3.1
						};

						struct Move : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							ItemPosition SourceInventory;
							ItemPosition TargetStorage;
						};
					};

					struct CashItemStorage
					{
						struct List : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							// TODO : should these constants go into XRated.h ?
							const static uint8 NumberOfPages = 20;
							const static uint8 NumberOfItemsInAPage = 7;

							uint8 PageIndex;
						};

						struct Push : public IPacketSerializable // request to push a cash item into storage
						{
							NET_SERIALIZABLE;

							ItemPosition SourceInventory;
							ItemPosition TargetStorage;
						};

						struct Pop : public IPacketSerializable // request to pop a cash item from storage
						{
							NET_SERIALIZABLE;

							ItemPosition SourceStorage;
							ItemPosition TargetInventory;
						};

						struct MoveItemInStorage : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							ItemPosition SourceStorage;
							ItemPosition TargetStorage;
						};

						struct Stack : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							ItemPosition SourceInventory;
							ItemPosition TargetStorage;

							uint32 Count;
						};

					}; // CashItemStorage

					struct Pvp
					{
						struct PvpLeaveStage : public IPacketSerializable
						{
							NET_SERIALIZABLE;
						};
					};

					struct Mail
					{
						struct RequestMailList : public IPacketSerializable
						{
							NET_SERIALIZABLE;
						};

						struct ReadMail : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint64		Index;
						};


						struct SendMail : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							String								Receiver;
							String								Message;
							uint32						AttachedMoney;
							std::map< uint8 /*MailSlot Position*/, XRated::ItemPosition /*Item Position*/ >
								AttachedItems;
							XRated::ItemPosition			StampItemPosition;
						};

						struct DisposeMail : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint64		Index;
						};

						struct DisposeReadMails : public IPacketSerializable
						{
							NET_SERIALIZABLE;
						};

						struct RetrieveAttached : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint64		Index;
						};
					}; // Mailbox System
					struct Fishing
					{
						struct Start : public IPacketSerializable // Start Fishing
						{
							NET_SERIALIZABLE;
						};

						struct End : public IPacketSerializable // End Fishing
						{
							NET_SERIALIZABLE;
						};

						struct Do : public IPacketSerializable // Trying to 
						{
							NET_SERIALIZABLE;

							uint32 BaitHash;
							XRated::ItemPosition BaitPosition;
							//uint8	BaitCount;
						};

						struct ChangeBaitCount : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint8 Count;
						};

						struct List : public IPacketSerializable // request Fishing info
						{
							NET_SERIALIZABLE;
						};
					};

					struct Gamble
					{
						struct SlimeRace {
							struct Bet : public IPacketSerializable
							{
								NET_SERIALIZABLE;

								XRated::Gamble::BettingPosition Position;

								uint32							Chips;

							};

							struct ClearBet : public IPacketSerializable
							{
								NET_SERIALIZABLE;

								XRated::Gamble::BettingPosition Position;
							};

							struct RequestBettingState : public IPacketSerializable
							{
								NET_SERIALIZABLE;
							};

							struct RequestRecentResults : public IPacketSerializable
							{
								NET_SERIALIZABLE;
							};
						};
					};

					struct SaveKeySetting : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						std::wstring Account;
						std::wstring Keycodes;
					};
					//family
					struct Family
					{

						struct Invite : public IPacketSerializable
						{
							NET_SERIALIZABLE;
							XRated::Serial	TargetSerial;
							std::wstring	Target;
						};

						struct Leave : public IPacketSerializable
						{
							NET_SERIALIZABLE;
						};

						struct InviteAnwer : public IPacketSerializable
						{
							NET_SERIALIZABLE;
							bool IsJoin;
						};

						struct RefreshInfo : public IPacketSerializable
						{
							NET_SERIALIZABLE;
						};

						struct RequestPresent : public IPacketSerializable
						{
							NET_SERIALIZABLE;
							XRated::Family::PresentType	Type;
						};

						//family member
						struct Kick : public IPacketSerializable
						{
							NET_SERIALIZABLE;
							std::wstring		TargetCharacterName;
						};
					};

					struct Compose : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						uint8 Type;
						ItemPosition	Item1;
						ItemPosition	Item2;
					};

					struct PartyLog : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						uint8 LogType;
						uint8 PartyType;
						uint32 StageGroupHash;
						uint16 AccessLevel;
						bool Recommend;

						std::wstring BannedCharacterName;
						int64 PartyStartDuration;
						uint8 PlayerCount;
					};

					//Database
					//ItemInfo::ItemCategory == uint32;


					//OpenMarket
					struct OpenMarket
					{
						// ToServer
						struct RequestProductList : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							int								MinLevel;
							int								MaxLevel;
							std::wstring					Keyword;
							uint32							Category;

							XRated::OpenMarket::OrderType	OrderType;
							Constants::ClassType			ClassType;

							int								PageNumber;
							bool							IsWearable;

						};

						struct RequestProductInfo : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint64		RegistrationNumber;
						};

						struct RequestItemInfo : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							XRated::ItemPosition ItemPosition;
						};


						struct BuyProduct : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint64		RegistrationNumber;
						};

						struct RegisterItem : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							std::wstring			ItemName;
							XRated::ItemPosition	ItemPosition;
							XRated::ItemPosition	PremiumItemPosition;
							uint64					Price;
							uint32					RegistrationPeriod;
						};

						struct RegistrationCancel : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint64		RegistrationNumber;
						};

						struct RequestMyProductList : public IPacketSerializable
						{
							NET_SERIALIZABLE;

						};
					};

					struct CashShop
					{
						struct RequestCashAmount : public IPacketSerializable
						{
							NET_SERIALIZABLE;
						};

						struct Purchase : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 oidProduct;

							uint32 orderQuantity;

							uint32 oidCouponUser;
						};

						struct Present : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 oidProduct;

							uint32 orderQuantity;

							uint32 oidCouponUser;

							std::wstring receiver;

							std::wstring message;
						};

						struct RegistCoupon : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							std::wstring serial;
						};
					};
				}

			}
		}
	}
}