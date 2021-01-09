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
				AutoLock lock(m_Mtx);
				std::vector<XRated::Pet>::iterator iter = std::find_if(pets.begin(), pets.end(), XRated::Pet::Finder(pet.PetSerial));
				if (iter != pets.end()) {
					(*iter) = pet;
				}
				else {
					pets.push_back(pet);
				}
			}
			void PetDatas::Remove(const XRated::GlobalSerial& petSerial)
			{
				AutoLock lock(m_Mtx);
				std::vector<XRated::Pet>::iterator iter = std::find_if(pets.begin(), pets.end(), XRated::Pet::Finder(petSerial));
				if (iter != pets.end()) {
					pets.erase(iter);
				}
			}
			void PetDatas::RequestedSummonPet(XRated::GlobalSerial serial)
			{
				AutoLock lock(m_Mtx);
				std::vector<XRated::Pet>::iterator iter = std::find_if(pets.begin(), pets.end(), XRated::Pet::Finder(serial));
				if (iter != pets.end()) {
					(*iter).Appear = true;
				}
			}
			void PetDatas::Add(const std::vector<XRated::Pet>& in)
			{
				AutoLock lock(m_Mtx);
				pets = in;
			}
			void PetDatas::Add(const XRated::Pet& in)
			{
				AutoLock lock(m_Mtx);
				pets.push_back(in);
			}
			void PetDatas::Clear()
			{
				AutoLock lock(m_Mtx);
				pets.clear();
				pets.reserve(30);
			}
			std::mutex& PetDatas::GetSyncObject()
			{
				return m_Mtx;
			}
			bool PetOrigin::operator!=(const Pet& in) const
			{
				if (PetSerial != in.PetSerial) return true;
				if (Level != in.Level) return true;
				if (Exp != in.Exp) return true;
				if (Full != in.Full) return true;
				if (PetName != in.PetName) return true;
				if (RareProbability != in.RareProbability) return true;
				if (FullSum != in.FullSum) return true;
				if (LevelUpPeriod != in.LevelUpPeriod) return true;
				return false;
			}
			PetOrigin& PetOrigin::operator=(const Pet& in)
			{
				PetSerial = in.PetSerial;
				Level = in.Level;
				Exp = in.Exp;
				Full = in.Full;
				PetName = in.PetName;
				RareProbability = in.RareProbability;
				FullSum = in.FullSum;
				LevelUpPeriod = in.LevelUpPeriod;
				isDeleted = false;
				return *this;
			}
			PetOrigin::PetOrigin() : isDeleted(false), needChangeOwner(false) {}
			PetOrigin::PetOrigin(const XRated::Pet& in): XRated::Pet(in), isDeleted(false), needChangeOwner(false) {}
		}
    }
}
