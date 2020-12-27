#include "GameStateDatabase.h"
#include <Core/Utils/Math/euler.h>
#include "Map/Map.h"
#include "LogicDB/LogicDB.h"
#include <Core/Utils/StringUtil.h>
#include "State/States.h"
#include <Info/Info/Enchant/Enchant.h>
#include <Core/Utils/ConfigReader.h>
#include <iostream>
#include <Info/Info/InfoHelper.h>
using namespace std;
using namespace Lunia::Serializer;
using namespace Lunia::XRated::Database;

namespace Lunia { namespace XRated {	namespace Logic {


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//GameStateDatabase Implementation
	GameStateDatabase::GameStateDatabase(ILogic& l)
		: logic(l), playerSerials(PCSerialBase, NPCSerialBase), npcSeed(NPCSerialBase), projectileSeed(ProjectileSerialBase), bUpdateNPC(false), bUpdateMISC(false)
		, SmallSectorSize_X(40), SmallSectorSize_Z(40), MaxMapSize_X(4700), MaxMapSize_Z(4700), stageCnt(0), airDamage(1)
	{
		npcBuffer.reserve(50);
		objectBuffer.reserve(50);
		////sector����
		//const Database::Info::StageInfo::MoveMap& info = DatabaseInstance().InfoCollections.Stages.Retrieve(name.c_str())->GetMoveMapInfo();
		//int width = (int)(info.width * Map::StageMap::LOGIC_TILESIZE_X);
		//int height = (int)(info.height * Map::StageMap::LOGIC_TILESIZE_Y);
		bigSector.Init( MaxMapSize_X, MaxMapSize_Z, 240, 240 );
		smallSector.Init( MaxMapSize_X, MaxMapSize_Z, 40, 40 );

		bSquare = Config::GetInstance().GetKind() == ServerKind::SquareKind ? true : false;

		tickCnt = 0;
		totalReviveCount = 0;

#ifdef _SINGLE
		showNPCInfo = 0;
#endif
	}
 
	GameStateDatabase::~GameStateDatabase()
	{
		playerSerials.Clear();
		for (ObjectMap::iterator pos = objects.begin(); pos != objects.end() ; ++pos)
			delete (*pos).second;
		objects.clear();
		for (PlayerMap::iterator pos = players.begin(); pos != players.end() ; ++pos)
			delete (*pos).second;
		players.clear();
		for (NPCMap::iterator pos = npcs.begin(); pos != npcs.end() ; ++pos)
			delete (*pos).second;
		npcs.clear();
	}

	void GameStateDatabase::Clear()
	{
		playerSerials.Clear();
		npcSeed = NPCSerialBase;
		projectileSeed = ProjectileSerialBase;

		for (ObjectMap::iterator pos = objects.begin(); pos != objects.end() ; ++pos) {
			if ( (*pos).second->GetType() == Constants::ObjectType::Misc )
				LogicDBInstance().ReleaseMisc( (Misc*)(*pos).second );
			else
				delete (*pos).second;
		}
		objects.clear();

		for (PlayerMap::iterator pos = players.begin(); pos != players.end() ; ++pos) {
			if ( (*pos).second->IsPlayerPvpItem() )
				LogicDBInstance().ReleasePlayerPvpItem( (*pos).second );
			else
				LogicDBInstance().ReleasePlayer( (*pos).second );
		}
		players.clear();

		for (NPCMap::iterator pos = npcs.begin(); pos != npcs.end() ; ++pos)
			LogicDBInstance().ReleaseNPC( (*pos).second );
		npcs.clear();

		{
			AutoLock lock(csNPCBuffer);
			std::vector<NonPlayer*>::iterator ibn = npcBuffer.begin();
			while( ibn != npcBuffer.end() ) {
				LogicDBInstance().ReleaseNPC( *ibn );
				++ibn;
			}
			npcBuffer.clear();
		}

		{
			AutoLock lock(csMISCBuffer);
			std::vector<Actor*>::iterator ion = objectBuffer.begin();
			while( ion != objectBuffer.end() ) {
				if ( (*ion)->GetType() == Constants::ObjectType::Misc )
					LogicDBInstance().ReleaseMisc( (Misc*)(*ion) );
				else
					delete (*ion);
				++ion;
			}
			objectBuffer.clear();
		}

		itemManager.Init();
		projectileManager.Init();
		triggerSystem.Init();
		stage.Init();

		bigSector.Clear();
		smallSector.Clear();

		bUpdateMISC = false;
		bUpdateNPC = false;
	}

	void GameStateDatabase::LoadStage(Info::StageGroup* sGroup, uint32 name, uint16 uniqueId)
	{

		++stageCnt;// count how many stages loaded in this logic. for matching object loading thread.
		tickCnt = 0;
		totalReviveCount = 0;
		airDamage = sGroup->AirDamage; //�����޺� ������ �������� stagegroup �� ����Ǿ� ���� �ʱ⿡ ��������.
		airDamageByNpc = sGroup->AirDamageByNpc;
		stage.SetDatabase(this);
		Clear();
		stage.LoadStage(sGroup, name, uniqueId);
	}

	void GameStateDatabase::GameStart()
	{
		//pvp�� ��� ����ġ�� �̸� ����� ����.
		switch ( stage.GetGameType() ) {
		case Constants::GameTypes::DeathMatchGameType :
		case Constants::GameTypes::LadderGameType :
			{
				float tmpDeter[2];
				tmpDeter[0] = tmpDeter[1] = 0; // ����ġ ���� �Ǵ��� ����� ���ؼ�
				pvp.deathMatch.sumLv[0] = pvp.deathMatch.sumLv[1] = 0;

				pvp.deathMatch.memberCnt = (int)players.size()/2; //���ι��ΰ�
				for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter) {
					Player* p = (*iter).second;
					if (p->GetTeam()-1>=2) throw Exception(L"invalid team");

					pvp.deathMatch.sumLv[ p->GetTeam()-1 ] += p->GetLevel();
					tmpDeter[ p->GetTeam()-1 ] += ( p->GetLevel() * p->GetLevel() );
				}
				pvp.deathMatch.avgLv[0] = (float)pvp.deathMatch.sumLv[0]/(float)pvp.deathMatch.memberCnt;
				pvp.deathMatch.avgLv[1] = (float)pvp.deathMatch.sumLv[1]/(float)pvp.deathMatch.memberCnt;

				tmpDeter[0] = tmpDeter[0] / (float)pvp.deathMatch.memberCnt;
				tmpDeter[0] = sqrt(tmpDeter[0]);
				tmpDeter[1] = tmpDeter[1] / (float)pvp.deathMatch.memberCnt;
				tmpDeter[1] = sqrt(tmpDeter[1]);

				//pvp.deathMatch.xpPvp = (float)ExpFunctions::Pvp::GetXp(pvp.deathMatch.memberCnt) / pvp.deathMatch.memberCnt;
				pvp.deathMatch.deter[0] = (tmpDeter[1] - tmpDeter[0]); //* (1.0f + 0.3f*(float)(pvp.deathMatch.memberCnt-1)) * 0.1f + 1.0f;
				pvp.deathMatch.deter[1] = (tmpDeter[0] - tmpDeter[1]); //* (1.0f + 0.3f*(float)(pvp.deathMatch.memberCnt-1)) * 0.1f + 1.0f;

			}
			break;
		case Constants::GameTypes::BattleGroundType:
			{
				pvp.battleGround.killCount[0] = 0;
				pvp.battleGround.killCount[1] = 0;
				pvp.battleGround.limitPlayTime = -1;
			}
			break;
		case Constants::GameTypes::SurvivalGameType :
			break;
		}
		stage.PvpGameStart();
	}

	int GameStateDatabase::GetMaxTeamPlayerCnt() const
	{
		std::map<uint8,int> teams;

		PlayerMap::const_iterator iter = players.begin();
		PlayerMap::const_iterator end = players.end();
		for(; iter!=end; ++iter)
		{
			if(teams.find(iter->second->GetTeam()) == teams.end())
			{
				teams[iter->second->GetTeam()] = 1;
			}
			else
			{
				++teams[iter->second->GetTeam()];
			}
		}

		if(teams.size() == 0) 
		{
			//Lunia_IMPORTANT((L"Players is empty."));
			//Player�� ����???
			//�� ��� �ּ��� LocalPlayer�� �����Ƿ� 1�̶� �����ش�.
			return 1;
		}

		int max = teams.end()->second;

		for(auto& x : teams)
		{
			max = Lunia::Math::Maximum(max,x.second);
		}

		return max;
	}

	bool GameStateDatabase::IsPlayerTeam( uint8 team ) const
	{
		PlayerMap::const_iterator it = players.begin();
		for( ; it != players.end() ; ++it )
		{
			if( it->second->GetTeam() == team )
			{
				return true;
			}
		}
		return false;
	}

	int GameStateDatabase::GetActivePlayerCnt() const
	{
		int activeCnt = 0;
		for(PlayerMap::const_iterator iter = players.begin() ; iter != players.end() ; ++iter) {
			if ( !(*iter).second->CheckState(Database::Info::StateInfo::Type::GHOST) || (*iter).second->GetPlayerData().Life > 0 )
				++activeCnt;
		}
		return activeCnt;
	}

	uint16 GameStateDatabase::GetPlayerCnt(uint8 team) const// ���� �ش� ���� �Ҽӵ� ���� �� �Դϴ�.
	{
		uint16 cnt = 0;
		for(PlayerMap::const_iterator iter = players.begin() ; iter != players.end() ; ++iter) {
			if ( iter->second->GetTeam() == team ) ++cnt;
		}
		return cnt;
	}
	int GameStateDatabase::GetPlayerSerial( uint8 team, uint16 index ) const
	{
		int i=0;
		for(auto& iter : players) {
			if ( iter.second->GetTeam() == team ) 
			{
				if ( i == static_cast<int>(index) ) return static_cast<int>( iter.first );
				++i;
			}
		}
		return -1;
	}

	bool GameStateDatabase::IsPlayable(Player& p)
	{
		return ( p.GetPlayerData().Life > 1 		
			|| p.CheckState(Database::Info::StateInfo::Type::GHOST)  == false );		
	}

	uint16 GameStateDatabase::GetPlayablePlayerCnt()  const
	{
		uint16 cnt=0;
		for(auto& iter : players) {
			if ( IsPlayable( *(iter.second) ) ) 
				++cnt;
		}
		return cnt;
	}
	uint16 GameStateDatabase::GetPlayablePlayerCnt(uint8 team) const
	{
		uint16 cnt=0;
		for(auto& iter : players) {
			if ( iter.second->GetTeam()==team && IsPlayable( *(iter.second)) ) 
				++cnt;
		}
		return cnt;
	}
	bool GameStateDatabase::IsPlayable( uint32 playerSerial )  const
	{
		PlayerMap::const_iterator player=players.find(playerSerial);
		if (player != players.end()) 
			return IsPlayable( *(player->second) );
		else 
			return false;
	}





	Object* GameStateDatabase::Remove(Serial serial)
	{
		Object* ret;

		ObjectMap::iterator pos=objects.find(serial);
		if (pos!=objects.end()) {
			ret=pos->second;

			AutoLock locker(csUpdateObject);
			objects.erase(pos);
			return ret;
		}

		NPCMap::iterator npc=npcs.find(serial);
		if (npc!=npcs.end()) {
			ret=(Object*)(npc->second);

			AutoLock locker(csUpdateObject);
			npcs.erase(npc);
			return ret;
		}

		Logger::GetInstance().Info( L"[GameStateDatabase::Remove] no matching object [{0}]", serial );

		return 0;
	}

	NonPlayer* GameStateDatabase::RemoveNpc(Serial serial)
	{
		NonPlayer* ret;
		NPCMap::iterator npc=npcs.find(serial);
		if (npc!=npcs.end()) {
			ret=npc->second;
		
			AutoLock locker(csUpdateObject);
			npcs.erase(npc);
			return ret;
		}
		Logger::GetInstance().Info( L"[GameStateDatabase::RemoveNpc] no matching npc [{0}]", serial );
		return 0;
	}

	Player* GameStateDatabase::RemovePlayer(Serial serial)
	{
		Player* ret;
		PlayerMap::iterator player=players.find(serial);
		if (player!=players.end()) {
			ret=player->second;

			AutoLock locker(csUpdateObject);
			players.erase(player);

			// Destroy �Ǿ����� �����帮�ڽ��ϴ�~
			playerSerials.PushExtraSerial( serial );

			return ret;
		}
		Logger::GetInstance().Info( L"[GameStateDatabase::RemovePlayer] no matching player [{0}]", serial );
		return 0;
	}

	Object* GameStateDatabase::Retrieve(Serial serial)
	{
		NPCMap::iterator npc=npcs.find(serial);
		if (npc != npcs.end())
			return npc->second;
		PlayerMap::iterator player=players.find(serial);
		if (player != players.end()) 
			return player->second;
		ObjectMap::iterator pos=objects.find(serial);
		if (pos != objects.end()) 
			return pos->second;
		Projectile* projectile = projectileManager.Retrieve(serial);
		if (projectile)
			return projectile;
		return itemManager.Retrieve(serial);
	}

	Object* GameStateDatabase::RetrieveByID(uint32 name)
	{
		for (auto& pos : players)
			if ( (pos.second)->GetNameHash() == name)
				return pos.second;
		for (auto& pos : npcs)
			if ( (pos.second)->GetNameHash() == name)
				return pos.second;
		for (auto& pos : objects)
			if ( (pos.second)->GetNameHash() == name)
				return pos.second;
		return NULL;
	}

	Actor* GameStateDatabase::RetrieveActor(Serial serial)
	{
		ObjectMap::iterator pos=objects.find(serial);
		if (pos != objects.end()) 
			return pos->second;
		NPCMap::iterator npc=npcs.find(serial);
		if (npc != npcs.end()) 
			return npc->second;
		PlayerMap::iterator player=players.find(serial);
		if (player != players.end())
			return player->second;
		return NULL;
	}

	Actor* GameStateDatabase::RetrieveActorByID(uint32 name)
	{
		for (auto& pos : objects)
			if ( (pos.second)->GetNameHash() == name)
				return pos.second;
		for (auto& pos : npcs)
			if ( (pos.second)->GetNameHash() == name)
				return pos.second;
		for (auto& pos : players)
			if ( (pos.second)->GetNameHash() == name)
				return pos.second;
		return NULL;
	}

	NonPlayer* GameStateDatabase::RetrieveNPC(Serial serial)
	{
		NPCMap::iterator npc=npcs.find(serial);
		if (npc != npcs.end()) 
			return npc->second;
		return NULL;
	}

	NonPlayer* GameStateDatabase::RetrieveNPCByID(uint32 name)
	{
		for (auto& pos : npcs)
			if ( (pos.second)->GetNameHash() == name)
				return pos.second;
		return NULL;
	}

	Player* GameStateDatabase::RetrievePlayer(Serial serial)
	{
		PlayerMap::iterator player=players.find(serial);
		if (player != players.end()) 
			return player->second;
		return NULL;
	}

	Player* GameStateDatabase::NextPlayer(Serial serial)
	{
		PlayerMap::iterator player = players.find(serial);
		if (player == players.end()) {//�������ڰ� ����.
			if (players.begin() == players.end()) return 0;//�÷��̾ �ƹ��� ����.
			else return players.begin()->second;//player list�� ó������ �ٽ�
		}
		++player;
		if (player == players.end())//���� ����ڰ� ����.
			return players.begin()->second; //ù ����ڸ� ���� ������� �ٽ� �ɼ��� �ְ�. �ٸ����� �ɼ��� �ְ�
		else
			return player->second;	//���� ����ڸ� ��������
	}
	Player* GameStateDatabase::GetRandomPlayer()
	{
		size_t playerCount = players.size();
		const int selectedPlayerIndex = GetRandomInt(0, static_cast<int>(playerCount) );

		int count = 0;
		for( auto& iter : players) {
			if( count == selectedPlayerIndex ) {
				return iter.second;				
			}
			++count;
		}

		if( players.begin() != players.end() ) {
			Logger::GetInstance().Error(L"GetRandomPlayer Failed : return first player");
			return players.begin()->second;
		}
		return NULL;
	}

	Player* GameStateDatabase::RetrievePlayerByID(const std::wstring& id)
	{
		for (auto& pos : players)
			if ( (pos.second)->GetName() == id)
				return pos.second;
		return NULL;
	}

	bool GameStateDatabase::AddToObjectBuffer(uint32 stageCnt, Actor* object)
	{
		if ( stageCnt != this->stageCnt )
			return false;

		uint32 localNameHash = object->GetNameHash(); // to leave stack value to debug a dump

		//sector control
		Sector *sSector = smallSector.GetSector(object->GetPosition()), *bSector = bigSector.GetSector(object->GetPosition());
		object->SetSmallSector(sSector);
		object->SetBigSector(bSector);

		{
			AutoLock lock(csMISCBuffer);
			objectBuffer.push_back(object);
		}
		return true;
	}

	bool GameStateDatabase::AddToNPCBuffer(uint32 stageCnt, NonPlayer* npc)
	{
		if ( stageCnt != this->stageCnt )
			return false;

		{
			AutoLock lock(csNPCBuffer);
			npcBuffer.push_back(npc);
		}
		return true;
	}

	Serial GameStateDatabase::CreateObject(Actor* object)
	{
		Serial serial=object->GetSerial();
		//object->SetSerial(serial);

		{
			AutoLock locker(csUpdateObject);
			objects.insert(ObjectMap::value_type(serial, object));
		}

		//sector control
		Sector *sSector = smallSector.ObjectCreated(object), *bSector = bigSector.ObjectCreated(object);
		object->SetSmallSector(sSector);
		object->SetBigSector(bSector);

		ObjectCreated(object->GetObjectData());
		return serial;
	}

	Serial GameStateDatabase::CreateNonPlayer(NonPlayer* npc)
	{
		Serial serial = npc->GetSerial();

		{
			AutoLock locker(csUpdateObject);
			npcs.insert(NPCMap::value_type(serial, npc));
		}

		//sector control
		Sector *sSector = smallSector.NonPlayerCreated(npc), *bSector = bigSector.NonPlayerCreated(npc);
		npc->SetSmallSector(sSector);
		npc->SetBigSector(bSector);

		NonPlayerCreated( npc->GetNonPlayerData() );
		return serial;
	}

	Serial GameStateDatabase::CreatePlayer(Player* player)
	{
		Serial serial=playerSerials.GetSerial();

		if( serial == 0 )
		{
			throw Lunia::Exception(L"CreatePlayer has no serial");
		}
        
		player->SetSerial(serial);

		{
			AutoLock locker(csUpdateObject);
			players.insert(PlayerMap::value_type(serial, player));
		}


		//sector control
		Sector *sSector = smallSector.PlayerCreated(player), *bSector = bigSector.PlayerCreated(player);
		player->SetSmallSector(sSector);
		player->SetBigSector(bSector);

		if ( !PlayerCreated(player) ) {
			smallSector.PlayerDestroyed(player, sSector);
			bigSector.PlayerDestroyed(player, bSector);
			RemovePlayer(serial);
			return 0;
		}
		return serial;
	}

	Serial GameStateDatabase::CreatePlayer(Player* player, Serial serial)
	{
		PlayerMap::iterator pos=players.find(serial);
		if (pos!=players.end())
			throw(L"that serial already exsist");

		player->SetSerial(serial);

		{
			AutoLock locker(csUpdateObject);
			players.insert(PlayerMap::value_type(serial, player));
		}

		//sector control
		Sector *sSector = smallSector.PlayerCreated(player), *bSector = bigSector.PlayerCreated(player);
		player->SetSmallSector(sSector);
		player->SetBigSector(bSector);

		PlayerCreated(player);
		return serial;
	}

	void GameStateDatabase::CreateProjectile(uint32 name, float2 pos, uint16 fireAngle, uint16 type, Object* obj, float min, float max, uint16 toMyTarget, float createTime,Lunia::XRated::Serial creator)
	{
		projectileManager.CreateProjectile(name, projectileSeed++, pos, fireAngle, type, obj, min, max, toMyTarget, *this, createTime,creator);

		if( projectileSeed == 0 )
		{
			projectileSeed = ProjectileSerialBase;
		}

		//return projectileSeed;
	}

	Serial GameStateDatabase::CreateItem(uint32 name, const float3& pos, const float3& direction, uint16 stackCount, InstanceEx instance, bool isPrivateItem)
	{
		itemManager.CreateItem(*this, name, pos, direction, projectileSeed++, stackCount, instance, isPrivateItem);

		if( projectileSeed == 0 )
		{
			projectileSeed = ProjectileSerialBase;
		}

		return projectileSeed;
	}

	Serial GameStateDatabase::CreateItem(uint32 name, const float3& pos, const float3& direction, uint32 userSerial, uint16 stackCount, InstanceEx instance, bool isPrivateItem)
	{
		itemManager.CreateItem(*this, name, pos, direction, projectileSeed++, userSerial, stackCount, instance, isPrivateItem);

		if( projectileSeed == 0 )
		{
			projectileSeed = ProjectileSerialBase;
		}

		return projectileSeed;
	}

	void GameStateDatabase::ClearItem()
	{
		itemManager.ClearItem(this);
	}

	AI::TriggerID GameStateDatabase::AddTrigger(uint16 priority, AI::Trigger::TriggerType triggerType, Serial creator,
					Constants::ObjectType objType, float3 pos, float rad, float timeLimit, bool bUpdate, bool bImmediately)
	{
		return triggerSystem.AddTrigger(priority, triggerType, creator, objType, pos, rad, timeLimit, bUpdate, bImmediately);
	}

	int GameStateDatabase::GetNPCCnt()
	{
		return (int)npcs.size();
	}

	void GameStateDatabase::GetObjectList(std::vector<ObjectData*>& l)
	{
		for (auto& iter : objects)
			l.push_back(&(iter).second->GetObjectData());
	}

	void GameStateDatabase::GetItemList(std::vector<ItemData*>& l)
	{
		itemManager.GetObjectList(l);
	}

	void GameStateDatabase::GetNPCList(std::vector<NonPlayerData*>& l)
	{
		for (auto& iter : npcs)
			l.push_back(&(iter).second->GetNonPlayerData());
	}

	void GameStateDatabase::GetPlayerList(std::vector<PlayerData*>& l)
	{
		for (auto& iter : players)
			l.push_back(&(iter).second->GetPlayerData());
	}

	void GameStateDatabase::GetTamingList(std::vector<Lunia::XRated::Logic::ILogic::TamingInfo>& l)
	{
		for (auto& iter : players) {
			std::list<Player::Familiar>& familiars = iter.second->GetTamingList();
			std::list<Player::Familiar>::iterator iFamiliar = familiars.begin();
			for ( ; iFamiliar != familiars.end() ; ++iFamiliar ) {
				l.push_back(ILogic::TamingInfo(iter.second->GetSerial(), (*iFamiliar).object->GetSerial(), (*iFamiliar).type));
			}
		}
	}

	bool GameStateDatabase::IsAbleToBuy(Player* player, Constants::ShopType shopType)
	{
		return stage.IsAbletoBuy(player->GetPosition(), shopType);
	}

	bool GameStateDatabase::IsAbleToTrade(Player* player, Serial target)
	{
		Player* targetPlayer = RetrievePlayer( target );
		if (!targetPlayer)
			return false;
		if (Lunia::Math::Length<float>(player->GetPosition() - targetPlayer->GetPosition()) <= 100.0f)
			return true;
		return false;
	}

	void GameStateDatabase::CheckAbleToPlay()
	{
		switch ( stage.GetGameType() ) {
		case Constants::GameTypes::SideGameType :
		case Constants::GameTypes::StoryGameType :
			for(auto& iter : players) {
				if ( !iter.second->CheckState(Database::Info::StateInfo::Type::GHOST) || iter.second->GetPlayerData().Life > 0 )
					return;
			}
			stage.MissionClearFromLogic(false, 1);
			break;
		case Constants::GameTypes::DeathMatchGameType :
		case Constants::GameTypes::LadderGameType :
			// ������ ���������� ���Ҵ����� �˻��Ѵ�.
			{
				bool bFinished = true;
				uint8 team = 0;
				for(auto& iter : players) {
					Player* p = iter.second;
					if( p->GetPlayerData().Life > 0 || p->GetHP() > 0 ) {
						if ( !team ) 
							team = p->GetTeam();
						else if ( team == p->GetTeam() )
							; 
						else 
							bFinished = false;
					}
				}
				if ( bFinished )
					stage.MissionClearFromLogic(true, team);
			}
			break;
		case Constants::GameTypes::BattleGroundType :
			{
				std::set<uint8/*team*/> teams;
				for(auto& iter : players) 
				{
					teams.insert(iter.second->GetTeam());
				}

				if ( teams.size() <= 1 )
					stage.MissionClearFromLogic(true, *(teams.begin()) );
					
			}
			break;
		}
	}

	bool GameStateDatabase::GetRespawnPos(uint8 team, float3& pos)
	{
		uint8 n = team-1;
		Info::StageInfo* info = stage.GetStageInfo();
		switch ( stage.GetGameType() ) {
		case Constants::GameTypes::SideGameType :
		case Constants::GameTypes::StoryGameType :
			return false;
		case Constants::GameTypes::BattleGroundType :
		case Constants::GameTypes::DeathMatchGameType :
		case Constants::GameTypes::LadderGameType :
			{
				if ( info->RespawnPoints.size() < team )
					return false;
				if ( info->RespawnPoints[n].empty() )
					return false;
				int cnt = GetRandomInt(0, (int)info->RespawnPoints[n].size());
				pos = info->RespawnPoints[n][cnt];
			}

			break;
		case Constants::GameTypes::SurvivalGameType :
			{
				if ( info->RespawnPoints.empty() )
					return false;
				int cnt = GetRandomInt(0, (int)info->RespawnPoints.size());
				if ( info->RespawnPoints[cnt].empty() )
					return false;
				pos = info->RespawnPoints[cnt][GetRandomInt(0, (int)info->RespawnPoints[cnt].size())];
			}
			break;
		}
		return true;
	}

	bool GameStateDatabase::CheckAlliance(Actor* obj, Constants::AllianceType by, uint8 value)
	{
		switch(by) {
		case Constants::AllianceType::Alliance :
			if ( obj->GetAlliance() != value )
				return false;
			break;
		case Constants::AllianceType::ExAlliance:
			if ( obj->GetAlliance() == value )
				return false;
			break;
		case Constants::AllianceType::Team :
			if ( obj->GetTeam() != value )
				return false;
			break;
		case Constants::AllianceType::ExTeam :
			if ( obj->GetTeam() == value )
				return false;
			break;
		}
		return true;
	}

	Sector* GameStateDatabase::ChooseSector(float range, Object::Sectors& sectors)
	{
		return (range<=SmallSectorSize_X ? sectors.smallSector : sectors.bigSector);
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Object retrive
	template<> void GameStateDatabase::GetObjectbyName(uint32 name, std::vector<Object*>& l)
	{
		for (auto& pos : objects)
			if ( pos.second->GetNameHash() == name )
				l.push_back(pos.second);

		for (auto& npc : npcs)
			if (npc.second->GetNameHash() == name )
				l.push_back(npc.second);
	}

	template<> void GameStateDatabase::GetObjectbyName(uint32 name, std::vector<NonPlayer*>& l)
	{
		for (NPCMap::iterator npc = npcs.begin(); npc != npcs.end() ; ++npc)
			if ( (*npc).second->GetNameHash() == name )
				l.push_back((*npc).second);
	}

	void GameStateDatabase::GetAreaPlayerByTeam(uint8 team, float3 targetPos, float range, std::vector<Player*>& l)
	{
		for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter)
			if ( PhysicalFunctions::CheckCollision(targetPos, range, iter->second->GetObjectData()) && (*iter).second->GetTeam() == team )
				l.push_back(iter->second);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Retrieve object by location. using from Stage scripts
	void GameStateDatabase::GetAreaObjectRect(const float4& rect, std::vector<Object*>& l)
	{
		for(ObjectMap::iterator iter = objects.begin() ; iter != objects.end() ; ++iter) {
			float3 pos = iter->second->GetPosition();
			if (pos.x >= rect.x && pos.x <= rect.z && pos.z >= rect.w && pos.z <= rect.y)
				l.push_back(iter->second);
		}
		for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter) {
			float3 pos = iter->second->GetPosition();
			if (pos.x >= rect.x && pos.x <= rect.z && pos.z >= rect.w && pos.z <= rect.y)
				l.push_back(iter->second);
		}
		for(NPCMap::iterator iter = npcs.begin() ; iter != npcs.end() ; ++iter) {
			float3 pos = iter->second->GetPosition();
			if (pos.x >= rect.x && pos.x <= rect.z && pos.z >= rect.w && pos.z <= rect.y)
				l.push_back(iter->second);
		}
	}

	void GameStateDatabase::GetAreaObjectRect(const float4& rect, std::vector<Actor*>& l)
	{
		for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter) {
			float3 pos = iter->second->GetPosition();
			if (pos.x >= rect.x && pos.x <= rect.z && pos.z >= rect.w && pos.z <= rect.y)
				l.push_back(iter->second);
		}
		for(NPCMap::iterator iter = npcs.begin() ; iter != npcs.end() ; ++iter) {
			float3 pos = iter->second->GetPosition();
			if (pos.x >= rect.x && pos.x <= rect.z && pos.z >= rect.w && pos.z <= rect.y)
				l.push_back(iter->second);
		}
	}

	void GameStateDatabase::GetAreaObjectRect(const float4& rect, std::vector<NonPlayer*>& l)
	{
		for(NPCMap::iterator iter = npcs.begin() ; iter != npcs.end() ; ++iter) {
			float3 pos = iter->second->GetPosition();
			if (pos.x >= rect.x && pos.x <= rect.z && pos.z >= rect.w && pos.z <= rect.y)
				l.push_back(iter->second);
		}
	}

	void GameStateDatabase::GetAreaObjectRect(const float4& rect, std::vector<Player*>& l)
	{
		for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter) {
			float3 pos = iter->second->GetPosition();
			if (pos.x >= rect.x && pos.x <= rect.z && pos.z >= rect.w && pos.z <= rect.y)
				l.push_back(iter->second);
		}
	}

	template<> void GameStateDatabase::GetAreaObjectRect(const float4& rect, uint32 id, std::vector<Object*>& l)
	{
		ObjectMap::iterator iter, end = objects.end();
		for(iter = objects.begin() ; iter != end ; ++iter) {
			float3 pos = iter->second->GetPosition();
			if (pos.x >= rect.x && pos.x <= rect.z && pos.z >= rect.w && pos.z <= rect.y)
				if (iter->second->GetNameHash() == id)
					l.push_back(iter->second);
		}
	}

	template<> void GameStateDatabase::GetAreaObjectRect(const float4& rect, const uint8 team, std::vector<Object*>& l)
	{
		ObjectMap::iterator iter, end = objects.end();
		for(iter = objects.begin() ; iter != end ; ++iter) {
			float3 pos = iter->second->GetPosition();
			if (pos.x >= rect.x && pos.x <= rect.z && pos.z >= rect.w && pos.z <= rect.y)
				if (iter->second->GetTeam() == team)
					l.push_back(iter->second);
		}
	}

	template<> void GameStateDatabase::GetAreaObjectRect(const float4& rect, uint32 id, std::vector<NonPlayer*>& l)
	{
		NPCMap::iterator iter, end = npcs.end();
		for(iter = npcs.begin() ; iter != end ; ++iter) {
			float3 pos = iter->second->GetPosition();
			if (pos.x >= rect.x && pos.x <= rect.z && pos.z >= rect.w && pos.z <= rect.y)
				if (iter->second->GetNameHash() == id)
					l.push_back(iter->second);
		}
	}

	template<> void GameStateDatabase::GetAreaObjectRect(const float4& rect, const uint8 team, std::vector<NonPlayer*>& l)
	{
		NPCMap::iterator iter, end = npcs.end();
		for(iter = npcs.begin() ; iter != end ; ++iter) {
			float3 pos = iter->second->GetPosition();
			if (pos.x >= rect.x && pos.x <= rect.z && pos.z >= rect.w && pos.z <= rect.y)
				if (iter->second->GetTeam() == team)
					l.push_back(iter->second);
		}
	}

	template<> void GameStateDatabase::GetAreaObjectRect(const float4& rect, uint32 id, std::vector<Player*>& l)
	{
		PlayerMap::iterator iter, end = players.end();
		for(iter = players.begin() ; iter != end ; ++iter) {
			float3 pos = iter->second->GetPosition();
			if (pos.x >= rect.x && pos.x <= rect.z && pos.z >= rect.w && pos.z <= rect.y)
				if (iter->second->GetNameHash() == id)
					l.push_back(iter->second);
		}
	}

	template<> void GameStateDatabase::GetAreaObjectRect(const float4& rect, const uint8 team, std::vector<Player*>& l)
	{
		PlayerMap::iterator iter, end = players.end();
		for(iter = players.begin() ; iter != end ; ++iter) {
			float3 pos = iter->second->GetPosition();
			if (pos.x >= rect.x && pos.x <= rect.z && pos.z >= rect.w && pos.z <= rect.y)
				if (iter->second->GetTeam() == team)
					l.push_back(iter->second);
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void GameStateDatabase::GetAreaObjectForSector9(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* /*object*/, std::vector<Actor*>& l)
	{
		std::vector<Sector*> sectors;
		if( range<=SmallSectorSize_X ) {
			smallSector.GetSectorForCircle(targetPos,range,sectors);
		}else {
			bigSector.GetSectorForCircle(targetPos,range,sectors);
		}

		size_t max = sectors.size();
		for( size_t i =0; i < max; ++i )
		{
			Sector* sector = sectors[i];
			Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
			for (; npcIter.bEnd ; ++npcIter)
				if (CheckAlliance(*npcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, range, (*npcIter)->GetObjectData() ))
					l.push_back(*npcIter);
			Sector::ObjectIterator<Player> pcIter = sector->begin<Player>();
			for (; pcIter.bEnd ; ++pcIter)
				if (CheckAlliance(*pcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, range, (*pcIter)->GetObjectData() ))
					l.push_back(*pcIter);
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Retrieve object list functions.
	// ���� ////////////////////////////////////////////////
	void GameStateDatabase::GetAreaObject(const float3& targetPos, float range, Object* object, std::vector<Object*>& l)
	{
		Sector* sector = ChooseSector(range, object->GetSector());
		Sector::ObjectIterator<Actor> objIter = sector->begin<Actor>();
		for (; objIter.bEnd ; ++objIter)
			if (PhysicalFunctions::CheckCollision(targetPos, range, (*objIter)->GetObjectData() ))
				l.push_back(*objIter);
		Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
		for (; npcIter.bEnd ; ++npcIter)
			if (PhysicalFunctions::CheckCollision(targetPos, range, (*npcIter)->GetObjectData() ))
				l.push_back(*npcIter);
		Sector::ObjectIterator<Player> pcIter = sector->begin<Player>();
		for (; pcIter.bEnd ; ++pcIter)
			if (PhysicalFunctions::CheckCollision(targetPos, range, (*pcIter)->GetObjectData() ))
				l.push_back(*pcIter);
	}

	void GameStateDatabase::GetAreaObject(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Object*>& l)
	{
		Sector* sector = ChooseSector(range, object->GetSector());
		Sector::ObjectIterator<Actor> objIter = sector->begin<Actor>();
		for (; objIter.bEnd ; ++objIter)
			if (CheckAlliance(*objIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, range, (*objIter)->GetObjectData() ))
				l.push_back(*objIter);
		Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
		for (; npcIter.bEnd ; ++npcIter)
			if (CheckAlliance(*npcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, range, (*npcIter)->GetObjectData() ))
				l.push_back(*npcIter);
		Sector::ObjectIterator<Player> pcIter = sector->begin<Player>();
		for (; pcIter.bEnd ; ++pcIter)
			if (CheckAlliance(*pcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, range, (*pcIter)->GetObjectData() ))
				l.push_back(*pcIter);
	}

	void GameStateDatabase::GetAreaObject(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Projectile*>& l)
	{
		projectileManager.GetAreaObject(targetPos, range, by, value, object, l);
	}

	void GameStateDatabase::GetAreaObject(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Actor*>& l)
	{
		if ( targetPos.x <= 0 || targetPos.z <= 0 )
			return;

		Sector* sector = ChooseSector(range, object->GetSector());
		Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
		for (; npcIter.bEnd ; ++npcIter)
			if (CheckAlliance(*npcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, range, (*npcIter)->GetObjectData() ))
				l.push_back(*npcIter);
		Sector::ObjectIterator<Player> pcIter = sector->begin<Player>();
		for (; pcIter.bEnd ; ++pcIter)
			if (CheckAlliance(*pcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, range, (*pcIter)->GetObjectData() ))
				l.push_back(*pcIter);
	}

	void GameStateDatabase::GetAreaObject(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<NonPlayer*>& l)
	{
		if ( targetPos.x <= 0 || targetPos.z <= 0 )
			return;

		Sector* sector = ChooseSector(range, object->GetSector());
		Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
		for (; npcIter.bEnd ; ++npcIter)
			if (CheckAlliance(*npcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, range, (*npcIter)->GetObjectData() ))
				l.push_back(*npcIter);
	}

	void GameStateDatabase::GetAreaObject(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Player*>& l)
	{
		Sector* sector = ChooseSector(range, object->GetSector());
		Sector::ObjectIterator<Player> pcIter = sector->begin<Player>();
		for (; pcIter.bEnd ; ++pcIter)
			if (CheckAlliance(*pcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, range, (*pcIter)->GetObjectData() ))
				l.push_back(*pcIter);
	}
	// ������ ////////////////////////////////////////////////
	void GameStateDatabase::GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, Object* object, std::vector<Object*>& l)
	{
		Sector* sector = ChooseSector(range, object->GetSector());
		Sector::ObjectIterator<Actor> objIter = sector->begin<Actor>();
		for (; objIter.bEnd ; ++objIter)
			if (PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, (*objIter)->GetObjectData() ))
				l.push_back(*objIter);
		Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
		for (; npcIter.bEnd ; ++npcIter)
			if (PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, (*npcIter)->GetObjectData() ))
				l.push_back(*npcIter);
		Sector::ObjectIterator<Player> pcIter = sector->begin<Player>();
		for (; pcIter.bEnd ; ++pcIter)
			if (PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, (*pcIter)->GetObjectData() ))
				l.push_back(*pcIter);
	}

	void GameStateDatabase::GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Object*>& l)
	{
		Sector* sector = ChooseSector(range, object->GetSector());
		Sector::ObjectIterator<Actor> objIter = sector->begin<Actor>();
		for (; objIter.bEnd ; ++objIter)
			if (CheckAlliance(*objIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, (*objIter)->GetObjectData() ))
				l.push_back(*objIter);
		Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
		for (; npcIter.bEnd ; ++npcIter)
			if (CheckAlliance(*npcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, (*npcIter)->GetObjectData() ))
				l.push_back(*npcIter);
		Sector::ObjectIterator<Player> pcIter = sector->begin<Player>();
		for (; pcIter.bEnd ; ++pcIter)
			if (CheckAlliance(*pcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, (*pcIter)->GetObjectData() ))
				l.push_back(*pcIter);
	}

	void GameStateDatabase::GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Actor*>& l)
	{
		Sector* sector = ChooseSector(range, object->GetSector());
		Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
		for (; npcIter.bEnd ; ++npcIter)
			if (CheckAlliance(*npcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, (*npcIter)->GetObjectData() ))
				l.push_back(*npcIter);
		Sector::ObjectIterator<Player> pcIter = sector->begin<Player>();
		for (; pcIter.bEnd ; ++pcIter)
			if (CheckAlliance(*pcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, (*pcIter)->GetObjectData() ))
				l.push_back(*pcIter);
	}

	void GameStateDatabase::GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<NonPlayer*>& l)
	{
		Sector* sector = ChooseSector(range, object->GetSector());
		Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
		for (; npcIter.bEnd ; ++npcIter)
			if (CheckAlliance(*npcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, (*npcIter)->GetObjectData() ))
				l.push_back(*npcIter);
	}

	void GameStateDatabase::GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Player*>& l)
	{
		Sector* sector = ChooseSector(range, object->GetSector());
		Sector::ObjectIterator<Player> pcIter = sector->begin<Player>();
		for (; pcIter.bEnd ; ++pcIter)
			if (CheckAlliance(*pcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, (*pcIter)->GetObjectData() ))
				l.push_back(*pcIter);
	}
	// �簢�� ////////////////////////////////////////////////
	void GameStateDatabase::GetAreaObject(const float3& targetPos, const float3& targetDirection, float length, float width, Object* object, std::vector<Object*>& l)
	{
		Sector* sector = ChooseSector(max(length, width), object->GetSector());
		Sector::ObjectIterator<Actor> objIter = sector->begin<Actor>();
		for (; objIter.bEnd ; ++objIter)
			if (PhysicalFunctions::CheckCollision(targetPos, targetDirection, length, width, (*objIter)->GetObjectData()) )
				l.push_back(*objIter);
		Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
		for (; npcIter.bEnd ; ++npcIter)
			if (PhysicalFunctions::CheckCollision(targetPos, targetDirection, length, width, (*npcIter)->GetObjectData()) )
				l.push_back(*npcIter);
		Sector::ObjectIterator<Player> pcIter = sector->begin<Player>();
		for (; pcIter.bEnd ; ++pcIter)
			if (PhysicalFunctions::CheckCollision(targetPos, targetDirection, length, width, (*pcIter)->GetObjectData()) )
				l.push_back(*pcIter);
	}

	void GameStateDatabase::GetAreaObject(const float3& targetPos, const float3& targetDirection, float length, float width, Constants::AllianceType by, uint8 value, Object* object, std::vector<Object*>& l)
	{
		Sector* sector = ChooseSector(max(length, width), object->GetSector());
		Sector::ObjectIterator<Actor> objIter = sector->begin<Actor>();
		for (; objIter.bEnd ; ++objIter)
			if (CheckAlliance(*objIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, length, width, (*objIter)->GetObjectData()) )
				l.push_back(*objIter);
		Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
		for (; npcIter.bEnd ; ++npcIter)
			if (CheckAlliance(*npcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, length, width, (*npcIter)->GetObjectData()))
				l.push_back(*npcIter);
		Sector::ObjectIterator<Player> pcIter = sector->begin<Player>();
		for (; pcIter.bEnd ; ++pcIter)
			if (CheckAlliance(*pcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, length, width, (*pcIter)->GetObjectData()))
				l.push_back(*pcIter);
	}

	void GameStateDatabase::GetAreaObject(const float3& targetPos, const float3& targetDirection, float length, float width, Constants::AllianceType by, uint8 value, Object* object, std::vector<Actor*>& l)
	{
		Sector* sector = ChooseSector(max(length, width), object->GetSector());
		Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
		for (; npcIter.bEnd ; ++npcIter)
			if (CheckAlliance(*npcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, length, width, (*npcIter)->GetObjectData()))
				l.push_back(*npcIter);
		Sector::ObjectIterator<Player> pcIter = sector->begin<Player>();
		for (; pcIter.bEnd ; ++pcIter)
			if (CheckAlliance(*pcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, length, width, (*pcIter)->GetObjectData()))
				l.push_back(*pcIter);
	}

	void GameStateDatabase::GetAreaObject(const float3& targetPos, const float3& targetDirection, float length, float width, Constants::AllianceType by, uint8 value, Object* object, std::vector<NonPlayer*>& l)
	{
		Sector* sector = ChooseSector(max(length, width), object->GetSector());
		Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
		for (; npcIter.bEnd ; ++npcIter)
			if (CheckAlliance(*npcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, length, width, (*npcIter)->GetObjectData()))
				l.push_back(*npcIter);
	}

	void GameStateDatabase::GetAreaObject(const float3& targetPos, const float3& targetDirection, float length, float width, Constants::AllianceType by, uint8 value, Object* object, std::vector<Player*>& l)
	{
		Sector* sector = ChooseSector(max(length, width), object->GetSector());
		Sector::ObjectIterator<Player> pcIter = sector->begin<Player>();
		for (; pcIter.bEnd ; ++pcIter)
			if (CheckAlliance(*pcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, length, width, (*pcIter)->GetObjectData()))
				l.push_back(*pcIter);
	}
	// ��ä�� ////////////////////////////////////////////////
	void GameStateDatabase::GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, float startAngle, float endAngle, Object* object, std::vector<Object*>& l)
	{
		Sector* sector = ChooseSector(range, object->GetSector());
		Sector::ObjectIterator<Actor> objIter = sector->begin<Actor>();
		for (; objIter.bEnd ; ++objIter)
			if (PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, startAngle, endAngle, (*objIter)->GetObjectData()))
				l.push_back(*objIter);
		Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
		for (; npcIter.bEnd ; ++npcIter)
			if (PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, startAngle, endAngle, (*npcIter)->GetObjectData()))
				l.push_back(*npcIter);
		Sector::ObjectIterator<Player> pcIter = sector->begin<Player>();
		for (; pcIter.bEnd ; ++pcIter)
			if (PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, startAngle, endAngle, (*pcIter)->GetObjectData()))
				l.push_back(*pcIter);
	}

	void GameStateDatabase::GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, float startAngle, float endAngle, Constants::AllianceType by, uint8 value, Object* object, std::vector<Object*>& l)
	{
		Sector* sector = ChooseSector(range, object->GetSector());
		Sector::ObjectIterator<Actor> objIter = sector->begin<Actor>();
		for (; objIter.bEnd ; ++objIter)
			if (CheckAlliance(*objIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, startAngle, endAngle, (*objIter)->GetObjectData()))
				l.push_back(*objIter);
		Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
		for (; npcIter.bEnd ; ++npcIter)
			if (CheckAlliance(*npcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, startAngle, endAngle, (*npcIter)->GetObjectData()))
				l.push_back(*npcIter);
		Sector::ObjectIterator<Player> pcIter = sector->begin<Player>();
		for (; pcIter.bEnd ; ++pcIter)
			if (CheckAlliance(*pcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, startAngle, endAngle, (*pcIter)->GetObjectData()))
				l.push_back(*pcIter);
	}

	void GameStateDatabase::GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, float startAngle, float endAngle, Constants::AllianceType by, uint8 value, Object* object, std::vector<Actor*>& l)
	{
		Sector* sector = ChooseSector(range, object->GetSector());
		Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
		for (; npcIter.bEnd ; ++npcIter)
			if (CheckAlliance(*npcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, startAngle, endAngle, (*npcIter)->GetObjectData()))
				l.push_back(*npcIter);
		Sector::ObjectIterator<Player> pcIter = sector->begin<Player>();
		for (; pcIter.bEnd ; ++pcIter)
			if (CheckAlliance(*pcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, startAngle, endAngle, (*pcIter)->GetObjectData()))
				l.push_back(*pcIter);
	}

	void GameStateDatabase::GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, float startAngle, float endAngle, Constants::AllianceType by, uint8 value, Object* object, std::vector<NonPlayer*>& l)
	{
		Sector* sector = ChooseSector(range, object->GetSector());
		Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
		for (; npcIter.bEnd ; ++npcIter)
			if (CheckAlliance(*npcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, startAngle, endAngle, (*npcIter)->GetObjectData()))
				l.push_back(*npcIter);
	}

	void GameStateDatabase::GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, float startAngle, float endAngle, Constants::AllianceType by, uint8 value, Object* object, std::vector<Player*>& l)
	{
		Sector* sector = ChooseSector(range, object->GetSector());
		Sector::ObjectIterator<Player> pcIter = sector->begin<Player>();
		for (; pcIter.bEnd ; ++pcIter)
			if (CheckAlliance(*pcIter, by, value) && PhysicalFunctions::CheckCollision(targetPos, targetDirection, range, startAngle, endAngle, (*pcIter)->GetObjectData()))
				l.push_back(*pcIter);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	void GameStateDatabase::NPCArrived(NonPlayer* npc, int pathCnt, int posCnt)
	{
		stage.NPCArrived(npc->GetNPCInfo()->HashcodeName, npc->GetSerial(), pathCnt, posCnt);
	}

	void GameStateDatabase::ObjectMoved(Object* obj)
	{
		Lunia::XRated::ObjectData &data = obj->GetObjectData();
		switch ( data.Type) {
		case Constants::ObjectType::Item :
		case Constants::ObjectType::TombStone :
			break;
		case Constants::ObjectType::Projectile :
			{
				float2 pos( data.Position.x, data.Position.z );
				Sector* sector = smallSector.GetSector( pos );
				obj->SetSmallSector(sector);
				sector = bigSector.GetSector( pos );
				obj->SetBigSector(sector);
			}
			break;
		case Constants::ObjectType::Player :
		case Constants::ObjectType::Misc :
		case Constants::ObjectType::NonPlayer :
		case Constants::ObjectType::Vehicle :
			{
				Sector* sector = smallSector.ChangeSector(obj, obj->GetSector().smallSector);
				if ( !sector )
					return;
				obj->SetSmallSector(sector);

				//ū sector��ȭ ����. network �̺�Ʈ�� ���ؼ� ���� ���͸� ����.
                Sector* oldBigSector = obj->GetSector().bigSector;
				sector = bigSector.ChangeSector(obj, oldBigSector);
				if ( sector ) {//ū sector�� ���ߴٸ�..
					//ObjectData& data = obj->GetObjectData();
					//switch(obj->GetType()) {
					//	case Constants::ObjectType::Player : //Player���� �ֺ��� �ִ� object���� animation change �� �����ش�.
					//	case Constants::ObjectType::Misc :
					//	case Constants::ObjectType::NonPlayer :
					//	case Constants::ObjectType::Vehicle :
					//		{
					//		}
					//		break;
					//	case Constants::ObjectType::Projectile :
					//		break;
					//	//case Constants::ObjectType::TombStone :
					//	//case Constants::ObjectType::Item :
					//}
					obj->SetBigSector(sector);

					int index = sector->GetIndex() - oldBigSector->GetIndex();
					logic.GetEventListener().SectorChanged(data.GameObjectSerial, sector, bigSector.GetDirection(index));
					//Lunia_INFO(( L"[GameStateDatabase::ObjectMoved] direction : {}", (int)bigSector.GetDirection(index) ));
				}
			}
			break;
		}
	}

	float3 GameStateDatabase::GetAvailablePos(const float3& position, const float& rad)
	{
		float diameter = rad*2 + 1.0f;
		float3 startPos = position;
		float3 pos = startPos;
		float3 direction[4] = {float3(0, 0, 1), float3(1, 0, 0), float3(0, 0, -1), float3(-1, 0, 0)};
		int directionTurn = 0, turn = 0, repeat=1, currentRepeat=0;
		while( !IsAbletoLocate(0, pos, rad) ) {
            pos += direction[directionTurn]*diameter;
			++turn;
			if (repeat == ++currentRepeat) { if (!(turn%2)) ++repeat; currentRepeat=0; ++directionTurn; }
			if (directionTurn > 3) directionTurn=0;

			if (turn > 15)
				return float3(0,0,0);
		}
		return pos;
	}

	float3 GameStateDatabase::GetAvailablePos(const float3& position, const float& rad, Actor* it)
	{
		float diameter = rad*2 + 1.0f;
		float3 startPos = position;
		float3 pos = startPos;
		float3 direction[4] = {float3(0, 0, 1), float3(1, 0, 0), float3(0, 0, -1), float3(-1, 0, 0)};
		int directionTurn = 0, turn = 0, repeat=1, currentRepeat=0;
		while( !IsAbletoLocate(it->GetSerial(), pos, rad, it) ) {
            pos += direction[directionTurn]*diameter;
			++turn;
			if (repeat == ++currentRepeat) { if (!(turn%2)) ++repeat; currentRepeat=0; ++directionTurn; }
			if (directionTurn > 3) directionTurn=0;

			if (turn > 15) {
				return float3(0,0,0);
				break;
			}
		}
		return pos;
	}
	
	float3 GameStateDatabase::GetAvailableGhostPos(const float3& position, const float& rad)
	{
		float diameter = rad*2 + 1.0f;
		float3 startPos = position;
		float3 pos = startPos;
		float3 direction[4] = {float3(0, 0, 1), float3(1, 0, 0), float3(0, 0, -1), float3(-1, 0, 0)};
		int directionTurn = 0, turn = 0, repeat=1, currentRepeat=0;
		while( !IsAbletoLocateByGhost(pos, rad) ) {
            pos += direction[directionTurn]*diameter;
			++turn;
			if (repeat == ++currentRepeat) { if (!(turn%2)) ++repeat; currentRepeat=0; ++directionTurn; }
			if (directionTurn > 3) directionTurn=0;

			if (turn > 15) {
				return float3(0,0,0);
			}
		}
		return pos;
	}

	bool GameStateDatabase::IsAbletoLocateInMap(const float3& pos, float rad)
	{
		if (!stage.IsAbletoLocate(pos, rad, Map::StageMap::Height::FIELD))
			return false;
		return true;
	}

	bool GameStateDatabase::IsAbletoLocateByGhost(const float3& pos, float rad)
	{
		if (!stage.IsAbletoLocate(pos, rad, Map::StageMap::Height::FIELD))
			return false;

		for(ObjectMap::iterator iter = objects.begin() ; iter != objects.end() ; ++iter)
			if (!(iter->second->GetType() == Constants::ObjectType::TombStone) && PhysicalFunctions::CheckCollision(pos, rad, iter->second->GetObjectData()))
				return false;
        return true;
	}

	bool GameStateDatabase::IsAbletoLocate(Serial serial, const float3& pos, float rad)
	{
		if (!stage.IsAbletoLocate(pos, rad, Map::StageMap::Height::FIELD))
			return false;

		if ( bSquare )
			return true;

		Sector* sector = smallSector.GetSector(float2(pos.x, pos.z));
		Sector::ObjectIterator<Actor> objIter = sector->begin<Actor>();
		for (; objIter.bEnd ; ++objIter)
			if (PhysicalFunctions::CheckCollision(pos, rad, (*objIter)->GetObjectData()))
				if ( (*objIter)->GetType() != Constants::ObjectType::TombStone )
					return false;

		for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter)
			if ((*iter).second->GetSerial() != serial && PhysicalFunctions::CheckCollision(pos, rad, iter->second->GetObjectData()))
				if ( !(*iter).second->CheckState(Database::Info::StateInfo::Type::GHOST) )
					return false;

		Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
		for (; npcIter.bEnd ; ++npcIter)
			if ( (*npcIter)->GetSerial() != serial && PhysicalFunctions::CheckCollision(pos, rad, (*npcIter)->GetObjectData()) )
				return false;

		return true;
	}

	bool GameStateDatabase::IsAbletoLocate(Serial /*serial*/, const float3& pos, float rad, Actor* it)
	{
		if (!stage.IsAbletoLocate(pos, rad, Map::StageMap::Height::FIELD))
			return false;

		if ( bSquare )
			return true;

		Sector* sector = it->GetSector().smallSector;
		Sector::ObjectIterator<Actor> objIter = sector->begin<Actor>();

		Actor* actor;

		bool bOverlapped = false;

		if ( stage.GetGameType() & Constants::GameTypes::CooperativeGameTypeMask ) {
			switch ( it->GetType() ) {
			case Constants::ObjectType::NonPlayer :
				{

					for (; objIter.bEnd ; ++objIter) {
						actor = (*objIter);
						if (PhysicalFunctions::CheckCollision(pos, rad, actor->GetObjectData()))
							if ( ((actor)->IsOverlappable() == false ) && (!actor->CheckState(Info::StateInfo::Type::DOWN)) && (actor->GetType() != Constants::ObjectType::TombStone) )
								return false;
					}

					if( it->IsOverlapped() )
						break;

					for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter) {
						if ( PhysicalFunctions::CheckCollision(pos, rad, iter->second->GetObjectData()) )
							if ( !(*iter).second->CheckState(Database::Info::StateInfo::Type::GHOST) ) {
								if ( !(*iter).second->IsOverlapped() ) return false;
								else if( !(*iter).second->IsOverlappable() == false ) bOverlapped = true;
							}
					}
				}
				break;
			case Constants::ObjectType::Player :
				{
					for (; objIter.bEnd ; ++objIter) {
						if (PhysicalFunctions::CheckCollision(pos, rad, (*objIter)->GetObjectData()))
							if ( ((*objIter)->IsOverlappable() == false ) && ((*objIter)->GetType() != Constants::ObjectType::TombStone) )
								return false;
					}

					if( it->IsOverlapped() )
						break;

					Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
					for (; npcIter.bEnd ; ++npcIter) {
						if ( PhysicalFunctions::CheckCollision(pos, rad, (*npcIter)->GetObjectData()) ) {
							if ( !(*npcIter)->IsOverlapped() ) return false;
							else if( (*npcIter)->IsOverlappable() == false ) bOverlapped = true; // PASSABLE 
						}
					}
				}
				break;
			}
		} else {
			switch ( it->GetType() ) {
			case Constants::ObjectType::NonPlayer :
				{
					for (; objIter.bEnd ; ++objIter) {//DOWN
						if (PhysicalFunctions::CheckCollision(pos, rad, (*objIter)->GetObjectData()))
							if ( ((*objIter)->IsOverlappable() == false ) && (!(*objIter)->CheckState(Info::StateInfo::Type::DOWN)) && ((*objIter)->GetType() != Constants::ObjectType::TombStone) )
								return false;
					}
				}
				break;
			case Constants::ObjectType::Player :
				{
					for (; objIter.bEnd ; ++objIter) {
						if (PhysicalFunctions::CheckCollision(pos, rad, (*objIter)->GetObjectData()))
							if ( ((*objIter)->IsOverlappable() == false ) && ((*objIter)->GetType() != Constants::ObjectType::TombStone) )
								return false;
					}
				}
				break;
			}

			Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
			for (; npcIter.bEnd ; ++npcIter) {
				if ( (*npcIter)->GetSerial() == it->GetSerial() )
					continue;
				if ( PhysicalFunctions::CheckCollision(pos, rad, (*npcIter)->GetObjectData()) && (*npcIter)->GetTeam() != it->GetTeam() ) {
					if ( !(*npcIter)->IsOverlapped() ) return false;
					else if( (*npcIter)->IsOverlappable() == false ) bOverlapped = true; // PASSABLE
				}
			}
			for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter) {
				if ( (*iter).second->GetSerial() == it->GetSerial() )
					continue;
				if ( (*iter).second->GetTeam() != it->GetTeam() && PhysicalFunctions::CheckCollision(pos, rad, iter->second->GetObjectData()) )
					if ( !(*iter).second->CheckState(Database::Info::StateInfo::Type::GHOST) ) {
						if ( !(*iter).second->IsOverlapped() ) return false;
						else if( (*iter).second->IsOverlappable() == false ) bOverlapped = true;
					}
			}
		}

		if ( bOverlapped )
			it->SetOverlap(true);

		return true;
	}

	bool GameStateDatabase::IsOverlapped(Actor* actor)
	{
		float3 pos = actor->GetPosition();
		float rad = actor->GetRadius();

		if ( stage.GetGameType() & Constants::GameTypes::CooperativeGameTypeMask ) {
			switch ( actor->GetType() ) {
			case Constants::ObjectType::NonPlayer :
				for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter)
					if ( PhysicalFunctions::CheckCollision(pos, rad, (*iter).second->GetObjectData()) )
						if ( !(*iter).second->CheckState(Database::Info::StateInfo::Type::GHOST) )
							return true;
				break;
			case Constants::ObjectType::Player :
				{
					Sector* sector = actor->GetSector().smallSector;
					Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
					for (; npcIter.bEnd ; ++npcIter) {
						if ( PhysicalFunctions::CheckCollision(pos, rad, (*npcIter)->GetObjectData()) )
							return true;
					}
				}
				break;
			}
		} else {
			uint8 team = actor->GetTeam();
			for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter) {
				if ( team != (*iter).second->GetTeam() && PhysicalFunctions::CheckCollision(pos, rad, (*iter).second->GetObjectData()) &&
						!(*iter).second->CheckState(Database::Info::StateInfo::Type::GHOST) )
							return true;
			}
			Sector* sector = actor->GetSector().smallSector;
			Sector::ObjectIterator<NonPlayer> npcIter = sector->begin<NonPlayer>();
			for (; npcIter.bEnd ; ++npcIter) {
				if ( team != (*npcIter)->GetTeam() && PhysicalFunctions::CheckCollision(pos, rad, (*npcIter)->GetObjectData()) )
					return true;
			}
		}

		return false;
	}

	void GameStateDatabase::RemoveTombstone(Serial ownerSerial)
	{
		for(ObjectMap::iterator iter = objects.begin() ; iter != objects.end() ; ++iter) {
			if ((*iter).second->GetType() == Constants::ObjectType::TombStone) {
				if ( ((TombStone*)((*iter).second))->GetOwnerSerial() == ownerSerial ) {
					ObjectDestroyed((*iter).second, Constants::ObjectType::TombStone, 1);

					AutoLock locker(csUpdateObject);
					delete (*iter).second;
					objects.erase(iter);
					return;
				}
			}
		}
	}

	void GameStateDatabase::AddUnidentifiedItems(Database::Info::NonPlayerInfo* info, uint32 count)
	{
		int size = (int)info->UnidentifiedItems.size();
		for ( int a=0; a<size ; ++a) {
			XRated::UnidentifiedItemInfo* itemInfo = Database::DatabaseInstance().InfoCollections.Items.RetrieveUnidentifiedItem( info->UnidentifiedItems[a].c_str() );
			if (itemInfo) {
				int cnt = logic.GetConfig().NumberOfBossRewardUnidentifiedItem;

				for ( int k=0; k < cnt ; ++k ) {
					PlayerMap::iterator i = players.begin();
					for ( ; i != players.end() ; ++i) {
						Serial itemSerial = projectileSeed;
						Player* player = (*i).second;

						if(player->IsGainNormal() == false)
							continue;

						if( player->GetPlayTimePenalty() & XRated::Constants::PlayTimePenalty::Gold )
							continue;

						for( uint32 ea = 0 ; ea < count ; ++ea )
						{
							if ( !AcquireItem(player, info->UnidentifiedItemsHash.at(a), itemSerial) )
								CreateItem(info->UnidentifiedItemsHash[a], player->GetPosition(), player->GetDirection(), player->GetSerial());
							else
							{
								++projectileSeed;

								if( projectileSeed == 0 )
								{
									projectileSeed = ProjectileSerialBase;
								}
							}
						}
					}
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Update Functions	////////////////////////////////////////////////////////////////////////////////
	void GameStateDatabase::Update(float dt)
	{
		++tickCnt;

		if ( stage.GetGameType() & Constants::GameTypes::BattleGroundType 
			&& pvp.battleGround.limitPlayTime > 0
			)//&& pvp.battleGround.killCount[0]  != pvp.battleGround.killCount[1]) 
		{
			CriticalStatistics::StageStatistics ss;
			stage.GetStageStatistics( ss );
			if ( ss.ElapsedTime >= pvp.battleGround.limitPlayTime )
			{
				//stage.MissionClearFromLogic( true, pvp.battleGround.killCount[0] > pvp.battleGround.killCount[1] ? 1 : 2   );
				stage.MissionClearTimeOver();
				pvp.battleGround.limitPlayTime  = -1 ;
				return;
			}
		}

		triggerSystem.Update(dt, this);
		stage.Update();//stage trigger system update.

		projectileManager.Update(dt, *this);

		bUpdateMISC = bUpdateNPC = true;

		ObjectMap::iterator iter=objects.begin();
		for (;iter != objects.end() ;) {
			if ( (*iter).second->Update(dt, this) ) {
				//object �ı�
				ObjectDestroyed((*iter).second, (*iter).second->GetType());

				AutoLock locker(csUpdateObject);

				if ( (*iter).second->GetType() == Constants::ObjectType::Misc )
					LogicDBInstance().ReleaseMisc( (Misc*)(*iter).second );
				else
					delete (*iter).second;

				iter = objects.erase(iter);
			} else ++iter;
		}

		bUpdateMISC = false;

		//�ѹ��� �տ��� �ѹ��� �ڿ������� �÷��̾ ó�����ֱ����� ����.
		if ( tickCnt % 2 == 0 ) {
			PlayerMap::iterator i=players.begin();
			for (;i != players.end() ; ++i) {
				// TODO :  �׽�Ʈ �ڵ��. actor �� �Ѿ����� �� �浹üũ ���ϱ� ����
				if ( ( *i).second->IsOverlapped() && !(*i).second->IsOverlappable()) {
					if ( !IsOverlapped((*i).second) )
						(*i).second->SetOverlap(false);
				}
				// ���� ����.	////////////////////////////////////////////////////////////

				(*i).second->Update(dt, this);
			}
		} else {
			PlayerMap::reverse_iterator i = players.rbegin();
			for (;i != players.rend() ; ++i) {
				// TODO :  �׽�Ʈ �ڵ��. actor �� �Ѿ����� �� �浹üũ ���ϱ� ����
				if ( ( *i).second->IsOverlapped() && !(*i).second->IsOverlappable()) {
					if ( !IsOverlapped((*i).second) )
						(*i).second->SetOverlap(false);
				}
				// ���� ����.	////////////////////////////////////////////////////////////

				(*i).second->Update(dt, this);
			}
		}

		NPCMap::iterator npc=npcs.begin();
		NonPlayer* currentnpc;
		for ( ;npc!=npcs.end(); ) {
			currentnpc = (*npc).second;
			////�ڽ��� �����ִ� trigger�� �˻��Ѵ�.
			AI::TriggerSystem::TriggerMap& triggers = triggerSystem.GetTriggerList();
			AI::TriggerSystem::TriggerMap::iterator i = triggers.begin(), end = triggers.end();
			for (; i != end ; ++i) {
				// �ڽ��� �����ִ� Ʈ���Ÿ��� �޴´�.
				if ( !((*i).second.type & currentnpc->GetTriggerFlag()) )
					continue;
				// �ڽ��� ���� Ʈ���Ŵ� DESTINATION_ARRIVED���� �޴´�.
				if ( (*i).second.type == AI::Trigger::TriggerType::DESTINATION_ARRIVED && (*i).second.creator != currentnpc->GetSerial() )
					continue;
				//Ʈ���Ű� �Ÿ� �ȿ� �ִ°�.
				if ( (*i).second.radius != 0 && Lunia::Math::Length<float>((*i).second.position - currentnpc->GetPosition()) > (*i).second.radius )
					continue;
				if ( currentnpc->HandleTrigger((*i).second) )
					break;	//Ʈ���� ���Ͽ� �Ѱ����� ó��. -0-��
			}


			// Actor �� �Ѿ����� �� �浹üũ ���ϱ� ����
			if ( currentnpc->IsOverlapped() && !currentnpc->IsOverlappable()) {
				if ( !IsOverlapped( currentnpc) )
					currentnpc->SetOverlap(false);
			}
			// ���� ���� ////////////////////////////////////////////////////////////


			if ( currentnpc->Update(dt, this) ) {//true�� ��ü�� �׾�� �� �������� ���ϵȴ�.
				ObjectDestroyed(currentnpc, Constants::ObjectType::NonPlayer/*, currentnpc->GetPosition(), currentnpc->GetDirection(), 0, 0*/);
				//stage.ObjectDestroyed( currentnpc->GetName() );

				//�������� �기��.
				float r = GetRandomFloat();
				bool dropEventItem = false;

				uint32 dropCount = 1;

				Database::Info::StageGroup* stageGroup = stage.GetStageGroupInfo();
				if (stageGroup) 
				{
					dropCount = stageGroup->GetDropCount(GetDifficulty(), GetRandomFloat() );
				}
				//uint8 difficulty = GetDifficulty();
				//if( stageGroup != NULL && stageGroup->HasDifficulty )
				//{
				//	if( stageGroup->Difficulties.size() > difficulty )
				//	{
				//		float dropFactor = stageGroup->Difficulties[ difficulty ].DropFactor;
				//		dropCount = static_cast< uint32 >( dropFactor );
				//		dropFactor -= dropCount;
				//		if( dropFactor > 0.0f )
				//		{
				//			if( dropFactor > GetRandomFloat() )
				//			{
				//				++dropCount;
				//			}
				//		}
				//	}
				//}


				if (currentnpc->IsDropEventNPC() && (stage.GetGameType() & Constants::GameTypes::CooperativeGameTypeMask) )
				{
					const Info::NonPlayerInfo::Item* eventItem = Database::DatabaseInstance().InfoCollections.EventNonPlayerItemLists.GetEventItem(r, currentnpc->GetNameHash(), logic.GetEventListener().GetCurrentStageLicense() );
					if (eventItem )
					{
						dropEventItem = true;

						//�������� ��ȴ�
						static const float maxDistance = 15;
						static const float minDistance = 7;
						static const float theta = 30;
						
						static float3 posRevision[12];
						static int initRevisionVec = false;
		                
						if(!initRevisionVec)
						{
							float3 baseVector(1,0,0);
							for(int posIndex = 0; posIndex < 12; ++posIndex)
							{
								baseVector = PhysicalFunctions::RotatePosition(baseVector, theta);
								posRevision[posIndex] = baseVector;
							}
							initRevisionVec = true;
						}

						int index = 0;
						PlayerMap::iterator i = players.begin();
						for ( ; i != players.end() ; ++i) 
						{
							float3 pos = currentnpc->GetPosition();
							pos.y = 0;

							for( uint32 ea = 0 ; ea <  dropCount ; ++ea )
							{
								Player* player = (*i).second;
								CreateItem(eventItem->Hash, pos + (posRevision[index] * GetRandomInt(minDistance, maxDistance)), currentnpc->GetDirection(), player->GetSerial(),Lunia::uint16(1) );						
							}
							
							++index;
						}
					}
				}
				
				if (dropEventItem == false)
				{
					std::vector<Info::NonPlayerInfo::Item>& items = currentnpc->GetItemList();
					std::vector<Info::NonPlayerInfo::Item>::iterator item = items.begin(), itemEnd = items.end();

					float prob=0;
					for (; item != itemEnd ; ++item) 
					{
						if (item->Probability==0.0f) 
						{
							continue;
						}
					
						prob += (*item).Probability;
						if ( r <= prob ) 
						{	
							static const float maxDistance = 15;
							static const float minDistance = 7;
							static const float theta = 30;
							
							static float3 posRevision[12];
							static int initRevisionVec = false;
	                        
							if(!initRevisionVec)
							{
								float3 baseVector(1,0,0);
								for(int posIndex = 0; posIndex < 12; ++posIndex)
								{
									baseVector = PhysicalFunctions::RotatePosition(baseVector, theta);
									posRevision[posIndex] = baseVector;
								}
								initRevisionVec = true;
							}

							int createItemCount = (currentnpc->GetNPCInfo()->GiveItemToAllPlayer ? players.size() : 1);
							for(int index = 0; index < createItemCount; ++index)
							{
								for( uint32 ea = 0 ; ea < dropCount ; ++ea )
								{
									float3 pos = currentnpc->GetPosition();
									pos.y = 0;
									CreateItem((*item).Hash, pos + (posRevision[index] * GetRandomInt(minDistance, maxDistance)), currentnpc->GetDirection(), uint16(1));
								}
							}
							break;
						}
					}
				}

				Info::NonPlayerInfo* info = currentnpc->GetNPCInfo();
				if ( !info->UnidentifiedItems.empty() )
					AddUnidentifiedItems(info, dropCount);

				{
					AutoLock locker(csUpdateObject);

					LogicDBInstance().ReleaseNPC(currentnpc);
					npc = npcs.erase(npc);
				}
			} else ++npc;
		}

		itemManager.Update(dt, this);

		NonPlayer* obj = NULL;
		{ 
			AutoLock lock(csNPCBuffer);
			if ( !npcBuffer.empty() ) {
				obj = *npcBuffer.begin();
				npcBuffer.erase( npcBuffer.begin() );
			}
		}
		if ( obj ) {
			float3 position = obj->GetPosition();
			obj->InitPosition( GetAvailableGhostPos(position, obj->GetRadius()) );
			CreateNonPlayer(obj);

			obj->SetOverlap(true);

			std::vector<Player*> l;
			GetAreaObject(obj->GetPosition(), obj->GetRadius(), Constants::AllianceType::AnyObjects, 0, obj, l);
			for ( std::vector<Player*>::iterator ip = l.begin() ; ip != l.end() ; ++ip )
				(*ip)->SetOverlap(true);
		}

		bUpdateNPC = false;

		Actor* misc=NULL;
		{
			AutoLock lock(csMISCBuffer);
            if ( !objectBuffer.empty() ) {
				misc = *objectBuffer.begin();
				objectBuffer.erase( objectBuffer.begin() );
			}
		}

		if ( misc ) {
			CreateObject(misc);

			//object�� �ִ� ��ġ�� player, npc���� �ֺ����� �о� ����.
			std::vector<Actor*> l;
			GetAreaObject(misc->GetPosition(), misc->GetRadius(), Constants::AllianceType::AnyObjects, 0, misc, l);
			for(std::vector<Actor*>::iterator i = l.begin() ; i != l.end() ; ++i) {
				float3 newPos = GetAvailableGhostPos((*i)->GetPosition(), (*i)->GetRadius());
				if ( newPos == float3(0,0,0) ) { //���� ��ġ�� ������. player�� 0,0,0���� ���� ������ ������ ���������� ������ �α׸�..
					if ( (*i)->GetType() == Constants::ObjectType::Player ) {
						//Lunia_INFO((L"[GameStateDatabase::Update] Can't locate player(serial: %s, position : %f,%f,%f)", (*i)->GetName().c_str(), (*i)->GetPosition().x, (*i)->GetPosition().y, (*i)->GetPosition().z ));
						//(*i)->SetPosition( stage.GetStartPosition() );
					}
				} else {
					(*i)->SetPosition( newPos );
				}

				(*i)->SetOverlap(true);
				Lunia::XRated::CharacterData& data = (*i)->GetCharacterData();
				StatusChanged(data.BaseObject.GameObjectSerial, data.BaseObject.Position, data.BaseObject.Direction,
											data.Hp, data.Mp, 0, 0);
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////





	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Event to logic	////////////////////////////////////////////////////////////////////////////////////////////
	void GameStateDatabase::Initialized(uint16 uniqueId)
	{
		logic.GetEventListener().Initialized( stage.GetStageInfo(), uniqueId );
		//Lunia_INFO(( L"[GameStateDatabase::Initialized] stage initialized." ));
	}

	ILockable& GameStateDatabase::GetSyncRoom()
	{
		return logic.GetEventListener().GetSyncRoom();
	}


	bool GameStateDatabase::PlayerCreated(Player* player)
	{
		return logic.GetEventListener().PlayerCreated(player);
	}

	void GameStateDatabase::NonPlayerCreated(NonPlayerData& data)
	{
		logic.GetEventListener().NonPlayerCreated(data);
	}

	bool GameStateDatabase::Tamed(Player* player, Serial familiar, Constants::Familiar::Type type)
	{
		return logic.GetEventListener().Tamed(player, familiar, type);
	}

	void GameStateDatabase::StructureCreated(CharacterData& data)
	{
		logic.GetEventListener().StructureCreated(data);
	}

	void GameStateDatabase::VehicleCreated(CharacterData& data)
	{
		logic.GetEventListener().VehicleCreated(data);
	}

	void GameStateDatabase::ProjectileCreated(ObjectData& data, const float3& targetPos, Serial target, const std::wstring& owner)
	{
		logic.GetEventListener().ProjectileCreated(data, targetPos, target, owner);
	}

	void GameStateDatabase::ItemCreated(Lunia::XRated::ObjectData& data, const std::wstring& owner, float dt, uint16 stackCount, bool isPrivateItem)
	{
		logic.GetEventListener().ItemCreated(data, owner, dt, stackCount, isPrivateItem);
	}

	void GameStateDatabase::ObjectCreated(ObjectData& data)
	{
		logic.GetEventListener().ObjectCreated(data);
	}

	void GameStateDatabase::ObjectDestroyed(Object* obj, Constants::ObjectType type, bool silent)
	{
		switch(type) {
		case Constants::ObjectType::Misc :
			logic.GetEventListener().ObjectDestroyed(obj->GetSerial(), type, ((Misc*)obj)->GetMiscInfo().NameHash, silent, ((Misc*)obj)->GetCharacterData().Team);
			smallSector.ObjectDestroyed((Actor*)obj, obj->GetSector().smallSector);
			bigSector.ObjectDestroyed((Actor*)obj, obj->GetSector().bigSector);
			break;
		case Constants::ObjectType::TombStone :
			logic.GetEventListener().ObjectDestroyed(obj->GetSerial(), type, 0, silent);
			smallSector.ObjectDestroyed((Actor*)obj, obj->GetSector().smallSector);
			bigSector.ObjectDestroyed((Actor*)obj, obj->GetSector().bigSector);
			break;
		case Constants::ObjectType::NonPlayer :
			logic.GetEventListener().ObjectDestroyed(obj->GetSerial(), type, ((NonPlayer*)obj)->GetNPCInfo()->HashcodeName, silent, ((NonPlayer*)obj)->GetCharacterData().Team, ((NonPlayer*)obj)->GetNonPlayerData().Npc);
			smallSector.NonPlayerDestroyed((NonPlayer*)obj, obj->GetSector().smallSector);
			bigSector.NonPlayerDestroyed((NonPlayer*)obj, obj->GetSector().bigSector);
			((Actor*)obj)->ClearAirAttackData();
			break;
		case Constants::ObjectType::Player :
			if ( logic.GetEventListener().GetItemCount((Player*)obj, Database::Info::FindReviveState(Database::Info::StateInfo::Type::REVIVE)) )//���������� ������. ���������� �̺�Ʈ�� ������.
				stage.CoinPlayerGiveUp(obj->GetSerial());

			if ( ( stage.GetGameType() & Constants::GameTypes::DeathMatchGameType ) != 0 || ( stage.GetGameType() & Constants::GameTypes::LadderGameType ) != 0 ) {
				if ( !stage.IsMissionCleared() ) {//pvp���ӿ��� �߰��� ������.
					uint32 xpPvp = DatabaseInstance().InfoCollections.Statuses.GetExp(Info::StatusInfoManager::ExpType::Pvp, ((Player*)obj)->GetPlayerData().PvpLevel-1);
					((Player*)obj)->PvpLevelUp(XRated::Constants::ExpAcquiredType::ExpPvp ,-300, xpPvp); //����ġ�� ���̴� ���� ������ �ִ� ����ġ�� �� ���ʿ䰡 ����. �� 0�� �ѱ��.
				}
			}
			logic.GetEventListener().ObjectDestroyed(obj->GetSerial(), type, 0, silent);
			smallSector.PlayerDestroyed((Player*)obj, obj->GetSector().smallSector);
			bigSector.PlayerDestroyed((Player*)obj, obj->GetSector().bigSector);
			((Actor*)obj)->ClearAirAttackData();
			break;
		default :
			logic.GetEventListener().ObjectDestroyed(obj->GetSerial(), type, 0, silent);
			break;
		}

		stage.ObjectDestroyed( obj->GetSerial(), obj->GetNameHash(), type );
	}

	void GameStateDatabase::Died(Player* player)
	{
		stage.PlayerDied(player->GetSerial(), player->GetLife());

		logic.GetEventListener().Died(player);

		Database::Info::StageInfo* currentStageInfo = stage.GetStageInfo();

		switch ( stage.GetGameType() ) {
		case Constants::GameTypes::SideGameType :
		case Constants::GameTypes::StoryGameType :	//��� �÷��̾ �׾������� �˻��Ѵ�.
			if ( logic.GetEventListener().GetItemCount(player, Database::Info::FindReviveState(Database::Info::StateInfo::Type::REVIVE)) ) //���������� ������. ���������� �̺�Ʈ�� ������.
				stage.CoinPlayerDied(player->GetSerial());

			//��� �׾����� mission ���и� �����ش�.
			for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter) {
				if ( player != (*iter).second && !(*iter).second->CheckState(Database::Info::StateInfo::Type::GHOST) )
						return;//���� ���������� �ִ�.
				else {
#ifdef _CHINA
					if ( (*iter).second->GetPlayerData().Life > 0 )
						return;//�������� ���� �������� �ִ�.
					else if ( iter->second->GetUsableBonusLife() > 0 && players.size() == 1 )
						return;//���� ���� ��Ȱ�� ���Ҵ�.
#else
					if ( (*iter).second->GetPlayerData().Life > 0 )
						return;//�������� ���� �������� �ִ�.
#endif

						
				}
			}

			if( currentStageInfo != NULL )
			{
				if( currentStageInfo->isReviveAvailable == false )
				{
					return;
				} 
			}
			logic.GetEventListener().ContinueCountDown();
			stage.ContinueCountDown();
			stage.MissionClearFromLogic(false, 1);
			break;
		case Constants::GameTypes::DeathMatchGameType :	//������ ����� �ٸ� ���� ��� �� �׾������� �˻��Ѵ�.
		case Constants::GameTypes::LadderGameType :
			{
				uint8 team = 0;
				for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter) {
					Player* p = (*iter).second;
					if( p->GetPlayerData().Life > 0 ||
						( !p->CheckState(Database::Info::StateInfo::Type::GHOST) && p != player ) ) {
						if ( !team )
							team = p->GetTeam();
						else if ( team == p->GetTeam() )
							;
						else 
							return;
					}
				}

				if ( team )
					stage.MissionClearFromLogic(true, team);
			}
			break;
		
		case Constants::GameTypes::BattleGroundType :
			{
			}
			break;
			
		case Constants::GameTypes::SurvivalGameType :
			{
				Player* p, *survived=NULL;
				for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter) {
					p = (*iter).second;
					if( p->GetPlayerData().Life > 0 ||
						( !p->CheckState(Database::Info::StateInfo::Type::GHOST) && p != player ) ) {
						if ( survived )
							return;
						else
							survived = p;
					}
				}

				if ( survived )
					stage.MissionClearFromLogic(true, survived->GetTeam());
			}
			break;
		}
	}

	void GameStateDatabase::Pause(float time)
	{
		for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter)
			(*iter).second->Command(Action::Action::CommandType::STOP, 0);

		logic.Pause(time);
	}

	void GameStateDatabase::CampfireBegin()
	{
		int teamMemberCnt[Constants::MaxTeamCnt] = {0,};
		uint8 teamCnt=1;
		uint8 team;
		std::vector< std::vector<Info::StageInfo::Placement> >& campPos = stage.GetStageInfo()->CampfirePoint;
		for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter) {
			Player* player = iter->second;
			team = player->GetTeam();
			if (team>=Constants::MaxTeamCnt) Logger::GetInstance().Exception(L"invalid team range({0}/{1})", team, Constants::MaxTeamCnt);
			if ( (int)campPos.size() >= team ) {
				int index = teamMemberCnt[team] % (int)campPos[team-1].size();
				player->SetPosition( campPos[team-1][index].Position );
				player->SetDirection( campPos[team-1][index].Direction );
				++ teamMemberCnt[team];
			}
			player->SetTeam(teamCnt++);
			player->AddState(Stat::CreateState<Actor>(Database::Info::StateInfo::Type::CAMPFIRE, player, NULL, NULL));
			//player->SetAction(Info::HashTable::Actions::Campfire, NULL, true);
			player->ClearFamiliar();
			if ( player->CheckState(Database::Info::StateInfo::Type::GHOST) ) { 
				if ( player->GetPlayerData().Life > 0 ) {
					player->ReduceLife();
				}
				player->Revive( Player::ReviveStatus::None, false);
			}
			player->SetAction(Info::HashTable::Actions::Campfire, NULL, true);

			player->RemoveAllStateBundle(Database::Info::StateInfo::DOT);
			player->RemoveAllStateBundle(Database::Info::StateInfo::CHOKE);
			player->SetHP(100000);
			player->SetMP(100000); 
		}

		for (NPCMap::iterator npc=npcs.begin() ; npc!=npcs.end(); ++npc) {
			Object::Sectors& sectors = (*npc).second->GetSector();
			sectors.bigSector->RemoveNpc( (*npc).second );
			sectors.smallSector->RemoveNpc( (*npc).second );
			LogicDBInstance().ReleaseNPC( (*npc).second );
		}
		npcs.clear();
		stage.SetGameType(Constants::GameTypes::DeathMatchGameType);
	}

	void GameStateDatabase::FastMatch()
	{
		std::vector<Player*> l;
		uint16 minLife=10000;
		for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter) {
			Player* p = iter->second;
			uint16 life = p->GetPlayerData().Life;
			bool ghost = p->CheckState(Database::Info::StateInfo::Type::GHOST);
			
			if (!life && !ghost)
				return;


			if ( life ) {
				if ( minLife > life ) minLife = life;
				l.push_back(p);
			}
		}

		int cnt = (int)l.size();
		while( cnt-- ) {
			l[cnt]->ReduceLife(minLife);
		}
	}

	float GameStateDatabase::GetObjectDistance(Player* player, uint32 hash) const
	{
		AutoLock lock(csUpdateObject);
		float3 sourcePos, targetPos;
		sourcePos = player->GetPosition();

		float lastDistance(-1);

		for( NPCMap::const_iterator i = npcs.begin() ; i != npcs.end() ; ++i ) {

			if ( (*i).second->GetNameHash() == hash ) {
				targetPos = (*i).second->GetPosition();
				float distance=Lunia::Math::Length<float>(targetPos - sourcePos);
				if (lastDistance<0) lastDistance=distance; else lastDistance=min(lastDistance, distance);
			}
		}
		if (lastDistance>=0) return lastDistance;

		for( ObjectMap::const_iterator iObj = objects.begin() ; iObj != objects.end() ; ++iObj ) {
			if ( (*iObj).second->GetNameHash() == hash ) {
				targetPos = (*iObj).second->GetPosition();
				float distance=Lunia::Math::Length<float>(targetPos - sourcePos);
				if (lastDistance<0) lastDistance=distance; else lastDistance=min(lastDistance, distance);
			}
		}

		return lastDistance;
	}

	int GameStateDatabase::GetReviveLimitCount()
	{
		Info::StageInfo* stageInfo = GetStageInfo();
		if (!stageInfo) return -1;

		return stageInfo->ReviveLimitCount;
	}

	int	GameStateDatabase::GetSecretCount()
	{
		Info::StageInfo* stageInfo = GetStageInfo();
		if (!stageInfo) return 0;
		
		std::map<std::wstring, int32>::iterator valueIter = stageInfo->values.find(L"secretFound");
		if( valueIter == stageInfo->values.end() ) return 0;

		return valueIter->second;
	}

	int	GameStateDatabase::GetStandardTime()
	{
		Info::StageGroup* stageGroupInfo = GetStageGroupInfo();
		if (!stageGroupInfo) return 0;

		Database::Info::StagePlayTime* info=Database::DatabaseInstance().InfoCollections.StagePlayTimes.Retrieve(stageGroupInfo->StageHashes[ logic.GetEventListener().GetCurrentStageLicense().Level ]);
		if (!info) return 0;

		return info->AveragePlayTime;
	}
	
	float GameStateDatabase::GetStandardDiviation()
	{
		Info::StageGroup* stageGroupInfo = GetStageGroupInfo();
		if (!stageGroupInfo) return 0;

		Database::Info::StagePlayTime* info=Database::DatabaseInstance().InfoCollections.StagePlayTimes.Retrieve(stageGroupInfo->StageHashes[ logic.GetEventListener().GetCurrentStageLicense().Level ]);
		if (!info) return 0;

		return info->StandardDiviation;
	}
	
	int	GameStateDatabase::GetStageProprietyLevel() //�������� �������� �����´�.
	{
		//�������� �������� = (�������� ���巹�� Max + �������� ���巹�� Min) / 2			

		Info::StageInfo* stageInfo = GetStageInfo();
		if (!stageInfo) {
			Logger::GetInstance().Info( L"GetStageProprietyLevel() : GetStageInfo Failed" );
			return XRated::Constants::PlayerMaxLevel; //stageInfo�� ����? ��ġ��.. �׷� ���� ���� �ֱ� ���� ������ MaxLevel�� �Ѱ��ش�.
		}


		if (stageInfo->ProprietyLevel == 0) {
			Logger::GetInstance().Info( L"GetStageProprietyLevel() : {0} Stage ProprietyLevel is 0", stageInfo->Id.c_str() );
			return XRated::Constants::PlayerMaxLevel;
		}

		return stageInfo->ProprietyLevel;
	}

	int	GameStateDatabase::GetPlayTime(int level)
	{	
	
		if (level <= 0) 
			return 1000000;

		if (level >= 1 && level <=10)
			return static_cast<int>((level * level)*2.5f + 112.5f*level + 5 + 0.5f);
		else if (level >= 11 && level <=20)
			return static_cast<int>((level * level)*5.0f + 65.0f*level + 230 + 0.5f);
		else if (level >= 21 && level <=30)
			return static_cast<int>((level * level)*7.5f - 32.5f*level + 1180 + 0.5f);
		else if (level >= 31 && level <=40)
			return static_cast<int>((level * level)*10.0f - 180.0f*level + 3355 + 0.5f);
		else if (level >= 41 && level <=50)
			return static_cast<int>((level * level)*12.5f - 377.5f*level + 7255 + 0.5f);
		else if (level >= 51 && level <=60)
			return static_cast<int>((level * level)*15.0f - 625.0f*level + 13380 + 0.5f);
		else if (level >= 61 && level <=70)
			return static_cast<int>((level * level)*17.5f - 922.5f*level + 22230 + 0.5f);
		else if (level >= 71 && level <=80)
			return static_cast<int>((level * level)*20.0f - 1270.0f*level + 34305 + 0.5f);
		else if (level >= 81 && level <=90)
			return static_cast<int>((level * level)*22.5f - 1667.5f*level + 50105 + 0.5f);
		else if (level >= 91 && level <=99)
			return static_cast<int>((level * level)*25.0f - 2115.0f*level + 70130 + 0.5f);

		return 1000000;
	}

	float GameStateDatabase::GetXpPerSecond(int level, int playTime)
	{
		//�ʴ� ȹ�� ����ġ(Ideal) = ExpTable�� / �÷���Ÿ��			

		uint32 neededXp = Database::DatabaseInstance().InfoCollections.Statuses.GetExp( Info::StatusInfoManager::ExpType::Story, level-1);

		return static_cast<float>(neededXp) / static_cast<float>(playTime);
	}

	float GameStateDatabase::GetMissionClearXp(float xpForSecond)
	{	
		return xpForSecond * static_cast<float>(GetStandardTime() );
	}
	
	void GameStateDatabase::MissionClear(bool success, uint8 team)
	{
		CriticalStatistics::StageStatistics SS;
		stage.GetStageStatistics(SS);

		//pvp������ ��� ����ġ�� �й����ش�.
		switch ( stage.GetGameType() ) {
			case Constants::GameTypes::StoryGameType:
			case Constants::GameTypes::SideGameType:
				{
					int stageProprietyLevel = GetStageProprietyLevel();
					int playTime = GetPlayTime(stageProprietyLevel);
					float xpForSecond = GetXpPerSecond(stageProprietyLevel, playTime);
					float gainXp = GetMissionClearXp(xpForSecond);

					int totalSecret = GetSecretCount();
					int standardTime = GetStandardTime();
					
					///	��° �Ⱦ��� �Ƴ׿�.... 2008.02.28 by Yong-Eun, Jung
					//float standardDiviation = GetStandardDiviation();
					//float reviveXpRate = XRated::GetGainXPRateByReviveCount(totalReviveCount);
					//SS.AllPlayerReviveCount = totalReviveCount;
					totalReviveCount = 0; // 3.1 by ycr4zy 14-11-2019 [ on finish stage reset all total revive :C ]

					for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter) {
						Player* player = iter->second;
				
						uint8 rank = MissionResultFunctions::GetStageClearRank(success, static_cast<int>(SS.ElapsedTime), standardTime, player->IsDead(), player->GetLife(),
																			SS.FoundSecret, totalSecret, player->GetMaxAirAttackComboCnt(), player->GetSytlePointPercentage() );

						player->SetTotalRank(rank);
						player->SetTimeRank(MissionResultFunctions::GetPlayTimeRank(static_cast<int>(SS.ElapsedTime), standardTime) );
						player->SetLifeRank(MissionResultFunctions::GetLifeRank(player->IsDead(), player->GetLife() ) );
						player->SetSecretRank(MissionResultFunctions::GetSecretRank(SS.FoundSecret, totalSecret) );
						player->SetAirComboRank(MissionResultFunctions::GetAirComboRank(player->GetMaxAirAttackComboCnt() ) );
						player->SetStylePointRank(MissionResultFunctions::GetStylePointRank(player->GetSytlePointPercentage()));

						uint64 xp = XRated::GetGainXPByStatisticsRank(rank, gainXp);
						xp = ExpFunctions::Stage::CalculateXpWithLevelGap(xp, stageProprietyLevel-static_cast<int>(player->GetLevel()));
						if (xp > 0) player->XpGained(XRated::Constants::ExpAcquiredType::ExpRank, xp, true);

						//Lunia_INFO(( L"[GameStateDatabase::MissionClear] ProprietyLevel:{}, PlayTime:{}, XpForSecond:%f, GainXp:%f, Rank:%s, RealGainXp:{} "
						//				 , stageProprietyLevel, playTime, xpForSecond, gainXp, XRated::GetRank(rank).c_str(), xp));

						//if (reviveXpRate > 0.0f)
						//{
						//	uint64 reviveXp = static_cast<uint64>(player->GetStageXp() * reviveXpRate);
						//	if (reviveXp > 0) player->XpGained(XRated::Constants::ExpAcquiredType::ExpReviveCount, reviveXp, false);
						//}
					}
				}
				break;
			case Constants::GameTypes::DeathMatchGameType :
			case Constants::GameTypes::LadderGameType :
				{
					if (static_cast<float>(tickCnt / 20.0f / 60.f) >= XRated::Constants::PvPLimitTime) 
					{
						// StageGroupHash �� �ش� ��������� ��� �������� �ִ��� Ȯ��
						uint32 stageHash = stage.GetStageId();
						Info::PvPRewardInfo rewardInfo;
						bool existRewardItem = DatabaseInstance().InfoCollections.PvPRewardItemInfos.GetRewardInfo( stageHash, rewardInfo );

						// �ִٸ� ���� �����ۿ� ���� ������ �� Ŭ���̾�Ʈ�鿡�� �ѱ��.
						if( existRewardItem )
						{
							logic.GetEventListener().SendPvPRewardItemInfo( rewardInfo );
						}

						//Lunia_INFO(( L"[GameStateDatabase::MissionClear] PVP result. Team Cnt : {}", pvp.deathMatch.memberCnt ));
						for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter) {
							Player* p = iter->second;
							uint8 currentTeam = p->GetTeam();
							uint8 opponent = currentTeam == 1 ? 2 : 1 ;
							int xp;
							PlayerData& playerData = p->GetPlayerData();
							uint32 xpPvp = DatabaseInstance().InfoCollections.Statuses.GetExp(Info::StatusInfoManager::ExpType::Pvp, playerData.PvpLevel-1);

							if( currentTeam == team ) { // �̱� ��
								xp = ExpFunctions::Pvp::CalculateXpForWinner( p->GetLevel(), p->GetPvpLevel(), pvp.deathMatch.avgLv[currentTeam-1], pvp.deathMatch.deter[currentTeam-1], pvp.deathMatch.memberCnt );
								p->PvpLevelUp( XRated::Constants::ExpAcquiredType::ExpPvp, xp, xpPvp);
								
								xp = ExpFunctions::Pvp::CalculateStageXpForWinner( p->GetLevel(), pvp.deathMatch.avgLv[ opponent-1 ] - pvp.deathMatch.avgLv[ currentTeam - 1 ]  , tickCnt / 20.0f , pvp.deathMatch.memberCnt );
								if ( xp > 0 ) p->XpGained(XRated::Constants::ExpAcquiredType::ExpPvp, xp );
								//if ( tickCnt >= 1800 ) {
								//	xp = ExpFunctions::Pvp::CalculateStageXpForWinner( p->GetLevel(), pvp.deathMatch.avgLv[currentTeam-1], pvp.deathMatch.deter[currentTeam-1], pvp.deathMatch.memberCnt);
								//	p->XpGained(xp);
								//}

								//RewardMoney
								if ( !(p->GetPlayTimePenalty() & XRated::Constants::PlayTimePenalty::Gold) ) {
									GoldGained(p, XRated::Constants::GoldRewardAcquiredType::PvPResult, 
																GoldFunctions::Pvp::CacluateVictoryMoneyReward(playerData.BaseCharacter.Level, pvp.deathMatch.memberCnt, 
																logic.GetEventListener().GetKillCount(p->GetSerial() ), 
																logic.GetEventListener().GetDeathCount(p->GetSerial() ) ) );
								}

								//RewardItem
								if( existRewardItem )
								{
									logic.GetEventListener().AcquireItemWithMailSupport( p, rewardInfo.WinnerItem.Hash, rewardInfo.WinnerItem.Count );
								}
							} else {// �� ��
								xp = ExpFunctions::Pvp::CalculateXpForLoser( p->GetLevel(), p->GetPvpLevel() );
								p->PvpLevelUp( XRated::Constants::ExpAcquiredType::ExpPvp, xp, xpPvp);

								xp = ExpFunctions::Pvp::CalculateStageXpForLoser( p->GetLevel(), pvp.deathMatch.avgLv[ opponent-1 ] - pvp.deathMatch.avgLv[ currentTeam - 1 ]  , tickCnt / 20.0f , pvp.deathMatch.memberCnt );
								if ( xp > 0 ) p->XpGained( XRated::Constants::ExpAcquiredType::ExpPvp, xp );

								//if ( tickCnt >= 1800 ) {
								//	xp = ExpFunctions::Pvp::CalculateStageXpForLoser( p->GetLevel(), p->GetPvpLevel(), pvp.deathMatch.avgLv[currentTeam-1], pvp.deathMatch.deter[currentTeam-1], pvp.deathMatch.memberCnt );
								//	p->XpGained(xp);
								//}

								//RewardMoney
								if ( !(p->GetPlayTimePenalty() & XRated::Constants::PlayTimePenalty::Gold) ) {
									GoldGained(p, XRated::Constants::GoldRewardAcquiredType::PvPResult, 
																GoldFunctions::Pvp::CacluateDefeatedMoneyReward(playerData.BaseCharacter.Level, pvp.deathMatch.memberCnt, 
																logic.GetEventListener().GetKillCount(p->GetSerial() ), 
																logic.GetEventListener().GetDeathCount(p->GetSerial() ) ) );
								}

								//RewardItem
								if( existRewardItem )
								{
									logic.GetEventListener().AcquireItemWithMailSupport( p, rewardInfo.LoserItem.Hash, rewardInfo.LoserItem.Count );
								}
							}
							//Lunia_INFO(( L"[GameStateDatabase::MissionClear] %s, team:{}, lv:{}, pvpLv:{}, avgLv:%f/%f, xp:{} "
							//			, playerData.BaseCharacter.BaseObject.Name.c_str(), currentTeam, p->GetLevel(), p->GetPvpLevel(), pvp.deathMatch.avgLv[0], pvp.deathMatch.avgLv[1] ,xp ));
						}
					}
				}
				break;
			case Constants::GameTypes::BattleGroundType:
				{
					if (static_cast<float>(tickCnt / 20.0f / 60.f) >= XRated::Constants::PvPLimitTime) 
					{
						////TODO: ���� ����ġ ����� ���⼭ �ϸ�ɲ��ϵ� caspian
						//for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter) 
						//{
						//	Player* p = iter->second;
						//	PlayerData& playerData = p->GetPlayerData();
						//	if ( p->GetTeam() == team )
						//	{							
						//		uint32 xpWar = DatabaseInstance().InfoCollections.Statuses.GetExp(Info::StatusInfoManager::ExpType::War, playerData.WarLevel-1);
						//		p->WarLevelUp( XRated::Constants::ExpAcquiredType::ExpPvp, 50 , xpWar );
						//	}
						//}		

						uint32 stageHash = stage.GetStageId();
						Info::PvPRewardInfo rewardInfo;
						bool existRewardItem = DatabaseInstance().InfoCollections.PvPRewardItemInfos.GetRewardInfo( stageHash, rewardInfo );

						// �ִٸ� ���� �����ۿ� ���� ������ �� Ŭ���̾�Ʈ�鿡�� �ѱ��.
						if( existRewardItem )
						{
							logic.GetEventListener().SendPvPRewardItemInfo( rewardInfo );

							for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter) 
							{
								Player* p = iter->second;
								uint8 currentTeam = p->GetTeam();

								if( currentTeam == team ) 
								{ // �̱� ��
									logic.GetEventListener().AcquireItemWithMailSupport( p, rewardInfo.WinnerItem.Hash, rewardInfo.WinnerItem.Count );
								}
								else
								{ // �� ��
									logic.GetEventListener().AcquireItemWithMailSupport( p, rewardInfo.LoserItem.Hash, rewardInfo.LoserItem.Count );
								}
							}
						}	
					}
				}
				break;
			case Constants::GameTypes::SurvivalGameType :
				{
					uint32 stageHash = stage.GetStageId();
					Info::PvPRewardInfo rewardInfo;
					bool existRewardItem = DatabaseInstance().InfoCollections.PvPRewardItemInfos.GetRewardInfo( stageHash, rewardInfo );

					// �ִٸ� ���� �����ۿ� ���� ������ �� Ŭ���̾�Ʈ�鿡�� �ѱ��.
					if( existRewardItem )
					{
						logic.GetEventListener().SendPvPRewardItemInfo( rewardInfo );

						for(PlayerMap::iterator iter = players.begin() ; iter != players.end() ; ++iter) 
						{
							Player* p = iter->second;
							uint8 currentTeam = p->GetTeam();

							if( currentTeam == team ) 
							{ // �̱� ��
								logic.GetEventListener().AcquireItemWithMailSupport( p, rewardInfo.WinnerItem.Hash, rewardInfo.WinnerItem.Count );
							}
							else
							{ // �� ��
								logic.GetEventListener().AcquireItemWithMailSupport( p, rewardInfo.LoserItem.Hash, rewardInfo.LoserItem.Count );
							}
						}
					}	
				}
				break;
		}

		uint32 dropCount = 1;
		Database::Info::StageGroup* stageGroup = stage.GetStageGroupInfo();
		if (stageGroup) 
		{
			dropCount = stageGroup->GetDropCount(GetDifficulty(), GetRandomFloat() );
		}

		logic.GetEventListener().MissionClear(success, team, SS, dropCount);

		//CampfireBegin();
	}

	bool GameStateDatabase::AddItem(Player* player, uint32 id, int cnt)
	{
		if ( player->GetPlayTimePenalty() & XRated::Constants::PlayTimePenalty::Item ) {
			return true;
		}

		// ȹ~ �ѹ��� ���ư��ٸ� �̸����� �ʰڿ�;;
		if( static_cast<Serial>( projectileSeed + cnt ) < projectileSeed )
		{
			projectileSeed = ProjectileSerialBase;
		}

		// �ٵ��� �ȵǸ� �̰� ��;;
		if( static_cast<Serial>( projectileSeed + cnt ) < projectileSeed )
		{
			Logger::GetInstance().Exception( L"Projectile serial overflow - cnt {0}", cnt );
		}

		bool returnValue = logic.GetEventListener().AddItem(player, id, projectileSeed, cnt);
		if ( returnValue )
			projectileSeed += cnt;
		return returnValue;
	}

	bool GameStateDatabase::AddItem(Player* player, int* arrayId, int* arrayCnt)
	{
		if ( player->GetPlayTimePenalty() & XRated::Constants::PlayTimePenalty::Item ) {
			return true;
		}

		if( static_cast<Serial>( projectileSeed + arrayCnt[0] ) < projectileSeed )
		{
			projectileSeed = ProjectileSerialBase;
		}

		// �ٵ��� �ȵǸ� �̰� ��;;
		if( static_cast<Serial>( projectileSeed + arrayCnt[0] ) < projectileSeed )
		{
			Logger::GetInstance().Exception(L"Projectile serial overflow - cnt {0}", arrayCnt[0] );
		}

		bool returnValue = logic.GetEventListener().AddItem(player, projectileSeed, arrayId, arrayCnt);
		if ( returnValue )
			projectileSeed += arrayCnt[0];
		return returnValue;
	}

	bool GameStateDatabase::RemoveItem(Player* player, uint32 id, int cnt)
	{
		return logic.GetEventListener().RemoveItem(player, id, cnt);
	}

	bool GameStateDatabase::RemoveItem(Player* player, int* arrayId, int* arrayCnt)
	{
		return logic.GetEventListener().RemoveItem(player, arrayId, arrayCnt);
	}

	bool GameStateDatabase::ExamineItemSlotFromPlayer(uint32 serial, uint32 itemId, int cnt)
	{
		Player* player = RetrievePlayer(serial);
		if (!player) {
			Logger::GetInstance().Info( L"[GameStateDatabase::ExamineItemSlotFromPlayer] unknown player [{0}]", serial );
			return false;
		}
		Database::Info::ItemInfo* info = Database::DatabaseInstance().InfoCollections.Items.Retrieve(itemId);
		if (!info) {
			Logger::GetInstance().Info( L"[GameStateDatabase::ExamineItemSlotFromPlayer] unknown item name [{0}]", itemId);
			return false;
		}
		return logic.GetEventListener().ExamineItemSlotFromPlayer(player, info, cnt);
	}

	void GameStateDatabase::AcquirePvpItem(Player* player, uint32 itemHash)
	{
		logic.GetEventListener().AcquirePvpItem(player, itemHash);
	}

	void GameStateDatabase::UsePvpItem(Player* player, uint32 itemHash)
	{
		logic.GetEventListener().UsePvpItem(player, itemHash);
	}

	void GameStateDatabase::DropPvpItem(Player* player)
	{
		logic.GetEventListener().DropPvpItem(player);
	}

	uint8 GameStateDatabase::GetQuestState(Player* player, uint32 questHash)
	{
		return logic.GetEventListener().GetQuestState(player, questHash);
	}

	uint8 GameStateDatabase::SetQuestState(Player* player, uint32 questHash, uint8 newState)
	{
		return logic.GetEventListener().SetQuestState(player, questHash, newState);
	}

	uint32 GameStateDatabase::GetQuestParameter(Player* player, uint32 questHash, uint8 paramIndex)
	{
		return logic.GetEventListener().GetQuestParameter(player, questHash, paramIndex);
	}

	uint32 GameStateDatabase::SetQuestParameter(Player* player, uint32 questHash, uint8 paramIndex, uint32 newValue)
	{

		return logic.GetEventListener().SetQuestParameter(player, questHash, paramIndex, newValue);
	}
	void GameStateDatabase::CompletedQuest(Serial player, uint32 questHash)
	{
		stage.CompletedQuest(player, questHash);
	}
	void GameStateDatabase::AcceptedQuest(Serial player, uint32 questHash)
	{
		stage.AcceptedQuest(player, questHash);
	}
	void GameStateDatabase::NoticeHolidayEvent(uint32 eventId, bool start)
	{
		stage.NoticeHolidayEvent(eventId, start);
	}
	void GameStateDatabase::Revived(Player* player, bool bSelf)
	{
		std::cout << "GameStateDatabase::Revived" << std::endl;
		if ( !bSelf )
			totalReviveCount++;

		logic.GetEventListener().Revived(player, totalReviveCount); //Call to stageserver
		stage.CoinPlayerRevived(player->GetSerial());

		//Fast match �ߵ� ���θ� �˻��ؼ� �ߵ����� ����.
		if ( stage.IsFastMatchActivated() )
			FastMatch();

		
	}
	void GameStateDatabase::AnimationChanged(Serial s, uint32 ani, const float3& pos, const float3& dir, const float p)
	{
#ifdef _SINGLE
		if ( s >= 10000 && s < 100000 ) {
			NonPlayer* npc = RetrieveNPC(s);
			if ( npc && showNPCInfo == 1 ) {
				std::wstring msg = StringUtil::Format(L"%s\n%s\n{}\n%s\n{}", npc->GetNPCInfo()->TemplateName.c_str(), npc->GetNPCInfo()->Name.c_str(), npc->GetNPCInfo()->Class, npc->GetNPCInfo()->BasicStat.c_str(), ani);
				logic.GetEventListener().NPCDebugMessage( s, msg );
			}
		}
#endif
		logic.GetEventListener().AnimationChanged(s, ani, pos, dir, p);
	}
	void GameStateDatabase::CollisionStateChanged(Serial serial, bool col, const float3& position)
	{
		logic.GetEventListener().CollisionStateChanged(serial, col, position);
	}
	void GameStateDatabase::StatusChanged(Serial serial, float3 pos, float3 dir, float hp, float mp, Serial byWhom, uint32 byWhat, unsigned int sFlag, uint32 airComboCount)
	{
		logic.GetEventListener().StatusChanged(serial, pos, dir, hp, mp, byWhom, byWhat, sFlag, airComboCount);
	}
	void GameStateDatabase::XpGained(Player* player, XRated::Constants::ExpAcquiredType type, uint64 storyXp, int32 pvpXp, int32 warXp, Serial beKilledNpc)
	{
		logic.GetEventListener().XpGained(player, type, storyXp, pvpXp, warXp, beKilledNpc);
	}
	bool GameStateDatabase::AcquireItem(Player* player, uint32 itemId, Serial itemSerial, uint16 stackCount, InstanceEx instance)
	{
		return logic.GetEventListener().AcquireItem(player, itemId, itemSerial, stackCount, instance);
	}
	void GameStateDatabase::ItemEquipped(bool bEquipped, Player* player, uint32 itemId, Database::Enchant::EnchantBitfields fields, uint32 itemOldId)
	{
		logic.GetEventListener().ItemEquipped(bEquipped, player, itemId, fields, itemOldId);
	}
	void GameStateDatabase::EquipementSwapped(bool bequipped, Player* player, const std::vector< EquippedItem>& newEquipments )
	{
		logic.GetEventListener().EquipementSwapped(bequipped, player, newEquipments);
	}
	void GameStateDatabase::CashEquipmentSwapped(bool bequipped, Player* player, const std::vector< EquippedItem>& newEquipments )
	{
		logic.GetEventListener().CashEquipementSwapped(bequipped, player, newEquipments);
	}
	void GameStateDatabase::SkillPointChanged(Player* player, uint32 skillGroupHash, int level)
	{
		logic.GetEventListener().SkillPointChanged(player, skillGroupHash, level);
	}
	void GameStateDatabase::AddedSkillPointPlus(Player* player, uint16 currentSkillPoint, uint16 addedSkillPointPlus, uint16 storedSkillPoint)
	{
		logic.GetEventListener().AddedSkillPointPlus(player, currentSkillPoint, addedSkillPointPlus, storedSkillPoint);
	}
	void GameStateDatabase::Casted(Serial gameObjectSerial, uint32 skillGroupName, uint8 skillLevel)
	{
		logic.GetEventListener().Casted(gameObjectSerial, skillGroupName, skillLevel);
	}
	void GameStateDatabase::LevelUp(Player* player, XRated::Constants::ExpType expType, uint16 level, uint32 storedLevel)
	{
		logic.GetEventListener().LevelUp(player, expType, level, storedLevel);
	}
	void GameStateDatabase::SkillPointGot(Player* player, int skillPoint)
	{
		logic.GetEventListener().SkillPointGot(player, skillPoint);
	}
	void GameStateDatabase::StateChanged(Serial gameObjectSerial, uint32 id, int level, Serial byWhom)
	{
		logic.GetEventListener().StateChanged(gameObjectSerial, id, level, byWhom);
	}
	void GameStateDatabase::LocPlayers(bool lock)
	{
		logic.GetEventListener().LocPlayers(lock);
	}
	void GameStateDatabase::SetFloor( int floor )
	{
		logic.GetEventListener().SetFloor( floor );
	}
	void GameStateDatabase::CheckStateBundleForAchievement( uint32 stateBundleHash )
	{
		logic.GetEventListener().CheckStateBundleForAchievement( stateBundleHash );
	}
	void GameStateDatabase::DisplayTextEvent(Constants::DisplayTextType type, uint16 textId, float param)
	{
		logic.GetEventListener().DisplayTextEvent(type, textId, param);
	}
	void GameStateDatabase::NoticeTextEvent(Player* player, Constants::DisplayTextType type, uint16 textId, float param)
	{
		logic.GetEventListener().NoticeTextEvent(player, type, textId, param);
	}
	void GameStateDatabase::DisplayText(Constants::DisplayTextType type, const std::wstring& message)
	{
		logic.GetEventListener().DisplayText(type, message);
	}
	void GameStateDatabase::NoticeText(Player* player, Constants::DisplayTextType type, const std::wstring& message)
	{
		logic.GetEventListener().NoticeText(player, type, message);
	}
	void GameStateDatabase::DisplayTimer(Constants::DisplayTimerType timer, float seconds)
	{
		logic.GetEventListener().DisplayTimer(timer, seconds);
	}
	void GameStateDatabase::BroadcastEvent(int eventId, const std::wstring& param)
	{
		logic.GetEventListener().BroadcastEvent(eventId, param);

		///	PvP
		/////	���ӽ����ϸ� �ð��� �ʱ�ȭ ����� �Ѵ�. ���� �ʱ�ȭ�ϴ� �κ��� ��� PvP���� 445(ŰǮ�� �̺�Ʈ)�� �߻��ϸ� �ð��� 0����....
		if ( eventId == XRated::PvpStartEvent::UnbindKeyAndHide  )
		{
			if( ( stage.GetGameType() == Constants::GameTypes::DeathMatchGameType ) != 0 || ( stage.GetGameType() == Constants::GameTypes::LadderGameType ) != 0 )
			{
				tickCnt = 0;
				//totalReviveCount = 0;
				stage.ResetMissionPlayTime();
			}
			if(stage.GetGameType() == Constants::GameTypes::BattleGroundType)
			{
				//�����忡���� Ŭ�󿡼� 445�̺�Ʈ�� ���� ī��Ʈ �����Ѵ�.
				tickCnt = 0;
				//totalReviveCount = 0;
				stage.ResetMissionPlayTime();
				pvp.battleGround.killCount[0]=0;
				pvp.battleGround.killCount[1]=0;
				pvp.battleGround.limitPlayTime = logic.GetEventListener().GetBattleGroundInfo().LimitPlayTime;
			}
		}
	}
	void GameStateDatabase::NoticeEvent(Player* player, int eventId, const std::wstring& param)
	{
		logic.GetEventListener().NoticeEvent(player, eventId, param);
	}
	void GameStateDatabase::NoticeClientStageEvent( Player* player, int eventId, int eventParam )
	{
		logic.GetEventListener().NoticeClientStageEvent( player, eventId, eventParam );
	}
	void GameStateDatabase::TeamChange(Serial object, uint8 team)
	{
		logic.GetEventListener().TeamChange(object, team);
	}
	void GameStateDatabase::AddMinimapPing(int pingId, const float3 position, Constants::MiniMapPingType type)
	{
		logic.GetEventListener().AddMinimapPing(pingId, position, type);
	}
	void GameStateDatabase::NoticeAddMinimapPing(int pingId, const float3 position, Constants::MiniMapPingType type, Player* player)
	{
		logic.GetEventListener().NoticeAddMinimapPing(pingId, position, type, player);
	}
	void GameStateDatabase::RemoveMinimapPing(int pingId)
	{
		logic.GetEventListener().RemoveMinimapPing(pingId);
	}

	void GameStateDatabase::GoldGained(Player* player, Constants::GoldRewardAcquiredType type, uint32 amount)
	{
		logic.GetEventListener().GoldGained(player, type, amount);
	}

	void GameStateDatabase::GiveLicense(Player* player, uint32 licenseGroup, uint32 accessLevel)
	{
		logic.GetEventListener().GiveLicense(player, licenseGroup, accessLevel);
	}

	void GameStateDatabase::GiveCharacterLicense(Player* player, uint32 characterId)
	{
		logic.GetEventListener().GiveCharacterLicense(player, characterId);
	}

	void GameStateDatabase::ChangePlayerStatusLimit( Player* player, const std::vector<uint32>& limits )
	{
		logic.GetEventListener().ChangePlayerStatusLimit( player, limits );
	}

	void GameStateDatabase::ChangePlayerStatusRate( Player* player, const std::vector<float>& rates )
	{
		logic.GetEventListener().ChangePlayerStatusRate( player, rates );
	}

	void GameStateDatabase::PlayerDie(Serial objectSerial, Serial fromSerial)
	{
		logic.GetEventListener().PlayerDie(objectSerial, fromSerial);

		if ( stage.GetGameType() == Constants::GameTypes::BattleGroundType )
		{
			if ( logic.GetEventListener().GetBattleGroundInfo().GoalKillCount == 0 ) 
			{
				return; // �������̴� ������
			}
			else
			{
				PlayerMap::iterator iter = players.find( fromSerial ) ;
				if ( iter != players.end() )
				{
					++( pvp.battleGround.killCount [ iter->second->GetTeam() -1 ] );

					if ( pvp.battleGround.killCount [ iter->second->GetTeam() -1 ]  >= logic.GetEventListener().GetBattleGroundInfo().GoalKillCount  )
						stage.MissionClearFromLogic( true, iter->second->GetTeam() );
				}
			}
		}
	}

	void GameStateDatabase::PlayerSave(Serial objectSerial, Serial fromSerial)
	{
		logic.GetEventListener().PlayerSave(objectSerial, fromSerial);
	}

	void GameStateDatabase::StyplePointType(Serial objectSerial, XRated::Constants::StylePointType type)
	{
		logic.GetEventListener().StyplePointType(objectSerial, type);
	}

	uint32 GameStateDatabase::GetNextObjectSerial()
	{
		return ::InterlockedIncrement(reinterpret_cast<long*>(&npcSeed));
	}
	
	bool GameStateDatabase::Rebirthed(Player& player)
	{
		return logic.GetEventListener().Rebirthed(player);
	}

	void GameStateDatabase::RebirthRollBacked(Player& player)
	{
		return logic.GetEventListener().RebirthRollBacked(player);
	}

	void GameStateDatabase::NoticeStylePointState(XRated::StylePoint::State state)
	{
		logic.GetEventListener().NoticeStylePointStateToAllUser(state);
	}

	void GameStateDatabase::ChangeWeatherToNone()
	{
		logic.GetEventListener().ChangeWeatherToNone( 0.0f );
	}

	void GameStateDatabase::ChangeWeatherToClear()
	{
		logic.GetEventListener().ChangeWeatherToClear( 0.0f );
	}

	void GameStateDatabase::ChangeWeatherToRain()
	{
		logic.GetEventListener().ChangeWeatherToRain( 0.0f );
	}

	void GameStateDatabase::ChangeWeatherToSnow()
	{
		logic.GetEventListener().ChangeWeatherToSnow( 0.0f );
	}

	void GameStateDatabase::ChangeWeatherToAqua()
	{
		logic.GetEventListener().ChangeWeatherToAqua( 0.0f );
	}

	void GameStateDatabase::GambleGameEnd(const std::vector<uint8>& rankList)
	{
		logic.GetEventListener().GambleGameEnd(rankList);
	}

	void GameStateDatabase::GambleNextGame(Player* player, float seconds)
	{
		if ( player != NULL )
			logic.GetEventListener().GambleNextGame(player, seconds);
	}

	void GameStateDatabase::GambleNextGame(float seconds)
	{
		logic.GetEventListener().GambleNextGame(seconds);
	}

	int GameStateDatabase::GetSlimeRaceIntervalInMin() const
	{
		return logic.GetEventListener().GetSlimeRaceIntervalInMin();
	}

	/* Holiday Event */
	bool GameStateDatabase::IsHolidayEventTime(uint32 eventId)
	{
		return logic.GetEventListener().IsHolidayEventTime(eventId);
	}

	uint32 GameStateDatabase::GetHolidayEventNpcHash(uint32 eventId, uint32 oldNpcHash)
	{
		return Database::DatabaseInstance().InfoCollections.EventInfoManager.GetEventNPC(eventId, oldNpcHash);            
	}

	void GameStateDatabase::PetRevived( Logic::Player* player )
	{
		logic.GetEventListener().PetRevived( player );
	}

	void GameStateDatabase::PetChange(Logic::Player* player, uint32 petItemHash, XRated::GlobalSerial petItemSerial)
	{
		logic.GetEventListener().PetChange(player, petItemHash, petItemSerial);
	}

	void GameStateDatabase::PetInfoUpdated( Player* player, const XRated::Pet& pet, const XRated::Serial petSerial, const std::wstring& petExpression )
	{
		return logic.GetEventListener().PetInfoUpdated( player, pet, petSerial, petExpression );
	}
	void GameStateDatabase::PetDeleted(Logic::Player* player, XRated::GlobalSerial petSerial)
	{
		return logic.GetEventListener().PetDeleted( player, petSerial );
	}

	void GameStateDatabase::PetCaredBySchool( Player* player, const XRated::PetCaredBySchool& caredPet, const XRated::Pet& pet )
	{
		return logic.GetEventListener().PetCaredBySchool( player, caredPet, pet );
	}

	void GameStateDatabase::PetTakenOutFromSchool( Player* player, const XRated::PetCaredBySchool& caredPet, const XRated::Pet& pet )
	{
		return logic.GetEventListener().PetTakenOutFromSchool( player, caredPet, pet );
	}


#ifdef _SINGLE
	void GameStateDatabase::NPCDebugMessage(Serial serial, std::wstring msg)
	{
		logic.GetEventListener().NPCDebugMessage(serial, msg);
	}

	void GameStateDatabase::ChangeNPCDebugMessage(int type)
	{
		NPCMap::iterator npc=npcs.begin();
		NonPlayer* currentnpc;
		std::map<Serial, float> l;

		switch (type) {
		case 0 :
			for ( ;npc!=npcs.end(); ++npc) {
				currentnpc = (*npc).second;
				NPCDebugMessage(currentnpc->GetSerial(), L"");
			}
			showNPCInfo = 0;
			break;
		case 1 :
			showNPCInfo = 1;
			break;
		case 2 :
			showNPCInfo = 2;
			{
				for ( ;npc!=npcs.end(); ++npc) {
					currentnpc = (*npc).second;
					currentnpc->GetHateList(l);
					std::wstring msg;

					for( std::map<Serial, float>::iterator i = l.begin() ; i != l.end() ; ++i) {
						Object* obj = Retrieve((*i).first);
						if ( !obj ) continue;
						msg += StringUtil::Format(L"%s %f\n", obj->GetName().c_str(), (*i).second);
					}
					NPCDebugMessage(currentnpc->GetSerial(), msg);
				}
			}
			break;
		case 3 :
			showNPCInfo = 3;
			break;
		}
	}

	void GameStateDatabase::Reload()
	{
		npcSeed = NPCSerialBase;
		projectileSeed = ProjectileSerialBase;
		for (ObjectMap::iterator pos = objects.begin(); pos != objects.end() ; ++pos)
			delete (*pos).second;
		objects.clear();
		for (NPCMap::iterator pos = npcs.begin(); pos != npcs.end() ; ++pos)
			delete (*pos).second;
		npcs.clear();

		itemManager.Init();
		projectileManager.Init();
		triggerSystem.Init();

		PlayerMap::iterator iter = players.begin();
		for(; iter != players.end() ; ++iter) {
			delete (*iter).second;
		}
		players.clear();

		smallSector.Clear();
		bigSector.Clear();

		stage.SetDatabase(this);
		stage.Init();

		std::wstring strAccessLevel;
		std::wstring strStageGroup;
		Config::ConfigInstance().Get(L"single", L"stageGroup", strStageGroup, L"NormalScenario");
		uint32 stageGroupHash = StringUtil::Hash( strStageGroup.c_str() );
		Config::ConfigInstance().Get(L"single", L"accessLevel", strAccessLevel, L"0");
		uint32 accessLevel = StringUtil::To<uint32>( strAccessLevel.c_str() );
		Info::StageGroup* stageGroup = Lunia::XRated::Database::DatabaseInstance().InfoCollections.StageGroups.Retrieve(stageGroupHash);

		stage.LoadStage(stageGroup, stageGroup->StageHashes[accessLevel],0);
	}

#endif
} } }

