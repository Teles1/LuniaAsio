#pragma once
#include "NPC.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class NPCInfoManager : public Serializer::ISerializable {
				public:
					struct BasicResist : public Serializer::ISerializable {
						struct Resist : public Serializer::ISerializable {
							float resist[11];		//[regist type]
						public:
							inline float& operator[] (int a) {
								if (a < 0 || a >= (sizeof(resist) / sizeof(float)))
									throw std::invalid_argument("invalid argument range");
								return resist[a];
							};
						public:
							void Serialize(Serializer::IStreamWriter& out) const;

							void Deserialize(Serializer::IStreamReader& in);
						};

						Resist resist[11];//[Race]

						void LoadBasicResist();

					public:
						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);
					};
				protected:
					typedef std::map<uint32, NonPlayerInfo, Less<uint32> > NPCInfoMap;

					NPCInfoMap Npcs;
					BasicResist basicResist;

				protected:
					void LoadNPCInfo(const wchar_t* xml);

				public:
					NPCInfoManager();
					virtual ~NPCInfoManager() {}

					void Init(bool bForceXml);
					void Add(const NonPlayerInfo& info);

					typedef NPCInfoMap::iterator NPCInfoIter;
					inline NPCInfoIter Begin() { return Npcs.begin(); }
					inline NPCInfoIter End() { return Npcs.end(); }
					inline size_t Size() { return Npcs.size(); }

					void LoadXmlData();
					virtual void LoadBinaryData() = 0;

					NonPlayerInfo* Retrieve(const wchar_t* id);
					NonPlayerInfo* Retrieve(uint32 hash);
					virtual BasicResist::Resist* Retrieve(Database::Info::NonPlayerInfo::Races race);
					void GetNPCList(std::vector<std::wstring>& l) const;


				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}