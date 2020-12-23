#include <StageServer/QuickSlot.h>
namespace Lunia {
	namespace StageServer {
		QuickSlot::QuickSlot()
		{
			Init();
		}


		QuickSlot::~QuickSlot()
		{
		}


		void QuickSlot::Init()
		{
			quickSlotList.assign(MAXQUICKSLOT, XRated::QuickSlot());
			originQuickSlotList.assign(MAXQUICKSLOT, XRated::QuickSlot());

			size_t i;
			for (i = 0; i < MAXQUICKSLOT; ++i)
				quickSlotList[i].Pos = i;
			for (i = 0; i < MAXQUICKSLOT; ++i)
				originQuickSlotList[i].Pos = i;

		}


		//bool QuickSlot::SetSlot(uint32 pos, XRated::QuickSlot& slot)
		//{
		//	if( pos >= MAXQUICKSLOT )
		//	{
		//		ALLM_ERROR(( L"QuickSlot::SetSlot() - Error : pos(%d) >= MAXQUICKSLOT", pos ));
		//		return false;
		//	}

		//	quickSlotList[pos].Id = slot.Id;
		//	quickSlotList[pos].IsSkill = slot.IsSkill;
		//	return true;
		//}


		bool QuickSlot::SetSlot(uint32 pos, uint32 itemHash, bool isSkill, const XRated::InstanceEx& instance)
		{
			if (pos >= MAXQUICKSLOT)
			{
				Logger::GetInstance().Error(L"QuickSlot::SetSlot() - Error : pos({0}) >= MAXQUICKSLOT", pos);
				return false;
			}

			quickSlotList[pos].Id = itemHash;
			quickSlotList[pos].IsSkill = isSkill;
			quickSlotList[pos].InstanceEx = instance;
			return true;
		}

		bool QuickSlot::SetSlot(const XRated::QuickSlot& inQuick)
		{
			if (inQuick.Pos >= MAXQUICKSLOT)
			{
				Logger::GetInstance().Error(L"QuickSlot::SetSlot() - Error : pos({0}) >= MAXQUICKSLOT", inQuick.Pos);
				return false;
			}
			quickSlotList[inQuick.Pos].Id = inQuick.Id;
			quickSlotList[inQuick.Pos].IsSkill = inQuick.IsSkill;
			quickSlotList[inQuick.Pos].InstanceEx = inQuick.InstanceEx;
			return true;
		}


		bool QuickSlot::MoveSlot(uint32 pvSlot, uint32 nxSlot)
		{
			if ((pvSlot >= MAXQUICKSLOT) || (nxSlot >= MAXQUICKSLOT))
			{
				Logger::GetInstance().Error(L"QuickSlot::MoveSlot() - Error : pvSlot({0}), nxSlot({1}) >= MAXQUICKSLOT", pvSlot, nxSlot);
				return false;
			}

			uint32 tempId = quickSlotList[nxSlot].Id;
			bool tempIsSkill = quickSlotList[nxSlot].IsSkill;

			quickSlotList[nxSlot].Id = quickSlotList[pvSlot].Id;
			quickSlotList[nxSlot].IsSkill = quickSlotList[pvSlot].IsSkill;
			quickSlotList[pvSlot].Id = tempId;
			quickSlotList[pvSlot].IsSkill = tempIsSkill;
			return true;
		}


		bool QuickSlot::ClearSlot(uint32 pos)
		{
			if (pos >= MAXQUICKSLOT)
			{
				Logger::GetInstance().Error(L"QuickSlot::DropSlot() - Error : pos({0}) >= MAXQUICKSLOT", pos);
				return false;
			}

			quickSlotList[pos].Id = 0;
			quickSlotList[pos].IsSkill = false;
			return true;
		}


		void QuickSlot::Clear()
		{
			uint32 i;
			for (i = 0; i < MAXQUICKSLOT; ++i)
				ClearSlot(i);

			for (i = 0; i < MAXQUICKSLOT; ++i) {
				originQuickSlotList[i].Id = 0;
				originQuickSlotList[i].IsSkill = false;
			}
		}
		void QuickSlot::UpdateOriginData()
		{
			originQuickSlotList = quickSlotList;
		}
		void QuickSlot::GetChangedQuickSlots(QuickSlotList& inserts, QuickSlotList& updates, QuickSlotList& deletes)
		{
			for (size_t i = 0; i < MAXQUICKSLOT; ++i) {
				const XRated::QuickSlot& origin(originQuickSlotList[i]);
				const XRated::QuickSlot& now(quickSlotList[i]);
				if (((origin.Id != now.Id) || (origin.InstanceEx != now.InstanceEx) || (origin.IsSkill != now.IsSkill)) && ((origin.Id != 0) || (now.Id != 0))) {
					if (0 == origin.Id) {
						//insert
						inserts.push_back(now);
					}
					else if (0 == now.Id) {
						//delete
						deletes.push_back(origin);
					}
					else {
						//update
						updates.push_back(now);
					}
				}
			}
		}
	}
}