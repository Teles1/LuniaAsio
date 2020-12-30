#ifndef CommonStageServer_H
#define CommonStageServer_H
#pragma once
#include <Info/Info.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;
			namespace Common {
				enum ROOMKIND { STAGE = 0, SQUARE, PVP };
				enum { NONPARTY = -1, PARTYMAX = 4 };

				typedef struct stItemSlotEx : ItemSlot
				{
					const Database::Info::ItemInfo* ItemInfo;
					GlobalSerial serial;
					int IsBank; // 0 = false, 1 = true

					bool operator==(const stItemSlotEx& rhs) const;
					bool operator!=(const stItemSlotEx& rhs) const;

					//struct FinderByPos
					//{
					//	XRated::ItemPosition pos;
					//	FinderByPos(XRated::ItemPosition pos) : pos(pos) {}
					//	bool operator()(stItemSlotEx& p) const { return p.Position==pos; }
					//};
				}ItemSlotEx;



				struct PetItemSlotEx : PetItemSlot
				{
					const Database::Info::ItemInfo* ItemInfo;
					GlobalSerial Serial;
					GlobalSerial PetSerial;

				};

				struct ItemEx
				{
					const Database::Info::ItemInfo* Info;
					GlobalSerial Serial;
					InstanceEx InstanceEx;
					uint16 StackedCount;

					void operator=(const ItemEx& rhs);
					bool operator==(const ItemEx& rhs) const;
					bool operator!=(const ItemEx& rhs) const;
					bool IsTradable(User* user) const;

					// functors for comparing interfaces //////////////////////////////////////////////////
					class IFindCondition
					{
					public: virtual bool operator()(const ItemEx& rhs) const = 0;
					};
					class ICountCondition
					{
					public: virtual uint32 operator()(const ItemEx& rhs) const = 0;
					};


					// helper conditions //////////////////////////////////////////////////////////////////
					class FindCondition /*: public Common::ItemEx::IFindCondition*/
					{
						uint32 hash;
					public:
						FindCondition(uint32 hash) : hash(hash) {}
						bool operator()(const Common::ItemEx& rhs) const;
					};
					class CountCondition /*: public Common::ItemEx::ICountCondition*/
					{
						uint32 hash;
					public:
						CountCondition(uint32 hash) : hash(hash) {}
						uint32 operator()(const Common::ItemEx& rhs) const;
					};


					ItemEx()
						: Info(NULL), Serial(0), InstanceEx(0), StackedCount(0)
					{
					}
				};

				struct GuildInfo {
					int GuildId;
					String GuildAlias;
					uint32 GuildExp;
					uint32 Point;
					DateTime	ProfitStartDate;
					DateTime	ProfitEndDate;
					String GuildName;
					String Message; //Guild Message`
					String MasterName; //Guild Master character name
					uint8 GuildLevel;
					int MemberCount;
					String CreatedDate;
					GuildInfo() : GuildId(0), GuildExp(0), Point(0), GuildLevel(0), MemberCount(0) {}

					GuildInfo& operator =(const AllMGuildInfo& info);
				};
			}
		}
	}
}
#endif // !CommonStageServer_H