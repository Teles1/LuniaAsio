#include "Common.h"
namespace Lunia {
	namespace XRated {
		namespace Lobby {
			UserRegistryClass<User>& UserRegistry(const uint32& timeout)
			{
				return UserRegistryClass<User>::GetInstance(timeout);
			}
		}
	}
}