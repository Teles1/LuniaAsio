#include <StageServer/User/Services/FamilyManager.h>
#include <StageServer/User/User.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			const static std::string guestStageLevelUpLogName("family-gift-levelup-g");
			const static std::string memberStageLevelUpLogName("family-gift-levelup-m");
			const static std::string guestStoredLevelUpLogName("family-gift-slevelup-g");
			const static std::string memberStoredLevelUpLogName("family-gift-slevelup-m");
			const static std::string rebirthLogName("family-gift-rebirth");
			const static std::string contributeLogName("family-gift-contribute");
			const static std::string groupPlayTimeLogName("family-gift-playtime");
			const static std::string fishingLogName("family-gift-fishing");
			const static std::string memorialDayLogName("family-gift-memorialday");
			const static std::string stateClearLogName("family-gift-stageclear");

			FamilyManager::FamilyManager(User& owner) 
				: m_Owner(owner)
			{

			}
		}
	}
}