#pragma once
#include "PetsManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				NPCInfoManager database;

				void PetManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::PetManager");
					out.Write(L"pets", pets);
					out.Write(L"genuses", genuses);
					out.Write(L"species", species);
					out.Write(L"effectStates", effectStates);
					out.Write(L"petExpressions", petExpressions);
					out.Write(L"petFoodList", petFoodList);
				}

				void PetManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::PetManager");
					in.Read(L"pets", pets);
					in.Read(L"genuses", genuses);
					in.Read(L"species", species);
					in.Read(L"effectStates", effectStates);
					in.Read(L"petExpressions", petExpressions);
					in.Read(L"petFoodList", petFoodList);
				}

				void PetManager::Init(bool bForceXml)
				{
					if (bForceXml)	/// XML Loading
						LoadXmlData();
					else	/// Binary Loading
						LoadBinaryData();
				}

				void PetManager::LoadXmlData()
				{
					Clear();

					LoadPetXml(L"Database/NPC/Pet/Pet.xml");
					LoadSpeciesXml(L"Database/NPC/Pet/Species.xml");
					LoadGenusXml(L"Database/NPC/Pet/Genus.xml");
					LoadEffectStatesXml(L"Database/NPC/Pet/EffectStates.xml");
					LoadPetExpressionsXml(L"Database/NPC/Pet/Expressions.xml");
					LoadPetFoodListXml(L"Database/NPC/Pet/PetFoodList.xml");

					PostLoadingProcess();
				}

				void PetManager::LoadBinaryData()
				{
					Clear();

					LoadBinary(L"Database/Pet.b");

					PostLoadingProcess();
				}

				const Info::Pet* PetManager::GetPet(const uint32 hash) const
				{
					PetMap::const_iterator itr = pets.find(hash);

					if (itr != pets.end())
						return &itr->second;

					return NULL;
				}

				const Info::Pet* PetManager::GetPet(const std::wstring& id) const
				{
					return GetPet(StringUtil::Hash(id.c_str()));
				}

				const Info::Pet::Genus* PetManager::GetGenus(const uint32 hash) const
				{
					ReversedPetGenusMap::const_iterator i = reversedGenusIndexer.find(hash);
					if (i != reversedGenusIndexer.end())
						return i->second;

					return NULL;
				}

				const Info::Pet::Genus* PetManager::GetGenus(const std::wstring& id) const
				{
					return GetGenus(StringUtil::Hash(id.c_str()));
				}

				const Info::Pet::Genus* PetManager::GetGenus(XRated::Constants::ClassType genusType) const
				{
					ReversedPetGenusMap::const_iterator it = reversedGenusIndexer.begin();
					for (; it != reversedGenusIndexer.end(); ++it)
					{
						if (it->second != NULL && it->second->EquipClassType == genusType)
						{
							return it->second;
						}
					}

					return NULL;
				}

				const Info::Pet::Species* PetManager::GetSpecies(const uint32 hash/*Pet Hash*/) const
				{
					ReversedPetSpeciesMap::const_iterator i = reversedSpeciesIndexer.find(hash);
					if (i != reversedSpeciesIndexer.end())
						return i->second;

					return NULL;
				}

				const Info::Pet::Species* PetManager::GetSpecies(const std::wstring& id/*Pet Id*/) const
				{
					return GetSpecies(StringUtil::Hash(id.c_str()));
				}

				const Info::Pet::EffectStates* PetManager::GetEffect(const uint32 hash) const
				{
					EffectStatesMap::const_iterator itr = effectStates.find(hash);

					if (itr != effectStates.end())
						return &itr->second;

					return NULL;
				}

				const Info::Pet::EffectStates* PetManager::GetEffect(const std::wstring& id) const
				{
					return GetEffect(StringUtil::Hash(id.c_str()));
				}

				/**
					Get Normal Pet EffectState & EffectBuff

				*/
				const Info::Pet::EffectStates* PetManager::GetNormalPetEffectState(const uint32 hash/*Pet Hash*/, XRated::Constants::ClassType classType) const
				{
					PetMap::const_iterator itr = pets.find(hash);

					if (itr != pets.end())
					{
						std::map<XRated::Constants::ClassType, std::wstring>::const_iterator it_effect = itr->second.NormalPetAbilities.EffectState.find(classType);
						std::map<XRated::Constants::ClassType, std::wstring>::const_iterator it_effectForAny = itr->second.NormalPetAbilities.EffectState.find(XRated::Constants::ClassType::AnyClassType);
						if (it_effect != itr->second.NormalPetAbilities.EffectState.end())
						{
							return GetEffect(it_effect->second);
						}
						else if (it_effectForAny != itr->second.NormalPetAbilities.EffectState.end())
						{
							return GetEffect(it_effectForAny->second);
						}
					}

					return NULL;
				}
				const Info::Pet::EffectStates* PetManager::GetNormalPetEffectState(const std::wstring& id/*Pet Id*/, XRated::Constants::ClassType classType) const
				{
					return GetNormalPetEffectState(StringUtil::Hash(id.c_str()));
				}

				const Info::Pet::EffectStates* PetManager::GetNormalPetEffectBuff(const uint32 hash/*Pet Hash*/, XRated::Constants::ClassType classType) const
				{
					PetMap::const_iterator itr = pets.find(hash);

					if (itr != pets.end())
					{
						std::map<XRated::Constants::ClassType, std::wstring>::const_iterator it_effect = itr->second.NormalPetAbilities.EffectBuff.find(classType);
						std::map<XRated::Constants::ClassType, std::wstring>::const_iterator it_effectForAny = itr->second.NormalPetAbilities.EffectBuff.find(XRated::Constants::ClassType::AnyClassType);
						if (it_effect != itr->second.NormalPetAbilities.EffectBuff.end())
						{
							return GetEffect(it_effect->second);
						}
						else if (it_effectForAny != itr->second.NormalPetAbilities.EffectBuff.end())
						{
							return GetEffect(it_effectForAny->second);
						}
					}

					return NULL;
				}

				const Info::Pet::EffectStates* PetManager::GetNormalPetEffectBuff(const std::wstring& id/*Pet Id*/, XRated::Constants::ClassType classType) const
				{
					return GetNormalPetEffectBuff(StringUtil::Hash(id.c_str()));
				}

				/**
					Get Rare Pet EffectState & EffectBuff

				*/
				const Info::Pet::EffectStates* PetManager::GetRarePetEffectState(const uint32 hash/*Pet Hash*/, XRated::Constants::ClassType classType) const
				{
					const Info::Pet::Species* species = GetSpecies(hash);
					if (species != NULL) {
						std::map<XRated::Constants::ClassType, std::wstring>::const_iterator it_effect = species->RarePetAbilities.EffectState.find(classType);
						std::map<XRated::Constants::ClassType, std::wstring>::const_iterator it_effectForAny = species->RarePetAbilities.EffectState.find(XRated::Constants::ClassType::AnyClassType);
						if (it_effect != species->RarePetAbilities.EffectState.end())
						{
							return GetEffect(it_effect->second);
						}
						else if (it_effectForAny != species->RarePetAbilities.EffectState.end())
						{
							return GetEffect(it_effectForAny->second);
						}
					}
					return NULL;
				}

				const Info::Pet::EffectStates* PetManager::GetRarePetEffectState(const std::wstring& id/*Pet Id*/, XRated::Constants::ClassType classType) const
				{
					return GetRarePetEffectState(StringUtil::Hash(id.c_str()), classType);
				}

				const Info::Pet::EffectStates* PetManager::GetRarePetEffectBuff(const uint32 hash/*Pet Hash*/, XRated::Constants::ClassType classType) const
				{
					const Info::Pet::Species* species = GetSpecies(hash);
					if (species != NULL)
					{
						std::map<XRated::Constants::ClassType, std::wstring>::const_iterator it_effect = species->RarePetAbilities.EffectBuff.find(classType);
						std::map<XRated::Constants::ClassType, std::wstring>::const_iterator it_effectForAny = species->RarePetAbilities.EffectBuff.find(XRated::Constants::ClassType::AnyClassType);
						if (it_effect != species->RarePetAbilities.EffectBuff.end())
						{
							return GetEffect(it_effect->second);
						}
						else if (it_effectForAny != species->RarePetAbilities.EffectBuff.end())
						{
							return GetEffect(it_effectForAny->second);
						}
					}
					return NULL;
				}

				const Info::Pet::EffectStates* PetManager::GetRarePetEffectBuff(const std::wstring& id/*Pet Id*/, XRated::Constants::ClassType classType) const
				{
					return GetRarePetEffectBuff(StringUtil::Hash(id.c_str()), classType);
				}

				bool PetManager::IsPetItem(const uint32 hash/*Pet Hash*/) const
				{
					return GetPet(hash) != NULL;
				}

				bool PetManager::IsPetItem(const std::wstring& id/*Pet Id*/) const
				{
					return GetPet(id) != NULL;
				}


				const Info::Pet::PetExpressions* PetManager::GetPetExpression(const uint32 hash) const
				{
					PetExpressionsMap::const_iterator itr = petExpressions.find(hash);

					if (itr != petExpressions.end())
						return &itr->second;

					return NULL;
				}

				const Info::Pet::PetExpressions* PetManager::GetPetExpression(const std::wstring& situation) const
				{
					return GetPetExpression(StringUtil::Hash(situation.c_str()));
				}

				const std::vector<uint32>& PetManager::GetPetFoodList() const
				{
					return petFoodList;
				}

				void PetManager::PostLoadingProcess()
				{
					MakeReversedIndexer();
					MakePetNPC();
				}

				void PetManager::MakeReversedIndexer()
				{
					reversedGenusIndexer.clear();
					reversedSpeciesIndexer.clear();

					for (GenusMap::iterator itr = genuses.begin(); itr != genuses.end(); ++itr)
					{
						for (std::vector<std::wstring>::iterator itr2 = itr->second.Species.begin(); itr2 != itr->second.Species.end(); ++itr2)
						{
							Info::Pet::Species* speciesInfo = &(species.find(StringUtil::Hash(itr2->c_str()))->second);

							for (std::vector<std::wstring>::iterator itr3 = speciesInfo->Pets.begin(); itr3 != speciesInfo->Pets.end(); ++itr3)
							{
								if (pets.find(StringUtil::Hash(itr3->c_str())) != pets.end())
								{
									reversedGenusIndexer[StringUtil::Hash(itr3->c_str())] = &((*itr).second);
									reversedSpeciesIndexer[StringUtil::Hash(itr3->c_str())] = speciesInfo;
								}
							}
						}
					}
				}

				void PetManager::MakePetNPC()
				{
					for (PetMap::iterator itr = pets.begin(); itr != pets.end(); ++itr)
					{
						const Pet::Genus* genus = GetGenus(itr->second.Id);

						if (genus == NULL)
						{
							Logger::GetInstance().Warn(L"{0} - Not Linked Genus", itr->second.Id.c_str());
							continue;
						}

						NonPlayerInfo* npcData = database.Retrieve(genus->DefaultNPCId.c_str());

						if (npcData == NULL)
						{
							Logger::GetInstance().Error(L"{0} - Not Exist Default Pet NPC ID", genus->DefaultNPCId.c_str());
							continue;
						}

						NonPlayerInfo petNpcData = *npcData;

						petNpcData.Name = itr->second.Id;
						petNpcData.HashcodeName = StringUtil::Hash(petNpcData.Name.c_str());
						petNpcData.DefaultMeshs = itr->second.DefaultMeshs;

						if (database.Retrieve(petNpcData.Name.c_str()) == NULL)
							database.Add(petNpcData);
						else
							Logger::GetInstance().Error(L"{0} - Already Exist Pet NPC ID", petNpcData.Name.c_str());

					}
				}

				void PetManager::Clear()
				{
					pets.clear();
					species.clear();
					genuses.clear();
					effectStates.clear();

					reversedGenusIndexer.clear();
					reversedSpeciesIndexer.clear();
				}

				void PetManager::LoadPetXml(const wchar_t* filename)
				{
					std::vector<Info::Pet> tempPets;
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(filename);
					reader->Read(L"Pets", tempPets);

					for (size_t i = 0; i < tempPets.size(); ++i)
						pets[tempPets[i].Hash] = tempPets[i];
				}

				void PetManager::LoadSpeciesXml(const wchar_t* filename)
				{
					std::vector<Info::Pet::Species> tempSpecies;
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(filename);
					reader->Read(L"Species", tempSpecies);

					for (size_t i = 0; i < tempSpecies.size(); ++i)
						species[tempSpecies[i].Hash] = tempSpecies[i];
				}

				void PetManager::LoadGenusXml(const wchar_t* filename)
				{
					std::vector<Info::Pet::Genus> tempGenuses;
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(filename);
					reader->Read(L"Genuses", tempGenuses);

					for (size_t i = 0; i < tempGenuses.size(); ++i)
						genuses[tempGenuses[i].Hash] = tempGenuses[i];
				}

				void PetManager::LoadEffectStatesXml(const wchar_t* filename)
				{
					std::vector<Info::Pet::EffectStates> tempEffectStates;
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(filename);
					reader->Read(L"EffectStates", tempEffectStates);

					for (size_t i = 0; i < tempEffectStates.size(); ++i)
						effectStates[tempEffectStates[i].Hash] = tempEffectStates[i];
				}

				void PetManager::LoadPetExpressionsXml(const wchar_t* filename)
				{
					std::vector<Info::Pet::PetExpressions> tempPetExpressions;
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(filename);
					reader->Read(L"Expressions", tempPetExpressions);

					for (size_t i = 0; i < tempPetExpressions.size(); ++i)
						petExpressions[tempPetExpressions[i].Hash] = tempPetExpressions[i];
				}

				void PetManager::LoadPetFoodListXml(const wchar_t* filename)
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(filename);
					reader->Read(L"PetFoodList", petFoodList);
				}

				void PetManager::LoadBinary(const wchar_t* filename)
				{
					Clear();

					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(filename);
					reader->Read(L"PetManager", *this);
				}

				PetManager::PetMap* PetManager::GetPets()
				{
					return &pets;
				}
				PetManager::GenusMap* PetManager::GetGenuses()
				{
					return &genuses;
				}
				PetManager::SpeciesMap* PetManager::GetSpecies()
				{
					return &species;
				}
				PetManager::EffectStatesMap* PetManager::GetEffectStates()
				{
					return &effectStates;
				}
				PetManager::PetExpressionsMap* PetManager::GetPetExpressions()
				{
					return &petExpressions;
				}

			}
		}
	}
}