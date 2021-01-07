#pragma once
#include "QuestsInfo.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class QuestInfoManager : public Serializer::ISerializable {
				protected:
					typedef std::map<uint32, QuestInfo, Less<uint32> > QUESTMap;
					QUESTMap quests;
					std::vector<uint32> periodQuests;
					std::vector<uint32> acItems;		//AcceptCondition ItemList;

				private:
					void LoadQuestInfo(const wchar_t* xml);

				public:
					void Init(bool bForceXml);
					void LoadXmlData();
					void LoadBinaryData();

					void Remove(uint32 id);
					void Add(const QUESTMap& addQuests);
					void Add(const std::map<uint32, QuestInfo>& addQuests);
					void AddPreiodQuests(const std::vector<uint32>& addPeriodQuests);

					bool IsExistQuest(uint32 questHash);
					bool IsExistACItemList(uint32 itemHash);

					const QuestInfo* Retrieve(uint32 id) const;
					const std::vector< std::pair<uint32/*QuestHash*/, const QuestInfo*> >& Retrieve(const StageLocation& stage) const; /// @return quest list of given stage,
				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);

				public:
					const std::vector<uint32> GetQuestList();
					const std::vector< uint32 > GetEpicQuestList(const uint32 playerLevel) const;
					const std::vector<uint32>& GetPeriodQuestList() { return periodQuests; }

				private:
					/* post process */
					typedef std::map< StageLocation, std::vector< std::pair<uint32, const QuestInfo*> > > StageReferenceContainer;
					StageReferenceContainer stageReference; /// to manage completed quests effectively in database, see detail of QuestManager in StageServer
					void PostLoadProcess();

					typedef uint32 PlayerLevel;

					typedef uint32 QuestId;

					typedef std::vector< QuestId > QuestIds;

					typedef std::map< PlayerLevel, QuestIds > EpicQuests;

					EpicQuests epicQuests;
				};

				class EventQuestInfoManager : public Serializer::ISerializable
				{
				protected:
					typedef std::map<uint32, QuestInfo, Less<uint32> > QUESTMap;
					QUESTMap quests;
					std::vector<uint32> periodQuests;

					void PostLoadProcess();

					std::map<uint32, ClientQuestInfo> clientQuests;
					std::map< uint32, std::vector<uint32> > questObjectList; 

				public:
					void Init();
					void LoadXmlData();

					const std::vector<uint32>& GetPeriodQuestList() { return periodQuests; }
					const std::map<uint32, QuestInfo, Less<uint32> >& GetQuests() { return quests; }
					const std::map<uint32, ClientQuestInfo>& GetClientQuestInfos() { return clientQuests; }
					const std::map<uint32, std::vector<uint32> >& GetQuestObjectList() { return questObjectList; }

				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);

				public:

				};


			}
		}
	}
}