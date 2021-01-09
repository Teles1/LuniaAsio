#pragma once
#include <Core/GameConstants.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class PetDatas
			{
			private:
				std::vector<XRated::Pet> pets;
			public:
				std::mutex m_Mtx;	// thread can be accesed from net receive, background(http response) and logic event (room)
			
				const XRated::Pet* GetPetData(const XRated::GlobalSerial& petSerial);
				void UpdatedPetData(const XRated::Pet& pet);
				void RequestedSummonPet(XRated::GlobalSerial serial);
				void Add(const std::vector<XRated::Pet>& in);
				void Add(const XRated::Pet& in);
				void Remove(const XRated::GlobalSerial& petSerial);
				void Clear();

				std::mutex& GetSyncObject();
			public:
				PetDatas();
				virtual ~PetDatas();
			};

			class PetOrigin : public XRated::Pet
			{
			public:
				bool operator !=(const Pet& in) const;
				PetOrigin& operator =(const Pet& in);
			public:
				bool needChangeOwner;
				bool isDeleted;

				PetOrigin();
				PetOrigin(const XRated::Pet& in);
			};
		}
	}
}