#pragma once
#include <Core/GameConstants.h>
namespace Lunia {
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
		}
	}
}

