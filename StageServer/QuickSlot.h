#pragma once
#include <Core/GameConstants.h>
namespace Lunia {
	namespace StageServer {
		typedef std::vector<XRated::QuickSlot> QuickSlotList;

		class QuickSlot
		{
		private:
			enum { MAXQUICKSLOT = 30 };

		public:
			QuickSlot();
			virtual ~QuickSlot();

			void Init();

			//bool SetSlot(uint32 pos, XRated::QuickSlot& slot);
			bool SetSlot(uint32 pos, uint32 itemHash, bool isSkill, const XRated::InstanceEx& instance);
			bool SetSlot(const XRated::QuickSlot& inQuick);
			bool MoveSlot(uint32 pvSlot, uint32 nxSlot);
			bool ClearSlot(uint32 pos);

			void Clear();

			const QuickSlotList& GetQuickSlotList() { return quickSlotList; }

			void UpdateOriginData();
			void GetChangedQuickSlots(QuickSlotList& inserts, QuickSlotList& updates, QuickSlotList& deletes);
		private:
			QuickSlotList	quickSlotList;
			QuickSlotList	originQuickSlotList;
		};
	}
}