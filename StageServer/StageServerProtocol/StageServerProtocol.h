#pragma once
#include <Core/GameConstants.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			namespace Protocol {
				/*
					We shall see the reason why they did this.
				*/
				class IPacketSerializable : public Serializer::ISerializable {
				public:
					virtual void Serialize(Serializer::IStreamWriter& out) const = 0;
					virtual void Deserialize(Serializer::IStreamReader& in) = 0;

					virtual const wchar_t* GetTypeName() = 0;

					virtual ~IPacketSerializable() {}
				};

				struct Error : public IPacketSerializable
				{
					int			 errorcode;
					std::wstring errorstring;

					NET_SERIALIZABLE;
				};

				struct Way : public IPacketSerializable
				{
					uint32 EncryptKey;

					NET_SERIALIZABLE;
				};

				struct Stage : public IPacketSerializable
				{
					//To Client
					std::wstring charactername;
					XRated::StageLicense targetStage;
					//From Client
					std::wstring secukey;
					uint32 Version;
					std::wstring Locale;

					NET_SERIALIZABLE;
				};

				struct Alive : public IPacketSerializable
				{
					uint64	RequestTime;
					uint32 EncryptKey;

					NET_SERIALIZABLE;
				};

				struct Granted : public IPacketSerializable
				{
					std::wstring charactername;
					XRated::StageLicense targetStage;

					NET_SERIALIZABLE;
				};

				struct CharacterInfo : public IPacketSerializable
				{
					XRated::Constants::ClassType classtype;
					uint16 level;
					uint32 xp;
					uint16 pvpLevel;
					uint32 pvpXp;
					uint16 warLevel;
					uint32 warXp;
					uint16 storedLevel;
					uint16 rebirthCount;
					uint32 money;
					uint32 bankMoney;
					uint16 life;
					uint16 skillpoint;
					uint16 addedSkillPointPlus;
					uint16 storedSkillPoint;
					uint16 extrabag;
					uint16 extrabank;
					uint32 ladderPoint;
					uint16 ladderMatchCount;
					uint32 ladderWinCount;
					uint32 ladderLoseCount;
					uint32 achievementScore;
					bool IsSpectator;

					NET_SERIALIZABLE;
				};

				struct ListSkillLicenses : public IPacketSerializable
				{
					std::vector< uint32 > listSkillLicenses;

					NET_SERIALIZABLE;
				};

				struct ListSkill : public IPacketSerializable
				{
					std::vector< XRated::Skill > listskill;

					NET_SERIALIZABLE;
				};

				struct ListQuickSlot : public IPacketSerializable
				{
					std::vector<XRated::QuickSlot> quickslotlist;

					NET_SERIALIZABLE;
				};

				struct ListItem : public IPacketSerializable
				{
					std::vector< XRated::ItemSlot >	listitem;

					NET_SERIALIZABLE;
				};

				struct ListPetItem : public IPacketSerializable
				{
					std::map< XRated::GlobalSerial, std::vector< XRated::PetItemSlot > >	PetsItems;

					NET_SERIALIZABLE;
				};

				struct PetInfo : public IPacketSerializable
				{
					std::vector<XRated::PetDataWithItemPos> PetDataWithPos;

					NET_SERIALIZABLE;
				};

				struct Join : public IPacketSerializable
				{
					NET_SERIALIZABLE;
				};

				struct PetsCaredBySchool : public IPacketSerializable
				{
					XRated::Serial                        OwnerSerial;
					std::vector<XRated::PetCaredBySchool> CaredPets;

					NET_SERIALIZABLE;
				};

				struct LoadEnd : public IPacketSerializable
				{
					std::wstring charName;
					float progress; // 100% = 1.0f

					NET_SERIALIZABLE;
				};

				struct BagStates : public IPacketSerializable
				{
					std::vector<XRated::BagState> Bags;
					std::vector<XRated::BagState> BankBags;

					NET_SERIALIZABLE;
				};

				struct CreatePlayer : public IPacketSerializable
				{
					uint32 playerserial;
					XRated::Constants::ClassType classtype;
					std::wstring charactername;
					uint16 level;
					uint16 pvpLevel;
					uint16 warLevel;
					uint16 storedLevel;
					uint16 rebirthCount;
					uint32 ladderPoint;
					uint16 ladderMatchCount;
					uint32 ladderWinCount;
					uint32 ladderLoseCount;
					uint32 achievementScore;
					uint16 addedSkillPointByRebirth;
					float3 position;
					float3 direction;
					float hp;
					uint16 team;
					struct Equipment : public Serializer::ISerializable
					{
						uint16 Position;
						uint32 ItemHash;
						XRated::InstanceEx instanceEx;

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

					NET_SERIALIZABLE;
				};

				struct Quest
				{
					struct WorkingList : public IPacketSerializable
					{
						std::vector< XRated::Quest > Quests;

						NET_SERIALIZABLE;
					};

				}; // Quest

				struct Family
				{
					struct Info : public IPacketSerializable
					{
						typedef std::vector<XRated::Family::MemberInfo> FamilyMemberList;

						XRated::Family::Info	FamilyInfo;
						XRated::Family::RewardCondition	Condition;
						FamilyMemberList		Members;

						NET_SERIALIZABLE;
					};
				}; // Family
			}
		}
	}
}

