#include "Protocol.h"
namespace Lobby {
	namespace Protocol {
		namespace ToServer {
			const String Head::TypeName = L"Head";
			const uint16 Head::TypeHash = StringUtil::Hash(Head::TypeName);
		}
	}
}
