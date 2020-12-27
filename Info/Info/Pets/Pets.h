#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>
#include <Info/Info/Actions/Actions.h>
#include <Info/Info/NPCs/NPCManager.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct Pet
				{
					struct PetAbilities
					{
						std::map<XRated::Constants::ClassType, std::wstring> EffectState; 
						std::map<XRated::Constants::ClassType, std::wstring> EffectBuff;  

						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);
					};

					struct PetExpressions
					{
						std::wstring Situation;
						uint32       Hash;

						std::vector< std::wstring > PlayEffects;        
						std::wstring				AnimationClipName;
						uint32						EmoticonIndex;	    
						uint32                      SubEmoticonIndex;   

						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);
					};

					/**
						Pet-Genus Data Structure
					*/
					struct Genus
					{
						std::wstring Id; 
						uint32 Hash;

						float ExpFactor;
						float TiredFactor; 
						float PetGrowUpFactor; 

						float MaxRarePetProbability;
						float RareProbabilityAddPerLevel;

						std::wstring PetIconAfterSummon;
						std::wstring PetRareEffect;
						std::vector<std::wstring> Species; 

						std::wstring DefaultNPCId;
						std::wstring UI3DDisplayInfo;

						XRated::Constants::ClassType EquipClassType;
						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);
					};

					struct Species
					{
						std::wstring Id;
						uint32 Hash;	

						std::vector<std::wstring> Pets;

						int32 LevelInterval;
						int32 ArrayInterval;

						int InventorySlotCount;

						PetAbilities RarePetAbilities;

						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);
					};

					struct EffectStates
					{
						std::wstring Id;
						uint32 Hash;	

						std::wstring DisplayName; 
						std::wstring Description; 

						std::vector<ActionInfo::State> States;
						std::vector<std::wstring> StateBundles;

						int GetStateBundleIndex(int level, int additionalIndex, int levelInterval, int arrayInterval) const;
						const wchar_t* GetStateBundle(int level, int additionalIndex, int levelInterval, int arrayInterval) const;
						ActionInfo::State GetState(int level, int levelInterval) const;

						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);
					};

					PetAbilities NormalPetAbilities;

					std::wstring Id;
					uint32 Hash;	


					std::wstring PetIcon;

					std::vector<std::wstring> DefaultMeshs;

					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}