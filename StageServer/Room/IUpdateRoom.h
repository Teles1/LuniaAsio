#pragma once
#include <Logic/Logic.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;
			typedef std::shared_ptr<User> UserSharedPtr;
			class IUpdateRoom
			{
			public:
				virtual ~IUpdateRoom() {}

			public:
				virtual uint16 GetIndex() = 0;
				virtual uint16 GetThreadIndex() = 0;
				virtual void SetThreadIndex(const uint16& index) = 0;

				virtual bool Update(const float& dt) = 0;
				//virtual void	UpdateExpFactor() = 0;
				//virtual void	NoticeHolidayEvent(uint32 eventId, bool start) = 0;

				virtual bool JoinUser(UserSharedPtr user, const std::string& roomPass) = 0;
				virtual bool PartUser(UserSharedPtr user) = 0;

				virtual const Logic::ILogic::UpdateInfo& GetLogicUpdateInfo() = 0;
			};
		}
	}
}