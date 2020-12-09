#include "Core.h"
#include <Core/Utils/DateTime.h>
#include <Core/ReferenceCounted.h>
#include <Core/Utils/Math/matrix4x4.h>
#include <Core/Utils/Locator.h>

namespace Lunia {
	namespace XRated {
		typedef uint32 Serial;
		typedef uint64 GlobalSerial;
		typedef uint16 Type;

		struct Constants
		{
			struct PartyLogType
			{
				enum
				{
					None,
					Create,
					Join,
					JoinRefused,
					Leave,
					Kicked,
					Start,
				};
			};

			// ĳ���� �̸��� CreateCharacterNameAsciiMinSize, CreateCharacterNameAsciiMaxSize�� ���� �ʴ� ���̸� ������ �ִ� ĳ���Ͱ� ������ �� �ִ�.
			// ������ ���� �������� �ϸ鼭 ĳ���� �̸��� Ư������ ���� �־��ְų� �߱� �����̴�.			
			const static uint32 CreateCharacterNameAsciiMinLength = 4;		///< ĳ���� ������ ĳ���� �̸��� ascii�ڵ� ������ �ּ� ����(*���� : �ش� �������� ĳ���� �����ÿ��� �����ȴ�).
			const static uint32 CreateCharacterNameAsciiMaxLength = 16;		///< ĳ���� ������ ĳ���� �̸��� ascii�ڵ� ������ �ִ� ����(*���� : �ش� �������� ĳ���� �����ÿ��� �����ȴ�).
			const static uint16 PlayerMaxLevel = 99;			///< �Ϲ� �÷��̾� ĳ������ level�� limit.
			const static uint16 PlayerMaxPvpLevel = 60;			///< �Ϲ� �÷��̾� ĳ������ pvp level�� limit.
			const static uint16 PlayerMaxWarLevel = 75;			///< �Ϲ� �÷��̾� ĳ������ War level�� limit.

			const static float Tick;
			const static int MaxTeamCnt = 8;
			const static uint16 MaxLevel = 1100/*99*/;			///< NpcLevel�� 200���� �÷��޶��� ������ �����ż� ����~
			const static uint16 StoredMaxLevel = 0xFFFF; // 65535
			const static uint16 RebirthMaxCount = 0xFFFF; // 65535
			const static int NormalScreenRadius = 100; ///< maya units
			const static float PvPLimitTime; /// ������ '��'�̴�.			

			const static uint32 BasicLadderPoint = 1500; /// PvpLadderPoint�� �⺻���̴�.
			const static uint32 LadderMatchLimit = 10;


			const static uint32 MaxOfSlotsInInventoryBag = 20;			///< �κ��丮 ���� �ϳ��� ������ �ִ� ���� ��.
			const static uint32 MaxOfInventoryBags = 7;				///< �κ��丮 ���� �ϳ������� ���� ��.
			const static uint32 MaxOfBankBags = 7;						///< ���࿡�� ���� ����.			
			const static uint32 MaxOfSlotsInBankBag = 20;				///< ���� ���� �ϳ������� ���� ����.


			enum AddItemType
			{
				Fish,
				NoneAddItemType,
			};

			//��������
			const static uint32 GoldMaxLimit = 4000000000;

			// arcrus. �����Խý��� ���� ����, �ϴ� �̰� ����.
			const static uint32 PostalChargePerAttachedItem = 100;	// 1 silver
			const static uint32 PostalServiceChargePercent = 5;	// Service charge percent.

			enum ServerEventType
			{
				NonEvent,
				ServerRaise,		//���� �츮�� �̺�Ʈ..
			};

			enum ExpFactorCategoryFromItem
			{
				CategoryNone = 0,
				CategoryGuild,
				MaxCategory
			};

			enum ExpAcquiredType
			{
				ExpStage,
				ExpQuest,
				ExpPvp,
				ExpScript,
				ExpNonPlayer,
				ExpPlayer,
				ExpRank,
				ExpReviveCount
			};

			enum ExpType
			{
				Stage, Pvp, War
			};

			enum GoldRewardAcquiredType
			{
				PvPResult,
				GambleGameAllot,
				InstantCoin, //Dropped instant coin item in game.
			};

			enum ObjectType
			{
				Player = 0,
				NonPlayer,
				Structure,
				Projectile,
				Vehicle,
				Misc,
				Item,
				TombStone,
			};

			enum AllianceType
			{
				AnyObjects = 0,
				Team,
				ExTeam,
				Alliance,
				ExAlliance
			};

			enum DamageType
			{
				FIRE = 0,
				WATER,
				ICE,
				LIGHTNING,
				LAND,
				WIND,
				POISON,
				LIGHT,
				CURSE,
				PHYSICAL,
				INDEPENDENCE,
				CNT
			};
			// DamageType�� �߰��� �� �߰��� ���� ������ �ȵȴ�. ������ ������ �����ؾ� ��

			static DamageType StringToDmgType(const std::wstring& str);

			/**
			TODO : �����޺��� üũ�ϱ� ����.. ���� Type�̴�
			�����޺��� �Ϲ� �޺��� �޸�.. 1Ÿ�� �������� NPC�� Player�� �¾Ƶ� 1�޺��� ���ֵǾ��� �ϱ� ������.
			ATTACK State�� DAMAGE State���� ���� ���� üũ�� ������ �ϱ� �����̴�.
			*/
			enum AttackStateType
			{
				ATTACK = 0,
				DAMAGE,
				NONE
			};

			enum GameTypes
			{
				/* cooperative game types */
				StoryGameType = 0x00000001, // one of "NormalScenario", "NightmareScenario" and "HellScenario"
				SideGameType = 0x00000002,
				HardcoreGameType = 0x00000004,

				/* pvp game types */
				DeathMatchGameType = 0x00000100,
				SurvivalGameType = 0x00000200,
				ChaosGameType = 0x00000400,
				BattleGroundType = 0x00000800,
				LadderGameType = 0x00001000,

				/* peaceful game types */
				SquareType = 0x00010000,


				/* masks */
				CooperativeGameTypeMask = 0x000000ff,
				PvpGameTypeMask = 0x0000ff00,
				PeacefulGameTypeMask = 0x00ff0000,
			};

			enum Direction
			{
				LeftDown = 1,
				Down,
				RightDown,
				Left,
				None,
				Right,
				LeftUp,
				Up,
				RightUp,
			};

			struct DirectionF
			{
				static const float3 LeftDown, Down, RightDown, Left, Right, LeftUp, Up, RightUp;
				static const float3& GetDirection(Direction dir);
				static const float3 GetDirection(const float3& dir);
				static const Direction GetDirectionEnum(const float3& dir);
				static const Direction GetRealDirection(Direction myDir, Direction relativeD); //������ ���� �ִ� �������� ���� �������� ������ myDir�� �������� �������ش�.
			};

			enum Command
			{
				Move, Dash, Stop, AttackA, AttackARelease, AttackB, AttackBRelease, DownAttack, DownAttackRelease, Jump,
				Cast, Use, Equip, Revive, InstantCoinRevive, DailyRevive, IncSkill, DecSkill, ResetSkill, ChangeTeam, Join, Part, CreateItem, SetExpFactor, GiveUpRevive, GiveExp, GiveExpWithNoFactor, GivePvpExp, GiveWarExp, GiveStateBundle,
				ControlFamiliar, ChangeFamiliarFormation,
				GameStart, ChangeStylePointState, ChangeStylePointPlayerCount,
				AddSkillPointPlus, AddPassive, RemovePassive, AddGuildUser, RemoveGuildUser, UpdateGuildExp, CompletedQuest, AcceptedQuest, NoticeHolidayEvent,
				ChangePlayTimePenalty, ValidateEquippedItems, SetPvPRank,


				Temp,
				DevControl,

				CheatCommand = 0x20000000,
				MissionClearCommand,
			};

			//�ű�ĳ���߰� : ��ȹ������ ���� ���� ID�� �ش��ϴ� enum�� �߰�. Character�� ������ �Ϳ��� �ٸ� ������ ����.( ��� )
			//���� ���� ������ LuniaQuestTool�� �߰��Ѵ�.
			//LuniaQuestTool/DataHandler/QuestStringUtil.cpp ���� �ű�ĳ���߰��� �˻�
			enum ClassType ///< @remarks keep NumberOfClassTypes as next element of last class type.
			{
				Healer = 0, Knight, Wizard, Thief, Slime, DollMaster, DarkTemplar, IceSorceress, Archer, BountyHunter, Bard
				, DualWield ///�ű�ĳ�� by kpongky( 09.06.09 )
				, Fighter	/// �ű�ĳ�� by kpongky( 09.08.10 )
				, DarkEir	/// �ű�ĳ�� by kpongky( 09.11.19 )
				, Aruta		/// �ű�ĳ�� by kpongky( 10.02.11 )
				, Gaon //�ű�ĳ��
				, Iris //�ű�ĳ��


				, NumberOfClassTypes, AnyClassType = 9999
				, NoClass // for pvp slots
				, Genus_1 = 12000	// for legs
				, Genus_2			// two legs
				, Genus_3			// flying type
				, Genus_4			// 
				, Genus_5			// 
				, Genus_6			// 
				, Genus_7			// 
				, Genus_8			// 
				, Genus_9			// -------------------- arcrus �߰� (���⸦ ��ġ�� xrated.cpp ���ִ� �ϵ� �ڵ��� ���ľ� ��. �̷� �ϵ��ڵ��� ������ ���� -_�� : "arcrus. Pet Genus" �� �˻��غ��ƿ�. =��=)
				, Genus_10			// 
				, Genus_11			// 
				, Genus_12			// 
				, Genus_13			// 
				, Genus_14			// 
				, Genus_15			// 
				, Genus_16			// 
				, Genus_17			// 
				, Genus_18			// 
				, Genus_19			// 
				, Genus_20			//
				, AnyGenus
				, NumberOfGenusClassTypes

			};

			static bool IsDefaultClass(ClassType type);
			static const wchar_t* GetClassStr(ClassType type);
			static const std::wstring GetClassLocaleStr(ClassType type);
			static uint32 GetClassHash(ClassType type);
			//static ClassType GetClassType(const wchar_t* str);

			enum CharacterMajorStatType
			{
				Str = 0,
				Dex = 1,
				Vit = 2,
				Int = 3,
				NumberOfCharacterMajorStatTypes,
				NoMajorStat = 9999
			};

			enum StatusElement
			{
				CurrentHp,
				CurrentMp,
				MaxHp,
				MaxMp,
				CurrentSp,
				MaxSp,
			};

			enum Equipment // equip slots
			{
				Body = 0, Legs, Gloves, Shoes, Head, Weapon, Support, Ring1, Ring2, Necklace,	// == 9
				Earing1, Earing2, CashBody, CashLegs, CashGloves, CashShoes, CashHead, CashWeapon, CashSupport, CashMask,	// == 19
				CashBack, CashHip, CashEtc1, CashEtc2, Pet, FameAdj, FameNoun, //  == 26

				Cnt,
				Pet_Mask = 50,		///< �ϴ� ������ �������� �ʾұ� ������ 1,2�̵������� ����.
				Pet_Etc1,
				Pet_Etc2,
				MaxPetEquipCnt = 3,
			};

			enum EquipFlag
			{
				CASH_Weapon = 1 << 0,
				CASH_Head = 1 << 1,
				CASH_Chest = 1 << 2,
				CASH_Leg = 1 << 3,
				CASH_Hand = 1 << 4,
				CASH_Foot = 1 << 5,
				CASH_Support = 1 << 6,
				CASH_Mask = 1 << 7,
				CASH_Back = 1 << 8,
				CASH_Hip = 1 << 9,
				CASH_Etc1 = 1 << 10,
				CASH_Etc2 = 1 << 11,
				REBIRTHWING = 1 << 12
			};

			enum EquipParts
			{
				WEAPON = 0, HEAD, CHEST, LEG, HAND, FOOT, SUPPORT, NECKLACE, RING, EARING,			// == 9
				CASH_WEAPON, CASH_HEAD, CASH_CHEST, CASH_LEG, CASH_HAND, CASH_FOOT, CASH_SUPPORT, CASH_MASK, CASH_BACK, CASH_HIP, CASH_ETC1, CASH_ETC2,	// == 19
				PET, FAMEADJ, FAMENOUN,

				MaxPartCnt,
				PET_MASK = 50,		///< �ϴ� ������ �������� �ʾұ� ������ 1,2�̵������� ����.
				PET_ETC1,
				PET_ETC2,
				MaxPetPartCnt = 3,
			};

			struct EquipPartsEx
			{
				enum Type
				{
					REBIRTH_WING = static_cast<uint32>(EquipParts::MaxPartCnt),
					CARPET,
				};
			};

			static EquipFlag GetEquipFlag(EquipParts part);
			static bool IsEquippable(Equipment, EquipParts);
			static bool IsAccessory(EquipParts part);
			static Equipment GetEquipPosition(EquipParts part); ///< @remarks in case of ring and earing, returns first slot.
			static EquipParts GetEquipParts(Equipment position);
			static const wchar_t* GetDefaultEquipmentMeshName(ClassType type, EquipParts part);

			enum ChatTypes
			{
				/* local */
				NormalChat = 0,
				EmoteChat,
				TradeChat,
				TradeSellChat,
				TradeBuyChat,
				ShoutChat,

				/* global */
				GlobalShoutChat,
				TextBoardChat,
				LastTextBoardMsg,

				/* to one */
				WhisperChat,
				OneToOneChat,

				/* to group */
				TeamChat,
				GuildChat,

				/* from server */
				SystemChat,
				ErrorChat,

				/* inside gui */
				SystemInfoChat,
				EpisodeChat,
				FamilyChat,

				PartyChat,
				PartyNoticeChat,
				NormalAchievementChat, // achievement chat type - 3.1 by ycr4zy 15-11-2019
				EpisodeAchievementChat, // achievement chat type - 3.1 by ycr4zy 15-11-2019
				AcquireLicenseChat, // 3.1 by ycr4zy 15-11-2019

				ChatTypeSize
			};


			static ChatTypes ConvertStringToChatTypes(const wchar_t* string);
			static const wchar_t* ConvertChatTypesToString(ChatTypes type);

			enum VoiceChatType
			{
				GlobalVoice = 0,
				TeamVoice = 1,
				RadioVoice = 2,
			};

			enum DisplayTextType
			{
				SystemNotice = 0,
				ChatNotice,
				Chat,
				Mission,
				Quest,
				QuestSuccess,
				Debug,
				StageNotice,
			};

			enum DisplayTimerType
			{
				Hide = 0x0000,

				Normal = 0x0001,
				Emergent = 0x0011,

				Countdown = 0x0002,
				EmergentCountdown = 0x0012,
			};

			enum VoteTypes
			{
				CancelVote = 0, // reserved

				ChangePvpVote,	// pvp lobby
				PvpReady,		// pvp lobby
				ChangePvpSetting, // pvp lobby

				ChangeStage,	// stage
				CreateStage,	// square
				ChangeSquare,
				JoinStage,		// square
				MissionClear,	// Play NextStage | Play ThisStage Again
				RePvp,			// pvp
				GoLobby,		// pvp
				Kick,		// kick user
				ReturnToPvpLobbyVote,	///< return go to pvp lobby vote,

				PrivateTeamMatch,	// pvp lobby - call votes 
				WaitVote,			// pvp lobby - wait vote		<-- client�� ������ �ִ� �Ӽ��̴�.. �׷��� ���� �������� �־��� �Ѵ�..
			};

			enum AutoVoteTypes
			{
				Auto_ChangeStage = 0,
			};

			enum PersonalVoteTypes
			{
				AreYouThere = 0, //�ڸ��������� üũ
			};

			enum ResultPersonalVoteTypes
			{
				ExistUser = 0, //�ڸ��� ������.
				DisconnectUser = 1, //©����.
			};

			/*
			enum VoteType  // legacy vote types
			{
			//VoteStage=0, VoteReload, VoteMissionSuccess, VoteMissionFaild, VoteKick,VoteCreateStage,VoteJoinStage,VoteNextStage,
			VoteStage=0, VoteKick, VoteCreateStage, VoteJoinStage, VoteRePvp,

			//VoteStage  = �������� �̵�
			//VoteReload = �������� �ٽ� ����
			//VoteMissionSuccess = �������� �̼� ����
			//VoteMissionFaild   = �������� �̼� ����
			//VoteKick   = ���� �߹�

			//VoteCreateStage = �������� ���� ��ǥ
			//VoteJoinStage = �������� ���� ��ǥ
			//VoteNextStage = ���� ����������

			};
			//*/

			enum VoteResult
			{
				NO = 0,
				YES,
			};

			enum VoteSummary
			{

			};

			enum ShopType
			{
				NoneShopType = -2,
				Decorative = -1,
				Bank = 0,
				WeaponShop,
				ComponentShop,
				ArmorShop,
				SundryShop,
				SmithyShop,
				LaboratoryShop,
				MagicalAttachShop,
				RemoteShop,
				FancyWeaponShop,
				PetSchool,
				GuildShop

				/*
				SmithyShop,				///< ���尣.
				,StrengthenShop			///< ��ȭ.
				,ExtractionShop			///< ����.
				,AppraisalShop			///< ����.
				*/

				, ShopTypeCnt
			};

			enum StageEventType
			{
				EnterShop = 0,
				ShowSelectStage,
				ShowEnterPvP,
				ShowSelectSquare,
				QuickJoinStage,
				ShowGuildOffice,
				ShowBasicControlInfo,
				Directioning,
				GoToBeginnerSquare,
				ShowOpenMarket,
			};

			enum ActivePointType
			{
				NoActive = -1,
				ShowStageSelect = 0,
				ShowPvPSelect,
				ShowTrainingSelect,
				ActivePointTypeCnt
			};

			enum StatusFlag
			{
				Critical = 0x00001,		//ũ��Ƽ��.
				Minimize = 0x00002,
				Maximize = 0x00004,
				IgnoreDmg = 0x00008,
				LessMP = 0x00010,
				CriticalMagic = 0x00020,
				Immune = 0x00040,
				Dodge = 0x00080,
				ResetCooldown = 0x00100,

				//enum DamageType { FIRE=0, WATER, ICE, LIGHTNING, LAND, WIND, POISON, LIGHT, CURSE, PHYSICAL, INDEPENDENCE, CNT };
				Fire = 0x00200,
				Water = 0x00400,
				Ice = 0x00800,
				Lightning = 0x01000,
				Land = 0x02000,
				Wind = 0x04000,
				Poison = 0x08000,
				Light = 0x10000,
				Curse = 0x20000,
				Physical = 0x40000,
				Independence = 0x80000,

				HitIgnore = 0x100000,

				NoDisplay = 0x8000000
			};

			// ��Ÿ�� ����Ʈ(�������̺���)���� Type ����
			enum StylePointType
			{
				MultiHit = 0,
				DownAttackHit, // �̸� ��ġ�� �ʵ��� �ڿ� ������ �̺�Ʈ�� ���ؼ� hit�� ���δ�.
				AirComboHit,
				FantasticSave,
				BackStabHit,
			};
			static const int LimitAirCombo = 3;

			static const int MaxStat = 9999999; //������ �ִ���ġ.
			//���� Ÿ�� 1���� ������.
			static const float MapTileSizeWidth;
			static const float MapTileSizeHeight;
			//1������ �ʼ� ������.
			static const float PIXELPERMAYA;

			//�̴ϸʿ� ping�� ������ ���� �̳�.
			enum MiniMapPingType
			{
				PingNonDelayEvent = 0,
				PingDelayEvent = 1,
				PingPlayerEvent = 2,
				MainMark = 3,
				SubMark = 4,
				PathMarkNonDelay = 5,
				PathMarkDelay = 6,
			};

			struct GhostRules {
				static const float tGhostWhenInstantPop;
				static const float tGhostWhenAutoTimeRevive_Sec10;
				static const float tGhostWhenAutoTimeRevive_Sec5;
				static const float tGhostWhenAutoTimeRevive_Sec3;
				enum Type
				{
					TombStone = 0,
					InstantPop,
					InstantPopAtWill,
					AutoTimeRevive_Sec10,
					AutoTimeRevive_Sec5,
					AutoTimeRevive_Sec3/*�ð��� ������ �ڵ����� ���Ƴ�. �������忡 �����ϱ� ����*/
				};
			};

			enum AttackTargetType
			{
				Ground = 0x01,
				Stand = 0x02,
				Air = 0x04,
				All = 0x07
			};

			static const float CampfireDelayCooperative;
			static const float CampfireDelayPVP;

			struct Condition
			{
				struct ClassType
				{
					const static uint16 NOLIMIT = 0x007F;

					const static uint16 HEALER = 0x0001;
					const static uint16 KNIGHT = 0x0002;
					const static uint16 WIZARD = 0x0004;
					const static uint16 THIEF = 0x0008;
					const static uint16 SLIME = 0x0010;
					const static uint16 DOLLMASTER = 0x0020;

					const static uint16 IceSorceress = 0x0040;
				};
			};

			//Hak �߰�
			//RadioButtonType
			enum RadioButtonType
			{
				QuestWindow = 0,
				QuestDetailWindow = 1,
				NumberOfRadioButtonType,
			};

			enum QuestEffectType
			{
				NoQuest = 0,			//QuestObject�� �ƴϰų� �ƹ��͵� �������� ���� ����.
				RecieveableLevel2 = 1,	//Quest��������2�� Ȯ�� �� �� �ִ� QuestObject�� ��Ÿ��.
				RecieveableLevel1 = 2,	//Quest��������1�� Ȯ�� �� �� �ִ� QuestObject�� ��Ÿ��.
				Recieve = 3,			//Quest�� ���� �� �ִ� QuestObject�� ��Ÿ��.
				Working = 4,			//������ Quest�� �������� ����Ʈ�� �ִ� QuestObject�� ��Ÿ��.
				Failed = 5,
				Success = 6,			//������ ������ Quest�� ���� �ִ� QuestObject�� ��Ÿ��.
				Complete = 7,			//������ ����Ʈ�� �Ϸ�������..
			};
			static const uint32 HiddenQuestStageGroupHash = 33431880; //���� ����Ʈ StageGroup Hash
			static const uint16 HiddenQuestStageGroupLevel = 0; //���� ����Ʈ StageGroup Level

			struct Familiar
			{
				enum Type
				{
					None = -1,
					SoldierOfFortune = 0,
					Summon,
					Pet,
					Doll,
					TypeCount
				};

				const static int MaxCountForEachType[TypeCount];
				const static int MaxCount = 3;

				enum Command
				{
					Attack,
					Come,
					Guard,
					Praise,
					Suicide/*�ڻ�*/,
					CommandCount
				};

				enum Formation
				{
					Follow = 0,
					Surround,
					Side,
					Front,
					FormationCount,
				};
				const static float3 FormationPosition[Formation::FormationCount][MaxCount];
			};

			enum MissionClearRewardType
			{
				NotSelect = 0,
				AType = 1,
				BType = 2,
			};

			struct PlayTimePenalty
			{
				const static uint32 Exp = 0x01;
				const static uint32 Item = 0x02;
				const static uint32 Gold = 0x04;

				enum State
				{
					NoLimit = 0,
					TimeLeft,
					TimeExpired
				};

				typedef uint32	Type;

				const static uint32 LimitInSec = 10800;
				const static uint32 WarningIntervalInSec = 3600;
				const static uint32 WarningIntervalInSecInLimit = 900;
				const static uint32 Penalty = Exp | Item | Gold;
			};

			/**
			*/
			struct LocaleSpecificSetting
			{
				struct Type
				{
					const static int kLocaleID_KR = 0x00000001;	//	Korea
					const static int kLocaleID_JP = 0x00000100;	//	Japan
					const static int kLocaleID_CN = 0x00000101;	//	China
					const static int kLocaleID_TW = 0x00000102;	//	Taiwan
					const static int kLocaleID_US = 0x00000200;	//	Global
				};
				struct CountryCode
				{
					const static int KR = 0;
					const static int JP = 1;
					const static int US = 2;
					const static int TW = 3;
					const static int BR = 4;
					const static int CN = 5;
					const static int EU = 6;
					const static int MY = 7;
				};
				const static int COUNTRY_CODE;
				static int GetLocaleCode()
				{
					return COUNTRY_CODE;
				}

				struct GuildType
				{
					const static int NexonGuild = 0x00;
					const static int AllMGuild = 0x01;
				};
				struct MessengerType
				{
					const static int Nexon = 0x00;
					const static int AllM = 0x01;
				};

				struct WrapType
				{
					const static int WordWrap = 0;
					const static int CharacterWrap = 1;
				};
				const static int WRAPTYPE;
				const static int LIGHTREINFORCEMENT_LEVEL = 4;

				const static int NEXONMODULELOCALE = LocaleSpecificSetting::Type::kLocaleID_TW;
				const static int GUILDTYPE = LocaleSpecificSetting::GuildType::AllMGuild;
				const static bool UseGuildId = false;
				const static int MESSENGERTYPE = LocaleSpecificSetting::MessengerType::AllM;
				const static int USENEXONMODULE = 0;
				const static int USEMANUALLOGIN = 1;
				const static bool USEALLMMESSENGER = true;
				const static bool USEINGAMECASHSHOP = true;
				const static int REINFORCEMENT_LEVEL = 15;

			};
			struct SecurityCard
			{
				const static int MaxRow;
				const static int MaxColumn;
				const static int QuestionCount;
			};

		};

		struct QuestErrorType
		{
			enum Type
			{
				NoError = 0,
				InvalidUser,
				OnlyScript,

				MoreCharacterStatusRequired,
				NotEnoughItems,
				NotEnoughLicense,
				NotEnoughMoney,
				NotEqualAcceptLocation,
				PrecededQuestRequired,
				AlreadyCompletedQuest,
				AlreadyWorking,
				CompletedTooManyTimes,
				AlreadyFullQuestCount,
				FarDistance,

				NotSelectRewardItem,  ///<���ú��� �������� �������� �ʾ��� ����..
				NotEnoughInventory,   ///<�����޴� �������� �κ��丮ĭ���� ���� ����..
				GoldOverflow, ///< ���� ���� ���� ������, ��ü ���� ���Ѻ��� �Ѿ����..
				GameAddictsProtect,
				ShareFailed_NotExistQuest,	///< �����Ϸ��� ����Ʈ�� ������ ���� �ʴ�.
				WaitToResetQuest,			///< �Ⱓ ���� ����Ʈ�� ������ ���ٸ��� �ִ�.				

				NotEventPeriod,
			};
		};

		struct StylePoint
		{
			enum State
			{
				Start = 0,
				Stop,
				Restart,
			};
		};

		struct StageEvent : public Serializer::ISerializable
		{
			int Type;
			int Value;
			float3 Position;
			Constants::Direction Direction;
			float3 Scale;
			Locator Effect; // effect that gonna loop
			float4 Area; // effective area to step into the shop

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct ActivePoint : public Serializer::ISerializable
		{
			Constants::ActivePointType Type;
			Locator Animation; // resource path
			float3 Position;
			Constants::Direction Direction;
			float3 Scale;
			Locator Effect; // effect that gonna loop
			float4 Area; // effective area to step into the shop

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct MissionResultInfo
		{
			uint32	MaxAirCombo;
			uint8	Rank;

			int	ClearXp;
			uint64 StageXp;

			virtual void Serialize(Serializer::StreamWriter& out) const;
			virtual void Deserialize(Serializer::StreamReader& in);

			MissionResultInfo()
				: MaxAirCombo(0)
				, Rank(0)
			{
			}
		};

		// CriticalStatistics means managed statistics by sever-side
		struct CriticalStatistics // CriticalStatistics means managed statistics by sever-side
		{
			struct PersonalStatistics
			{
				std::map<Serial/*Player*/, MissionResultInfo> PlayerInfos;
			} Personal;

			struct StageStatistics
			{
				float ElapsedTime;
				int FoundSecret;
				float EventExpFactor;
				int BossItemGainCount;
				uint32 ladderPoint;
				//uint16 AllPlayerReviveCount;
			} Stage;
		};

		struct Statistics
		{
			///< PersonalStatistics �� ���� �濡���� �����͸� �����ϱ� ���� �����Ѵ�.
			struct PersonalStatistics
			{
				CriticalStatistics::PersonalStatistics Critical;

				struct Personal
				{
					uint16 Life;
					bool Dead;
					std::wstring CharacterName;

					Personal()
						: Life(0)
						, Dead(false)
					{
					}
				};
				std::map< Serial/*Player*/, Personal > 	PersonalInfos;

				int Xp;						///< ���� �濡�� ���� ���� XP.. LocalPlayer���̴�..
				int PvpXp;					///< ���� �濡�� ���� pvp exp.

				//int Kill;					///< ���� �濡�� ���� ����.
				//int Money;				///< ���� �濡�� ���� money.
				//int MaxCombo;				///< ���� �濡���� combo�� ������ �ִ� ��.

				PersonalStatistics()
					: Xp(0)
					, PvpXp(0)
				{
				}

			} Personal;

			struct StageStatistics
			{
				CriticalStatistics::StageStatistics Critical;

				int StandardTime;			///< ���� ���������� ���ؽð�.
				float StandardDiviation;	///< ���� ���������� ǥ������..
				int MaxSecret;				///< ���� ���������� ��Secret Count

				StageStatistics()
					: StandardTime(0)
					, MaxSecret(0)
				{
				}
			} Stage;
		};
		enum StatisticsRank
		{
			RankS = 0,
			RankA,
			RankB,
			RankC,
			RankF,
			Cnt,
		};

		std::wstring GetRank(uint8 rank);
		int32 GetGainXPByStatisticsRank(uint8 rank, float gainXp);
		float GetGainXPRateByReviveCount(uint16 totalReviveCount);
		int GetNextRankTime(uint8 rank, int standatdTime);
		float GetGradeRank(uint8 rank);
		uint8 GetPlayerRankWithLevelGap(uint16 playerLevel, uint8 playerRank, int proprietyLevel);

		struct MissionResultFunctions {
			static uint8 GetPlayTimeRank(int playTime, int standardTime);
			static uint8 GetLifeRank(bool death, uint16 curLife);
			static uint8 GetSecretRank(int foundSecret, int totalSecret);
			static uint8 GetAirComboRank(uint32 maxAirComboCount);
			static uint8 GetStylePointRank(float stylePointPercentage);
			static uint8 GetStageClearRank(bool clear, int playTime, int standardTime, bool death, uint16 curLife, int foundSecret, int totalSecret, uint32 maxAirComboCount, float stylePointPercentage);
		};

		/* data that are shared more than a namespace ********************************************/
		struct ItemPosition
			: public Serializer::ISerializable
		{
			uint8 Bag;
			uint8 Position;
			static const ItemPosition Invalid;

			ItemPosition(uint8 bag, uint8 position);
			ItemPosition();

			bool operator==(const ItemPosition& rhs) const
			{
				return (Bag == rhs.Bag && Position == rhs.Position);
			}

			bool operator<(const ItemPosition& rhs) const
			{
				if (Bag < rhs.Bag)
				{
					return true;
				}
				if (Position < rhs.Position)
				{
					return true;
				}
				return false;
			}

			bool operator!=(const ItemPosition& rhs) const
			{
				return (Bag != rhs.Bag) || (Position != rhs.Position);
			}

			/** helper to find a ItemPosition
			@code
			std::vector<ItemPosition>::iterator i=std::find_if(q.begin(), q.end(), ItemPosition::Finder(Bag, Position));
			@endcode
			*/
			class Finder
			{
				uint32 Bag;
				uint16 Position;
			public:
				Finder(uint32 bag, uint16 position)
					: Bag(bag), Position(position)
				{
				}

				bool operator()(const ItemPosition& rhs) const
				{
					return (Bag == rhs.Bag) && (Position <= rhs.Position);
				}
			};


			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		/* to picking items  */
		struct ItemPack : public Serializer::ISerializable
		{
			ItemPosition Position;
			uint16 Count; // 3.1 by Robotex
			uint64 ExtendInfo; // ex) 0:none used, others:PetSerial

			ItemPack()
				: Position(XRated::ItemPosition::Invalid)
				, Count(0)
				, ExtendInfo(0)
			{
			}

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		// Robotex
		struct InstanceEx : public Serializer::ISerializable
		{
			static const DateTime NoExpiration;

			int64 Instance;
			DateTime ExpireDate;

			InstanceEx()
				: Instance(0)
				, ExpireDate()
			{}

			InstanceEx(int64 instance) // 2.6 Compatibility constructor
				: Instance(instance)
				, ExpireDate()
			{
				NormalBitfields normalInstance(instance);
				DateTime dateTime = DateTime(normalInstance.ExpiredYear, normalInstance.ExpiredMonth, normalInstance.ExpiredDay, normalInstance.ExpiredHour, normalInstance.ExpiredMinute, 0);
				if (normalInstance.ExpiredYear == 0 || !dateTime.IsValid())
					ExpireDate = NoExpiration;
				else
					ExpireDate = dateTime;
			}

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);

			bool operator==(const InstanceEx& rhs) const
			{
				return (Instance == rhs.Instance) && (ExpireDate == rhs.ExpireDate);
			}
			void operator=(const int64& b)
			{
				Instance = b;
				NormalBitfields normalInstance(b);
				DateTime dateTime = DateTime(normalInstance.ExpiredYear, normalInstance.ExpiredMonth, normalInstance.ExpiredDay, normalInstance.ExpiredHour, normalInstance.ExpiredMinute, 0);
				if (normalInstance.ExpiredYear == 0 || !dateTime.IsValid())
					ExpireDate = NoExpiration;
				else
					ExpireDate = dateTime;
			}
			operator int64 () const
			{
				return Instance;
			}
			/* operator XRated::Database::Enchant::NormalBitfields& (){
				XRated::Database::Enchant::NormalBitfields normalinstance(Instance);
				return normalinstance;
			} */

			struct ExpireDateFormat {
				uint32 Year : 12, Month : 4, Day : 5, Hour : 5, Minute : 6;
			};

			struct NormalBitfields
			{
				uint64
					/* item expiration information */
					ExpiredYear : 12,	/* 0-4095 ,0 means no expiration. */
					ExpiredMonth : 4,	/* 1-12 in 0-15 */
					ExpiredDay : 5,	/* 1-31 in 0 - 31 */
					ExpiredHour : 5,	/* 0-23 in 0 - 31 */
					ExpiredMinute : 6,	/* 0-59 in 0 - 64 */
					Reserved : 32;

				operator int64 () const { return reinterpret_cast<const int64&>(*this); }
				NormalBitfields& operator =(int64 rhs) { reinterpret_cast<int64&>(*this) = rhs; return *this; }
				NormalBitfields() { *this = 0; }
				NormalBitfields(int64 rhs) { *this = rhs; }
			};
		};

		static const int EquipemntBagPosition = 1;
		static const int MaxBag = 5;
		static const int MaxInventorysSlot = 15;
		static const int MaxEquipmentSlot = 12;
		static const int MaxQuickSlot = 10;
		const static float INVENTORYCOOLDOWN = 3.0f;


		struct Item : public Serializer::ISerializable
		{
			uint32 Id; 
			InstanceEx InstanceEx;

			Item()
				: Id(0)
				, InstanceEx(0)
			{
			}

			Item(uint32 id, int64 inst)
				: Id(id)
				, InstanceEx(inst)
			{
			}
		public:
			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct ItemSlot : public Serializer::ISerializable
		{
			uint32 Id;
			ItemPosition Position;
			//int64& Instance;
			InstanceEx instanceEx;
			uint16 Stacked;

			ItemSlot()
				: Id(0)
				, Stacked(1)
				, instanceEx(0)
			{
			}

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);

			void operator=(const ItemSlot& rhs)
			{
				Id = rhs.Id;
				Position = rhs.Position;
				instanceEx = rhs.instanceEx;
				Stacked = rhs.Stacked;
			}

			bool operator==(const ItemSlot& rhs) const
			{
				return (Id == rhs.Id) && (Position == rhs.Position) && (instanceEx == rhs.instanceEx) && (Stacked == rhs.Stacked);
			}

			bool operator!=(const ItemSlot& rhs) const
			{
				return !(*this == rhs);
			}

			struct Finder
			{
				uint32 itemID;
				Finder(uint32 itemID)
					: itemID(itemID)
				{
				}
				bool operator()(const ItemSlot& p) const
				{
					return p.Id == itemID;
				}
			};

			struct FinderByPos
			{
				ItemPosition pos;
				FinderByPos(ItemPosition pos)
					: pos(pos)
				{
				}
				bool operator()(ItemSlot& p) const
				{
					return p.Position == pos;
				}
			};
		};

		struct InvalidEquippedItem
			: public Serializer::ISerializable
		{
			Constants::Equipment where;
			bool enable;

		public:
			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct EquippedItem
			: public Serializer::ISerializable
		{
			uint32	itemHash;
			InstanceEx	instanceEx;
			Constants::Equipment	where;

		public:
			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct StoreSlot : public Serializer::ISerializable
		{
			uint32 ItemHash;
			uint16 StackedCount;
			InstanceEx instanceEx;
			uint32 SellPrice;

			bool operator==(const StoreSlot& rhs) const;

			virtual void Serialize(Serializer::StreamWriter& out) const;
			virtual void Deserialize(Serializer::StreamReader& in);
		};

		struct ItemBasicInfo : public Serializer::ISerializable
		{
			uint32	ItemHash;
			InstanceEx	instanceEx; // 3.1 by Robotex
			uint64	ItemSerial;
			uint16	StackCount; // 3.1 by Robotex

			ItemBasicInfo()
				: ItemHash(0)
				, instanceEx(0)
				, StackCount(0)
			{
			}
			ItemBasicInfo(uint32 id, int64 inst, GlobalSerial serial, uint16 count)
				: ItemHash(id)
				, instanceEx(inst)
				, ItemSerial(serial)
				, StackCount(count)
			{
			}
		public:
			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct RewardItem : public Serializer::ISerializable
		{
			uint32	ItemHash;
			InstanceEx	instanceEx;
			uint16	StackCount; // 3.1 by Robotex

			RewardItem()
				: ItemHash(0)
				, instanceEx(0)
				, StackCount(0)
			{
			}
			RewardItem(uint32 id, int64 inst, uint8	count)
				: ItemHash(id)
				, instanceEx(inst)
				, StackCount(count)
			{
			}
		public:
			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct PresentEventMailReward
			: public Serializer::ISerializable
		{
			struct ExpireInfo
			{
				enum Type
				{
					Fixed,	// ������ ��¥�� �Ǹ� ������ ���� �Ⱓ ����
					Dynamic,// �������� ���� �� ������ ���� �Ⱓ�� ���Ѵ�
				};
			};

			ExpireInfo::Type expireInfo;

			RewardItem item;

		public:
			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		typedef uint32 PlayTimeMinute;
		typedef uint32 Level;
		typedef std::vector< PresentEventMailReward > RewardItems;
		typedef std::map< PlayTimeMinute, RewardItems > PlayTimeEventRewards;
		typedef std::map< Level, RewardItems > LevelUpEventRewards;

		struct ItemPackage : public Serializer::ISerializable
		{
			uint32 ItemHash;
			uint16 Count;
			uint16 ExpireDay; ///< 0 means no expiration, and day unit
			std::string Description; ///< no wide string character to reduce packet size
			uint16 Attr1;	///< primary additional attribute index
			uint16 Attr2;	///< secondary additional attribute index
			uint16 Attr3;	/// 3.1 by ultimate 

			virtual void Serialize(Serializer::StreamWriter& out) const;
			virtual void Deserialize(Serializer::StreamReader& in);

			struct Finder
			{
				uint32 ItemHash;
				Finder(uint32 itemHash)
					: ItemHash(itemHash)
				{
				}
				bool operator()(const ItemPackage& item) const
				{
					return item.ItemHash == ItemHash;
				}
			};

			struct StackableFinder
			{
				uint32 ItemHash;
				uint16 MaxStackCount;
				StackableFinder(uint32 itemHash, uint16 maxStackCount)
					: ItemHash(itemHash)
					, MaxStackCount(maxStackCount)
				{
				}
				bool operator()(const ItemPackage& item) const
				{
					return (item.ItemHash == ItemHash && item.Count < MaxStackCount);
				}
			};
		};

		struct CashItem
			: public Serializer::ISerializable
		{
			int OrderNumber;
			int ProductNumber;
			int Quantity;
			uint32 RepresentativeItemHash;
			std::string Name;	///< no wide string character to reduce packet size
			std::vector<ItemPackage> Packages;

			virtual void Serialize(Serializer::StreamWriter& out) const;
			virtual void Deserialize(Serializer::StreamReader& in);
		};

		struct BagState
			: public Serializer::ISerializable
		{
			int BagNumber;
			DateTime ExpireDate;
			bool	 Expired;

			virtual void Serialize(Serializer::StreamWriter& out) const;
			virtual void Deserialize(Serializer::StreamReader& in);
		};

		struct Skill
			: public Serializer::ISerializable
		{
			Skill()
			{
				Id = 0;
				Level = 0;
			}
			uint32 Id; uint8 Level; Skill(uint32 id, uint8 lv) : Id(id), Level(lv) {}

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);

			/* helper
			@code
			std::vector<XRated::Skill>::iterator skill=std::find_if( playerInitData.skills.begin(), playerInitData.skills.end()
			, XRated::Skill::Finder(id) );
			@endcode
			*/
			class Finder
			{
				uint32 id;
			public:
				Finder(uint32 id)
					: id(id)
				{
				}
				bool operator()(const XRated::Skill& skill)
				{
					return (skill.Id == id) ? true : false;
				}
			};
		};

		struct QuickSlot : public Serializer::ISerializable
		{
			QuickSlot()
				: Id(0)
				, IsSkill(false)
				, instanceEx(0)
			{
			} // default constructor
			QuickSlot(const QuickSlot& q)
				: Id(q.Id)
				, IsSkill(q.IsSkill)
				, Pos(q.Pos)
				, instanceEx(q.instanceEx)
			{
			} // copy constuctor
			void operator =(const QuickSlot& q)
			{
				Id = q.Id;
				IsSkill = q.IsSkill;
				Pos = q.Pos;
				instanceEx = q.instanceEx;
			}
			void swap(QuickSlot& q)
			{
				QuickSlot temp(q);
				q = *this;
				*this = temp;
			}
			uint32 Id;
			bool IsSkill;
			uint8 Pos;
			InstanceEx instanceEx;

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct StageLocation
			: public Serializer::ISerializable
		{
			uint32 StageGroupHash;
			uint16 Level;
			uint8 Difficulty;
			std::vector< std::wstring > Tags; // arcrus. �±׽ý���.

			/** helper to find a quest by quest hash in general
			@code
			std::vector<StageLocation>::iterator i=std::find_if(q.begin(), q.end(), StageLocation::Finder(StageGroupHash, Level));
			@endcode
			*/
			class Finder
			{
				uint32 StageGroupHash;
				uint16 Level;
			public:
				Finder(uint32 stageGroupHash, uint16 level)
					: StageGroupHash(stageGroupHash)
					, Level(level)
				{
				}
				bool operator()(const StageLocation& rhs) const
				{
					return StageGroupHash == rhs.StageGroupHash && Level <= rhs.Level;
				}
			};

			/**
			@code
			std::vector<StageLocation>::iterator i=std::find_if(q.begin(), q.end(), StageLocation::FinderOnlyStageGroupHash(StageGroupHash));
			@endcode
			*/
			class FinderOnlyStageGroupHash
			{
				uint32 StageGroupHash;
			public:
				FinderOnlyStageGroupHash(uint32 stageGroupHash)
					: StageGroupHash(stageGroupHash)
				{
				}
				bool operator()(const StageLocation& rhs) const
				{
					return StageGroupHash == rhs.StageGroupHash;
				}
			};

			bool operator<(const StageLocation& rhs) const
			{
				if (StageGroupHash < rhs.StageGroupHash)
				{
					return true;
				}
				if (StageGroupHash == rhs.StageGroupHash && Level < rhs.Level)
				{
					return true;
				}
				return false;
			}
			bool operator==(const StageLocation& rhs) const
			{
				return StageGroupHash == rhs.StageGroupHash && Level == rhs.Level;
			}
			bool operator!=(const StageLocation& rhs) const
			{
				return !(*this == rhs);
			}

			void operator=(const StageLocation& rhs)
			{
				StageGroupHash = rhs.StageGroupHash;
				Level = rhs.Level;
				Difficulty = rhs.Difficulty;
				Tags = rhs.Tags;
			}

			StageLocation()
				: StageGroupHash(0)
				, Level(0)
				, Difficulty(1)
			{
			}

			StageLocation(uint32 StageGroupHash, uint16 Level)
				: StageGroupHash(StageGroupHash)
				, Level(Level)
				, Difficulty(1)
			{
			}

			StageLocation(uint32 StageGroupHash, uint16 Level, uint8 Difficulty)
				: StageGroupHash(StageGroupHash)
				, Level(Level)
				, Difficulty(Difficulty)
			{
			}

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};
		typedef StageLocation StageLicense;

		struct SquareInfo
			: public Serializer::ISerializable
		{
			String Name;
			enum SquareStatus
			{
				Closed = 0,
				Quiet,
				Normal,
				Cloudy,
				Full
			} Status;
			uint32 StageGroupHash;
			int AccessLevel;
			int	Capacity;
			int OrderNumber;

			virtual void Serialize(Serializer::StreamWriter& out) const;
			virtual void Deserialize(Serializer::StreamReader& in);
		};
		
		struct Friend : public Serializer::ISerializable
		{
			String CharacterName;
			bool IsOnline;
			Constants::ClassType ClassType;
			uint16 Level;
			String CurrentLocation;
			String Memo;

			struct Finder
			{
				std::wstring name;
				Finder(std::wstring name)
					: name(name)
				{
				}
				bool operator()(const Friend& f) const
				{
					return f.CharacterName == name;
				}
			};
			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};
		
		typedef std::vector<Friend> Friends;

		struct CharacterStateFlags
		{
			unsigned IsPcRoom : 1;
			unsigned IsTestCharacter : 1;
			unsigned IsInvalid : 1; ///< blocked or forbidden state
			unsigned IsAdmin : 1; ///< set character as admin.
			unsigned IsGuestID : 1; ///< nexon guest id

			unsigned reserve1 : 3;

			unsigned IsBalanced : 1; ///< make players blanced condition by strength or handicap
			unsigned IsSpectator : 1; ///< joined as spectator
			unsigned IsPetNotUsable : 1; ///< allow summoning pet in pvp game
			unsigned IsEquipSetRewardDisable : 1; ///< equip set reward on/off flag
			unsigned IsRebirthSkillDisable : 1; ///< rebirth Skill on/off flag
			unsigned IsInvincibleAfterRevive : 1; ///< invincible for 3sec after revive on/off flag
			unsigned StatusLimit : 2; ///< status limit, 0:unlimited, 1:1000, 2:500

			unsigned EquipmentSet : 2; ///< equip swap
			unsigned CashEquipmentSet : 2; ///< cash equip swap

			unsigned LastSquareLocation : 5; ///< LastSquareLocation 0:None, 1:Ep1, 2:Ep2, 3:Ep3, 4:Ep4, 5:Ep5, 6:Ep6, 7:Ep7, 8:Myth, 9:���� ��

			operator int() const
			{
				return reinterpret_cast<const int&>(*this);
			}

			CharacterStateFlags& operator =(int rhs)
			{
				reinterpret_cast<int&>(*this) = rhs;
				return *this;
			}

			CharacterStateFlags()
			{
				*this = 0;
			}

			CharacterStateFlags(int rhs)
			{
				*this = rhs;
			}

			~CharacterStateFlags()
			{
				////LOKI_STATIC_CHECK(sizeof(CharacterStateFlags) == sizeof(int), CharacterStateFlagsMustBeSameAsInteger);
			}
		};

		struct CharacterRewardStateFlags
		{
			unsigned AleradyGetMissionClearRewardItem : 1;

			operator int() const
			{
				return reinterpret_cast<const int&>(*this);
			}

			CharacterRewardStateFlags& operator =(int rhs)
			{
				reinterpret_cast<int&>(*this) = rhs;
				return *this;
			}

			CharacterRewardStateFlags()
			{
				*this = 0;
			}

			CharacterRewardStateFlags(int rhs)
			{
				*this = rhs;
			}

			~CharacterRewardStateFlags()
			{
				////LOKI_STATIC_CHECK(sizeof(CharacterRewardStateFlags) == sizeof(int), CharacterRewardStateFlagsMustBeSameAsInteger);
			}
		};

		struct LobbyPlayerInfo : public Serializer::ISerializable
		{
			String CharacterName;
			int64 CharacterSerial; // Robotex
			uint32 VirtualIdCode;
			Constants::ClassType ClassType;
			uint16 Level;
			uint32 Exp;
			uint16 PvpLevel;
			uint32 PvpExp;
			uint16 WarLevel;
			uint32 WarExp;
			uint16 StoredLevel;
			uint16 RebirthCount;
			uint32 LadderPoint;
			uint32 LadderWinCount;
			uint32 LadderLoseCount;
			uint16 LadderMatchCount;
			CharacterStateFlags StateFlags;

			DateTime LastLoggedDate;
			std::vector<ItemSlot> Equipments; // item hashes
			std::vector<StageLicense> Licenses; // license hashes

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);

			struct Finder
			{
				std::wstring name;
				Finder(std::wstring name)
					: name(name)
				{
				}
				bool operator()(const LobbyPlayerInfo& p) const
				{
					return p.CharacterName == name;
				}
			};

			LobbyPlayerInfo()
			{
				LastLoggedDate = DateTime::Now();
			}
			void Clear()
			{
				CharacterName.clear();
				VirtualIdCode = 0;
				Equipments.clear();
				Licenses.clear();
				Level = 0;
				Exp = 0;
				StateFlags = 0;
			}
		};
		
		typedef LobbyPlayerInfo TeamMember;
		
		typedef std::vector<TeamMember> TeamMembers;

		struct GameMode
			: public Serializer::ISerializable
		{
			Constants::GameTypes GameType;
			bool ItemPopup;

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct StateFlag
			: public Serializer::ISerializable
		{
			uint32 Id;
			uint16 Level;
			StateFlag(uint32 id, uint16 lv)
				: Id(id)
				, Level(lv)
			{
			}
			StateFlag()
				: Id(0)
				, Level(0)
			{
			}

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct Quest : public Serializer::ISerializable
		{
			const static int MaxCount = 25; ///< maximum number of concurrent working quests
			const static int MaxParameterCount = 3; ///< maximum number of parameters

			struct State
			{
				const static uint8 Working = 0x00;		///< right after quest accept
				const static uint8 Succeeded = 0x01;	///< right after quest condition completed
				const static uint8 Failed = 0x02;		///< right after quest failed
				const static uint8 Completed = 0x03;	///< right after rewards
				const static uint8 NotAccepted = 0x04;  ///< before quest accept 
				const static uint8 Offered = 0x05;		///< script can only make this state
				const static uint8 CompletedAndDropWaiting = 0x06;	///< right after quest drop waiting 
				const static uint8 FailedAndDropWaiting = 0x07;	///< right after quest drop waiting 
			};

			/** helper to find a quest by quest hash in general
			@code
			std::vector<Quest>::iterator i=std::find_if(q.begin(), q.end(), Quest::FindByQuestHash(hash));
			@endcode
			*/
			class FindByHash
			{
				uint32 hash;
			public:
				FindByHash(uint32 hash)
					: hash(hash)
				{
				}
				bool operator()(const Quest& rhs) const
				{
					return hash == rhs.Id;
				}
			};
			/** helper to find a quest by current state in general
			@code
			std::vector<Quest>::iterator i=std::find_if(q.begin(), q.end(), Quest::FindByQuestState(Quest::State::Working));
			@endcode
			*/
			class FindByState
			{
				uint8 state;
			public:
				FindByState(uint8 state)
					: state(state)
				{
				}
				bool operator()(const Quest& rhs) const
				{
					return state == rhs.CurrentState;
				}
			};


			//expired�� �������� ���� �� �ʱ�ȭ �ϴµ� �����ϴ� Date/�̰��� �������� ������ Warrning���� Log�� ���� �ȴ�.
			const static DateTime::Date NotUsedDate;

			uint32 Id; // hash
			uint8 CurrentState; // �������� ; one of Quest::State members
			DateTime::Date	ExpiredDate;
			std::vector<uint32> Params;

			Quest() : CurrentState(State::Working), Params(MaxParameterCount, 0), ExpiredDate(NotUsedDate)
			{
			}

			Quest(uint32 questHash, const DateTime::Date& expiredDate/* = DateTime::Date() */)
				: Id(questHash)
				, CurrentState(State::Working)
				, Params(MaxParameterCount, 0)
				, ExpiredDate(expiredDate)
			{
			}

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct ObjectData
		{
			Serial GameObjectSerial;
			std::wstring Name;
			uint32 NameHash;
			Constants::ObjectType Type;

			float3 Position;
			float3 Direction;
			float Radius;

			bool Move;

			ObjectData();
			void operator= (const ObjectData& obj);
		};

		struct ItemData
		{
			ObjectData& BaseObject;

			std::wstring OwnerId;
			uint16 StackCount;
			bool  PrivateItem;	//�������� �������� ������
			int64 Instance;

			ItemData(ObjectData& obj);
			void operator= (const ItemData&);
		};

		struct AddItemState
		{
			struct Data
			{
				enum DataType
				{
					NoType = 0,
					LifePlus = 1,
				};
				bool pvpUse;
				bool Stackable;
				int Value;
				DataType Type;

				Data()
					: Value(0)
					, Stackable(false)
					, pvpUse(false)
					, Type(NoType)
				{
				}
				Data(int value, bool pvpuse, bool stackable, DataType type)
					: Value(value)
					, Stackable(stackable)
					, pvpUse(pvpuse)
					, Type(type)
				{
				}
			};
			//State �߰� ����
			std::vector<Data> itemStateData;

			AddItemState()
			{
				itemStateData.clear();
			}
		};

		struct CharacterData
		{
			ObjectData& BaseObject;

			uint32 Xp;
			uint16 Level;
			uint32 Money;
			uint32 BankMoney;

			uint8 Team; // 0 means neutral element
			uint8 Alliance; // reserved in current

			float Hp, MaxHp;
			float Mp, MaxMp;

			std::vector<StateFlag> States;

			float SpeedVector;//�⺻�� 1 ���ο쳪 ���̽�Ʈ ���� ������ �ðų� �پ�����.
			float sizeRate; //�ɸ����� ũ��. Growth ������ ���Ҽ� �ִ�.

			//���ɿ��� ������ �ʿ��� ����
			uint32 ActionName;
			float Speed;

			CharacterData(ObjectData& obj);
			void operator= (const CharacterData&);
		};

		struct NonPlayerData
		{
			enum NpcType
			{
				Normal = 0,
				Leader,
				Elite,
				Rare,
				SemiBoss,
				StageBoss,
				EpisodeBoss,
				Cnt
			};

			CharacterData& BaseCharacter;

			NpcType Npc;
			//Constants::Familiar::Type type;
			int partyCntWhenCreated;

			NonPlayerData(CharacterData& character);
			void operator= (const NonPlayerData&);
		};

		struct PetCaredBySchool
		{
			uint32               PetItemHash;
			XRated::GlobalSerial PetItemSerial;
			uint64               PetItemInstance;
			uint8				 PetItemCount;
			float                ExpFactor; // ������ Factor�� ���ϱ�
			DateTime       Start;
			DateTime       End;

			PetCaredBySchool();

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);

			class Finder
			{
				GlobalSerial Serial;
			public:
				Finder(GlobalSerial serial) : Serial(serial)
				{
				}
				bool operator()(const XRated::PetCaredBySchool& info) const
				{
					return info.PetItemSerial == Serial;
				}
			};

		};

		struct PetItemSlot
		{
			enum PositionType
			{
				Equipment = 0,
				Inventory,
				Invaild
			};

			PositionType Type;			//< Area�� enum������ �ִ´�.
			uint8 Position;
			uint32 ItemHash;
			uint16 Stacked;			// 3.1 by Robotex
			InstanceEx instanceEx;

			PetItemSlot();
			PetItemSlot(PositionType type, uint8 position, uint32 itemHash, int64 instance, uint16 stacked = 0);
		public:
			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct Pet
		{
			const static uint16 MaxLevel = 60;
			const static uint16 RarePetLevel = MaxLevel + 1;
			const static float ExpUpPerSec;
			const static uint16 GrowUpLevelUnit = 10;

			const static float MaxFull;
			const static float MaxOverFull;
			const static float AddEffectStateTime;

			const static uint32 EmotiionMinDelayTime = 30;
			const static uint32 EmotiionMaxDelayTime = 180;
			const static uint32 MaxPetInventorySlotSize = 14;
			const static uint16 MaxPetNameSize = 16;

			struct Feeling
			{
				struct Border
				{
					const static float OverFull;
					const static float UnderFull_OverGood;
					const static float UnderGood_OverSoso;
					const static float UnderSoso_OverNotGood;
					const static float UnderNotGood_OverHungry;
					const static float UnderHungry_OverStarving;
					const static float UnderStarving;
				};

				enum State
				{
					Starving = 0, // < 3
					Hungry, // 3~30
					NotGood, // 30~40
					Soso, // 40~70
					Good, // 70~90
					Full, // > 90
					EndOfFeelingState
				};
			};

			struct TiredFactorRelatedWithFull
			{
				const static float Full;
				const static float Good;
				const static float Soso_NotGood;
				const static float Hungry_Starving;
			};

			struct RareProbabilityFactorRelatedWithFull
			{
				const static float Full_Good;
				const static float Soso;
				const static float NotGood;
				const static float Hungry_Starving;
			};

			struct RelatedWithPresentNFull
			{
				// �Ϲ� ���� ����
				// Y = L - ( C*L / ( ( C*P/L - P ) + A*L*X + C ) )
				//
				// --- ���� ���� ---
				// Y : �þ ��������
				// X : sellPrice * itemCount * 6
				// L : ���Ű� 
				// C, A : ����

				const static float PresentLimitFull;// L
				const static float Constant_C;// C
				const static float Constant_A;// A
			};

			bool IsRarePet;

			XRated::GlobalSerial PetSerial; // Item Serial�̶� ����
			//Pet�� ���� ���� �ϰ� �ִ� ����.
			std::vector<Item>	Equipments;		//< Serialize���� �������� �ʴ´�. by kwind.

			String PetName;
			uint32 PetHash;
			uint16 Level;
			double Exp;
			uint32 NextLevelExp;

			bool Appear;

			float Full; //< �� ������ DB���� ���� �Ǵµ� DB������ int������ ���� �ȴ�(DB�� ������ ���� �������̴�).
			float PrevFull;

			float TiredFactor; ///< �Ƿ��� ����ġ(�� Factor���� ���� �ð��� ����ġ�� �ش�).
			float ExpFactor; //< ����ġ�� ����ġ(�� Factor���� ���� �ð��� ����ġ�� �ش�).

			float EffectStateDelayTime;
			float EffectStateDelayTimeForDisplay;	//Ŭ���̾�Ʈ���� �� ��. Ŭ���̾�Ʈ������ ���۷����� �̻��ϰ� �ڵ��س��� ���⼭ ������Ʈ �ؾ��Ѵ�. ��ġ���� �ϸ� �ð��� ���Ƹ԰� ������. ��������.
			float EmotionDelay; ///< 0�� �Ǵ� ���� EmotionDelay�� ǥ��.

			float          RareProbability; // RarePet�� �Ǵ� Ȯ��
			float          FullSum;         // �������� ���� ���ϰ� �ֽ�
			float          LevelUpPeriod;   // Level Up���� �ɸ��� �ð�
			uint32 EnchantSerial; // 3.1 by Robotex

			// Equipment�Լ��� �ִ� ������ equipment position�� Equipments vector�� index�� 1:1 ��ġ�� �ƴϱ� ����.
			void Equipment(uint32 itemHash, int64 instance, Constants::Equipment position);
			void UnEquipment(Constants::Equipment position);
			void AllUnEquipment();
			const Item* GetEquipment(Constants::Equipment position);

			// �Ϲ� ���� ����
			// Y = L - ( C*L / ( ( C*P/L - P ) + A*L*X + C ) )
			//
			// --- ���� ���� ---
			// Y : �þ ��������
			// X : sellPrice * itemCount * 6
			// L : ���Ű� 
			// C, A : ����
			float GetFullChangeByPresent(uint32 sellPrice, uint32 count) const;

			void Feed(uint32 amount, uint32 count, bool overFeed = false);
			void Give(uint32 sellPrice, uint32 count);
			int GetFeedableFoodCount(uint32 foodAmount) const;
			int GetOverFeedableFoodCount(uint32 foodAmount) const;

			int GetBuffIndex() const;
			double GetTiredFactorRelatedWithFull() const;

			// ������ ����Ȯ�� ����
			// Y = ( M / ( T - 1 ) - ( T - 2 ) * / 2 + ( Lv - 1 ) * d ) * f
			//
			// --- ���� ���� ---
			// Y : ������ Ȯ�� ����ġ
			// M : �������� Ȯ�� �ִ밪
			// D : Ȯ���������� ������
			// T : ����
			// Lv : ������
			// f : ���θ� ���¿� ���� ����
			float GetAddedRareProbability(double fullRate, float maxRareProbability, float rareProbabilityAddPerLevel, uint32 level) const;

			float GetFullRatio() const;
			float GetPrevFullRatio() const;

			Feeling::State GetFeeling() const;
			Feeling::State GetPrevFeeling() const;

			bool Update(float dt);

			void UpdateByPetSchool(float dt, float schoolExpFactor);

			Pet();

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);

			//!= operator�� appear�� �����ϰ� �����Ѵ�.
			bool operator !=(const Pet& in) const;

			class Finder
			{
				GlobalSerial Serial;
			public:
				Finder(GlobalSerial serial) : Serial(serial)
				{
				}
				bool operator()(const XRated::Pet& info) const
				{
					return info.PetSerial == Serial;
				}
			};

			static bool IsValidPetNameSize(const std::wstring& name);
		};

		struct PetDataWithItemPos : public Serializer::ISerializable
		{
			Pet Pet;
			ItemPosition PetItemPosition;
		public:
			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};
		
		struct StoreSlotForPet : public StoreSlot
		{
			Pet	PetData;

			bool operator==(const StoreSlot& rhs) const;
			//�� Finder�� ���� �Ҷ��� ������ PetSerial�� 0�� �ƴҶ� ���� �ؾ� �Ѵ�.
			struct Finder
			{
			private:
				StoreSlot	Slot;
				XRated::GlobalSerial PetSerial;
			public:
				Finder(XRated::GlobalSerial petSerial, const StoreSlot& slot)
					: PetSerial(petSerial)
					, Slot(slot)
				{
				}
				bool operator()(const StoreSlotForPet& p) const
				{
					if ((p.PetData.PetSerial == PetSerial) && (p == Slot))
					{
						return true;
					}
					return false;
				}
			};

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct PlayerData
		{
			std::mutex SyncRoot;
			CharacterData& BaseCharacter;

			Constants::ClassType Job; // class is a keyword, shit.

			uint16 Life;
			uint16 MaxLife;
			uint16 BonusLife;
			uint16 UsableBonusLifeInStage;
			uint16 SkillPoint;
			uint16 AddedSkillPointPlus;

			float Sp, MaxSp; // stamina

			std::vector<Skill> Skills;
			std::vector<Item> Equipments;
			std::vector<Quest> quests;

			uint32 PvpXp;
			uint16 PvpLevel;

			uint32 WarXp;
			uint16 WarLevel;

			uint16 StoredLevel;
			uint16 RebirthCount;
			uint16 StoredSkillPoint;
			DateTime LastRebirthDateTime;

			uint32 LadderPoint;
			uint32 LadderWinCount;
			uint32 LadderLoseCount;
			uint16 LadderMatchCount;
			uint32 achievementScore;

			float expFactor;

			bool bDead; // ���ɻ������� üũ

			//float fullFactor, goodFactor, sosoFactor, hungryFactor;

			typedef std::vector< Item >::iterator ItemIterator;

			//CriticalStatistics::PersonalStatistics PS;

			void* user; // �̸� �˾Ƽ� ����. -0-;

			PlayerData(CharacterData& character);
			void operator =(const PlayerData& pc);
		};

		struct NexonGuildInfo : public Serializer::ISerializable
		{
			int GuildOid;
			String GuildId;
			String GuildName;
			String CreatedDate;
			String MasterCharacterName;
			int NumberOfMembers;
			String Intro;

			bool IsJoined() const { return GuildId.empty() ? false : true; }
			void Clear() { GuildId.clear(); }

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct AllMGuildInfo : public Serializer::ISerializable
		{
			/**
			Guild Info for allm guild system.
			It's totally same as nexon guild now. But, It'll be different from nexon guild later.
			So create another guild structure for later changes.

			Only used in client side. Server doesn't have any guild information because of our server system.
			*/
			struct GradeInfo
				: public Serializer::ISerializable
			{
				struct Auth
				{
					const static int8 Invite = 0x01;
					const static int8 GuildMessage = 0x02;
					const static int8 Kick = 0x04;
					const static int8 ChangeGrade = 0x08;
					const static int8 ShowMoreInfo = 0x10;
					const static int8 UseGuildShop = 0x20;
				};

				String GradeName;
				unsigned char Authority;

			public:
				GradeInfo()
					: Authority(0)
				{
				}
				GradeInfo(const String& name, int8 auth)
					: GradeName(name)
					, Authority(auth)
				{
				}

			public:
				virtual void Serialize(Serializer::StreamWriter& out) const;
				virtual void Deserialize(Serializer::StreamReader& in);
			};

			int GuildId;
			String GuildAlias;		///< Ȩ������ �ּҵ �����ȴ�.
			float MyPlayTime;			///< ���� playTime(���� ����ġ ���� ����).
			DateTime ConnnectionDate;		///< Stage�� ������ �ð�(���� ���Ե� ��������/Ŭ���̾�Ʈ���Դ� ���� �ʴ´�).
			int64 TotalPlayTime;
			DateTime	StartedTimeToPlay; ///< exp ���� ���� �ð�.
			uint32 GuildExp;
			uint32 MyContributed;		///< �ڽ��� ���� �⿩��.
			uint32 Point;				///< ���� ������ ���� point.
			DateTime	ShopOpenDate;
			DateTime	ShopCloseDate;

			int OnlineMemberCount; // arcrus. �������� ������ ��.
			int Rank;
			DateTime	RankExpiredDate;
			uint32 Tax;
			DateTime	TaxPayDay;
			String GuildName;
			String Message; //Guild Message`
			String MasterName; //Guild Master character name
			uint8 GuildLevel;
			int MemberCount;

			String CreatedDate;

			std::vector<GradeInfo> Grade; //Grade list. Index means guild grade order

			uint8 MyGrade; //�� ���忡�� ���� ����
			int GuildMemberId; //���� ���� ���� ID. �������� �ڽ��� ���� �˻��� ���δ�.

			bool IsAbleToInvite() const { if (MyGrade >= Grade.size()) return false; return Grade[MyGrade].Authority & GradeInfo::Auth::Invite ? true : false; }
			bool IsAbleToChangeMessage() const { if (MyGrade >= Grade.size()) return false; return Grade[MyGrade].Authority & GradeInfo::Auth::GuildMessage ? true : false; }
			bool IsAbleToKick() const { if (MyGrade >= Grade.size()) return false; return Grade[MyGrade].Authority & GradeInfo::Auth::Kick ? true : false; }
			bool IsAbleToChangeGrade() const { if (MyGrade >= Grade.size()) return false; return Grade[MyGrade].Authority & GradeInfo::Auth::ChangeGrade ? true : false; }
			bool IsAbleToShowMoreInfo() const { if (MyGrade >= Grade.size()) return false; return Grade[MyGrade].Authority & GradeInfo::Auth::ShowMoreInfo ? true : false; }
			uint16 GetLimitMemberCount() const;

		public:
			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct GuildRankInfo
		{
			int				GuildID;
			uint16			Rank;				//���� ����
			std::wstring	GuildName;			//���� �̸�
			uint16			GuildMemberCount;	//���� �ο� ��
			int64			TotalPlayTime;		//������ �� �÷��� �ð�.
			int64			CurrentTotalPlayTime;
			uint32			TotalContribution;	//�� �⿩��
			uint16			GuildLevel;			//���� ����



		public:
			struct CompareByRank
			{
				bool operator()(const GuildRankInfo& guild1, const GuildRankInfo& guild2) const;
			};
			struct CompareByName
			{
				bool operator()(const GuildRankInfo& guild1, const GuildRankInfo& guild2) const;
			};
			struct CompareByMemberCount
			{
				bool operator()(const GuildRankInfo& guild1, const GuildRankInfo& guild2) const;
			};
			struct CompareByContribution
			{
				bool operator()(const GuildRankInfo& guild1, const GuildRankInfo& guild2) const;
			};
			struct CompareByLevel
			{
				bool operator()(const GuildRankInfo& guild1, const GuildRankInfo& guild2) const;
			};
			struct CompareByPlayTime
			{
				bool operator()(const GuildRankInfo& guild1, const GuildRankInfo& guild2) const;
			};
			struct CompareByCurrentPlayTime
			{
				bool operator()(const GuildRankInfo& guild1, const GuildRankInfo& guild2) const;
			};

			struct Finder
			{
				int guildID;
				Finder(int guildID)
					: guildID(guildID)
				{
				}
				bool operator()(const GuildRankInfo& p) const
				{
					return p.GuildID == guildID;
				}
			};

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct GuildShopItem
		{
			uint32			ItemHash;
			DateTime	ExpiredDate;
			class FindByHash
			{
				uint32 hash;
			public:
				FindByHash(uint32 findHash)
					: hash(findHash)
				{
				}
			public:
				bool operator ()(const GuildShopItem& in) const
				{
					if (hash == in.ItemHash)
					{
						return true;
					}
					return false;
				}
			};
		public:
			GuildShopItem() {}
			GuildShopItem(uint32 hash, DateTime expiredDate)
				: ItemHash(hash)
				, ExpiredDate(expiredDate)
			{
			}
			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct AllMBasicGuildInfo
		{
			String GuildName;
			int GuildId;
			int MemberCount;

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct AllMGuildUserInfo : public Serializer::ISerializable
		{
			/**
				Used in server and client both. GuildMemberId and GuildId are used in server only. For searching DB faster using index.
			*/
			int GuildMemberId; //My unique guildMemberId

			struct Bitfields
			{
				uint64
					Grade : 8,
					Level : 16,
					PvPLevel : 16,
					WarLevel : 16,
					Job : 8;

				operator uint64 () const
				{
					return reinterpret_cast<const uint64&>(*this);
				}
				Bitfields& operator =(uint64 rhs)
				{
					reinterpret_cast<uint64&>(*this) = rhs;
					return *this;
				}
				Bitfields()
				{
					*this = 0;
				}
				Bitfields(uint64 rhs)
				{
					*this = rhs;
				}
			};
			Bitfields CharacterInfo;

			String CharacterName;
			uint32 LastLogin;
			uint16 StoredLevel;
			uint16 RebirthCount;
			uint32 Contributed;		///< ���� �⿩��.
			std::wstring PrivateMessage; // 3.1 by Robotex


		public:
			void Clear()
			{
				GuildMemberId = 0;
				CharacterName.clear();
				CharacterInfo.Grade = 0;
				CharacterInfo.Level = 0;
				CharacterInfo.PvPLevel = 0;
				CharacterInfo.WarLevel = 0;
				CharacterInfo.Job = 0;
				LastLogin = 0;
			}

			struct Finder
			{
				std::wstring name;
				Finder(std::wstring name) : name(name) {}
				bool operator()(const AllMGuildUserInfo& member) const { return member.CharacterName == name; }
			};

		public:
			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		struct Gamble
		{
			// dolph, Gamble
			enum GameType
			{
				SlimeRace = 1,
			};
			enum BettingNumber
			{
				ONE = 1 << 0,
				TWO = 1 << 1,
				THREE = 1 << 2,
				FOUR = 1 << 3,
				FIVE = 1 << 4,
				SIX = 1 << 5,
				SEVEN = 1 << 6,
				EIGHT = 1 << 7,
				NINE = 1 << 8,
				OUTSIDE = 1 << 9,
			};

			enum ChipType
			{
				Pink,			//1��
				Green,			//1��
				Yellow,			//100��
			};

			// credit == chip(s) == money?
			typedef struct tagCredit
			{
				uint32 Pink;
				uint32 Green;
				uint32 Yellow;

				const uint32 Sum()
				{
					return Yellow * 10000 + Green * 100 + Pink;
				}


				tagCredit();
				tagCredit(uint32 value)
				{
					Yellow = static_cast<uint32>(value / 10000);
					Green = static_cast<uint32>((value - 10000 * Yellow) / 100);
					Pink = (value - 10000 * Yellow) - (100 * Green);
				}

				tagCredit(uint16 pink, uint16 green, uint16 yellow)
					: Pink(pink)
					, Green(green)
					, Yellow(yellow)
				{
				}
			}Credit;

			typedef uint32 BettingPosition;

			const static uint16 PinkChipsInAGreenChip = 100;
			const static uint16 GreenChipsInAYellowChip = 100;
			const static uint16 PinkChipsInAYellowChip = PinkChipsInAGreenChip * GreenChipsInAYellowChip;

			struct SlimeRace
			{
				struct Position
				{
					const static uint32 One = ONE;
					const static uint32 Two = TWO;
					const static uint32 Three = THREE;
					const static uint32 Four = FOUR;
					const static uint32 InsideFive = FIVE;
					const static uint32 OutSideFive = FIVE | OUTSIDE;
					const static uint32 Six = SIX;
					const static uint32 Seven = SEVEN;
					const static uint32 Eight = EIGHT;
					const static uint32 Nine = NINE;
					const static uint32 Red = ONE | THREE | SEVEN | NINE;
					const static uint32 Black = TWO | FOUR | SIX | EIGHT;
					const static uint32 Low = ONE | TWO | THREE | FOUR;
					const static uint32 High = SIX | SEVEN | EIGHT | NINE;
					const static uint32 StreetBets123 = ONE | TWO | THREE;
					const static uint32 StreetBets456 = FOUR | FIVE | SIX;
					const static uint32 StreetBets789 = SEVEN | EIGHT | NINE;
				};
				const static uint8 MaxSlimeNumber = 9;
				const static float BettingLimitInSec; //Betting closing duration in sec from game start

				static float GetDividendRate(BettingPosition position);

				struct Constants
				{
					const static float Chip2MoneyConversionTax;
				};
			};

			struct BettingState : public Serializer::ISerializable
			{
				float RecentRate;
				uint32 MyChips;
				uint32 TotalChips;

				void Serialize(Serializer::StreamWriter& out) const;
				void Deserialize(Serializer::StreamReader& in);
			};
		};

		struct Mail
		{
			// arcrus. Mailbox System.
			struct HeaderInfo : public Serializer::ISerializable
			{
				enum FlagType
				{
					None = 0
					, Responsed = 1
					, Returned = 2 << 0
					, Cash = 2 << 1
					, SystemMail = 2 << 2
					, PresentEventMail = 2 << 3
				};

				uint64			Index;
				std::wstring			Title;
				int						Flag;
				bool					IsRead;
				std::wstring			Sender;
				DateTime 			SentDate;		// YYYY-MM-DD hh:mm:ss

			public:
				void Serialize(Serializer::StreamWriter& out) const;
				void Deserialize(Serializer::StreamReader& in);
			};


			struct ContentsInfo : public Serializer::ISerializable
			{
				uint64			Index;
				std::wstring			Receiver;
				std::wstring			Message;
				uint32			AttachedMoney;
				std::map< uint8 /*position*/, XRated::ItemBasicInfo /*itemInfo*/ >
					AttachedItems;

				uint32			StampItemHash;
				bool IsSystemMail; // 3.1 by Robotex
			public:
				void Init()
				{
					Index = 0;
					Receiver.clear();
					AttachedMoney = 0;
					AttachedItems.clear();
					StampItemHash = 0;
					IsSystemMail = false;
				}
			public:
				void Serialize(Serializer::StreamWriter& out) const;
				void Deserialize(Serializer::StreamReader& in);

			};
			static uint64 CalculatePostalCharge(uint64 money, uint16 itemCount);

			const static float FeeFactor;
			const static int ItemDeliverFee = 1000;
			const static int MailFee = 100;
			struct Limit
			{
				const static uint32 GoldWithoutStamp;
				const static uint8	ItemWithoutStamp;
				const static int	TARGETLEN_MAX;
			};
		};

		struct Fishing
		{
			struct FishingInfo : public Serializer::ISerializable
			{
				uint32 ItemHash;
				String CharacterName;
				uint8 Count;
				DateTime FishingTime;

			public:
				FishingInfo() {}
				FishingInfo(uint32 hash, String name, uint8 cnt, const DateTime& time)
					: ItemHash(hash)
					, CharacterName(name)
					, Count(cnt)
					, FishingTime(time)
				{
				}
			public:
				void Serialize(Serializer::StreamWriter& out) const;
				void Deserialize(Serializer::StreamReader& in);
			};

			const static int MaxList = 50;
			const static int FishingHeight = 2;
			const static int MaxBaitCount = 5;
			const static float CorkRelativePosition; //Cork position from character

			enum Grade
			{
				Low = 0,
				Medium,
				Normal,
				High,
				Rare,
				Epic,
				Cnt
			};

			const static std::wstring& GradeToString(int32 grade)
			{
				const static std::wstring strGrade[Grade::Cnt] = {
					L"Low",
						L"Medium",
						L"Normal",
						L"High",
						L"Rare",
						L"Epic"
				};
				if (grade < 0 || grade >= Grade::Cnt)
				{
					Logger::GetInstance()->Exception("Wrong gradeType: {0}", grade);
                    throw fmt::format("Wrong gradeType: {0}", grade);
				}
				return strGrade[grade];
			};
		};

		/* Global Function */
		struct ExpFunctions
		{
			struct Pvp
			{
				static float CalculateSurvivalXpWithLevelDifference(uint16 opLv, uint16 myLv);
				static int CalculateXpForWinner(uint16 myLv, uint16 myPvpLv, float avgLv, float deter, int memCnt);
				static int CalculateXpForLoser(uint16 myLv, uint16 myPvpLv);

				static int CalculateMinimumPlayMinute(int memCnt); /*return Minutes*/
				static float CalculateLevelGapBonusRate(float deltaAvgLv); /* 1.0~1.1*/
				static float CalculateMemCntBonusRate(int memCnt);
				static int CalculateStageXpForWinner(uint16 myLv, float deltaAvgLv /* ����������lv - �츮������lv */, float playTime /*����:��*/, int memCnt);
				static int CalculateStageXpForLoser(uint16 myLv, float deltaAvgLv /* ����������lv - �츮������lv */, float playTime /*����:��*/, int memCnt);
				//static int CalculateStageXpForWinner(uint16 myLv, float avgLv, float deter, int memCnt);
				//static int CalculateStageXpForLoser(uint16 myLv, uint16 myPvpLv, float avgLv, float deter, int memCnt) ;
				static int GetXp(int memberCnt);
			};
			struct Stage
			{
				static float CalculateXpWithLevelGap(float xp, int lvGap);
				static inline float GetWeightValue(uint16 lv)
				{
					/*
					float weight[25]={ 0.77500f,0.60799f,0.48274f,0.38788f,0.31535f,
					0.25938f,0.21580f,0.18160f,0.15454f,0.13298f,
					0.11569f,0.10175f,0.09046f,0.08128f,0.07380f,
					0.06697f,0.06074f,0.05506f,0.04988f,0.04517f,
					0.04088f,0.03698f,0.03343f,0.03020f,0.02727f };
					*/


					float weight[50] = { 0.95000000f, 0.90250000f,0.85737500f,0.81450625f,0.77378094f
						,0.73509189f,0.69833730f,0.66342043f,0.63024941f,0.59873694f
						,0.56880009f,0.54036009f,0.51334208f,0.48767498f,0.46329123f
						,0.44012667f,0.41812034f,0.39721432f,0.37735360f,0.35848592f
						,0.25094015f,0.17565810f,0.12296067f,0.08607247f,0.06025073f
						,0.04217551f,0.02952286f,0.02066600f,0.01446620f,0.01012634f
						,0.00708844f,0.00496191f,0.00347333f,0.00243133f,0.00170193f
						,0.00119135f,0.00083395f,0.00058376f,0.00040863f,0.00028604f
						,0.00017163f,0.00010298f,0.00006179f,0.00003707f,0.00002224f
						,0.00001335f,0.00000801f,0.00000480f,0.00000288f,0.00000274f };
					int gap = (int)lv - 50;
					switch (gap) {
					case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24:
					case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47: case 48: case 49:
						return weight[gap];
					}
					return 1.0f;
				}

				static inline float GetWeightValueFactor(uint16 lv)
				{
					return XRated::ExpFunctions::Stage::GetWeightValue(lv) / XRated::ExpFunctions::Stage::GetWeightValue(lv - 1);
				}

				static uint64 GetInverseXp(uint16 userLevel, uint64 curXp)
				{
					if (curXp == 0) return 0;

					double levelExpFactor = static_cast<double>(XRated::ExpFunctions::Stage::GetWeightValue(userLevel));
					return static_cast<uint64>((static_cast<double>(curXp) / levelExpFactor) + 0.5);
				}

				static float GetAddExpFactorByRebirthCount(const uint16 rebirthCount);
			};
		};

		struct GoldFunctions
		{
			struct Pvp
			{
				static float GetPlayTimeRate(int memCnt);
				static float GetBasicMoneyPerMin(uint16 level);
				static float GetLifeBonusRate(int deathCount);
				static float GetKillBonusRate(int killCount);
				static uint32 CacluateVictoryMoneyReward(uint16 level, int memCnt, int killCount, int deathCount);
				static uint32 CacluateDefeatedMoneyReward(uint16 level, int memCnt, int killCount, int deathCount);
			};
		};

		struct StatusFunctions
		{
			//static float GetHpRatePerUserCnt(NonPlayerData::NpcType type, int userCnt);
			static float GetHPperVit(uint16 lv);
			static float GetMPperInt(uint16 lv);
			static inline float GetDodgeProb(int dex)
			{
				return dex / 3000.0f;
			}
			//static inline float GetDodgeProb(int dex) { return dex * 3.0f / 10000.0f; }
			static inline float GetMinDmg(int str)
			{
				return (float)str / 3.0f;
			}
			static inline float GetMaxDmg(int str, int dex)
			{
				return (float)dex / 8.0f + (float)str / 3.0f;
			}
			// GetReducedCooldownRate�� GetIncreasedDamageRate�� ���� 33�� 1%�� ���� ȿ���� �ִ�.
			static float GetReducedCooldownRate(int dex);
			static float GetIncreasedDamageRate(int str);					///< �� ������ ���� ������ �÷��̾��� ���쿡�� ���� �޴´�.
			static float GetIncreasedDamageRateInPvp(int str);					///< �� ������ ���� ������ �÷��̾��� ���쿡�� ���� �޴´�.
			static float GetIncreasedDamageRateByAtk(float atkVal);			///< �� ������ ���� ������ �÷��̾��� ���쿡�� ���� �޴´�. ���ݷ¿� ���� ��Ƣ����.
			static float GetIncreasedDamageRateByAtkInPvp(float atkVal);			///< �� ������ ���� ������ �÷��̾��� ���쿡�� ���� �޴´�. ���ݷ¿� ���� ��Ƣ����.
			static float GetIncreasedDamageRateForNonPlayer(int str);		///< �� ������ ���� ������ �÷��̾ �ƴ� ���쿡 ���� �޴´�(Nonplayer, misc..).
			static float GetIncreasedHealRate(int intelligence);
		};

		struct PhysicalFunctions
		{
			static float4x4 GetOrientation(const float3& position, const float3& direction);
			static float3 ApplyGravity(const float3& prev, float dt); // returns velocity
			static float3 ApplyGravity(const float3& position, const float3& prev, float dt); // returns position 

			static const float3& DirectionToDefinedDirection(float3 dir);

			static inline bool IsBehindTarget(const float3& sourceDir, const float3& targetDir)
			{
				Constants::Direction source = Constants::DirectionF::GetDirectionEnum(sourceDir);
				Constants::Direction target = Constants::DirectionF::GetDirectionEnum(targetDir);
				switch (source)
				{
				case Constants::Direction::Down:
					switch (target)
					{
					case Constants::Direction::LeftDown:
					case Constants::Direction::Down:
					case Constants::Direction::RightDown:
						return true;
					}
					return false;
				case Constants::Direction::Left:
					switch (target)
					{
					case Constants::Direction::LeftDown:
					case Constants::Direction::Left:
					case Constants::Direction::LeftUp:
						return true;
					}
					return false;
				case Constants::Direction::LeftDown:
					switch (target)
					{
					case Constants::Direction::Left:
					case Constants::Direction::LeftDown:
					case Constants::Direction::Down:
						return true;
					}
					return false;
				case Constants::Direction::LeftUp:
					switch (target)
					{
					case Constants::Direction::Left:
					case Constants::Direction::LeftUp:
					case Constants::Direction::Up:
						return true;
					}
					return false;
				case Constants::Direction::Right:
					switch (target)
					{
					case Constants::Direction::RightDown:
					case Constants::Direction::Right:
					case Constants::Direction::RightUp:
						return true;
					}
					return false;
				case Constants::Direction::RightDown:
					switch (target)
					{
					case Constants::Direction::Right:
					case Constants::Direction::RightDown:
					case Constants::Direction::Down:
						return true;
					}
					return false;
				case Constants::Direction::RightUp:
					switch (target)
					{
					case Constants::Direction::Up:
					case Constants::Direction::RightUp:
					case Constants::Direction::Right:
						return true;
					}
					return false;
				case Constants::Direction::Up:
					switch (target)
					{
					case Constants::Direction::LeftUp:
					case Constants::Direction::Up:
					case Constants::Direction::RightUp:
						return true;
					}
					return false;
				}
				return false;
			}

			static inline uint32 ReverseCmd(uint32 dir)
			{
				switch (dir)
				{
				case 1: return 9;
				case 2: return 8;
				case 3: return 7;
				case 4: return 6;
				case 6: return 4;
				case 7: return 3;
				case 8: return 2;
				case 9: return 1;
				}
				return 2; //�հ� �������� ����Ű�� �����´ٸ� �� �Ʒ������� ����.
			}

			static float3 GetSpinnedDirection(const float3& targetDirection, float3 sourceDirection, uint16 spinLimit, float dt = 0.1f);
			//8���⿡ �������� ȸ����ȯ. ���� ������ ����.
			static float3 RotatePositionByDirection(const float3& position, int angle, const float3& direction);
			static float3 RotatePosition(const float3& position, int angle);
			static float3 RotatePositionLimitedAngle(const float3& targetPos, int direction);
			// objData�� �浹�� �Ѱų� ���� �༮�� ����Ÿ
			// �����浹
			static bool CheckCollision(const float3& targetPos, float range, ObjectData& objData);
			// �����浹 range = ������ ����
			static bool CheckCollision(const float3& targetPos, const float3& targetDirection, float range, ObjectData& objData);
			// �簢�� ���� �������� 
			static bool CheckCollision(const float3& targetPos, const float3& direction, float height, float width, ObjectData& objData);
			// ��ä�� 
			static bool CheckCollision(const float3& targetPos, const float3& targetDirection, float range, float startAngle, float endAngle, ObjectData& objData);
		};

		bool IsAbleToJoinStage(StageLicense target, const std::vector<StageLicense>& myLicenses);

		struct PvpTeamType
		{
			enum type
			{
				Blue = 0,
				Red,
				NumberOfTypes,

				Spectator = 0xff,
			};
		};

		typedef std::vector< TeamMembers > PvpTeamContainerType;

		struct PvpSubInfo
		{
			enum Type
			{
				NORMAL = 0,
				GIVEUP,
				DRAW,
			};
		};

		struct PvpRoomInfo : public Serializer::ISerializable
		{
			struct RoomStatusType
			{
				enum type
				{
					Opened = 0,
					Closed,
					Playing,
				};
			};

			struct BattleGroundInfoType
				: public Serializer::ISerializable
			{
				uint8  GoalKillCount;
				float  LimitPlayTime;

				BattleGroundInfoType()
					: GoalKillCount(0)
					, LimitPlayTime(-1)
				{
				}

				void Serialize(Serializer::StreamWriter& out) const;
				void Deserialize(Serializer::StreamReader& in);

			} BattleGroundInfo;

			uint32 Id;
			std::wstring Title;
			RoomStatusType::type RoomStatus;
			Constants::GameTypes GameType;
			StageLocation Map;
			std::wstring Owner;
			bool IsPrivateRoom;
			bool IsRandomTeam;
			bool IsEquipSetRewardDisable;
			bool IsPetNotUsable;
			bool IsRebirthSkillDisable;
			bool IsInvincibleAfterRevive;
			uint8 StatusLimit;
			uint32 MaxNumberOfMembersPerTeam;
			uint32 MaxNumberOfSpectator;

			PvpRoomInfo()
				: RoomStatus(RoomStatusType::Opened)
				, MaxNumberOfMembersPerTeam(4)	//default 4:4
				, MaxNumberOfSpectator(50) //default 50��
				, IsRandomTeam(false)
				, IsPrivateRoom(false)
				, IsEquipSetRewardDisable(false)
				, IsPetNotUsable(false)
				, IsRebirthSkillDisable(false)
				, IsInvincibleAfterRevive(false)
				, StatusLimit(0)
			{
			}

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);
		};

		typedef std::vector< PvpRoomInfo > PvpRoomInfoList;

		struct PvpChannelInfo : public Serializer::ISerializable
		{
			String Name;

			int		MinLevel;
			int		MaxLevel;
			bool	IsBalanced;

			struct StatusType
			{
				enum type
				{
					Closed = 0,
					Quiet,
					Normal,
					Cloudy,
					Full
				};
			};
			StatusType::type Status;

			virtual void Serialize(Serializer::StreamWriter& out) const;
			virtual void Deserialize(Serializer::StreamReader& in);
		};

		struct PvpStartEvent
		{
			enum type
			{
				Show = 444,
				UnbindKeyAndHide = 445,

				PlayBgm = 460,

				Count1 = 451,
				Count2,	//452
				Count3,	//453
				Count4,	//454
				Count5,	//455
				Start,	//456

				// Send Flow : 444 -> 451 -> 452 -> 453 -> 454 -> 455 -> 456 ->445 -> 460 ( handled by scripts.)
				// Intruded users could receive 460(PlayBgm) only. (<- hard-corded in StageServer::Room::Room.cpp)

			};
		};

		class StylePointProcess : public Lunia::ReferenceCounted
		{
			int currentPoint; // ���� ����Ʈ
			int maxPoint; // �ִ� ����Ʈ
			bool nowProcess; // ���� ����������

			float timerData; // timerData+=lastTimer-timeGetTime. �� ������ �������ݴϴ�.
			float minusStartTime;
			int processMinusScore;
			int processMinusTime;
			float processStartGab;

			int	stopCount;		//������ ������ Stop�Ǵ� ������.. 0�϶��� Restart�� �ȴ�.
			int	playerCount;    //��Ƽ���� ����. ��Ƽ�� ���� ������ StylePoint�� ����ġ�� �ٴ´�.
		public:
			void Start(int startScore, int maxScroe);
			//void Pause();
			bool ReStart();
			bool Stop();
			bool Process(float dt/*time*/); // update

			bool GetNowProcess()
			{
				return nowProcess;
			}
			int GetCurrentStylePoint()
			{
				return currentPoint;
			}
			float GetGaguePercentage();

			int AddStylePoint(int addValue);
			void SetCurrentStylePoint(int point);
			void SetPlayerCount(int count)
			{
				playerCount = count;
			}

			int GetStopCount()
			{
				return stopCount;
			}
		public:
			StylePointProcess(int minusScore, int minusTime, float startGab = 10.f);
			~StylePointProcess();
		};

		struct PetToolTipInfo
		{
			String	PetName;
			uint16	Level;
			double	Exp;

			bool	IsRarePet;
			float	RareProbability; // RarePet�� �Ǵ� Ȯ��
			uint32	EnchantSerial; // 3.1 by Robotex

			PetToolTipInfo()
				: PetName(L"")
				, Level(0)
				, Exp(0)
				, IsRarePet(false)
				, RareProbability(0)
			{
			}

			PetToolTipInfo(const PetToolTipInfo& info)
				: PetName(info.PetName)
				, Level(info.Level)
				, Exp(info.Exp)
				, IsRarePet(info.IsRarePet)
				, RareProbability(info.RareProbability)
			{
			}

			virtual void Serialize(Serializer::StreamWriter& out) const;
			virtual void Deserialize(Serializer::StreamReader& in);
		};

		struct ConfirmTradeInfo
		{
			struct TradeItem
			{
				uint32 hash;
				InstanceEx instanceEx;
				uint16 count;
				bool isPetItem; // 3.1 by Robotex
				PetToolTipInfo pet; // 3.1 by Robotex
				virtual void Serialize(Serializer::StreamWriter& out) const;
				virtual void Deserialize(Serializer::StreamReader& in);
			};

			XRated::Serial playerSerial;
			std::wstring playerName;
			std::vector< TradeItem > items;
			uint32 money;

			virtual void Serialize(Serializer::StreamWriter& out) const;
			virtual void Deserialize(Serializer::StreamReader& in);
		};

		struct Family
		{
			typedef int FamilySerial;		///< ���� FamilySerial�̶��� �� ������ Serial�̶��� �Ǿ� �ִ� �ٸ� ������ �ߺ����� �ʱ� ���ؼ��̴�.
			typedef int64 FamilyMemberSerial;

			enum PresentType
			{
				GroupPlay,				///< �Ϸ� �ȿ����� playtime�� ���� ����.
				PersonalPlay,			///< ���� �� �÷��� Ÿ�Ӵ� �ִ� ����.
				FamilyMemorialDay,		///< ������ ����.
				LevelUp,				///< level up�ÿ� �ִ� ����.
				Rebirth,				///< ���½� �ִ� ����.
				Fishing,				///< ���ÿ��� Ư�� ������ ���� ���� �ִ� ����.
				StageClear,				///< Stage Clear�ÿ� �ִ� ����.
				StoredLevelUp,			///< ���� ���� up�ÿ� �ִ� ����.
			};

			//�йи� �ý��� ����.
			struct Info
			{
			public:
				FamilySerial	Serial;				///< �йи� �ø���(����ũ key).				
				DateTime	CreatedTime;		///< �йи� ���� �ð�.

			public:
				virtual void Serialize(Serializer::StreamWriter& out) const;
				virtual void Deserialize(Serializer::StreamReader& in);
			};

			struct MemberInfo
			{
			public:
				FamilyMemberSerial				MemberSerial;	///< �ش� ĳ������ ���� ���̵�(���� �йи��� ������ ���� �йи� �����鿡���� ����ũ key).
				std::wstring					CharacterName;	///< ĳ���� �̸�.			
				XRated::Constants::ClassType	Class;			///< ����.
				uint16							StageLevel;		///< ����.
				uint16							PvpLevel;		///< pvp ����.				
				bool							IsGuest;		///< �Խ�Ʈ ����.
				bool							IsOnline;		///< �¶��� ���� ����.				
				int64							PlayTime;		///< �йи��� ������ �� ������ total play�ð�(����: ��)
				int64							GroupPlayTime;	///< �λ����̻� �������϶� ���������� �÷����� �ð�
				DateTime					JoinedDate;		///< �йи��� ������ ��¥ : Append by rapperkjm (09/07/30)
				DateTime					LastLoggedDate;	///< ���ӿ� ���������� �α��� �� ��¥ : Append by rapperkjm (09/07/30)

			public:
				virtual void Serialize(Serializer::StreamWriter& out) const;
				virtual void Deserialize(Serializer::StreamReader& in);

			public:
				class FindByMemberSerial
				{
					const FamilyMemberSerial serial;
				public:
					FindByMemberSerial(const FamilyMemberSerial& memberSerial)
						: serial(memberSerial)
					{
					}
					bool operator()(const MemberInfo& rhs) const
					{
						return rhs.MemberSerial == serial;
					}
				};

				class FindByCharacterName
				{
					const std::wstring name;
				public:
					FindByCharacterName(const std::wstring& characterName)
						: name(characterName)
					{
					}
					bool operator()(const MemberInfo& rhs) const
					{
						return rhs.CharacterName == name;
					}
				};
			};

			struct RewardCondition
			{
				int64			NextPlayTimeForPersonalPlay;	///< �⿩���� ������ ���� ��¥.(����/��)
				DateTime		ReceivedDateGroupPlay;			///< �Ϸ� �ȿ��� 3�ð� �÷��̽� �޴� ������ ���� ��¥.
				DateTime		MemorialDay;					///< ������.

			public:
				virtual void Serialize(Serializer::StreamWriter& out) const;
				virtual void Deserialize(Serializer::StreamReader& in);
			};
		};

		struct OpenMarket
		{
			enum RegistrationGrade
			{
				Normal = 0,
				Grade1,
				Grade2,
				Grade3,
			};

			enum OrderType
			{
				None = 0,
				NameAscend = 1 << 0,
				NameDescend = 1 << 1,
				LevelAscend = 1 << 2,
				LevelDescend = 1 << 3,
				ExpiredDateAscend = 1 << 4,
				ExpiredDateDescend = 1 << 5,
				SellerAscend = 1 << 6,
				SellerDescend = 1 << 7,
				PriceAscend = 1 << 8,
				PriceDescend = 1 << 9,
			};

			struct ProductInfo
			{
				int64					RegistrationNumber;
				RegistrationGrade		Grade;

				uint32					ItemHash;
				InstanceEx				instanceEx;
				uint16					StackedCount;

				std::wstring			Seller;
				DateTime			ExpirationDate;
				int64					Price;

				bool					IsPetProduct;
				PetToolTipInfo			Pet;

				ProductInfo()
					: RegistrationNumber(0)
					, Grade(RegistrationGrade::Normal)
					, ItemHash(0)
					, instanceEx(0)
					, StackedCount(0)
					, Seller(L"")
					, ExpirationDate(DateTime::Infinite)
					, Price(0)
					, IsPetProduct(false)
					, Pet(PetToolTipInfo())
				{
				}

				ProductInfo(const ProductInfo& info)
					: RegistrationNumber(info.RegistrationNumber)
					, Grade(info.Grade)
					, ItemHash(info.ItemHash)
					, instanceEx(info.instanceEx)
					, StackedCount(info.StackedCount)
					, Seller(info.Seller)
					, ExpirationDate(info.ExpirationDate)
					, Price(info.Price)
					, IsPetProduct(info.IsPetProduct)
					, Pet(info.Pet)
				{
				}

				virtual void Serialize(Serializer::StreamWriter& out) const;
				virtual void Deserialize(Serializer::StreamReader& in);
			};

			typedef std::vector< ProductInfo > ProductInfoList;
		};

		struct UnidentifiedItemInfo : public Serializer::ISerializable
		{
			struct ProbabilityTable : public Serializer::ISerializable {
				std::wstring Id;
				uint16 StackedCount;
				float Probability;

				/* in case of unenchantable item */
				uint32 ExpireMin;
				uint32 ExpireDue;
				std::wstring ExpireDate;
				uint16 CashEnchant1Index;
				uint16 CashEnchant2Index;
				uint16 CashEnchant3Index; // 3.1 by Robotex
				uint16 CashEnchant4Index; // 3.1 by Robotex

				/* in case of enchantable item */
				InstanceEx instanceEx;

			public: /* ISerializable implementation */
				virtual void Serialize(Serializer::StreamWriter& out) const;
				virtual void Deserialize(Serializer::StreamReader& in);
			};

			std::wstring Id;
			bool AutomaticIdentify;
			//std::vector<ProbabilityTable> ProbabilityTables;

			typedef std::vector<ProbabilityTable>	ProbabilityTableList;
			const ProbabilityTableList* GetProbabilityTable(uint32 level) const;
			ProbabilityTableList* GetProbabilityTableForModify(uint32 level);

			struct ProbabilityTableListPerLevel
			{
				uint16					Level;
				ProbabilityTableList	Table;

			public:
				virtual void Serialize(Serializer::StreamWriter& out) const;
				virtual void Deserialize(Serializer::StreamReader& in);
			};
			typedef std::vector< ProbabilityTableListPerLevel > ProbabilityTablesPerLevelList;

			struct EnchantProbabilityList
			{
				std::vector< float >	ReinforcementProbabilitys;
				std::vector< float >	LightReinforcementProbabilitys;
			public:
				virtual void Serialize(Serializer::StreamWriter& out) const;
				virtual void Deserialize(Serializer::StreamReader& in);
				EnchantProbabilityList() {}
			};
			float ClassSpecializedRate;
			float MajorStatSpecializedRate;

			EnchantProbabilityList	EnchantProbabilitys;
		public: /* ISerializable implementation */
			virtual void Serialize(Serializer::StreamWriter& out) const;
			virtual void Deserialize(Serializer::StreamReader& in);

			void Sort();

			ProbabilityTablesPerLevelList::const_iterator Begin() const {
				return probabilityTablesPerLevelList.begin();
			}
			ProbabilityTablesPerLevelList::const_iterator End() const {
				return probabilityTablesPerLevelList.end();
			}
		private:

			ProbabilityTablesPerLevelList probabilityTablesPerLevelList;

			struct ProbabilityListSort
			{
				bool operator()(const ProbabilityTableListPerLevel& a, const ProbabilityTableListPerLevel& b) const
				{
					if (a.Level > b.Level) return true;
					return false;
				}
			};
		};
	} // namespace XRated
} // namespace Lunia
