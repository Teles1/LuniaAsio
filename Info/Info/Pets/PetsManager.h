#pragma once
#include "Pets.h"
#include "../NPCs/Npc.h"
#include <Info/Info.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class PetManager
				{
				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);

					void Init(bool bForceXml);

					void LoadXmlData();
					void LoadBinaryData();

					/**
						Get Pet Data

						@param hash ���� Hash
						@return	const Database::Info::Pet*

					*/
					const Info::Pet* GetPet(const uint32 hash/*Pet Hash*/) const;
					const Info::Pet* GetPet(const std::wstring& id/*Pet Id*/) const;

					/**
						Get Genus Data

						@brief ���� Hash�� ������ ���� �����ִ� '��'�� �����͸� ���� �� �ִ�.
						@param petHash ���� Hash
						@return	const Database::Info::Genus*

					*/
					const Info::Pet::Genus* GetGenus(const uint32 hash/*Pet Hash*/) const;
					const Info::Pet::Genus* GetGenus(const std::wstring& id/*Pet Id*/) const;
					const Info::Pet::Genus* GetGenus(XRated::Constants::ClassType genusType/*EquipClassType*/) const;

					/**
						Get Species Data

						@brief ���� Hash�� ������ ���� �����ִ� '��'�� �����͸� ���� �� �ִ�.
						@param petHash ���� Hash
						@return	const Database::Info::Species*

					*/
					const Info::Pet::Species* GetSpecies(const uint32 hash/*Pet Hash*/) const;
					const Info::Pet::Species* GetSpecies(const std::wstring& id/*Pet Id*/) const;


					/**
						Get Genus Data

						@brief EffectState�� ID-Hash�� ������ �ش� EffectStates.xml�� ���ǵǾ� �ִ� Effect �߿� �ϳ��� ������ �´�.
						@param EffectState�� Hash
						@return	const Database::Info::EffectStates*

					*/
					const Info::Pet::EffectStates* GetEffect(const uint32 hash/*EffectState Hash*/) const;
					const Info::Pet::EffectStates* GetEffect(const std::wstring& id/*EffectState Id*/) const;

					/**
						Get Normal Pet EffectState & EffectBuff

					*/
					const Info::Pet::EffectStates* GetNormalPetEffectState(const uint32 hash/*Pet Hash*/
						, XRated::Constants::ClassType classType = XRated::Constants::ClassType::AnyClassType) const;
					const Info::Pet::EffectStates* GetNormalPetEffectState(const std::wstring& id/*Pet Id*/
						, XRated::Constants::ClassType classType = XRated::Constants::ClassType::AnyClassType) const;

					const Info::Pet::EffectStates* GetNormalPetEffectBuff(const uint32 hash/*Pet Hash*/
						, XRated::Constants::ClassType classType = XRated::Constants::ClassType::AnyClassType) const;
					const Info::Pet::EffectStates* GetNormalPetEffectBuff(const std::wstring& id/*Pet Id*/
						, XRated::Constants::ClassType classType = XRated::Constants::ClassType::AnyClassType) const;

					/**
						Get Rare Pet EffectState & EffectBuff

					*/
					const Info::Pet::EffectStates* GetRarePetEffectState(const uint32 hash/*Pet Hash*/
						, XRated::Constants::ClassType classType = XRated::Constants::ClassType::AnyClassType) const;
					const Info::Pet::EffectStates* GetRarePetEffectState(const std::wstring& id/*Pet Id*/
						, XRated::Constants::ClassType classType = XRated::Constants::ClassType::AnyClassType) const;

					const Info::Pet::EffectStates* GetRarePetEffectBuff(const uint32 hash/*Pet Hash*/
						, XRated::Constants::ClassType classType = XRated::Constants::ClassType::AnyClassType) const;
					const Info::Pet::EffectStates* GetRarePetEffectBuff(const std::wstring& id/*Pet Id*/
						, XRated::Constants::ClassType classType = XRated::Constants::ClassType::AnyClassType) const;

					/// ��ȹ���� ��� ��������������
					/// Pet�� Id�� PetItem�� Id�� ���� Yong-Eun, Jung 2008.08.01		
					bool IsPetItem(const uint32 hash/*Pet Hash*/) const;
					bool IsPetItem(const std::wstring& id/*Pet Id*/) const;

					/**
						���� Expression(ǥ��) �����͸� ������ �´�.
						Id ���� Situation ������, Database/NPC/Pet/Expressions.xml �� ���ǵǾ� �ִ�.
					*/
					const Info::Pet::PetExpressions* GetPetExpression(const uint32 hash) const;
					const Info::Pet::PetExpressions* GetPetExpression(const std::wstring& situation) const;

					const std::vector<uint32>& GetPetFoodList() const;

					typedef std::map<uint32, Info::Pet> PetMap;
					typedef std::map<uint32, Info::Pet::Genus> GenusMap;
					typedef std::map<uint32, Info::Pet::Species> SpeciesMap;
					typedef std::map<uint32, Info::Pet::EffectStates> EffectStatesMap;
					typedef std::map<uint32, Info::Pet::PetExpressions> PetExpressionsMap;

					typedef std::map<uint32/*PetHash*/, const Info::Pet::Genus*, Less<uint32> > ReversedPetGenusMap;
					typedef std::map<uint32/*PetHash*/, const Info::Pet::Species*, Less<uint32> > ReversedPetSpeciesMap;

				protected:
					PetMap pets;
					GenusMap genuses;
					SpeciesMap species;
					EffectStatesMap effectStates;
					PetExpressionsMap petExpressions;

					ReversedPetGenusMap reversedGenusIndexer;
					ReversedPetSpeciesMap reversedSpeciesIndexer;

					std::vector<uint32> petFoodList;

					virtual void MakePetNPC();

					/* fileloading */
					void Clear();
				public:
					void LoadPetXml(const wchar_t* filename);
					void LoadSpeciesXml(const wchar_t* filename);
					void LoadGenusXml(const wchar_t* filename);
					void LoadEffectStatesXml(const wchar_t* filename);
					void LoadPetExpressionsXml(const wchar_t* filename);
					void LoadPetFoodListXml(const wchar_t* filename);
					void LoadBinary(const wchar_t* filename);

					void PostLoadingProcess();

					virtual void MakeReversedIndexer(); 
					PetMap* GetPets();
					GenusMap* GetGenuses();
					SpeciesMap* GetSpecies();
					EffectStatesMap* GetEffectStates();
					PetExpressionsMap* GetPetExpressions();
				};
			}
		}
	}
}