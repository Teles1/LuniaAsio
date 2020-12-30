#pragma once
#include <Core/GameConstants.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class PetDatas
			{
			private:
				std::vector<XRated::Pet> pets;
				std::mutex cs;	// thread can be accesed from net receive, background(http response) and logic event (room)
			public:
				const XRated::Pet* GetPetData(const XRated::GlobalSerial& petSerial);
				void UpdatedPetData(const XRated::Pet& pet);
				void RequestedSummonPet(XRated::GlobalSerial serial);
				void Add(const std::vector<XRated::Pet>& in);
				void Add(const XRated::Pet& in);
				void Remove(const XRated::GlobalSerial petSerial);
				void Clear();

				std::mutex& GetSyncObject();///< should be use by room
			public:
				PetDatas();
				virtual ~PetDatas();
			};
		}
	}
}