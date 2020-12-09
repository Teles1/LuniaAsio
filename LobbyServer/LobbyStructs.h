#ifndef LobbyStructs_Guard
#define LobbyStructs_Guard
#include <Core/GameConstants.h>
#include <Core/Serializer/ISerializable.h>
#include <Core/Utils/DateTime.h>
#pragma once
namespace Lunia {
	namespace Lobby {
		struct ItemSlot : public Serializer::ISerializable
		{
			uint32 Id;
			ItemPosition Position;
			InstanceEx InstanceEx;
			uint16 Stacked;

			ItemSlot()
				: Id(0)
				, Stacked(1)
				, InstanceEx(0)
			{
			}

			void Serialize(Serializer::StreamWriter& out) const;
			void Deserialize(Serializer::StreamReader& in);

			void operator=(const ItemSlot& rhs)
			{
				Id = rhs.Id;
				Position = rhs.Position;
				InstanceEx = rhs.InstanceEx;
				Stacked = rhs.Stacked;
			}

			bool operator==(const ItemSlot& rhs) const
			{
				return (Id == rhs.Id) && (Position == rhs.Position) && (InstanceEx == rhs.InstanceEx) && (Stacked == rhs.Stacked);
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

		struct LobbyPlayerInfo : public Serializer::ISerializable
		{
			String CharacterName;
			int64 CharacterSerial;
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
		// StateFlags /////////////////////////////////////////////////////////////////////////////

	/* character state flag contians account's like IsPcRoom because usually character states contain account states */
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

			}
		};

	}
}
#endif // !LobbyStructs_Guard
