#pragma once
#include <Core/GameConstants.h>
#include <Info/Info.h>
#include <StageServer/Common.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			struct SlotPos
			{
				int bag;
				int pos;
				SlotPos(int bag, int pos) : bag(bag), pos(pos) {}
			};
			class Slot
			{
			public:
				enum { MAXCOUNT = 100 };

			public:
				Slot();
				~Slot();

				void Clear();
				void Set(const Common::ItemEx& item);
				bool Drop(const uint16& count); ///< @return false if the drop is failed. @remarks failes every mismatch.
				uint32 Delete(const uint16& count); ///< @return dropped count. @remarks drop as it possible and if the coun is larger than stacked, the slot clears automatically
				void Swap(Slot& slot);
				bool Stack(Slot& slot);

				const Common::ItemEx* GetItem() const;
			private:
				Common::ItemEx item_;
			};


		}
	}
}