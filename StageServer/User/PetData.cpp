#include "PetData.h"

namespace Lunia {
    namespace XRated {
        namespace StageServer {
			PetDatas::PetDatas()
			{
				pets.reserve(30);
			}
			PetDatas::~PetDatas()
			{
			}
			const XRated::Pet* PetDatas::GetPetData(const XRated::GlobalSerial& petSerial)
			{
				std::vector<XRated::Pet>::iterator iter = std::find_if(pets.begin(), pets.end(), XRated::Pet::Finder(petSerial));
				if (iter != pets.end()) {
					return &(*iter);
				}
				return NULL;
			}
			void PetDatas::UpdatedPetData(const XRated::Pet& pet)
			{
				AutoLock lock(cs);
				std::vector<XRated::Pet>::iterator iter = std::find_if(pets.begin(), pets.end(), XRated::Pet::Finder(pet.PetSerial));
				if (iter != pets.end()) {
					(*iter) = pet;
				}
				else {
					pets.push_back(pet);
				}
			}
			void PetDatas::Remove(const XRated::GlobalSerial petSerial)
			{
				AutoLock lock(cs);
				std::vector<XRated::Pet>::iterator iter = std::find_if(pets.begin(), pets.end(), XRated::Pet::Finder(petSerial));
				if (iter != pets.end()) {
					pets.erase(iter);
				}
			}
			void PetDatas::RequestedSummonPet(XRated::GlobalSerial serial)
			{
				AutoLock lock(cs);
				std::vector<XRated::Pet>::iterator iter = std::find_if(pets.begin(), pets.end(), XRated::Pet::Finder(serial));
				if (iter != pets.end()) {
					(*iter).Appear = true;
				}
			}
			void PetDatas::Add(const std::vector<XRated::Pet>& in)
			{
				AutoLock lock(cs);
				pets = in;
			}
			void PetDatas::Add(const XRated::Pet& in)
			{
				AutoLock lock(cs);
				pets.push_back(in);
			}
			void PetDatas::Clear()
			{
				AutoLock lock(cs);
				pets.clear();
				pets.reserve(30);
			}
			std::mutex& PetDatas::GetSyncObject()
			{
				return cs;
			}
        }
    }
}
