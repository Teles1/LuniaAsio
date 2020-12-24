#pragma once
#include "StageInfo.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				std::wstring StageInfo::GetStageId(uint8 difficulty, uint8 episode, uint8 stage)
				{
					wchar_t buffer[256];
					swprintf_s(buffer, L"%02d%02d%02d", difficulty, episode, stage);
					return std::wstring(buffer);
				}

				bool StageInfo::MoveMap::LogicWalk(int x, int y) const
				{
					if (x % 2 == 0)
					{
						return tiles[y * width / 2 + x / 2].Walk1;
					}
					return tiles[y * width / 2 + x / 2].Walk2;
				}

				uint8 StageInfo::MoveMap::GetHeight(int x, int y) const
				{
					if (x % 2 == 0)
					{
						return tiles[y * width / 2 + x / 2].Height1;
					}
					return tiles[y * width / 2 + x / 2].Height2;
				}

				void StageInfo::MoveMap::LoadMoveMap()
				{
					//MoveMap loading	////////////////
					com_ptr<Resource::IStream> stream = Resource::ResourceSystemInstance().OpenStream(MoveMapName.c_str());
					stream->Read((unsigned char*)(&width), sizeof(int));
					stream->Read((unsigned char*)(&height), sizeof(int));
					
					bool newMoveMap = (MoveMapName.find(L".NewMove") != -1);

					height *= 2;

					int tileInfoSize = sizeof(StageInfo::MoveMap::TileInfo);
					tiles = (StageInfo::MoveMap::TileInfo*) new char[width * height * tileInfoSize];

					if (newMoveMap)
					{
						unsigned char tile;
						for (int a = height - 1; a >= 0; --a) {
							int colStart = a * width * tileInfoSize;
							for (int b = 0; b < width; ++b) {
								stream->Read((unsigned char*)&tile, tileInfoSize);
								memcpy((unsigned char*)tiles + colStart + b * tileInfoSize, &tile, tileInfoSize);
							}
						}
					}
					else
					{
						int tile[2];
						for (int a = height - 1; a >= 0; --a) {
							int colStart = a * width * tileInfoSize;
							for (int b = 0; b < width; ++b) {
								stream->Read((unsigned char*)&tile, sizeof(int) * 2);
								tiles[colStart + b * tileInfoSize].Walk1 = tile[0] & 1;
								tiles[colStart + b * tileInfoSize].Height1 = tile[0] >> 1;

								stream->Read((unsigned char*)&tile, sizeof(int) * 2);
								tiles[colStart + b * tileInfoSize].Walk2 = tile[0] & 1;
								tiles[colStart + b * tileInfoSize].Height2 = tile[0] >> 1;

							}
						}
					}
					width *= 2;
					bLoaded = true;
				}

				void StageInfo::LoadScript()
				{
					com_ptr<Resource::IStream> stream = Resource::ResourceSystemInstance().OpenStream(ScriptName);
					int len = stream->GetSize();
					ScriptSource.resize(len);
					stream->Read((unsigned char*)(&ScriptSource[0]), len);

					bScriptLoaded = true;
				}

				void StageInfo::Timer::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::StageInfo::Timer");
					out.Write(L"Countdown", (int)Countdown);
				}

				void StageInfo::Timer::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::StageInfo::Timer");
					int cntDown;
					in.Read(L"Countdown", cntDown);
					Countdown = cntDown ? true : false;
				}

				void StageInfo::Placement::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::StageInfo::Placement");
					out.Write(L"Position", Position);
					out.Write(L"Direction", Direction);
				}

				void StageInfo::Placement::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::StageInfo::Placement");
					in.Read(L"Position", Position, float3(0, 0, 0));
					in.Read(L"Direction", Direction, float3(0, 0, -1));
				}

				void StageInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::StageInfo");
					out.Write(L"Id", Id);
					out.Write(L"Title", Title);
					out.Write(L"Episode", Episode);
					out.Write(L"MoveMapName", MoveMapName);
					out.Write(L"ScriptName", ScriptName);
					out.Write(L"MapPath", MapPath);
					out.Write(L"Description", Description);
					out.Write(L"MissionDescription", MissionDescription);
					out.Write(L"ChannelName", ChannelName);
					out.Write(L"Locations", Locations);
					out.Write(L"CampfirePoint", CampfirePoint);
					out.Write(L"PlayerStartingPoints", PlayerStartingPoints);
					out.Write(L"SquareStartingPoints", SquareStartingPoints);
					out.Write(L"RespawnPoints", RespawnPoints);
					out.Write(L"PathGroups", PathGroups);
					out.Write(L"values", values);
					out.Write(L"Timers", Timers);
					out.Write(L"Events", Events);
					out.Write(L"ActivePoints", ActivePoints);
					out.Write(L"CameraPosition", CameraPosition);
					out.Write(L"StartingCinematic", StartingCinematic);
					out.Write(L"EndingCinematic", EndingCinematic);
					out.Write(L"BGM", BGM);
					out.Write(L"PreloadModels", PreloadModels);
					out.Write(L"PreloadEffects", PreloadEffects);
					out.Write(L"ProprietyLevel", ProprietyLevel);
					out.Write(L"GoalKillCount", GoalKillCount);
					out.Write(L"LimitPlayTime", LimitPlayTime);
					out.Write(L"bUpdateScript", bUpdateScript);
					out.Write(L"IsEventStage", isEventStage);
					out.Write(L"IsOpened", isOpened);
					out.Write(L"RebirthCountEquipLimit", RebirthCountEquipLimit);
					out.Write(L"isSkillAvailable", isSkillAvailable);
					out.Write(L"isReviveAvailable", isReviveAvailable);
					out.Write(L"ReviveLimitCount", ReviveLimitCount); // 3.1 by ycr4zy 14-11-2019
					out.Write(L"lowGuildRank", lowGuildRank);
					out.Write(L"highGuildRank", highGuildRank);
					out.Write(L"RaidStageDifficulty", RaidStageDifficulty);
					out.Write(L"QuitPlayerAfterLockRoom", QuitPlayerAfterLockRoom);
					out.Write(L"tokenUsed", tokenUsed);
					out.Write(L"tokenBackSizeByRebirthCount", tokenBackSizeByRebirthCount);
					out.Write(L"MinimumPlayerCount", MinimumPlayerCount);
				}

				void StageInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::StageInfo");
					in.Read(L"Id", Id);
					in.Read(L"Title", Title);
					in.Read(L"Episode", Episode, static_cast<uint16>(0));
					in.Read(L"MoveMapName", MoveMapName);
					in.Read(L"ScriptName", ScriptName);
					in.Read(L"MapPath", MapPath);
					in.Read(L"Description", Description);
					in.Read(L"MissionDescription", MissionDescription);
					in.Read(L"ChannelName", ChannelName, std::wstring());
					in.Read(L"Locations", Locations);
					//Calculate locations center position
					std::vector<float4>::iterator iterLoc = Locations.begin(), iterLocEnd = Locations.end();
					for (; iterLoc != iterLocEnd; ++iterLoc) {
						float4 rec = (*iterLoc);
						LocationPositions.push_back(float3(rec.x + (rec.z - rec.x) / 2, 0, rec.y - (rec.y - rec.w) / 2));
					}
					in.Read(L"CampfirePoint", CampfirePoint, std::vector< std::vector<StageInfo::Placement> >());
					in.Read(L"PlayerStartingPoints", PlayerStartingPoints, std::vector< std::vector<StageInfo::Placement> >());
					in.Read(L"SquareStartingPoints", SquareStartingPoints, std::map< uint16, Placement >());
					in.Read(L"RespawnPoints", RespawnPoints, std::vector< std::vector<float3> >());
					in.Read(L"PathGroups", PathGroups);
					in.Read(L"values", values);
					in.Read(L"Timers", Timers);
					in.Read(L"Events", Events, std::map<std::wstring, StageEvent>());
					in.Read(L"ActivePoints", ActivePoints, std::map<std::wstring, ActivePoint>());
					in.Read(L"CameraPosition", CameraPosition);
					in.Read(L"StartingCinematic", StartingCinematic);
					in.Read(L"EndingCinematic", EndingCinematic);
					in.Read(L"BGM", BGM, std::wstring());
					in.Read(L"PreloadModels", PreloadModels, std::vector<std::wstring>());
					in.Read(L"PreloadEffects", PreloadEffects, std::vector<std::wstring>());
					in.Read(L"ProprietyLevel", ProprietyLevel, 0);
					in.Read(L"GoalKillCount", GoalKillCount, static_cast<uint8>(0));
					in.Read(L"LimitPlayTime", LimitPlayTime, static_cast<float>(-1.0f));
					in.Read(L"bUpdateScript", bUpdateScript, true);
					in.Read(L"IsEventStage", isEventStage, false);
					in.Read(L"IsOpened", isOpened, true);
					in.Read(L"RebirthCountEquipLimit", RebirthCountEquipLimit, static_cast<uint16>(0));
					in.Read(L"isSkillAvailable", isSkillAvailable, true);
					in.Read(L"isReviveAvailable", isReviveAvailable, true);
					in.Read(L"ReviveLimitCount", ReviveLimitCount, static_cast<uint32>(0)); // 3.1 by ycr4zy 14-11-2019
					in.Read(L"lowGuildRank", lowGuildRank, 0);
					in.Read(L"highGuildRank", highGuildRank, 0);
					in.Read(L"RaidStageDifficulty", RaidStageDifficulty, static_cast<uint8>(0));
					in.Read(L"QuitPlayerAfterLockRoom", QuitPlayerAfterLockRoom, false);
					in.Read(L"tokenUsed", tokenUsed, uint32(0));
					in.Read(L"tokenBackSizeByRebirthCount", tokenBackSizeByRebirthCount, uint32(0));
					in.Read(L"MinimumPlayerCount", MinimumPlayerCount, uint32(1)); // 3.1 by ycr4zy 14-11-2019
				}
			}
		}
	}
}