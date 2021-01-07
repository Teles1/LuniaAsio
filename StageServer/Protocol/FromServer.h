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
				namespace FromServer
				{
					struct Error : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						int			 errorcode;
						std::wstring errorstring;
					};

					// Way = Who Are You
					struct Way : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						uint32 EncryptKey;
					};

					struct Alive : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						uint32 EncryptKey;
						uint64	RequestTime;
					};

					struct Stage : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::wstring charactername;
						XRated::StageLicense targetStage;
					};

					struct Floor : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						int floor;
					};

					struct Granted : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::wstring charactername;
						XRated::StageLicense targetStage;
					};

					struct CharacterInfo : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::Constants::ClassType classtype;
						unsigned short level;
						unsigned int xp;
						unsigned short pvpLevel;
						unsigned int pvpXp;
						unsigned short warLevel;
						unsigned int warXp;
						unsigned short storedLevel;
						unsigned short rebirthCount;
						unsigned int money;
						unsigned int bankMoney;
						unsigned short life;
						unsigned short skillpoint;
						unsigned short addedSkillPointPlus;
						unsigned short storedSkillPoint;
						unsigned short extrabag;
						unsigned short extrabank;
						uint32	ladderPoint;
						uint16	ladderMatchCount;
						uint32	ladderWinCount;
						uint32	ladderLoseCount;
						uint32 achievementScore; // 3.1 
						bool IsSpectator;
					};

					struct BonusLife : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int bonusLife;
						unsigned int usableBonusLife;
					};

					struct ListQuickSlot : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::vector< XRated::QuickSlot > quickslotlist;
					};


					struct ListSkill : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::vector< XRated::Skill > listskill;
					};

					struct ListSkillLicenses : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::vector< uint32 > listSkillLicenses;
					};

					struct ListItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::vector< XRated::ItemSlot >	listitem;
					};

					struct LoadEnd : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::wstring charName;
						float progress; // 100% = 1.0f
					};


					struct Chat : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int playerserial;
						XRated::Constants::ChatTypes chattype;
						std::wstring message;
					};

					struct SpectatorChat : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::wstring characterName;
						XRated::Constants::ChatTypes chattype;
						std::wstring message;
					};


					struct Voice : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int playerserial;
						unsigned short chattype;
						unsigned short messageid;
					};

					struct StageMove : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::string serverAddress;
						int portNumber;
						std::string keyCode;
					};

					struct Cast : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						enum Result
						{
							Success = 0,
							NotEnoughSlot,
						};

						Result result;
						unsigned int playerserial;
						uint32 skillgroupid;
						unsigned short skilllevel;
					};

					struct SetSkillLevel : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 skillgroupid;
						unsigned short skilllevel;
						unsigned short LeftSkillPoint;
					};

					struct CreatePlayer : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int playerserial;
						XRated::Constants::ClassType classtype;
						std::wstring charactername;
						unsigned short level;
						unsigned short pvpLevel;
						unsigned short warLevel;
						unsigned short storedLevel;
						unsigned short rebirthCount;
						uint32 ladderPoint;
						uint16 ladderMatchCount;
						uint32 ladderWinCount;
						uint32 ladderLoseCount;
						uint32 achievementScore;
						unsigned short addedSkillPointByRebirth;
						float3 position;
						float3 direction;
						float hp;
						unsigned short team;
						struct Equipment : public Serializer::ISerializable
						{
							uint16 Position;
							uint32 ItemHash;
							InstanceEx instanceEx;

							virtual void Serialize(Serializer::IStreamWriter& out) const;
							virtual void Deserialize(Serializer::IStreamReader& in);
						};
						std::vector< Equipment > Equipments;
						std::vector< uint32/*item hash*/ > PassiveItems;

						std::vector< XRated::StateFlag > stateflags;
						bool shopping;
						std::vector< XRated::StageLicense> stageLicenses;
						uint8 lives;
						uint8 bonusLife;
						XRated::CharacterStateFlags CharacterStateFlags;
						DateTime lastRebirthDateTime;

						std::wstring partyChannelName;
						float eventExpFactor;	///< ServerInfo.BaseExp
					};

					struct CreateNonPlayer : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int objectserial;
						uint32 charactername;
						unsigned short level;
						float3 position;
						float3 direction;
						float hp;
						unsigned short team;

						int playerCntWhenCreated;
					};

					struct CreateItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int objectserial;
						uint32 itemname;
						float3 position;
						float3 direction;
						std::wstring owner;
						uint16 stackCount; // 3.1 
						bool isPrivateItem;
					};
					struct DiceItemCreated : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						std::vector< std::pair<Serial, int> > result;
					};

					struct CreateProjectile : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int objectserial;
						uint32 tilename;
						float3 position;
						float3 direction;
						float3 targetpos;
						unsigned int serial;
						std::wstring owner;
					};

					struct CreateStaticObject : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int objectserial;
						uint32 objectname;
						XRated::Constants::ObjectType type;
						float3 position;
						float3 direction;

					};

					struct DestroyObject : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int Serial;
						XRated::Constants::ObjectType Type;
						bool Silent; ///< 'true' means no effective stuffs like jewels, effects and quest relatives at destorying
						uint8 team;
					};

					struct ChangeAction : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int objectserial;
						uint32 animation;
						float3 position;
						float3 direction;
						float param;
					};

					struct ChangeColState : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int objectserial;
						bool   collision;
						float3 position;
					};

					struct ChangeStatus : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int objectserial;
						float3 position;
						float3 direction;
						float hp;
						float mp;
						Serial bywhom;
						uint32 bywhat;
						unsigned int flag;
						uint32 airComboCount;
					};

					struct UseItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int playerserial;
						uint32 itemid;
					};

					struct UseItemEx : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						enum Results
						{
							Ok,
							Error,
							End
						}Result;
					};


					struct EquipItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::Serial PlayerSerial;
						uint32 ItemHash; ///< could be 0 for unequipping
						uint16 Position; ///< equip position, bag number should be 0
						InstanceEx instanceEx;
					};

					struct EquipmentSwapped : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint8	NewIndex;
						XRated::Serial PlayerSerial;
						std::vector< XRated::EquippedItem > NewEquipments;
						enum Results
						{
							Ok,
							Error
						}Result;
					};

					struct CashEquipmentSwapped : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint8 NewIndex;
						XRated::Serial PlayerSerial;
						std::vector< XRated::EquippedItem > NewEquipments;
						enum Results
						{
							Ok,
							Error
						}Result;
					};


					struct PetInfo : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						//XRated::Serial ownerserial;
						std::vector<XRated::PetDataWithItemPos> PetDataWithPos;
					};

					struct PetUpdated : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::Serial PlayerSerial;
						std::wstring   PetExpression;
						XRated::Serial PetSerial;
						Pet PetData;
						XRated::ItemPosition PetItemPosition;
					};

					struct PetEquip : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::Serial PlayerSerial;
						XRated::Serial PetObjectSerial;
						uint32 ItemHash; ///< could be 0 for unequipping
						uint16 Position; ///< equip position, bag number should be 0
						InstanceEx instanceEx;
					};


					struct PetEmotion : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::Serial PetSerial;
						XRated::Pet::Feeling::State FeelingState;
					};

					struct PetLevelUp : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::Serial PlayerSerial;
						XRated::Serial PetSerial;
						Pet PetData;
					};

					struct UpdateItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition Position;
						uint32 ItemHash;
						InstanceEx instanceEx;
					};

					struct ListPetItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::map< XRated::GlobalSerial, std::vector< XRated::PetItemSlot > >	PetsItems;
					};

					struct MoveItemInventoryToPet : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						XRated::GlobalSerial	PetSerial;
						XRated::ItemPosition	Inventory;
						XRated::PetItemSlot::PositionType PetSlotType;
						uint8					PetSlotPosition;
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

					struct PetsCaredBySchool : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						XRated::Serial                        OwnerSerial;
						std::vector<XRated::PetCaredBySchool> CaredPets;
					};

					struct CaredPetUpdated : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						XRated::Serial              OwnerSerial;
						XRated::PetCaredBySchool	CaredPet;
					};

					struct CaredPetTakenOut : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						XRated::Serial              OwnerSerial;
						XRated::GlobalSerial        CaredPetSerial;
					};

					struct RemovePet : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						XRated::GlobalSerial PetSerial;
					};

					struct TakeOutAllPetItems : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::GlobalSerial PetSerial;
						enum Results
						{
							Ok = 0,
							InvalidPetSerial,
							InventorySlotInsufficient,
							InventorySlotFull,
							PetAppear,
							PetInventoryEmpty,
							Unknown,
						}Result;
					};

					struct DropItemInPetInventory : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::GlobalSerial PetSerial;
						XRated::PetItemSlot::PositionType PositionType;
						uint8 Position;
						uint16 StackCount;
					};

					struct LevelUp : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int playerserial;
						unsigned short level;
						//bool bStory;
						XRated::Constants::ExpType expType;
					};

					struct PlayerDied : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int playerserial;
						unsigned int hitplayerserial;
					};

					struct Revive : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int playerserial;
						float hp;
						float mp;
						unsigned short life;
						unsigned short bonusLife;
						unsigned short usableBonusLife;
						uint32 reviveCount; // 3.1 
					};

					struct InstantCoinRevive : public IPacketSerializable
					{
						enum Results {
							Ok = 0
							, FailedShopDBError = 1
							, FailedCGPointError = 2
							, FailedShopOrderError = 3
							, NoResponse = 0xff,
						} Result;
						NET_SERIALIZABLE;
					};


					struct SkillPoint : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned short skillpoint;
					};

					struct SkillPointPlus : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned short skillpoint;
						unsigned short addedSkillPointPlus;
						unsigned short storedSkillPoint;
					};

					/// report money current money status
					struct Money : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 CurrentMoney;
					};

					struct PvpLevelExp : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned short pvpLevel;
						unsigned int pvpXp;
					};

					struct MissionClear : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						bool success;
						float elapstime;
						int foundsecret;
						float eventExpFactor;	
						int bossItemGainCount;	
						uint32 ladderPoint;		
						//uint16 allPlayerReviveCount;

						std::map<Serial/*Player*/, XRated::MissionResultInfo> playerInfos;
					};

					struct MissionClearReward : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::vector<XRated::RewardItem> bossItems;
						std::vector<XRated::RewardItem> bonusItems;
						std::vector<XRated::RewardItem> eventItems; // 3.1 
					};

					struct MoveItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned short pbag;
						unsigned short ppos;
						unsigned short nbag;
						unsigned short npos;
					};

					struct StackItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned short sbag;
						unsigned short spos;
						unsigned short tbag;
						unsigned short tpos;
						uint32 count;
					};

					struct DropItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned short bag;
						unsigned short pos;
						uint16 DroppedCount; /// number of items that would be dropped from the inventory slot // 3.1 
						uint16 StackedCount; /// number of items that would be left in the inventory slot, 0 means empty slot // 3.1 
					};

					struct AcquireItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 ItemId;
						unsigned short Bag;
						unsigned short Pos;
						uint16 StackedCount;
						InstanceEx instanceEx;
						uint64 ItemSerial;
					};

					struct NoticeAcquireItemInfo : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						Serial PlayerSerial;
						uint32 ItemId;
						InstanceEx instanceEx;
					};

					struct AcquirePvPItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int playerserial;
						uint32 itemid;
					};

					struct UsePvPItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int playerserial;
						uint32 itemid;
					};

					struct DropPvPItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int playerserial;
					};

					struct AddMinimapPing : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int pingid;
						float3 position;
						XRated::Constants::MiniMapPingType type;

					};

					struct RemoveMinimapPing : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						int pingid;
					};

					struct DisplayTextEvent : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::Constants::DisplayTextType type;
						uint16 textId;
						float param;
					};

					struct DisplayText : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::Constants::DisplayTextType type;
						std::wstring message;
					};

					struct DisplayTimer : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::Constants::DisplayTimerType type;
						float seconds;
					};

					struct PlayEvent : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						int eventid;
						std::wstring param;
					};

					struct PlayClientStageEvent : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						int eventId;
						int eventParam;
					};

					struct CreateStructure : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int structureserial;
						std::wstring structurename;
						float3 position;
						float3 direction;
						float currenthp;
						unsigned short team;
					};

					struct CreateVehicle : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int vehicleserial;
						std::wstring vehiclename;
						float3 position;
						float3 direction;
						float currenthp;
						unsigned short team;
					};

					struct Notice : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int serial;
						std::wstring id;
						float3 position;
						float3 direction;
						std::vector< std::wstring > parameters;
					};

					struct PauseObject : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int serial;
						float pausetime;
					};

					struct PlayCinematic : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::wstring cinematic;
					};

					struct XpGain : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						XRated::Constants::ExpAcquiredType type;

						uint64 storyXp;
						int pvpXp;
						int warXp;
						Serial beKilledNpc;
					};

					struct GoldGain : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						XRated::Constants::GoldRewardAcquiredType type;

						unsigned int serial;
						uint32 money;
					};

					struct AcquireLicense : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int serial;
						XRated::StageLicense stageLicense;
						unsigned int sharedOtherPlayers; // 3.1 by ultimate
					};

					struct AcquireSkillLicense : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int serial;
						uint32 skillLicense;
					};

					struct ChangeState : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int serial;
						uint32 id;
						unsigned int level;
						unsigned int byWhom;
						float realTimeDuration; // 3.1 
					};

					struct MoveQuickSlot : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned short ppos;
						unsigned short npos;
					};

					struct VoteStart : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int serial;
						XRated::Constants::VoteTypes type;
						XRated::StageLicense targetStage;
						std::wstring charName;

					};

					struct AutoVoteStart : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::Constants::AutoVoteTypes type;
						XRated::StageLicense targetStage;

					};

					struct StartPersonalVoting : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::Constants::PersonalVoteTypes type;
						std::wstring charName;
					};

					struct ResultPersonalVoting : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::Constants::ResultPersonalVoteTypes type;
						std::wstring charName;
					};

					struct EnterShop : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int playerserial;

						Constants::ShopType shopnumber;
						uint32				param;
					};

					struct LeaveShop : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int playerserial;
						float3 position;
						float3 direction;
					};

					struct Buy : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 itemid;
						uint16 count;
						unsigned short bag;
						unsigned short pos;
						InstanceEx			instanceEx;
						uint32			money;
					};

					struct BuyBack : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 itemid;
						uint16 count;
						unsigned short bag;
						unsigned short pos;
						InstanceEx instanceEx;
						uint32 money;
					};

					struct BuyBarterItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 barterItemId;
						uint16 count;
					};

					struct Sell : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned short bag;
						unsigned short pos;
					};

					struct VoteOn : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned short yes;
						unsigned short no;
					};

					struct Trade : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						enum Results
						{
							Canceled = 0, /// compatible 'false' for old boolean result 
							Completed,

							InvalidTarget, TargetOnTrade, UserOnTrade, NotEnoughSlot, TargetGoldOverflow, UserGoldOverflow
						} Result;
					};

					struct VoteResult : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned short yes;
						unsigned short no;
					};

					struct VoteFailed : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						enum Results
						{
							Timeout = 0,
						} Result;

					};

					struct RequestTrade : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int serial;
					};

					struct AddTradeItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int serial;
						uint32 itemid;
						uint16 count;
						InstanceEx instanceEx;
					};

					struct AddTradePetItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int serial;
						uint32 itemid;
						uint16 count;
						InstanceEx instanceEx;
						XRated::Pet	Pet;
					};

					struct AddTradeMoney : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int serial;
						unsigned int money;
					};

					struct ReadyToTrade : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned int serial;
					};

					struct ConfirmTrade : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::vector<ConfirmTradeInfo> tradePlayers;
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
						enum ResultCode { Ok, OverFlow } Result;
					};

					struct BankMoneyOut : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 BankMoney;
						enum ResultCode { Ok, UnderFlow } Result;
					};

					struct QuickSlot : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						unsigned short number;
						uint32 id;
						bool isskill;
						InstanceEx instanceEx;
					};

					struct Identify : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition Position;
						uint32 ItemHash;
						InstanceEx instanceEx;
					};

					struct Reinforce : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition Position;
						uint32 ItemHash;
						InstanceEx instanceEx;
					};

					struct NoticeSuccessHighLevelReinforcement : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::wstring CharacterName;
						std::wstring ItemName;
						std::wstring Level;
					};

					struct NoticeSuccessHighLevelLightReinforcement : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::wstring CharacterName;
						std::wstring ItemName;
						std::wstring Level;
					};

					struct LightReinforce : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition Position;
						uint32 ItemHash;
						InstanceEx instanceEx;
					};

					struct AttachMagic : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition Position;
						uint32 ItemHash;
						int64 Instance;
					};

					struct Extract : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition Position;
						uint32 ItemHash;
						uint16 Count;
						enum Results { Ok = 0, OutOfSpace } Result;
					};

					struct Alchemy : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::vector<ItemSlot> Products;
						enum Results { Ok = 0, OutOfSpace } Result;
					};

					struct RestoreBelonging : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition Position;
						uint32 ItemHash;
						InstanceEx instanceEx;
					};

					struct RecoverReinforcement : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition Position;
						uint32 ItemHash;
						InstanceEx instanceEx;

						enum Results { Ok = 0, ImpossibleStoneHash, NotEnoughStones, UnknownError } Result;
					};

					struct RecoverLightReinforcement : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition Position;
						uint32 ItemHash;
						InstanceEx instanceEx;

						enum Results { Ok = 0, ImpossibleStoneHash, NotEnoughStones, UnknownError } Result;
					};

					struct ResetIndentification : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						XRated::ItemPosition Position;
						uint32 ItemHash;
						InstanceEx instanceEx;

						enum Results { Ok = 0, ImpossibleScrollHash, NotEnoughScrolls, UnknownError } Result;
					};

					struct Terminate : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						enum Results { Ok = 0, NoResponse = 0xff, } Result;
					};

					struct ListCashItems : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						int Total;
						std::vector< XRated::CashItem > Items;
					};

					struct PurchaseCashItem : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						enum Results { Ok = 0, NotEnoughCoin, WrongOidProduct, UnknownError, QuantityOverDue, PeriodExceeded, CannotPurchase, NotOnSale, PeriodExceededGoods } Result;
					};

					struct CashItemMove : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						enum Results { Ok = 0, NotEnoughInventory, NoPermissions, NoResponse = 0xff, } Result;

						std::vector< XRated::ItemSlot > Items;
					};

					struct CashItemRefund : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						enum Results { Ok = 0, InvalidOrder, OrderFail, BillingFail, InvalidAccount, OnlyCashPurchasedItem, AlreadyReceived, TooLateForRefund, Unkown } Result;
					};

					struct CashItemView : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::map< XRated::Serial /*player serial*/, std::pair< uint16, uint16 > /*cash item flag*/> userlist; // 3.1 
					};

					struct Compose : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						enum Results { Ok = 0, NoResponse = 0xff, } Result;
						int Type;
						uint32 GradeState;
						XRated::ItemSlot ItemSlot;
					};


					struct BagStates : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::vector<BagState> Bags;
						std::vector<BagState> BankBags;
					};

					struct ChangedBagStates : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::vector<BagState> Bags;
						std::vector<BagState> BankBags;
					};

					struct ChangeBagState : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						bool IsBank;
						XRated::BagState Bag;
					};

					struct Pause : public IPacketSerializable	// broadcasting
					{
						NET_SERIALIZABLE;

						bool Paused; ///< @return false if the pausing is released.
					};

					struct CharacterSlot : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 NumberOfSlots; ///< number of character slots of the account
					};

					struct AcquireCharacterLicense : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						enum Result { OK, AlreadyExist, InvalidCharacterLicense } result;
						bool ByItem;
						XRated::Constants::ClassType CharacterClass;
						AcquireCharacterLicense() : ByItem(false) {}
					};

					struct Tame : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 OwnerSerial;
						uint32 TamedSerial; ///< should be existing NPC in the stage
						Constants::Familiar::Type FamiliarType; ///< ???
						String TamedName; ///< @remarks if this is empty, use 'DisplayName' of the NPC 
					};

					struct PetTame : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 OwnerSerial;
						uint32 TamedSerial; ///< should be existing NPC in the stage
						Constants::Familiar::Type FamiliarType; ///< ???
						String TamedName; ///< @remarks if this is empty, use 'DisplayName' of the NPC 
						Pet PetInfo;
						std::vector< XRated::Item >	Equipments;
					};

					struct CheckJoinToSecretStage : public IPacketSerializable ///< notice that the joining of the guild is permitted (broadcasting)
					{
						NET_SERIALIZABLE;

						enum Results {
							Exist = 0, NonExist
						} Result;
					};

					struct Random : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 PlayerSerial; ///< who made random
						String RandomName; ///< somehting like 'dice', 'coin' or so
						uint16 MinValue; ///< minimum random value to be requested
						uint16 MaxValue; ///< maximum random value to be requested
						uint16 RandomValue; ///< choosen random value by server
					};

					struct Upgrade : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						enum Results
						{
							OK = 0, InvalidItemPosition, InvalidPotion, InvalidTargetItem, NoResponse = 0xff
						} Result;
					};
					struct Rename : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						enum Types { Character, Guild, } TargetType;
						enum Results { OK = 0, InvalidCharacter, AlreadyExistName, AlreadyReversedUser, UnknownError } Result;
					};

					struct ChangeBaseExpFactor : public IPacketSerializable //exp factor 
					{
						NET_SERIALIZABLE;
						float ExpFactor;
					};

					struct AllMGuild
					{
						struct SetPlayer : public IPacketSerializable ///< broadcasting
						{
							NET_SERIALIZABLE;

							uint32 Serial;			///< player serial as a key

							AllMGuildInfo GuildInfo;
						};

						struct Create : public IPacketSerializable ///< notice that the guild is created. @remarks SetPlayer will be broadcasted right after Create packet
						{
							NET_SERIALIZABLE;

							enum Results {
								Ok = 0, InvalidUser, FailedToCreateGuild,
								AlreadyExistingGuildId, AlreadyExistingGuildName, JoinedAnotherGuild,
								Unknown = 99
							} Result;

							int guildId;		///< guildId
						};

						struct Invite : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, NoRight, InvalidTarget, AlreadyJoined } Result;
						};

						struct InvitedFrom : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							int GuildId;
							String GuildName;
							String InvitorCharacterName;
						};

						struct InviteReply : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Deny = 0, Timeout = 1 } Result;

							String TargetCharacterName;
						};

						struct Join : public IPacketSerializable ///< notice that the joining of the guild is permitted (broadcasting)
						{
							NET_SERIALIZABLE;

							enum Results {
								Ok = 0, GuildNotExisting, AlreadyJoined, InvalidMemberGrade, UnableToInviteAsGuildMaster, TooManyGuildMember,
								Unknown = 99
							} Result;
							int guildId;		///< guild oid
						};

						struct Kick : public IPacketSerializable ///< only response by erros
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, InvalidGuildId = 1, InvalidTarget, UnableToKickMaster, InvalidUser, InvalidMemberGrade, HaveNoPermission, NotConnected } Result; // no okay
						};

						struct Leave : public IPacketSerializable ///< broadcasting
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, InvalidUser, Failed } Result;
							uint32 Serial;
							bool Kicked;
						};

						struct ListMembers : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, InvalidUser, Failed } Result;
							std::vector<AllMGuildUserInfo> Members;
							bool isSeparate; // 3.1 
						};

						struct Remove : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results {
								Ok = 0, InvalidUser, Failed,
								GuildNotExisting, MemberExist, MemberInWaitingList, AssociateMemberExist,
								AssociateMemberinWatingList,
								Unknown = 99
							} Result;
						};

						struct ValidateName : public IPacketSerializable /// result of guild name validation
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, AlreadyUsed } Result;
						};

						struct GuildRankList : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, Failed } Result;
							std::vector< GuildRankInfo > RankList;
						};
						struct ValidateAlias : public IPacketSerializable /// result of guild id validation
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, AlreadyUsed } Result;
						};
						struct AddedGuildExp : public IPacketSerializable // result guild exp and level and my guild exp.
						{
							NET_SERIALIZABLE;

							uint8 GuildLevel;
							uint32 GuildExp;
							float PlayTime;
							bool ChangedLevel;
							bool IsByItem;
							DateTime StartedTimeToPlay;
							enum Results { Ok = 0, Failed } Result;
						};
						struct AddedRankingPoint : public IPacketSerializable // result guild RankingPoint	/// by kpongky( 09.07.08 ) BMS 6705
						{
							NET_SERIALIZABLE;

							uint32 OldRankingPoint;
							uint32 NewRankingPoint;

							enum Results { Ok = 0, Failed } Result;
						};
						struct AddedGuildMaintenancePoint : public IPacketSerializable //guild maintanance point , available days
						{
							NET_SERIALIZABLE;
							uint32 GuildMaintenacePoint;
							uint32 IncreasePoint;
							DateTime	ShopOpenDate;
							DateTime	ShopCloseDate;
							enum Results { Ok = 0, Failed } Result;

						};
						struct GuildShopItemList : public IPacketSerializable //guild maintanance point , available days
						{
							NET_SERIALIZABLE;
							std::vector<GuildShopItem>	GuildShopItems;
						};
						struct AddedGuildShopItem : public IPacketSerializable //guild maintanance point , available days
						{
							NET_SERIALIZABLE;
							GuildShopItem		Item;
						};
						struct AddExpFactorByItem : public IPacketSerializable //exp factor 
						{
							NET_SERIALIZABLE;
							float ExpFactor;
						};

						struct TaxGained : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32			Tax;
							DateTime	TaxPayDate;

							enum Results { Ok = 0, NotEnoughTax, OverMaxGold, NotMaster, NotElapsedPayDate } Result;
						};

						struct TaxChanged : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32			Tax;
						};

						struct MessageChange : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, GuildNotExisting = 1, InvalidUser = 2, InvalidGrade = 3, HaveNoPermission = 5 } Result;
						};

						struct ChangeGrade : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, GuildNotExisting = 1, InvalidUser = 2, InvalidGrade = 3, HaveNoPermission = 5 } Result;
							String TargetCharacterName;
							uint8 Grade;
						};

						struct ChangeGradeName : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, GuildNotExisting = 1, InvalidUser = 2, InvalidGrade = 3, HaveNoPermission = 5 } Result;
							uint8 TargetGrade;
							String TargetGradeName;
						};

						struct ChangeGradeAuth : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, GuildNotExisting = 1, InvalidUser = 2, InvalidGrade = 3, HaveNoPermission = 5 } Result;
							uint8 TargetGrade;
							uint8 Authority;
						};

						struct ChangeGuildMaster : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, GuildNotExisting = 1, InvalidUser = 2, InvalidGrade = 3, HaveNoPermission = 5 } Result;
							String TargetCharacterName;
						};

						struct LevelUp : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, GuildNotExisting = 1, InvalidUser = 2, InvalidLevel = 3, HaveNoPermission = 5 } Result;
							int GuildId;
							uint8 Level;

						};
					}; // AllM Guild



					struct Quest
					{
						struct WorkingList : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							std::vector< XRated::Quest > Quests;
						};

						struct CompletedList : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							//std::vector<uint32/*Quest hash*/> Quests;
							std::map<uint32/*Quest hash*/, uint32/*Completed Count*/> Quests;
						};

						//quest complete count (use from quest share system)
						struct CompletedCount : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 QuestHash;
							uint32 Count;
						};

						struct Offer : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 QuestHash;
						};

						struct Error : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 QuestHash;
							XRated::QuestErrorType::Type Result;
							//enum Results
							//{
							//	NoError=0,
							//	NotEnoughInventory,
							//	NotEnoughMoney,
							//	AlreadyCompletedQuest,
							//	PrecededQuestRequired,
							//	CompletedTooManyTimes,
							//	AlreadyWorking,
							//} Result;
						};

						struct Accept : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 QuestHash;
							uint8 NewState;		// one of XRated::Quest::State members

							DateTime::Date	ExpiredDate;
						};

						struct ChangeState : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 QuestHash;
							uint8 NewState;		// one of XRated::Quest::State members
							uint32 Parameter;
							uint32 CompletedCount;
						};

						/// notice to drop the quest
						struct Drop : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 QuestHash;
						};

						/// update a single quest including state and parameters
						struct Update : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							XRated::Quest Quest;
						};

						struct ActivityItem : public IPacketSerializable
						{
							NET_SERIALIZABLE;
						};

						struct ShowDetailWindow : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 QuestHash;
							XRated::QuestErrorType::Type Result;
							//enum Results
							//{
							//	NoError=0,
							//	InvalidUser,
							//	OnlyScript,
							//	MoreCharacterStatusRequired,
							//	NotEnoughItems,
							//	NotEnoughLicense,
							//	PrecededQuestRequired,
							//	AlreadyCompletedQuest,
							//	AlreadyWorking,
							//	CompletedTooManyTimes,
							//} Result;
						};

						struct Share : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 QuestHash;
							std::wstring ShareOwner;
						};

						struct ShareResult : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 QuestHash;
							uint32 SuccessPlayerCount;				
						};

					}; // Quest

					struct QuestEvent
					{
						struct EnableQuestEvent : public IPacketSerializable
						{
							NET_SERIALIZABLE;
							uint8 EventQuestID;
							bool Enable;
							std::wstring EventExplain;
						};

						struct NoticeQuestEvent : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Notice
							{
								DeleteWorkingQuestBecauseNotEventPeriod = 0,
							};

							Notice EventNotice;
						};

					}; //QuestEvent

					struct NpcDropEvent
					{
						struct EnableNpcDropEvent : public IPacketSerializable
						{
							NET_SERIALIZABLE;
							uint8 EventID;
							bool Enable;
							std::wstring EventExplain;
						};
					};

					struct PlayerStore
					{
						/** should be broadcast when the user start player store and send to the user who joined the square */
						struct Open : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 PlayerSerial;
							uint32 StoreItemHash;
							String Title;
						};

						/** should be broadcast when the user stopped player store */
						struct Close : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 PlayerSerial;
						};

						/** response of the items to sell */
						struct List : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint32 PlayerSerial;
							std::vector< StoreSlot > Slots;
							std::vector< StoreSlotForPet > SlotsForPet;
						};

						/** response of buying an item to seller */
						struct Buy : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, AlreadySomeoneBought, NotExists } Result;

							uint32 Buyer;
							StoreSlot Item;
							ItemPosition Position;		///< item position to be put or taken by buying and selling
							uint32 CurrentMoney;
						};

					}; // PlayerStore

					struct Bank
					{
						struct List : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint8	BagNumber;
							std::vector< ItemSlot > Items;
						};

						struct Push : public IPacketSerializable // event a pushed cash item into storage from inventory
						{
							NET_SERIALIZABLE;

							ItemPosition SourceInventory;
							ItemPosition TargetStorage;
							uint16 Count;
						};

						struct Pop : public IPacketSerializable // event a pop cash item from storage to inventory
						{
							NET_SERIALIZABLE;

							ItemPosition SourceStorage;
							ItemPosition TargetInventory;
							uint32 ItemHash; // 3.1 
							uint16 Count; //
						};

						struct SwapItemInStorage : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							ItemPosition SourceStorage;
							ItemPosition TargetStorage;
							uint32 ItemHash; // 3.1 
							uint16 Count; //
						};

						struct Move : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							ItemPosition SourceInventory;
							ItemPosition TargetStorage;
						};
					}; // Bank


					struct CashItemStorage
					{
						struct List : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							// TODO : should this ItemUnit go into XRated.h ?
							struct ItemUnit : public Serializer::ISerializable
							{
								uint64 ItemSerial;
								InstanceEx instanceEx;
								uint32 ItemHash;
								uint16 StackedCount;

								void Serialize(Serializer::IStreamWriter& out) const;
								void Deserialize(Serializer::IStreamReader& in);
							};

							uint8 PageIndex;
							std::vector<std::pair<uint8/*position*/, ItemUnit> > Items;
						};

						struct Push : public IPacketSerializable // event a pushed cash item into storage from inventory
						{
							NET_SERIALIZABLE;

							ItemPosition SourceInventory;
							ItemPosition TargetStorage;
						};

						struct Pop : public IPacketSerializable // event a pop cash item from storage to inventory
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

					struct	AskContinueStage : public IPacketSerializable
					{
						NET_SERIALIZABLE;
					};


					struct PvpMissionCleared : public IPacketSerializable
					{
						bool isWin;
						XRated::PvpSubInfo::Type subInfo;
						NET_SERIALIZABLE;
					};

					struct	Pvp
					{
						struct	PlayerDie : public IPacketSerializable
						{
							Serial	ObjectSerial;
							Serial	FromSerial;

							NET_SERIALIZABLE;
						};

						struct	PlayerSave : public IPacketSerializable
						{
							Serial	ObjectSerial;
							Serial	FromSerial;

							NET_SERIALIZABLE;
						};

						struct	MatchResultInfo : public IPacketSerializable
						{
							std::map<Serial, int>	SerialDieCount;
							std::map<Serial, int>	SerialKillCount;

							NET_SERIALIZABLE;
						};

						struct	NotifySpectatorInOut : public IPacketSerializable
						{
							std::wstring CharacterName;
							bool Entered;
							NET_SERIALIZABLE;
						};

						struct NotifySpectators : public IPacketSerializable
						{
							std::vector< std::wstring > Spectators;
							NET_SERIALIZABLE;
						};

						struct BattleGroundInfo : public IPacketSerializable
						{
							XRated::PvpRoomInfo::BattleGroundInfoType Info;
							NET_SERIALIZABLE;
						};


						struct NotifyBattleGroundInfo : public IPacketSerializable
						{
							std::map< uint8 /*team*/, uint8/*KillCount*/> TeamlKillInfo;
							float RemainTime; // sec
							NET_SERIALIZABLE;
						};

						struct PvpLeaveStage : public IPacketSerializable
						{
							NET_SERIALIZABLE;
						};
					};

					struct StylePoint
					{
						struct ChangeState : public IPacketSerializable
						{
							XRated::StylePoint::State State;
							uint32	CurrentPoint;

							NET_SERIALIZABLE;
						};

						struct ActionType : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							XRated::Constants::StylePointType Type;
						};

					};

					struct Dailyitem {
						struct ConnectedWeek : public IPacketSerializable
						{
							DateTime::Week::type	Week;
							DateTime::Date	ConnectedDate;
							NET_SERIALIZABLE;
						};
					};

					struct GameAddictsPrevent : public IPacketSerializable {
						NET_SERIALIZABLE;

						enum Results { Disable = 0, TimeLeft, TimeExpired } Result;

						int32 StackedPlayTimeInSec;
					};

					struct RaiseEvent : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::vector<std::wstring> eventInfo;

						DateTime endTime;
					};

					struct Rebirth : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						enum Results { Fail = 0, RebirthFailedNeedMoreDay, Success } Result;

						XRated::Serial PlayerSerial;
						uint16 Level;
						uint16 StoredLevel;
						uint16 RebirthCount;
						uint16 StoredSkillPoint;
						DateTime LastRebirthDateTime;

						std::vector<XRated::StageLicense> UpdatedLicense;
					};

					struct Mail
					{
						struct RequestMailList : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							std::vector< XRated::Mail::HeaderInfo >	MailHeaderList;
							enum Results { Ok = 0, Failed } Result;
						};

						struct ReadMail : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							XRated::Mail::ContentsInfo		MailContents;
							enum Results { Ok = 0, NotExistMail, InvalidUser, Busy } Result;
						};


						struct SendMail : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, NotExistReceiver, MailBoxIsFull, ImpossibleSendToGuestUser, NotExistItem, NotEnoughFee, Busy, RollbackNeeded, InvalidStampInfo, InvenIsLocked, ExceedItemLimit, ForbiddenStageItem, Error } Result;
						};

						struct DisposeMail : public IPacketSerializable
						{
							NET_SERIALIZABLE;


							uint64	Index;
							enum Results { Ok = 0, NotExistMail, InvalidUser } Result;
						};

						struct DisposeReadMails : public IPacketSerializable
						{
							NET_SERIALIZABLE;


							std::vector< uint64 >	Indexes;
							enum Results { Ok = 0, NotExistMail, InvalidUser } Result;
						};

						struct MailAlarmed : public IPacketSerializable
						{
							NET_SERIALIZABLE;
							enum Types { Normal = 0, Event } Types;
						};

						struct RetrieveAttached : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint64	Index;
							uint32	AttachedMoney;
							enum Results { Ok = 0, NotExistMail, InvalidUser, NotExistItem, NotEnoughInventory, OverMaxGold, NotReadMode, Busy, RollbackNeeded, AllItemExpired, SomeItemExpired, RetrieveStageItem, PetInfoNotLoaded } Result;
						};

						struct RollbackFinished : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, UnknownError } Result;
						};

					}; // Mailbox System


					struct Fishing {
						struct Start : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, AlreadyInFishingMode, FishingRodNeeded, NotEnoughInventory, NoFishingArea, ProperRodNeeded, TooManyFishingUsers, Unknown } Result;

							Serial PlayerSerial;
						};

						struct Do : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, NotFishingMode, AlreadyFishingNow, FishingRodNeeded, BaitNeeded, NotBait, NotEnoughInventory, NoFishingArea, ProperRodNeeded, FishingTooFast /*Flood Control*/, Error } Result;

							Serial PlayerSerial;
						};

						struct FishingResult : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, Failed, FishingRodNeeded, BaitNeeded, NotEnoughInventory, TooManyBait, NoFishingArea, ProperRodNeeded, UnknownError } Result;

							Serial PlayerSerial;
							// Use AcqureItem packet when sending acquired item info.
						};

						struct ChangeBaitCount : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, NotFishingMode, OutOfRange } Result;

							uint16 Count;
						};

						struct List : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							//Item list
							std::deque<XRated::Fishing::FishingInfo> LatestList;
							std::deque<XRated::Fishing::FishingInfo> LatestRareList;
							XRated::Fishing::FishingInfo Epic;
							//rare prob
							//XRated::Fishing::RareProb RareList;
						};

						struct FishingInfo : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							Serial PlayerSerial;
							XRated::Fishing::FishingInfo Info;
						};

						struct End : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results { Ok = 0, NotFishingMode } Result;

							Serial PlayerSerial;
						};

						struct NoticeFishing : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							std::wstring CharacterName;
							std::wstring ItemName;
						};
					};

					struct Gamble
					{
						struct SlimeRace {
							struct GameStart : public IPacketSerializable
							{
								NET_SERIALIZABLE;
								std::vector< std::wstring > NotEnoughChipsUsers;
							};

							struct GameEnd : public IPacketSerializable
							{
								NET_SERIALIZABLE;

								uint8					WinnerNumber;
								std::vector< Serial >	Winners;
								Serial					JackPotCharacter;

								uint32					EarnedChips;
								uint64					EarnedMoney;
								bool					ReceivedMoneyByMail;

							};

							struct NoMoreBet : public IPacketSerializable 
							{
								NET_SERIALIZABLE;

								std::vector< std::wstring > NotEnoughChipsUsers;
							};

							struct ResetGame : public IPacketSerializable
							{
								NET_SERIALIZABLE;

								DateTime					NextGameTime;
							};

							struct Bet : public IPacketSerializable
							{
								NET_SERIALIZABLE;

								enum Results { Ok = 0, NotEnoughChips, WrongPosition, BettingLocked, GameAddictsPrevent } Result;

								String							CharacterName;
								XRated::Gamble::BettingPosition	Position;
								uint32							Chips;

							};

							struct ClearBet : public IPacketSerializable
							{
								NET_SERIALIZABLE;

								enum Results { Ok = 0, NotBetChips, WrongPosition, DuringRace } Result;

								String							CharacterName;
								XRated::Gamble::BettingPosition	Position;
								//uint32					Chips;
							};

							struct SentBettingState : public IPacketSerializable
							{
								NET_SERIALIZABLE;

								enum Results { Ok = 0, UnknownError } Result;
								std::map< uint16 /*Position*/, XRated::Gamble::BettingState /*bettingInfo*/ > BettingStates;
							};

							struct SentRecentResults : public IPacketSerializable
							{
								NET_SERIALIZABLE;

								enum Results { Ok = 0, UnknownError } Result;

								bool						bBettingPossible;
								DateTime				NextGameTime;
								std::list< uint8 /*WinnerNumber*/ >	Results;

							};
						};

					};

					struct NoticeRaisePopupStage : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::wstring CharacterName;
						uint32		 StageGroupHash;
						uint16		 AccessLevel;
					};

					struct NoticeMythTowerFloor : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::wstring CharacterName;
						std::wstring Floor;
						uint32		 StageGroupHash;
						uint16		 AccessLevel;
					};

					struct PvPRewardInfo : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 WinnerHash;
						uint32 WinnerCount;
						uint32 LoserHash;
						uint32 LoserCount;
					};

					struct PvPRewardItemGain : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						uint32 PlayerSerial;
						bool MailSupport;
						uint32 Hash;
						uint32 Count;
					};

					//family
					struct Family
					{
						struct Info : public IPacketSerializable
						{
							NET_SERIALIZABLE;
							typedef std::vector<XRated::Family::MemberInfo> FamilyMemberList;

							XRated::Family::Info	FamilyInfo;
							XRated::Family::RewardCondition	Condition;
							FamilyMemberList		Members;

						};

						struct InviteResult : public IPacketSerializable
						{
							NET_SERIALIZABLE;
							enum Type
							{
								Success,							
								Failed_JoinedFamilyMember,			
								Failed_NowSameFamilyDecisionWait,	
								Failed_NowOtherFamilyDecisionWait,	
								Failed_CanNotFoundUser,				
							};
							Type	Result;
						};

						struct RequestToInvite : public IPacketSerializable
						{
							NET_SERIALIZABLE;
							std::wstring FromOwner;			
						};

						struct JoineResult : public IPacketSerializable
						{
							NET_SERIALIZABLE;
							enum Type
							{
								Success,				
								Cancel,					
								Failed_NotExistFamily,	
								Failed_OverToFamilyCount,
								Failed_JoinedOtherFamily,
								Failed_JoinedOtherFamilyFromInviteUser,
								Failed_NotReadyJoin,
								Failed_NotReadyJoinFromInviteUser,
								Failed_Unknown,
							};
							Type Result;
							std::wstring InviteOwner;
						};

						struct LeavedResult : public IPacketSerializable
						{
							NET_SERIALIZABLE;
							enum Type
							{
								Success,				
								Failed_NotExistFamily,	
								Failed_NotMemberOfFamily,
								Failed_Unknown,
							};
							Type Result;
						};

						struct TakePresentResult : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							//level up : level, storedLevel up: storedLevel, fishing : fish item hash, memorial day : recieved memorial day.
							uint32	Param;							
							bool IsSuccess;
							XRated::Family::PresentType Type;
							XRated::Family::RewardCondition	Condition;
							std::vector< XRated::Family::FamilyMemberSerial > receiveMembers;
						};

						struct RefreshFailed : public IPacketSerializable
						{
							NET_SERIALIZABLE;
							enum Type
							{
								NotEntredFamily,				
							};
							Type	Result;
						};

						struct KickResult : public IPacketSerializable
						{
							NET_SERIALIZABLE;
							enum Type
							{
								GuestKickSuccess,	
								FixedMemberKickSuccess,
								Failed_NotEntredFamily,
								Failed_IsOnlineState,
							};
							Type	Result;
							std::wstring Target;
						};
					};

					struct OpenMarket
					{
						struct RequestProductList : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							XRated::OpenMarket::ProductInfoList ProductInfos;
							XRated::OpenMarket::OrderType		OrderType;

							int			CurrentPage;
							int			TotalPage;


							enum Results
							{
								OK,
								UnknownError,

							} Result;
						};

						struct RequestProductInfo : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint64		RegistrationNumber;
							uint64		AveragePrice;
							uint32		ReliableCount;

							enum Results
							{
								OK,
								NotExistProduct,
								ProductIsExpired,
								UnknownError,

							} Result;
						};

						struct RequestItemInfo : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint64		AveragePrice;
							uint32		ReliableCount;

							enum Results
							{
								OK,
								InvalidItemPosition,
								InvalidItem,
								UnknownError,

							} Result;
						};


						struct BuyProduct : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							enum Results
							{
								OK,
								NotExistProduct,
								ProductIsExpired,
								NotEnoughMoney,
								UnknownError,

							} Result;
						};



						struct RegisterItem : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint64		RegistrationNumber;

							enum Results
							{
								OK,
								InvalidItemPosition,
								InvalidItem,
								InvalidPremiumItemPosition,
								InvalidPremiumItem,
								InvalidRegistrationPeriod,
								NotEnoughFee,
								NonTradableItem,
								Busy,
								UnknownError,

							} Result;
						};

						struct RegistrationCancel : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							uint64		RegistrationNumber;

							enum Results
							{
								OK,
								NotMyProduct,
								NotExistProduct,
								ProductIsExpired,
								UnknownError,

							} Result;
						};

						struct MyProductList : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							XRated::OpenMarket::ProductInfoList ProductInfos;

							enum Results
							{
								OK,
								UnknownError,

							} Result;
						};
					};

					struct ChangeWeather : public IPacketSerializable
					{
						enum Type
						{
							None = 0,//
							Clear = 1,
							Rain = 2,
							Snow = 3,
							Aqua = 4,
						};

						NET_SERIALIZABLE;

						Type type;
						float fadeInTime;
					};

					struct PlayTimeEventInfo
						: public IPacketSerializable
					{
						NET_SERIALIZABLE;
						struct ItemInfo
						{
							uint32 hash;
							uint16 count;
						};
						float currentStackedTodayPlayTime;

						PlayTimeEventRewards playTimeEventRewards;

						LevelUpEventRewards levelUpEventRewards;
					};
					struct CashShop
					{
						struct CashAmount : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							struct Result
							{
								enum Type
								{
									Success = 0,
									Failed = 1,
									InvalidPacket = 999,
								};
							};

							Result::Type result;

							uint32 cashAmount;
							uint32 pointAmount; // 3.1 
						};

						struct PurchaseResponse : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							struct Result
							{
								enum Type
								{
									Success = 0,
									NoProduct = 1,				
									BillingFail = 2,			
									OrderFail = 3,				
									OverMaximumCount = 4,		
									WaitMoreForRepurchase = 5,	
									NomorePurchaseable = 6,		
									NotYetSalePeriod = 7,		
									EndSalePeriod = 8,			
									OverDailyMaximumCount = 9,	
									NotAvailableAgeRate = 10,	
									NoAccount = 99,				
									OverLimitCount = 103,		
									CashLockActivated = 104,	
									InvalidPacket = 999,		
								};
							};

							Result::Type result;
							uint32 availableLevelRate; // 3.1 by Ultimate
						};

						struct PresentResponse : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							struct Result
							{
								enum Type
								{
									Success = 0,
									NoProduct = 1,					
									BillingFail = 2,				
									OrderFail = 3,					
									OverMaximumCount = 4,			
									WaitMoreForRepurchase = 5,		
									NomorePurchaseable = 6,			
									NotYetSalePeriod = 7,			
									EndSalePeriod = 8,				
									OverDailyMaximumCount = 9,		
									NotAvailableAgeRate = 10,		
									SelfPresentNotAllowed = 97,		
									NoReceiverAccount = 98,			
									NoAccount = 99,					
									NotAvailablePresentAge = 101,	
									OverPresentLimit = 102,			
									OverLimitCount = 103,			
									CashLockActivated = 104,		
									CharacterLevelLimitation = 201,	
									NoRecentPurchaseReport = 202,	
									InvalidPacket = 999,			
								};
							};

							Result::Type result;
							uint32 availableLevelRate; // 3.1 by Ultimate
						};

						struct RegistCouponResponse : public IPacketSerializable
						{
							NET_SERIALIZABLE;

							struct Result
							{
								enum Type
								{
									Success = 0,
									CouponError = 1,		
									WrongCoupon = 2,		
									AlreadyRegistCoupon = 3,
									ExpiredCoupon = 4,		
									InvalidPacket = 999,	
								};
							};

							Result::Type result;
						};
					};

					struct AchievementServerAssigned : public IPacketSerializable {
						NET_SERIALIZABLE;

						struct Result
						{
							enum Type
							{
								Ok = 0,
								InvalidPacket = 999
							};
						};
						Result::Type Result;
						std::wstring serverName;
						std::wstring serverAddress;
					};

					struct Token
					{
						struct Update : public IPacketSerializable
						{

							uint32 Token;
						};

						struct RaiseMFCEvent : public IPacketSerializable
						{

							uint32 MfcEventPoint;
						};
					};

					struct VarifySelf
					{
						std::wstring name;
					};

					struct AchievementList : public IPacketSerializable
					{
						NET_SERIALIZABLE;
						std::wstring characterName;
						uint32 mainCategory;
						uint32 subCategory;

						struct {
							uint32 Hash;
						}completedList;

						uint32 totalScore;

						struct Result
						{
							enum Type
							{
								Ok = 0,
								InvalidPacket = 999
							};
						};
						Result::Type Result;
					};

					struct Summary
					{
						std::wstring characterName;
						uint32 mainCategory;
						uint32 subCategory;
					};

					struct AchievementInfo
					{
						std::wstring characterName;
						uint32 hash;
					};

					struct AchievementShowed
					{
						std::wstring characterName;
						uint32 hash;
					};

					struct ChangeStatusLimit : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::map<uint32, uint32> statusLimit;
					};

					struct ChangeStatusRate : public IPacketSerializable
					{
						NET_SERIALIZABLE;

						std::map<uint32, float> statusRates;
					};
				}
			}
		}
	}
}