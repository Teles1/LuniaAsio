#pragma once
#include "Pets.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				void Pet::PetAbilities::Serialize(Serializer::IStreamWriter& out) const
				{
					std::map<int32, std::wstring> tempStorage;
					std::map<XRated::Constants::ClassType, std::wstring>::const_iterator it_Effect;

					out.Begin(L"AllM::XRated::Database::Info::Pet::PetAbilities");

					it_Effect = EffectState.begin();
					while (it_Effect != EffectState.end()) {
						tempStorage[static_cast<int32>(it_Effect->first)] = it_Effect->second;
						++it_Effect;
					}
					out.Write(L"EffectState", tempStorage);
					
					it_Effect = EffectBuff.begin();
					while (it_Effect != EffectBuff.end())
					{
						tempStorage[static_cast<int32>(it_Effect->first)] = it_Effect->second;
						++it_Effect;
					}
					out.Write(L"EffectBuff", tempStorage);
				}

				void Pet::PetAbilities::Deserialize(Serializer::IStreamReader& in)
				{
					std::map<int32, std::wstring> tempStorage;
					std::map<int32, std::wstring>::const_iterator it_Effect;

					in.Begin(L"AllM::XRated::Database::Info::Pet::PetAbilities");

					in.Read(L"EffectState", tempStorage);
					it_Effect = tempStorage.begin();
					while (it_Effect != tempStorage.end())
					{
						EffectState[static_cast<Constants::ClassType>(it_Effect->first)] = it_Effect->second;
						++it_Effect;
					}
					in.Read(L"EffectBuff", tempStorage);
					it_Effect = tempStorage.begin();
					while (it_Effect != tempStorage.end())
					{
						EffectBuff[static_cast<Constants::ClassType>(it_Effect->first)] = it_Effect->second;
						++it_Effect;
					}
				}

				void Pet::PetExpressions::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::Pet::PetExpressions");
					out.Write(L"Situation", Situation);
					out.Write(L"PlayEffects", PlayEffects);
					out.Write(L"AnimationClipName", AnimationClipName);
					out.Write(L"EmoticonIndex", EmoticonIndex);
					out.Write(L"SubEmoticonIndex", SubEmoticonIndex);
				}

				void Pet::PetExpressions::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::Pet::PetExpressions");
					in.Read(L"Situation", Situation);
					in.Read(L"PlayEffects", PlayEffects);
					in.Read(L"AnimationClipName", AnimationClipName);
					in.Read(L"EmoticonIndex", EmoticonIndex);
					in.Read(L"SubEmoticonIndex", SubEmoticonIndex);

					Hash = StringUtil::Hash(Situation.c_str());
				}

				void Pet::Genus::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::Pet::Genus");
					out.Write(L"Id", Id);
					out.Write(L"ExpFactor", ExpFactor);
					out.Write(L"TiredFactor", TiredFactor);
					out.Write(L"PetGrowUpFactor", PetGrowUpFactor);
					out.Write(L"MaxRarePetProbability", MaxRarePetProbability);
					out.Write(L"RareProbabilityAddPerLevel", RareProbabilityAddPerLevel);
					out.Write(L"PetIconAfterSummon", PetIconAfterSummon);
					out.Write(L"PetRareEffect", PetRareEffect);
					out.Write(L"DefaultNPCId", DefaultNPCId);
					out.Write(L"UI3DDisplayInfo", UI3DDisplayInfo);
					out.Write(L"Species", Species);
					out.WriteEnum(L"EquipClassType", EquipClassType);
				}

				void Pet::Genus::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::Pet::Genus");
					in.Read(L"Id", Id);
					in.Read(L"ExpFactor", ExpFactor);
					in.Read(L"TiredFactor", TiredFactor);
					in.Read(L"PetGrowUpFactor", PetGrowUpFactor);
					in.Read(L"MaxRarePetProbability", MaxRarePetProbability);
					in.Read(L"RareProbabilityAddPerLevel", RareProbabilityAddPerLevel);
					in.Read(L"PetIconAfterSummon", PetIconAfterSummon);
					in.Read(L"PetRareEffect", PetRareEffect);
					in.Read(L"DefaultNPCId", DefaultNPCId);
					in.Read(L"UI3DDisplayInfo", UI3DDisplayInfo);
					in.Read(L"Species", Species);
					in.ReadEnum(L"EquipClassType", EquipClassType);
					Hash = StringUtil::Hash(Id.c_str());
				}

				void Pet::Species::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::Pet::Species");
					out.Write(L"Id", Id);
					out.Write(L"Pets", Pets);
					out.Write(L"InventorySlotCount", InventorySlotCount);
					out.Write(L"LevelInterval", LevelInterval);
					out.Write(L"ArrayInterval", ArrayInterval);
					out.Write(L"RarePetAbilities", RarePetAbilities);
				}

				void Pet::Species::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::Pet::Species");
					in.Read(L"Id", Id);
					in.Read(L"Pets", Pets);
					in.Read(L"InventorySlotCount", InventorySlotCount);
					in.Read(L"LevelInterval", LevelInterval);
					in.Read(L"ArrayInterval", ArrayInterval);
					in.Read(L"RarePetAbilities", RarePetAbilities);

					Hash = StringUtil::Hash(Id.c_str());
				}

				void Pet::EffectStates::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::Pet::EffectStates");
					out.Write(L"Id", Id);
					out.Write(L"DisplayName", DisplayName);
					out.Write(L"States", States);
					out.Write(L"StateBundles", StateBundles);
					out.Write(L"Description", Description);
				}

				void Pet::EffectStates::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::Pet::EffectStates");
					in.Read(L"Id", Id);
					in.Read(L"DisplayName", DisplayName);
					in.Read(L"States", States, std::vector<ActionInfo::State>());
					in.Read(L"StateBundles", StateBundles, std::vector<std::wstring>());
					in.Read(L"Description", Description, std::wstring());

					Hash = StringUtil::Hash(Id.c_str());
				}

				int Pet::EffectStates::GetStateBundleIndex(int level, int additionalIndex, int levelInterval, int arrayInterval) const
				{
					if (level < 0 || additionalIndex < 0 || levelInterval <= 0 || arrayInterval <= 0)
					{
						return -1;
					}

					int index = level / levelInterval;

					index *= arrayInterval;
					index += additionalIndex;

					if (StateBundles.size() <= index)
						index = (int)StateBundles.size() - 1;

					return index;
				}

				const wchar_t* Pet::EffectStates::GetStateBundle(int level, int additionalIndex, int levelInterval, int arrayInterval) const
				{
					int index = GetStateBundleIndex(level, additionalIndex, levelInterval, arrayInterval);

					if (index < 0)
						return NULL;

					return StateBundles.at(index).c_str();
				}

				ActionInfo::State Pet::EffectStates::GetState(int level, int levelInterval) const
				{
					if (level < 0 || levelInterval <= 0)
						Logger::GetInstance().Exception(L"ActionInfo::State Pet::EffectStates::GetState() Invalid Parameters");

					int index = level / levelInterval;

					if (States.size() <= index)
						index = (int)States.size() - 1;

					return States[index];
				}

				void Pet::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::Pet");
					out.Write(L"Id", Id);
					out.Write(L"PetIcon", PetIcon);
					out.Write(L"DefaultMeshs", DefaultMeshs);
					out.Write(L"NormalPetAbilities", NormalPetAbilities);
				}

				void Pet::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::Pet");
					in.Read(L"Id", Id);
					in.Read(L"PetIcon", PetIcon);
					in.Read(L"DefaultMeshs", DefaultMeshs);
					in.Read(L"NormalPetAbilities", NormalPetAbilities);

					Hash = StringUtil::Hash(Id.c_str());
				}
			}
		}
	}
}