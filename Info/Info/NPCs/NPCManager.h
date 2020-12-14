#pragma once
#include "IndexedNPCInfo.h"

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
							float& operator[] (int a) {
								if (a < 0 || a >= (sizeof(resist) / sizeof(float)))
									throw std::invalid_argument("invalid argument range");
								return resist[a];
							};
						public:
							virtual void Serialize(Serializer::IStreamWriter& out) const
							{
								out.Begin(L"AllM::XRated::Database::Info::NPCInfoManager::BasicResist::Resist");
								out.Write(L"FIRE", resist[0]);
								out.Write(L"WATER", resist[1]);
								out.Write(L"ICE", resist[2]);
								out.Write(L"LIGHTNING", resist[3]);
								out.Write(L"LAND", resist[4]);
								out.Write(L"WIND", resist[5]);
								out.Write(L"POISON", resist[6]);
								out.Write(L"LIGHT", resist[7]);
								out.Write(L"CURSE", resist[8]);
								out.Write(L"PHYSICAL", resist[9]);
								out.Write(L"INDEPENDENCE", resist[10]);
							}

							virtual void Deserialize(Serializer::IStreamReader& in)
							{
								in.Begin(L"AllM::XRated::Database::Info::NPCInfoManager::BasicResist::Resist");
								in.Read(L"FIRE", resist[0]);
								in.Read(L"WATER", resist[1]);
								in.Read(L"ICE", resist[2]);
								in.Read(L"LIGHTNING", resist[3]);
								in.Read(L"LAND", resist[4]);
								in.Read(L"WIND", resist[5]);
								in.Read(L"POISON", resist[6]);
								in.Read(L"LIGHT", resist[7]);
								in.Read(L"CURSE", resist[8]);
								in.Read(L"PHYSICAL", resist[9]);
								in.Read(L"INDEPENDENCE", resist[10]);
							}
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
					NPCInfoIter Begin() { return Npcs.begin(); }
					NPCInfoIter End() { return Npcs.end(); }
					size_t Size() { return Npcs.size(); }

					void LoadXmlData();
					virtual void LoadBinaryData();

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