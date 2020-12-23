#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				typedef float4 Rectangle;
				typedef std::vector<float2> Path;
				typedef std::vector<Path> PathGroup;

				struct StageInfo : public Serializer::ISerializable
				{
					std::wstring Id;
					std::wstring Title;
					std::wstring Note;
					std::wstring MoveMapName;
					std::wstring MapPath;
					std::wstring Description;
					std::wstring MissionDescription;
					std::wstring ChannelName;  

					uint16 Episode; // episode number
					bool isSkillAvailable; 
					bool isReviveAvailable;
					uint32 ReviveLimitCount;  // 3.1 by ycr4zy 14-11-2019

					std::vector<Rectangle> Locations;
					std::vector<float3> LocationPositions;

					struct Placement : public Serializer::ISerializable {
						float3 Position;
						float3 Direction;

					public: /* ISerializable implementation */
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};

					std::vector< std::vector<Placement> > CampfirePoint;
					std::vector< std::vector<Placement> > PlayerStartingPoints; 
					std::map< uint16/*episode number of last played stage*/, Placement > SquareStartingPoints; // in square has different starting position system for quest help
					std::vector< std::vector<float3> > RespawnPoints; 
					std::vector<PathGroup> PathGroups; // NPC AI PATH

					std::map<std::wstring, int32> values; 

					struct Timer : public Serializer::ISerializable
					{
						bool Countdown;
					public: /* ISerializable implementation */
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
					std::vector<Timer> Timers;

					std::map<std::wstring, StageEvent> Events; 
					std::map<std::wstring, ActivePoint> ActivePoints; 

					struct Npc// : public Serializer::ISerializable
					{
						enum IdleStateType { STAND = 0, AREADEFENCE, PATHFINDING, SOLDIEROFFORTUNE, PET, DOLL };
						enum TransferType { HATELIST = 0, SIGHT };
					};

					// Server specific	////////////////////////
					std::wstring ScriptName; 
				private:
					std::vector<char> ScriptSource;
					bool bScriptLoaded;

				public:
					void LoadScript();
					const std::vector<char>& GetScriptSource() {
						if (!bScriptLoaded) LoadScript();
						return ScriptSource;
					}


					// Local specific	////////////////////////
					float3 CameraPosition;
					std::wstring StartingCinematic;
					std::wstring EndingCinematic;
					std::wstring BGM;
					std::vector<std::wstring> PreloadModels;
					std::vector<std::wstring> PreloadEffects;

					static std::wstring GetStageId(uint8 difficulty, uint8 episode, uint8 stage);

					//Move map information	////////////////////
					struct MoveMap {
						std::wstring MoveMapName;
						int	width;	
						int	height;

						struct TileInfo {
							uint8
								Walk1 : 1,
								Height1 : 3,

								Walk2 : 1,
								Height2 : 3;
						} *tiles;
						bool LogicWalk(int x, int y) const;
						uint8 GetHeight(int x, int y) const;

						bool bLoaded;		
						MoveMap() : tiles(NULL), bLoaded(false), width(0), height(0) {}
						~MoveMap() { if (tiles) delete[]tiles; }
						void LoadMoveMap();
					};
				public:
					int				ProprietyLevel;
					uint8  GoalKillCount;
					float  LimitPlayTime;
					bool	bUpdateScript; 
					uint32 MinimumPlayerCount;
					bool isEventStage;	
					bool isOpened;
					uint16 RebirthCountEquipLimit;
					int  lowGuildRank;
					int	 highGuildRank;
					uint8	RaidStageDifficulty;
					bool	QuitPlayerAfterLockRoom;
				public:
					MoveMap* MoveMapInfo;

					const MoveMap& GetMoveMapInfo() const {
						if (!MoveMapInfo->bLoaded)
							MoveMapInfo->LoadMoveMap();
						return *MoveMapInfo;
					}
					int GetMoveMapHeigh(float posX, float posY) {
						int x = (int)(posX / XRated::Constants::MapTileSizeWidth);
						int y = (int)(posY / XRated::Constants::MapTileSizeHeight);
						if (MoveMapInfo == NULL || x <= 0 || x >= MoveMapInfo->width || y <= 0 || y >= MoveMapInfo->height)
							return -1;
						return MoveMapInfo->GetHeight(x, y);
					}

				public:
					StageInfo() : bScriptLoaded(false) {}
					~StageInfo() {}

				public: /* ISerializable implementation */
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}