#include "Slot.h"
namespace Lunia {
	namespace XRated {
		namespace StageServer {

			Slot::Slot()
			{
				Clear();
			}


			Slot::~Slot()
			{
			}


			void Slot::Clear()
			{
				delete item_.Info;
			}


			void Slot::Set(const Common::ItemEx& item)
			{
				item_ = item;
			}


			bool Slot::Drop(const uint16& count) // true or false
			{
				if (item_.Info == nullptr || count == 0) return false;
				if (item_.StackedCount < count) return false;

				item_.StackedCount = item_.StackedCount - count;
				if (item_.StackedCount == 0)
					Clear();
				return true;
			}


			uint32 Slot::Delete(const uint16& count) // as it possible
			{
				if (item_.Info == nullptr || count == 0) return 0;
				if (count >= item_.StackedCount) { uint16 count = item_.StackedCount; Clear(); return count; }

				item_.StackedCount = item_.StackedCount - count;
				return count;
			}


			void Slot::Swap(Slot& slot)
			{
				Slot tmp = *this;
				*this = slot;
				slot = tmp;
			}


			bool Slot::Stack(Slot& slot)
			{
				if (slot.item_.Info != item_.Info) return false;
				if (slot.item_.StackedCount + item_.StackedCount > item_.Info->MaxCnt) return false;

				item_.StackedCount = item_.StackedCount + slot.item_.StackedCount;
				slot.Drop(slot.item_.StackedCount);
				return true;
			}


			const Common::ItemEx* Slot::GetItem() const
			{
				return &item_;
			}

			// This is Bonus Method. *^^*


		}
	}
}