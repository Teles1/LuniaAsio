#include <Core/GameConstants.h>
#include <StageServer/User/User.h>
#include <Core/Utils/ConfigReader.h>
#include <StageServer/User/UserRegistry.h>
#include <queue>
namespace Lunia {
	namespace StageServer
	{
		class UserManager {
		public:
			UserManager();
			~UserManager();

		public:
			void Auth(UserSharedPtr& user, json data);
		};

		UserManager& UserManagerInstance();
	}
}
