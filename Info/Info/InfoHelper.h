#pragma once

#include "../Info.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				class FindReviveState : public ItemInfo::IFindCondition {
					StateInfo::Type type;
				public:
					FindReviveState(StateInfo::Type t) : type(t) {}
					bool operator()(const ItemInfo& rhs) const {
						std::vector<ActionInfo::State>::const_iterator i = rhs.States.begin();
						std::vector<ActionInfo::State>::const_iterator iEnd = rhs.States.end();
						for (; i != iEnd; ++i)
							if ((*i).type == type) return true;
						return false;
					}

				};
				class FindHash : public ItemInfo::IFindCondition
				{
					uint32 Hash;
				public:
					FindHash(uint32 hash)
						: Hash(hash)
					{}

					bool operator() (const ItemInfo& rhs) const
					{
						if (this->Hash == rhs.Hash)
							return true;
						else
							return false;
					}
				};

			}
		}
	}
}
