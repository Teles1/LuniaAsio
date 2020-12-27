#pragma once
#include "StatusInfo.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class StatusInfoManager : public Serializer::ISerializable
				{
				public:
					struct StatusGroup
					{
						std::vector<StatusInfo*> statusList;
						Constants::CharacterMajorStatType majorStat;
						//int StackedStr[3], StackedDex[6], StackedVit[3], StackedInt[3]; 
					};

					typedef std::map<uint32, BasicStatInfo, Less<uint32> > BasicStatMap;
					typedef std::map<uint32, StatusGroup, Less<uint32> > StatusTable;

					enum ExpType { Pvp, Story, PvpStory, Pet, War };

				protected:
					BasicStatMap basicStatMap;
					StatusTable statusTable;
					std::vector< std::vector<uint32> >	expTable;
					std::vector< std::vector<double> > elementTable;

					std::vector<uint32> rebirthAddtionalFactors;
					std::vector<uint32> rebirthAddtionalMajorFactors;

					std::map<uint8/*RebirthCount*/, std::vector<float>/*StatRate*/> dollStatApplyRate;

					void LoadStatusTable(const std::wstring& name, BasicStatInfo& info);

				public:
					StatusInfoManager();
					~StatusInfoManager();

					void Init(bool bForceXml);
					void LoadXmlData();
					virtual void LoadBinaryData();

					virtual StatusGroup* Retrieve(uint32 hash);
					virtual StatusGroup* Retrieve(const std::wstring& type);
					virtual StatusInfo* Retrieve(int level, const std::wstring& type);
					virtual StatusInfo* Retrieve(int level, uint32 hash);
					bool IsExist(uint32 hash);
					bool IsExist(const std::wstring& type);
					void GetBasicStatList(std::vector<std::wstring>& l);
					uint32 GetExp(ExpType type, int lv) { return expTable[type][lv]; }
					uint32 GetRebirthAdditionalFactor(uint16 currentLevel, uint16 rebirthCount, bool needStoreFactor = false);
					uint32 GetRebirthAdditionalMajorFactor(uint16 currentLevel, uint16 rebirthCount, bool needStoreFactor = false);
					uint32 GetRebirthSkillPoint(uint16 oldLevel);
					Constants::CharacterMajorStatType GetMajorStatType(const std::wstring& type);

					double GetDamagePercent(Constants::DamageType attackerType, Constants::DamageType targetType);

					void GetDollStatApplyRate(uint8 rebirthCount, float& Str, float& Dex, float& Vit, float& Int);

				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};

			}
		}
	}
}
