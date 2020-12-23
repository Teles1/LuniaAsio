#pragma once
#include <Core/GameConstants.h>
#include <Core/CriticalSection.h>
#include <Info/Info/Items/Item.h>
#include <Info/Info/StageInfo/StageInfo.h>
#include <Core/Logic/Sector/Sector.h>

namespace Lunia {
	namespace XRated {
		namespace Logic {

			class Player;
			class ILogic
			{
			public:
				struct Config
				{
					int NumberOfBossRewardUnidentifiedItem;

					Config()
						: NumberOfBossRewardUnidentifiedItem(1)
					{
					}
				};

				struct PlayerInitialData
				{
					CriticalSection SyncRoot;
					std::wstring name;
					Constants::ClassType type;
					uint32 xp;
					uint16 level;
					uint32 pvpXp;
					uint16 pvpLevel;
					uint32 warXp;
					uint32 warLevel;
					uint16 storedLevel;
					uint16 rebirthCount;
					uint16 storedSkillPoint;
					uint32 money;
					uint32 bankMoney;
					uint16 life;
					uint16 bonusLife;
					uint16 skillpoint;
					uint32 ladderPoint;
					uint16 ladderMatchCount;
					uint32 ladderWinCount;
					uint32 ladderLoseCount;
					uint32 achievementScore;
					uint16 addedSkillPointPlus;
					uint8 team;
					XRated::CharacterStateFlags characterStateFlags;
					void* user;

					struct FactorGroup
					{
						FactorGroup() : ExpFactor(1.0f), MaxHp(1.0f), MaxMp(1.0f) {}

						float ExpFactor; 
						float MaxHp;
						float MaxMp;
					} Factors;

					std::vector<XRated::Skill> skills;
					std::vector< std::pair<const Database::Info::ItemInfo*, int64/*enchant bitfields or normal bitfields*/> > equipments;
					//std::vector< XRated::Quest > quests;

					struct StartPositionTag
					{
						bool UseCustomStartPoint;
						Database::Info::StageInfo::Placement Place;

						StartPositionTag() : UseCustomStartPoint(false) { }
						StartPositionTag(const Database::Info::StageInfo::Placement& place) : UseCustomStartPoint(true), Place(place) { }
					} StartPosition;

					std::vector<Pet>		pets;
					std::vector<PetCaredBySchool> petCared;

					DateTime lastRebirthDateTime;

					PlayerInitialData& operator=(const PlayerInitialData& rhs);
				};

				struct UpdateInfo {
					bool bUpdated;
					double updateTime;
					double processCommandTime;
					double largest, largestCmd;
					int playerCnt, npcCnt, projectileCnt, miscCnt;
					int playerCntBefore, npcCntBefore, projectileCntBefore, miscCntBefore;
					int cmdCount;

					void Init() {
						bUpdated = false; updateTime = processCommandTime = largest = largestCmd = 0;
						playerCnt = npcCnt = projectileCnt = miscCnt = cmdCount = 0;
						playerCntBefore = npcCntBefore = projectileCntBefore = miscCntBefore = 0;
					}
					void SetProcessCommandTime(double time) { if (largestCmd < time) largestCmd = time; processCommandTime = time; }
					void SetProcessTime(double time) { if (largest < time) largest = time; updateTime = time; }
				};

				struct TamingInfo : public Serializer::ISerializable {
					Serial owner;
					Serial tamed;
					Constants::Familiar::Type type;

				public:
					TamingInfo(Serial o, Serial t, Constants::Familiar::Type tp) : owner(o), tamed(t), type(tp) {}

				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};

				enum InstantCoinReviveError
				{
					NoError = 0,
					RollbackRequested,
					PvPBlocked,
				};
			};
		}

	}
}
