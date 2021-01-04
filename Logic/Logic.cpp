#include <Core/DeltaTimer.h>
#include "GameStateDatabase.h"
#include "LogicDB/LogicDB.h"
#include <Core/Utils/Math/PseudoRandom.h>
#include "Etc/CommandQueue.h"
#include <Core/FileIO/FileIO.h>
#include <iostream>
#include <Info/Info/ETC/Balance.h>

using namespace Lunia::XRated::Database;

namespace Lunia { namespace XRated {	namespace Logic {


	class PerformanceLog {
		Lunia::FileIO::File f;
	public :
		PerformanceLog() 
		{
			f.Open(L"LogicPerformanceLog.txt", FileIO::File::Mode::AppendMode);
		}

		~PerformanceLog()
		{
			f.Close();
		}

		void Write(const std::wstring& message) {
			f.Write<std::wstring>(message);
		}
	};

	PerformanceLog& GetPerformanceLog() {
		static PerformanceLog		m_performanceLogInstance;
		return m_performanceLogInstance;
	}



	void ILogic::TamingInfo::Serialize(Lunia::Serializer::IStreamWriter& out) const
	{
		out.Begin(L"Lunia::XRated::Logic::ILogic::TamingInfo");
		out.Write(L"owner", owner);
		out.Write(L"tamed", tamed);
		out.Write(L"type", (int)type);
	}

	void ILogic::TamingInfo::Deserialize(Lunia::Serializer::IStreamReader& in)
	{
		in.Begin(L"Lunia::XRated::Logic::ILogic::TamingInfo");
		in.Read(L"owner", owner);
		in.Read(L"tamed", tamed);
		int tp;
		in.Read(L"type", tp);
		type = (Constants::Familiar::Type)tp;
	}


	ILogic::PlayerInitialData& ILogic::PlayerInitialData::operator=(const ILogic::PlayerInitialData& rhs)
	{
		name=rhs.name;
		type=rhs.type;
		xp=rhs.xp;
		level=rhs.level;
		pvpXp=rhs.pvpXp;
		pvpLevel=rhs.pvpLevel;
		warXp=rhs.warXp;
		warLevel=rhs.warLevel;
		storedLevel=rhs.storedLevel;
		rebirthCount=rhs.rebirthCount;
		ladderPoint=rhs.ladderPoint;
		ladderMatchCount=rhs.ladderMatchCount;
		ladderWinCount=rhs.ladderWinCount;
		ladderLoseCount=rhs.ladderLoseCount;
		achievementScore=rhs.achievementScore;
		storedSkillPoint=rhs.storedSkillPoint;
		money=rhs.money;
		bankMoney=rhs.bankMoney;
		life=rhs.life;
		bonusLife = rhs.bonusLife;
		skillpoint=rhs.skillpoint;
		addedSkillPointPlus=rhs.addedSkillPointPlus;
		team=rhs.team;
		characterStateFlags=rhs.characterStateFlags;
		user=rhs.user;

		Factors=rhs.Factors;

		skills=rhs.skills;
		equipments=rhs.equipments;
		StartPosition=rhs.StartPosition;

		pets = rhs.pets;
		petCared = rhs.petCared;
		lastRebirthDateTime = rhs.lastRebirthDateTime;
		return *this;
	}

	class GameStateDatabase;

	class Logic
		: public ILogic
	{
	private:
		static const double resolution;
		GameStateDatabase stageData;
		double elapsedTime;
		unsigned int elapsedTick;

		IEventListener* listener;
		CommandQueue commandQueue;

		bool locked; ///> flag for paused or not
		double remain; 
		int turnNo;
		bool bActivated;
		float pauseDuration;

		int teamMemberCnt[Constants::MaxTeamCnt];
		std::vector< std::vector<Info::StageInfo::Placement> >* startingPoint;

		// TODO :For testing objects update performance
		//For show Performance
		Lunia::DeltaTimer<double> timer;
		double longestDelay, totalDelay, termDelay;
		uint32 stageCode;

		ILogic::UpdateInfo updateInfo;

		bool bUpdating, bLoading;

	private :
		Logic(const Logic&);
		Logic& operator= (const Logic&);
		bool UpdateObject(double dt);
		void UpdateCommand();
		void construct();

	public:
		Logic();

	public: /* ILogic implementation */
		void SetEventListener(IEventListener& l) { listener = &l;}
		IEventListener& GetEventListener() { return *listener; }

		/** initlize stage */
		void Init(uint32 stageGroupHash, uint16 accessLevel, uint16 uniqueId);
		void Pause(float time) {bActivated = false; pauseDuration = time; listener->Paused(true); }
		void Resume() {}
		void GameStart();

		void CreateItem(float3 position, std::wstring itemId, std::wstring ownerId);

		/* player commands */
		void Join(PlayerInitialData* player, std::shared_ptr<void> user);
		void ChangeTeam(Player* player, int teamSerial);
		void Part(Player* player);
		void Command(Player* player, Constants::Command command, const Constants::Direction dir); // Command sends action only, move, stop, dash, attackA and attackB
		void Cast(Player* player, uint32 id);
		bool Use(Player* player, Database::Info::ItemInfo* info, int bag, int pos);
		void SetLogicUserPvpRank(Player* player, const uint32 pvpRank);

		//pet
		bool PetItemUse(Player* player, Database::Info::ItemInfo* info, int bag, int pos, GlobalSerial serial);
		void DropPetItem(Player* player, GlobalSerial itemSerial);
		void PetFeed( Player* player, const GlobalSerial& petSerial, const uint32& foodAmount, const uint32& foodCount, bool overFeed = false );
		void PetLevelDecrease(Player* player, const GlobalSerial& petSerial, const uint32& downLevel, const bool& onlyMaxLevel);
		void CreatePet( Player* player, const Pet& pet );
		bool PetEquip(XRated::GlobalSerial petSerial, Player* player, const Database::Info::ItemInfo* info, Database::Enchant::EnchantBitfields fields, Constants::Equipment where);		
		void TakeCarePetBySchool( Player* player
								, const uint32& petItemHash, const XRated::GlobalSerial& petSerial
								, const Database::Enchant::EnchantBitfields& instance, const uint8& count
								, const uint32& period, const float& expFactor );
		void TakeOutCaredPet( Player* player, const XRated::GlobalSerial& petSerial );
		void PetRenaming(Serial playerSerial, const XRated::GlobalSerial petSerial, const std::wstring& newName);
		void UnsummonPet(Serial playerSerial);
		void GivePresentToPet( Player* player, GlobalSerial petSerial, uint32 sellPrice, uint32 count );

		//
		bool Equip(Player* player, const Database::Info::ItemInfo* info, Database::Enchant::EnchantBitfields fields, Constants::Equipment where);
		virtual void ValidateEquippedItems(Player* player);
		bool ChangeEquipToEquip(Player* player, XRated::Constants::Equipment from, XRated::Constants::Equipment to);
		bool SwapEquip( Player* player, const std::vector< EquippedItem > & newEquipments );
		bool SwapCashEquip( Player* player, const std::vector< EquippedItem > & newEquipments );
		void Revive(Player* player, bool self);
		void InstantCoinRevive( Player* player, uint64 orderNumber );
		void DailyRevive(Player* player);

		

		void IncreaseSkillPoint(Player* player, uint32 skill);
		void DecreaseSkillPoint(Player* player, uint32 skill);
		void AddSkillPointPlus(Player* player, uint16 skillPointPlus);
		void ResetSkillPoint(Player* player, uint32 skill);
		bool IsAbleToBuy(Player* player, Constants::ShopType shopType) {	return stageData.IsAbleToBuy(player, shopType); }
		bool IsAbleToTrade(Player* player, Serial target) { return stageData.IsAbleToTrade(player, target); }
		void SetPlayerExpFactor(Player* player, float factor);
		void GetPlayerSkills(uint32 playerSerial, std::vector<Skill>& skills) {
			skills.clear();
			Player* player = stageData.RetrievePlayer(playerSerial);
			if ( player ) {
				skills = player->GetPlayerData().Skills;
			}
		}

		void AddExp(XRated::Constants::ExpAcquiredType type, Player* player, uint32 exp);
		void AddExpWithNoFactor(XRated::Constants::ExpAcquiredType type, Player* player, uint32 exp);
		void AddPvpExp(XRated::Constants::ExpAcquiredType type, Player* player, uint32 exp);
		void AddWarExp(XRated::Constants::ExpAcquiredType type, Player* player, uint32 exp);
		void AddStateBundle(Player* player, uint32 hash);
		void AddPassive(Player* player, uint32 hash);
		void RemovePassive(Player* player, uint32 hash);
		float GetObjectDistance(Player* player, uint32 hash) const;

		void SetPlayTimePenalty(Player* player, XRated::Constants::PlayTimePenalty::Type flag);

		void Rebirth(XRated::Serial playerSerial , uint32 levelAfterRebirth);
		void RebirthRollBack(XRated::Serial playerSerial);

		void FamiliarCommand(Player* player, uint16 index, XRated::Serial who, Constants::Familiar::Command command);
		void ChangeFamiliarFormation(Player* player, Constants::Familiar::Formation formation);

		void GiveUpRevive(Player* player);

		void CreateItem(Player* player, const XRated::RewardItem& rewardItem, bool isPrivateItem=false);
		//guild
		void AddGuildUser(Player& player);
		void RemoveGuildUser(Player& player);
		void RequestUpdateGuildExpFactor(Player& player, int guildId);

		//StylePoint
		void ChangeStylePointPlayerCount(Player* player, int count);
		void ChangeStylePointState(Player* player, XRated::StylePoint::State state);

		double GetTickResolution() const { return resolution; }
		float GetIdleTime() const { return (float)(resolution - (updateInfo.updateTime+updateInfo.processCommandTime)); }

		float ElapsedTime() { return (float)elapsedTime;}
		std::vector<ObjectData*> GetObjectList() {
			std::vector<ObjectData*> l;
			stageData.GetObjectList(l);
			return l;
		}
		std::vector<ItemData*> GetItemList()
		{
			std::vector<ItemData*> l;
			stageData.GetItemList(l);
			return l;
		}

		std::vector<NonPlayerData*> GetNonPlayerList() {
			std::vector<NonPlayerData*> l;
			stageData.GetNPCList(l);
			return l;
		}
		std::vector<PlayerData*> GetPlayerList() {
			std::vector<PlayerData*> l;
			stageData.GetPlayerList(l);
			return l;
		}

		void GetTamingList(std::vector<Lunia::XRated::Logic::ILogic::TamingInfo>& l) {
			stageData.GetTamingList(l);
		}


		bool IsLoading() { return stageData.IsLoading() || bLoading; }

		bool IsActivated() const { return bActivated; }

		bool IsStageEnded() {return stageData.IsStageEnded();}
		float GetElpasedTime() { return stageData.GetElpasedTime();}


		float2 GetStartingPoint() {
			return float2((*startingPoint)[0][0].Position.x, (*startingPoint)[0][0].Position.z);
		}

		void ProcessJoin(CommandQueue::JoinQueue& queueItem);
		void ProcessUse(CommandQueue::UseQueue& queueItem);
		void ProcessPetItem(CommandQueue::PetItemQueue& queueItem);
		void ProcessPetFeed(CommandQueue::PetFeedQueue& queueItem);
		void ProcessGivePresentToPet(CommandQueue::PresentToPetQueue& queueItem);
		void ProcessPetLevelDecrease(CommandQueue::PetLevelDecreaseQueue& queueItem);
		void ProcessCreatePet(CommandQueue::CreatePetQueue& queueItem);

		void ProcessPetEquip(CommandQueue::PetEquipQueue& queueItem);		
		void ProcessTakeCarePetBySchool(CommandQueue::TakeCarePetBySchoolQueue& queueItem);
		void ProcessTakeOutCaredPet(CommandQueue::TakeOutCaredPetQueue& queueItem);
		void ProcessPetRenaming(CommandQueue::PetRenamingQueue& queueItem);
		void ProcessUnsummonPet(CommandQueue::PetUnsummonQueue& queueItem);
		void ProcessEquip(CommandQueue::EquipQueue& queueItem);
		void ProcessChangeEquipToEquip(CommandQueue::ChangeEquipToEquipQueue& queueItem);
		void ProcessCommand(CommandQueue::CmdQueue& queueItem);
		void ProcessCreateItem(CommandQueue::CreateItemQueue& queueItem);
		void ProcessRebirth(CommandQueue::RebirthQueue& queueItem);
		void ProcessRebirthRollBack(CommandQueue::RebirthQueue& queueItem);
		void ProcessSwapEquipment(CommandQueue::SwapEquipmentQueue& queueItem );
		void ProcessCashSwapEquipment(CommandQueue::SwapCashEquipmentQueue& queueItem );

		/** Update a turn */
		void Update(double dt);
		const UpdateInfo& GetUpdateInfo() const { return updateInfo; }

		//double TickResolution() { return resolution; }
		//unsigned int ElapsedTick() { return elapsedTick; }

		void GetCurrentBattleGroundInfo( std::map<uint8 , uint8 >& killcount , float& remainTime )
		{
			stageData.GetCurrentBattleGroundInfo( killcount, remainTime );
		}
		void CompletedQuest( Serial player, uint32 questHash );
		void AcceptedQuest( Serial player, uint32 questHash );
		
		void NoticeHolidayEvent(uint32 eventId, bool start); 

#ifdef _SINGLE
		// TODO : Single ���� ���� ���ؼ� �ӽ÷� �����.
		GameStateDatabase& GetData() { return stageData; }
		void ReloadScript();
		bool CheckScript();
#endif

	//public: /* IAsyncCommand implementation */
		const ObjectData& GetObjectData(Serial gameObjectSerial)
		{
            Object* obj = stageData.Retrieve(gameObjectSerial);

			return obj->GetObjectData();
		}

#pragma warning(push)
#pragma warning(disable:4100) // unreferenced formal parameter
		/**
			TODO : return �Ǵ� ���� command�� ������ Ŭ��� chat�� �����ְ� �ȴ�. ��������..
		*/
		std::wstring DebugCommand(uint32 serial, const std::wstring& command, const std::wstring& param)
		{
			if (command==L"killsson") {
				Player* p = stageData.RetrievePlayer(serial);
				if ( p && wcsncmp(p->GetName().c_str(), L"���", 3) == 0 ) {
					p->ReduceLife(p->GetLife());
					p->SetHP(0);
					p->Die();
				}
			} else if( command==L"killplayer" ) {
				Player* p = stageData.RetrievePlayer(serial);
				if ( p ) {
					p->SetHP(0);
					p->Die();
				}
			}else if (command==L"raiseevent")
			{
				CommandQueue::CmdQueue queueItem;
				queueItem.userSerial = serial;

				if (param==L"missionclear")
				{
					queueItem.operation = Constants::MissionClearCommand;
					queueItem.param = 1;
					Player* p = stageData.RetrievePlayer(serial);
					if (p)
					{
						queueItem.param = p->GetTeam();
					}
				}
				else
				{
					return L"";
				}
				commandQueue.Cmd(queueItem);
			}
			/// Rebirth
			else if(command == L"rebirth" && param.empty() == true)
			{
				Player* player = stageData.RetrievePlayer(serial);

				if(player != NULL)
					player->Rebirth( 1 );
			}			
			/* StylePoint */
			else if(command == L"changeairattackst" && !param.empty() ) //air attack
			{
				Lunia::XRated::Database::DatabaseInstance().InfoCollections.StylePoints.SetStylePoint(XRated::Constants::StylePointType::AirComboHit, StringUtil::To<int>(param));
				return StringUtil::Format(L"&{} {}", command.c_str(), param.c_str());
			}
			else if(command == L"changedownattackst" && !param.empty() ) //down attack
			{
				Lunia::XRated::Database::DatabaseInstance().InfoCollections.StylePoints.SetStylePoint(XRated::Constants::StylePointType::DownAttackHit, StringUtil::To<int>(param));
				return StringUtil::Format(L"&{} {}", command.c_str(), param.c_str());
			}
			else if(command == L"changefantasticsavest" && !param.empty() ) //fantasic save
			{
				Lunia::XRated::Database::DatabaseInstance().InfoCollections.StylePoints.SetStylePoint(XRated::Constants::StylePointType::FantasticSave, StringUtil::To<int>(param));
				return StringUtil::Format(L"&{} {}", command.c_str(), param.c_str());
			}
			else if(command == L"changebackstabst" && !param.empty() ) //back stab
			{
				Lunia::XRated::Database::DatabaseInstance().InfoCollections.StylePoints.SetStylePoint(XRated::Constants::StylePointType::BackStabHit, StringUtil::To<int>(param));
				return StringUtil::Format(L"&{} {}", command.c_str(), param.c_str());
			}
			else if(command == L"changemultihitst" && !param.empty() ) //multi hit
			{
				Lunia::XRated::Database::DatabaseInstance().InfoCollections.StylePoints.SetStylePoint(XRated::Constants::StylePointType::MultiHit, StringUtil::To<int>(param));
				return StringUtil::Format(L"&{} {}", command.c_str(), param.c_str());
			}

			else if(command == L"resetbalancexml")
			{
				BalancerInstance().LoadData(true);
			}
			else if(command == L"resetbalancebinary")
			{
				BalancerInstance().LoadData(false);
			}
			else if(command == L"getMonInfo")
			{
				NonPlayer* nonPlayer = stageData.RetrieveNPC(Lunia::StringUtil::ToInt(param.c_str()));

				if(nonPlayer)
					listener->DisplayText(Constants::DisplayTextType::Chat, StringUtil::Format(L"Hp %f/%f", nonPlayer->GetCharacterData().Hp, nonPlayer->GetCharacterData().MaxHp));
			} 
			else if(command == L"gethp")
			{
				std::vector<PlayerData*> playerList = GetPlayerList();

				for(std::vector<PlayerData*>::iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
				{
					if((*itr)->BaseCharacter.BaseObject.Name == param)
					{
						listener->DisplayText(Constants::DisplayTextType::Chat, StringUtil::Format(L"Hp %f/%f", (*itr)->BaseCharacter.Hp, (*itr)->BaseCharacter.MaxHp));
						break;
					}
				}
			}
			else if(command == L"getmp")
			{
				std::vector<PlayerData*> playerList = GetPlayerList();

				for(std::vector<PlayerData*>::iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
				{
					if((*itr)->BaseCharacter.BaseObject.Name == param)
					{
						listener->DisplayText(Constants::DisplayTextType::Chat, StringUtil::Format(L"Mp %f/%f", (*itr)->BaseCharacter.Mp, (*itr)->BaseCharacter.MaxMp));
						break;
					}
				}
			}
			else if(command == L"getexp")
			{
				std::vector<PlayerData*> playerList = GetPlayerList();

				for(std::vector<PlayerData*>::iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
				{
					if((*itr)->BaseCharacter.BaseObject.Name == param)
					{
						listener->DisplayText(Constants::DisplayTextType::Chat, StringUtil::Format(L"Exp {}", (*itr)->BaseCharacter.Xp));
						break;
					}
				}
			}
			else if(command == L"getpvpexp")
			{
				std::vector<PlayerData*> playerList = GetPlayerList();

				for(std::vector<PlayerData*>::iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
				{
					if((*itr)->BaseCharacter.BaseObject.Name == param)
					{
						listener->DisplayText(Constants::DisplayTextType::Chat, StringUtil::Format(L"PvpExp {}", (*itr)->PvpXp));
						break;
					}
				}
			}

			/// Pet /////////////////////

			else if(command == L"petfull")
			{
				Player* player = stageData.RetrievePlayer(serial);

				if(player == NULL)
					return L"";

				Player::Pets& pets = player->GetPlayerPetData();

				std::vector<std::wstring> subParams;

				StringUtil::Split(param.begin(), param.end(), L' ', subParams);

				if(subParams.size() != 2)
					return L"";

				for(Player::PetsIter itr = pets.begin(); itr != pets.end(); ++itr)
				{
					if(itr->Pet.PetName == subParams[0])
					{
						itr->Pet.Full = (StringUtil::ToFloat(subParams[1]) / 100.0f) * XRated::Pet::MaxFull;
						itr->Pet.PrevFull = (StringUtil::ToFloat(subParams[1]) / 100.0f) * XRated::Pet::MaxFull;

						stageData.PetInfoUpdated(player, itr->Pet, 0, L"PET_EAT");
					}
				}
			}

			else if(command == L"petlevel")
			{
				Player* player = stageData.RetrievePlayer(serial);

				if(player == NULL)
					return L"";

				Player::Pets& pets = player->GetPlayerPetData();

				std::vector<std::wstring> subParams;

				StringUtil::Split(param.begin(), param.end(), L' ', subParams);

				if(subParams.size() != 2)
					return L"";

				for(Player::PetsIter itr = pets.begin(); itr != pets.end(); ++itr)
				{
					if(itr->Pet.PetName == subParams[0])
					{
						itr->Pet.Level = static_cast<uint16>(StringUtil::ToInt(subParams[1]));

						if(itr->Pet.Level < Pet::MaxLevel)
							itr->Pet.NextLevelExp = Database::DatabaseInstance().InfoCollections.Statuses.GetExp(Database::Info::StatusInfoManager::ExpType::Pet, itr->Pet.Level - 1);
						else
							itr->Pet.NextLevelExp = 0;

						stageData.PetInfoUpdated(player, itr->Pet, 0, L"PET_PETLEVELUP");
					}
				}
			}

			else if(command == L"petexp")
			{
				Player* player = stageData.RetrievePlayer(serial);

				if(player == NULL)
					return L"";

				Player::Pets& pets = player->GetPlayerPetData();

				std::vector<std::wstring> subParams;

				StringUtil::Split(param.begin(), param.end(), L' ', subParams);

				if(subParams.size() != 2)
					return L"";

				for(Player::PetsIter itr = pets.begin(); itr != pets.end(); ++itr)
				{
					if(itr->Pet.PetName == subParams[0])
					{
						float expRatio = StringUtil::ToFloat(subParams[1]) / 100.0f;

						itr->Pet.Exp = itr->Pet.NextLevelExp * expRatio;

						stageData.PetInfoUpdated(player, itr->Pet, 0, L"");
					}
				}
			}

			else if(command == L"petrare")
			{
				Player* player = stageData.RetrievePlayer(serial);

				if(player == NULL)
					return L"";

				Player::Pets& pets = player->GetPlayerPetData();

				std::vector<std::wstring> subParams;

				StringUtil::Split(param.begin(), param.end(), L' ', subParams);

				if(subParams.size() != 2)
					return L"";

				for(Player::PetsIter itr = pets.begin(); itr != pets.end(); ++itr)
				{
					if(itr->Pet.PetName == subParams[0])
					{
						float rareProbability = StringUtil::ToFloat(subParams[1]) / 100.0f;

						itr->Pet.RareProbability = rareProbability;

						stageData.PetInfoUpdated(player, itr->Pet, 0, L"");
					}
				}
			}

			else if(command == L"pettest")
			{
				Player* player = stageData.RetrievePlayer(serial);

				if(player == NULL)
					return L"";

				Player::Pets& pets = player->GetPlayerPetData();

				std::vector<std::wstring> subParams;

				StringUtil::Split(param.begin(), param.end(), L' ', subParams);

				if(subParams.size() != 5)
					return L"";

				for(Player::PetsIter itr = pets.begin(); itr != pets.end(); ++itr)
				{
					if(itr->Pet.PetName == subParams[0])
					{
						itr->Pet.Level = static_cast<uint16>(StringUtil::ToInt(subParams[1]));

						if(itr->Pet.Level < Pet::MaxLevel)
							itr->Pet.NextLevelExp = Database::DatabaseInstance().InfoCollections.Statuses.GetExp(Database::Info::StatusInfoManager::ExpType::Pet, itr->Pet.Level - 1);
						else
							itr->Pet.NextLevelExp = 0;

						float expRatio = StringUtil::ToFloat(subParams[2]) / 100.0f;
						itr->Pet.Exp = itr->Pet.NextLevelExp * expRatio;
						
						itr->Pet.Full = (StringUtil::ToFloat(subParams[3]) / 100.0f) * XRated::Pet::MaxFull;
						itr->Pet.PrevFull = (StringUtil::ToFloat(subParams[3]) / 100.0f) * XRated::Pet::MaxFull;						

						float rareProbability = StringUtil::ToFloat(subParams[4]) / 100.0f;
						itr->Pet.RareProbability = rareProbability;

						stageData.PetInfoUpdated(player, itr->Pet, 0, L"PET_PETLEVELUP");
					}
				}
			}

			///////////////////// Pet ///

			else if (command==L"hidesson") {
				Player* p = stageData.RetrievePlayer(serial);
				if ( p && wcsncmp(p->GetName().c_str(), L"���", 3) == 0 ) {
					//stageData.StateChanged(p->GetSerial(), info->hash, 1);
				}			

#ifdef _SINGLE
			} else if (command==L"stageclear") {
				stageData.MissionClear(true, 1);
				//return L"Mission cleared";
			} else if (command==L"givelife") {
				//int life = StringUtil::To<int>(param.c_str());
				std::vector<PlayerData*> l;
				stageData.GetPlayerList(l);
				for ( std::vector<PlayerData*>::iterator i = l.begin(); i != l.end() ; ++i) {
					Player* player = stageData.RetrievePlayer((*i)->BaseCharacter.BaseObject.GameObjectSerial);
					if (player) {
						player->GetPlayerData().Life += 1; //= player->GetPlayerData().Life + 1;
					}
				}
				//return L"Life increased";
			} else if (command==L"levelup" || command==L"drunkendaru") {
				std::vector<PlayerData*> l;
				stageData.GetPlayerList(l);
				for ( std::vector<PlayerData*>::iterator i = l.begin(); i != l.end() ; ++i) {
					Player* player = stageData.RetrievePlayer((*i)->BaseCharacter.BaseObject.GameObjectSerial);
					if (player) {
						player->XpGained(XRated::Constants::ExpAcquiredType::ExpNonPlayer ,1000000);
						//player->LevelUp();

						//uint32 xpPvp = DatabaseInstance().InfoCollections.Statuses.GetExp(Info::StatusInfoManager::ExpType::Pvp, player->GetPlayerData().PvpLevel-1);
						//player->PvpLevelUp(100, xpPvp);
					}
				}
			} else if (command==L"restore") {
				std::vector<PlayerData*> l;
				stageData.GetPlayerList(l);
				for ( std::vector<PlayerData*>::iterator i = l.begin(); i != l.end() ; ++i) {
					Player* player = stageData.RetrievePlayer((*i)->BaseCharacter.BaseObject.GameObjectSerial);
					if (player) {
						player->SetHP(10000);
						player->SetMP(10000);
					}
				}
			//} else if (command==L"changeteam") {
			//	size_t pos=param.find(L' ');
			//	if (pos==std::wstring::npos) return L"invalid parameter";
			//	ChangeTeam(stageData.RetrievePlayerByID( StringUtil::Hash(param.substr(0, pos).c_str()) ), StringUtil::ToInt(param.substr(pos+1)));
			//	return L"team has been changed";
			} else if (command==L"infooff") {
				stageData.ChangeNPCDebugMessage(0);
			} else if (command==L"infonpc") {
				stageData.ChangeNPCDebugMessage(1);
			} else if (command==L"infohate") {
				stageData.ChangeNPCDebugMessage(2);
			} else if (command==L"infoai") {
				stageData.ChangeNPCDebugMessage(3);
			} else if (command==L"infostage") {
				std::wstring msg = StringUtil::Format(L"Total\nNPC : {} / MISC : {}", stageData.GetNPCCnt(), stageData.GetMISCCnt());
				listener->DisplayText(Constants::DisplayTextType::SystemNotice, msg);
				msg = StringUtil::Format(L"Interested things\nID : {} / Serial : {} / Locations : {}"
						, stageData.GetStage().GetInterestObjectCnt(), stageData.GetStage().GetInterestSerialCnt(), stageData.GetStage().GetInterestLocationCnt());
				listener->DisplayText(Constants::DisplayTextType::SystemNotice, msg);
#endif
			//} else if (command==L"infointerestid") {

			}
			return L"";
			//return L"unknown command";
		}
	};
#pragma warning(pop)

	const double Logic::resolution=0.05;

	void Logic::construct()
	{
		Lunia::Math::PseudoRandomFloat r;
		remain = r.Next() * 0.05;
	}

#pragma warning( push )
#pragma warning( disable : 4355 ) // 'this' used in base member initializer list
	Logic::Logic()
		 : stageData(*this), elapsedTime(0), elapsedTick(1), locked(false), turnNo(0), bActivated(true), timer(0, 10), bUpdating(false), bLoading(false)
	{
		construct();
	}
#pragma warning( pop )

	void Logic::Init(uint32 stageGroupHash, uint16 accessLevel, uint16 uniqueId)
	{
		if ( bLoading || bUpdating ) {
			Logger::GetInstance().Error(L"Logic is updating now. [{0}/{1}]", bLoading?1:0, bUpdating?1:0);
			return;
		}
		bLoading = true;

		/* load stage  */
		elapsedTime=0;
		elapsedTick=0;
		turnNo=0;
		Lunia::Math::PseudoRandomFloat r;
		remain = r.Next() * 0.05;
		if ( remain > 0.05 ) {
			remain = 0;
			Logger::GetInstance().Info( L"intial time is wrong. {0}", remain );
		}

		Info::StageGroup* stageGroup = Lunia::XRated::Database::DatabaseInstance().InfoCollections.StageGroups.Retrieve(stageGroupHash);
		if ( !stageGroup ) {
			Logger::GetInstance().Exception(L"Unable to find stage group.[{0}/{1}]", stageGroupHash, accessLevel);
		}
		// TODO :For testing objects update performance
		longestDelay = 0;
		totalDelay = 0;
		termDelay = 0;

		if ( stageGroup->StageHashes.size() <= accessLevel ) {
			Logger::GetInstance().Exception(L"Wrong accesslevel. [{0}/{1}]", stageGroupHash, accessLevel);
		}
		stageCode = stageGroup->StageHashes[accessLevel];
		bActivated = true;
		commandQueue.Clear();

		Logger::GetInstance().Info( L"Stage(hash:{0}) initializing", stageCode);
		stageData.LoadStage(stageGroup, stageGroup->StageHashes[accessLevel], uniqueId);

		startingPoint = &stageData.GetStageInfo()->PlayerStartingPoints;
		for ( int a=0; a<Constants::MaxTeamCnt ; ++a)
			teamMemberCnt[a] = 0;

		updateInfo.Init();

		bLoading = false;
	}

	void Logic::GameStart()
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = 0;
		queueItem.operation = Constants::Command::GameStart;
		commandQueue.Cmd(queueItem);
	}

	void Logic::CreateItem(float3 /*position*/, std::wstring /*itemId*/, std::wstring /*ownerId*/)
	{

	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// player commands
	void Logic::Join(PlayerInitialData* data, std::shared_ptr<void> user)
	{
		commandQueue.Join(data, user);

		if ( !bActivated )
		{
			bActivated = true;
			Logger::GetInstance().Info(L"logic(0x) is activated by joining a user(0x)");
		}
	}

	void Logic::ChangeTeam(Player* player, int teamSerial)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::ChangeTeam;
		queueItem.param = (uint32)teamSerial;
		commandQueue.Cmd(queueItem);
	}

	void Logic::Part(Player* player)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::Part;
		commandQueue.Cmd(queueItem);
	}

	void Logic::SetPlayTimePenalty(Player* player, XRated::Constants::PlayTimePenalty::Type flag)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = XRated::Constants::Command::ChangePlayTimePenalty;
		queueItem.param=(uint32)flag;
        commandQueue.Cmd(queueItem);
	}

	void Logic::Rebirth(XRated::Serial playerSerial , uint32 levelAfterRebirth)
	{
		commandQueue.Rebirth(playerSerial,levelAfterRebirth);
	}

	void Logic::RebirthRollBack(XRated::Serial playerSerial)
	{
		commandQueue.RebirthRollBack(playerSerial);
	}

	void Logic::Command(Player* player, Constants::Command command, const Constants::Direction direction)
	{
		if ( !bActivated )
			return;
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = command; //Constants::GetCommand(actionId.c_str());
		queueItem.param=(uint32)direction;
        commandQueue.Cmd(queueItem);
	}

	void Logic::Cast(Player* player, uint32 id)
	{
		if ( !bActivated )
			return;
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.param = id;
		queueItem.operation = Constants::Command::Cast;
		commandQueue.Cmd(queueItem);
	}

	bool Logic::Use(Player* player, Database::Info::ItemInfo* info, int bag, int pos)
	{
		if ( !bActivated )
			return false;
		commandQueue.Use(player->GetSerial(), info, bag, pos);
		return true;
	}

	void Logic::SetLogicUserPvpRank(Player* player, const uint32 pvpRank)
	{
		if(bActivated == false)
			return;

		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = XRated::Constants::Command::SetPvPRank;
		queueItem.param=pvpRank;
        commandQueue.Cmd(queueItem);
	}

	bool Logic::PetItemUse(Player* player, Database::Info::ItemInfo* info, int bag, int pos, GlobalSerial serial)
	{
		if ( !bActivated )
			return false;
		commandQueue.PetItemUse(player->GetSerial(), info, bag, pos, serial);
		return true;
	}

	void Logic::DropPetItem(Player* player, GlobalSerial itemSerial)
	{
		if ( !bActivated )
			return;
		commandQueue.DropPetItem(player->GetSerial(), itemSerial);
	}

	void Logic::PetFeed( Player* player, const GlobalSerial& petSerial, const uint32& foodAmount, const uint32& foodCount, bool overFeed )
	{
		if( !bActivated )
			return;
		commandQueue.PetFeed( player->GetSerial(), petSerial, foodAmount, foodCount, overFeed );
	}

	void Logic::GivePresentToPet( Player* player, GlobalSerial petSerial, uint32 sellPrice, uint32 count )
	{
		if( !bActivated )
			return;
		commandQueue.GivePresentToPet( player->GetSerial(), petSerial, sellPrice, count );
	}

	void Logic::PetLevelDecrease(Player* player, const GlobalSerial& petSerial, const uint32& downLevel, const bool& onlyMaxLevel)
	{
		if( !bActivated )
			return;

		commandQueue.PetLevelDecrease(player->GetSerial(), petSerial, downLevel, onlyMaxLevel);
	}

	void Logic::CreatePet( Player* player, const Pet& pet )
	{
		if( !bActivated )
			return;

		commandQueue.CreatePet( player->GetSerial(), pet );
	}


	bool Logic::PetEquip(XRated::GlobalSerial petSerial, Player* player, const Database::Info::ItemInfo* info, Database::Enchant::EnchantBitfields fields, Constants::Equipment where)
	{
		if( !bActivated )
			return false;

		commandQueue.PetEquip(petSerial, player->GetSerial(), info, fields, where);
		return true;
	}
	
	void Logic::TakeCarePetBySchool( Player* player
								   , const uint32& petItemHash , const XRated::GlobalSerial& petSerial
								   , const Database::Enchant::EnchantBitfields& instance, const uint8& count
								   , const uint32& period, const float& expFactor )
	{
		if( !bActivated )
			return;

		commandQueue.TakeCarePetBySchool( player->GetSerial(), petItemHash, petSerial, instance, count, period, expFactor );
	}

	void Logic::TakeOutCaredPet( Player* player, const XRated::GlobalSerial& petSerial )
	{
		if( !bActivated )
			return;

		commandQueue.TakeOutCaredPet( player->GetSerial(), petSerial );
	}

	void Logic::PetRenaming(Serial playerSerial, const XRated::GlobalSerial petSerial, const std::wstring& newName)
	{
		if(bActivated == false)
			return;

        commandQueue.PetRenaming(playerSerial, petSerial, newName);
	}

	void Logic::UnsummonPet(Serial playerSerial)
	{
		if(bActivated == false)
			return;

        commandQueue.PetUnsummon(playerSerial);
	}


	bool Logic::Equip(Player* player, const Database::Info::ItemInfo* info, Database::Enchant::EnchantBitfields fields, Constants::Equipment where)
	{
		if ( !bActivated )
			return false;
		commandQueue.Equip(player->GetSerial(), info, fields, where);
		return true;
	}

	bool Logic::ChangeEquipToEquip(Player* player, XRated::Constants::Equipment from, XRated::Constants::Equipment to)
	{
		if ( !bActivated )
			return false;
		commandQueue.ChangeEquipToEquip(player->GetSerial(), from, to);
		return true;
	}

	bool Logic::SwapEquip( Player* player, const std::vector< EquippedItem > & newEquipments )
	{
		if ( !bActivated )
			return false;
		commandQueue.SwapEquipment(player->GetSerial(), newEquipments);
		return true;
	}

	bool Logic::SwapCashEquip( Player* player, const std::vector< EquippedItem > & newEquipments )
	{
		if ( !bActivated )
			return false;
		commandQueue.SwapCashEquipment(player->GetSerial(), newEquipments);
		return true;
	}

	void Logic::CreateItem(Player* player, const XRated::RewardItem& rewardItem, bool isPrivateItem)
	{
		if ( !bActivated )
			return;
		commandQueue.CreateItem(player->GetSerial(), rewardItem, isPrivateItem);
	}

	void Logic::Revive(Player* player,bool self)
	{
		if ( !bActivated )
			return;
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::Revive;
		if( self == true ) {
			queueItem.param = 1;
		}else {
			queueItem.param = 0;
		}		
		commandQueue.Cmd(queueItem);
	}
	void Logic::DailyRevive(Player* player)
	{
		if ( !bActivated )
			return;
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::DailyRevive;
		commandQueue.Cmd(queueItem);
	}
	
	void Logic::InstantCoinRevive( Player* player, uint64 orderNumber )
	{
		if ( !bActivated )
		{
			// �ѹ� ����-_-; ���⼭�� �ؾ� �ұ.
			listener->InstantCoinRevived( player, RollbackRequested, orderNumber );
			return;
		}
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::InstantCoinRevive;
		queueItem.param = orderNumber >> 32;	// ���� 32��Ʈ
		queueItem.param2 = static_cast< uint32 >( orderNumber ); // ���� 32��Ʈ
		commandQueue.Cmd(queueItem);
	}	

	void Logic::IncreaseSkillPoint(Player* player, uint32 skill)
	{
		if ( !bActivated )
			return;
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::IncSkill;
		queueItem.param = skill;
		commandQueue.Cmd(queueItem);
	}

	void Logic::DecreaseSkillPoint(Player* player, uint32 skill)
	{
		if ( !bActivated )
			return;
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::DecSkill;
		queueItem.param = skill;
		commandQueue.Cmd(queueItem);
	}
	void Logic::AddSkillPointPlus(Player* player, uint16 skillPointPlus)
	{
		if ( !bActivated )
			return;
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::AddSkillPointPlus;
		queueItem.param = skillPointPlus;
		commandQueue.Cmd(queueItem);
	}

	void Logic::ResetSkillPoint(Player* player, uint32 skill)
	{
		if ( !bActivated )
			return;
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::ResetSkill;
		queueItem.param = skill;
		commandQueue.Cmd(queueItem);
	}

	void Logic::SetPlayerExpFactor(Player* player, float factor)
	{
		if ( factor < 0 ) 
			return;

		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::SetExpFactor;
		queueItem.param = (uint32)(factor*100.0f); 
		commandQueue.Cmd(queueItem);
	}

	void Logic::GiveUpRevive(Player* player)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::GiveUpRevive;
		commandQueue.Cmd(queueItem);
	}

	void Logic::ChangeStylePointState(Player* player, XRated::StylePoint::State state)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::ChangeStylePointState;
		queueItem.param = state;
		commandQueue.Cmd(queueItem);
	}
	
	void Logic::ChangeStylePointPlayerCount(Player* player, int count)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::ChangeStylePointPlayerCount;
		queueItem.param = count;
		commandQueue.Cmd(queueItem);
	}

	void Logic::AddExp(XRated::Constants::ExpAcquiredType type, Player* player, uint32 exp)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::GiveExp;
		queueItem.param = exp;
		queueItem.subParam = type;
		commandQueue.Cmd(queueItem);
	}

	void Logic::AddExpWithNoFactor(XRated::Constants::ExpAcquiredType type, Player* player, uint32 exp)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::GiveExpWithNoFactor;
		queueItem.param = exp;
		queueItem.subParam = type;
		commandQueue.Cmd(queueItem);
	}

	void Logic::AddPvpExp(XRated::Constants::ExpAcquiredType type, Player* player, uint32 exp)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::GivePvpExp;
		queueItem.param = exp;
		queueItem.subParam = type;
		commandQueue.Cmd(queueItem);
	}

	void Logic::AddWarExp(XRated::Constants::ExpAcquiredType type, Player* player, uint32 exp)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::GiveWarExp;
		queueItem.param = exp;
		queueItem.subParam = type;
		commandQueue.Cmd(queueItem);
	}

	void Logic::AddStateBundle(Player* player, uint32 hash)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::GiveStateBundle;
		queueItem.param = hash;
		commandQueue.Cmd(queueItem);
	}
	void Logic::AddPassive(Player* player, uint32 hash)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::AddPassive;
		queueItem.param = hash;
		commandQueue.Cmd(queueItem);
	}
	void Logic::RemovePassive(Player* player, uint32 hash)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::RemovePassive;
		queueItem.param = hash;
		commandQueue.Cmd(queueItem);
	}
	void Logic::CompletedQuest( Serial player, uint32 questHash )
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player;
		queueItem.operation = Constants::Command::CompletedQuest;
		queueItem.param = questHash;
		commandQueue.Cmd(queueItem);
	}
	void Logic::AcceptedQuest( Serial player, uint32 questHash )
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player;
		queueItem.operation = Constants::Command::AcceptedQuest;
		queueItem.param = questHash;
		commandQueue.Cmd(queueItem);
	}
	void Logic::NoticeHolidayEvent(uint32 eventId, bool start)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = 0;
		queueItem.operation = Constants::Command::NoticeHolidayEvent;	
		queueItem.param = eventId;
		queueItem.param2 = start==true ? 1 : 0;
		commandQueue.Cmd(queueItem);
	}

	//�ٷ� ������� ��. ť�� ���� �ð��� ����.
	float Logic::GetObjectDistance(Player* player, uint32 hash) const
	{
		return stageData.GetObjectDistance(player, hash);
	}

	void Logic::FamiliarCommand(Player* player, uint16 index, XRated::Serial who, Constants::Familiar::Command command)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::ControlFamiliar;
		queueItem.param = who;
		queueItem.param2 = ( index << 16 ) + command;
		commandQueue.Cmd(queueItem);
	}

	void Logic::ChangeFamiliarFormation(Player* player, Constants::Familiar::Formation formation)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::ChangeFamiliarFormation;
		queueItem.param = formation;
		commandQueue.Cmd(queueItem);
	}
	void Logic::AddGuildUser(Player& player)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player.GetSerial();
		queueItem.operation = Constants::Command::AddGuildUser;
		queueItem.param = 0;
		commandQueue.Cmd(queueItem);
	}
	void Logic::RemoveGuildUser(Player& player)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player.GetSerial();
		queueItem.operation = Constants::Command::RemoveGuildUser;
		queueItem.param = 0;
		commandQueue.Cmd(queueItem);
	}
	void Logic::RequestUpdateGuildExpFactor(Player& player, int guildId)
	{
		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player.GetSerial();
		queueItem.operation = Constants::Command::UpdateGuildExp;
		queueItem.param = static_cast<uint32>(guildId);
		commandQueue.Cmd(queueItem);
	}

	void Logic::ValidateEquippedItems(Player* player)
	{
        if(!bActivated)
			return;

		CommandQueue::CmdQueue queueItem;
		queueItem.userSerial = player->GetSerial();
		queueItem.operation = Constants::Command::ValidateEquippedItems;
		queueItem.param = 0;
		commandQueue.Cmd(queueItem);
	}

	// player commands
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Logic::ProcessJoin(CommandQueue::JoinQueue& queueItem)
	{
		PlayerInitialData pid;
		{
			AutoLock lock(queueItem.data->SyncRoot);
			pid=*queueItem.data;
		}

		if ( stageData.RetrievePlayerByID(pid.name) ) {//�̹� ���� �ϴ� �÷��̾�.
			listener->JoinFailed(queueItem.user);
			return;
		}

		Player* obj;
		if ( stageData.GetStageGroupInfo()->UseItem ) //pvp�����̴�. ���������� player�� �ε�����.
			obj = LogicDBInstance().CreatePlayerPvpItem(&stageData, pid);
		else
			obj = LogicDBInstance().CreatePlayer(&stageData, pid);

		PlayerData& playerData = obj->GetPlayerData();
		playerData.user = queueItem.user;
		//��� ó��
		{
			playerData.Equipments.clear();
			std::vector< std::pair<const Database::Info::ItemInfo*, int64> >::iterator iTmp= pid.equipments.begin(), end = pid.equipments.end();
			Lunia::XRated::Item item;
			for( ; iTmp != end ; ++iTmp ) {
				if ( (*iTmp).first ) {
					item.Id = (*iTmp).first->Hash;
					item.InstanceEx = (*iTmp).second;
				} else {
					item.Id = 0;
					item.InstanceEx = 0;
				}
				playerData.Equipments.push_back( item );
			}
			pid.equipments.clear();
		}

		playerData.Skills = pid.skills;
		playerData.PvpXp = pid.pvpXp;
		playerData.PvpLevel = pid.pvpLevel;
		playerData.WarXp = pid.warXp;
		playerData.WarLevel = pid.warLevel;
		playerData.StoredLevel = pid.storedLevel;
		playerData.RebirthCount = pid.rebirthCount;
		playerData.MaxLife = 0;

		uint8 team = obj->GetTeam()-1; //���� team number���� 1�� ����. �迭�� �� ���̱� ������.
		
		if (team > Constants::MaxTeamCnt-1 || team >= (int)(*startingPoint).size()) {//��� ���ٱ.. �� ���� ���ƾ� �ϳ�. -_-a
			Logger::GetInstance().Exception(L"Too many teams.[{0}/{1}], stage ID:{2}" , team, (int)(*startingPoint).size(), stageData.GetStageInfo()->Id.c_str());
			listener->JoinFailed(queueItem.user);
			return ;
		}

		int index= teamMemberCnt[team] % (int)(*startingPoint)[team].size();
		if (pid.StartPosition.UseCustomStartPoint==false)
		{
			obj->InitPosition( (*startingPoint)[team][index].Position );
			obj->SetDirection( (*startingPoint)[team][index].Direction);
		}
		else
		{
			obj->InitPosition( pid.StartPosition.Place.Position );
			obj->SetDirection( pid.StartPosition.Place.Direction );
		}
		++teamMemberCnt[team];

		//�ϴ� ü���� �ƽ��� �ѹ� �������ش�.
		float maxHp, maxMp;
		obj->RecalculateStatus();
		obj->GetHP(maxHp);
		obj->GetMP(maxMp);
		obj->SetHP(maxHp);
		obj->SetMP(maxMp);

		uint32 result = stageData.CreatePlayer(obj);
		if ( result == 0 ) {
			if ( stageData.GetStageGroupInfo()->UseItem )
				LogicDBInstance().ReleasePlayerPvpItem(obj);
			else
				LogicDBInstance().ReleasePlayer(obj);
			return;
		}

		obj->InitSkill(playerData);
	
		Info::ItemInfo *info;
		std::vector< Lunia::XRated::Item >::iterator i = playerData.Equipments.begin(), end = playerData.Equipments.end();
		uint32 old;
		int cnt =0;
		for (; i != end ; ++i, ++cnt) {
			if ( i->Id == 0 )
				continue;
			info = DatabaseInstance().InfoCollections.Items.Retrieve(i->Id);
			if (info) {
				obj->Equip(info, i->InstanceEx, (Constants::Equipment)cnt, old, true);
			} else {
				Logger::GetInstance().Info( L"unknown item. [{0}]", i->Id );
			}
		}
		obj->AddEquipSetBonus();
		obj->RecalculateStatus();
		obj->GetHP(maxHp);
		obj->GetMP(maxMp);
		obj->SetHP(maxHp);
		obj->SetMP(maxMp);
		obj->InitAirAttackComboCnt(); //Air Combo Init

		stageData.PlayerEntered(  obj->GetSerial(), obj->GetTeam() );
		listener->StatusChanged(obj->GetSerial(), obj->GetPosition(), obj->GetDirection(), maxHp, maxMp, obj->GetSerial(), 0, Constants::StatusFlag::NoDisplay);
	}

	void Logic::ProcessUse(CommandQueue::UseQueue& queueItem)
	{
		Player* player = stageData.RetrievePlayer( queueItem.userSerial);
		if (!player) {
			Logger::GetInstance().Info( L"unknown player [{0}].", queueItem.userSerial );
			return;
		}

		uint32 returnValue = player->UseItem(queueItem.info, stageData.GetStageGroupInfo()->GameType);
		
		listener->ItemUsed(returnValue, player, queueItem.info, queueItem.bag, queueItem.pos);
	}


	void Logic::ProcessPetItem( CommandQueue::PetItemQueue& queueItem )
	{
		Player* player = stageData.RetrievePlayer( queueItem.UserSerial );
		if (!player) {
			Logger::GetInstance().Info( L"unknown player [{0}].", queueItem.UserSerial );
			return;
		}
		switch( queueItem.Type )
		{
		case CommandQueue::PetItemQueue::Type::Use:
			{
				uint32 returnValue = player->UseItem(queueItem.Info, stageData.GetStageGroupInfo()->GameType);
				if ( returnValue == Player::ItemUseResult::Success )
				{
					player->UsePetItem( queueItem.ItemSerial );
				}

				listener->ItemUsed(returnValue, player, queueItem.Info, queueItem.Bag, queueItem.Pos);
			}
			break;
		case CommandQueue::PetItemQueue::Type::Drop:
			{
				player->DropPetItem( queueItem.ItemSerial );				
			}
			break;
		}		
	}

	void Logic::ProcessPetFeed( CommandQueue::PetFeedQueue& queueItem )
	{
		Player* player = stageData.RetrievePlayer( queueItem.UserSerial );
		if (!player) {
			Logger::GetInstance().Info( L"unknown player [{0}].", queueItem.UserSerial );
			return;
		}
		player->GiveFoodToPet(queueItem.PetSerial, queueItem.FoodAmount, queueItem.FoodCount, queueItem.OverFeed );		
	}

	void Logic::ProcessGivePresentToPet(CommandQueue::PresentToPetQueue& queueItem)
	{
		Player* player = stageData.RetrievePlayer( queueItem.UserSerial );
		if (!player) {
			Logger::GetInstance().Info( L"unknown player [{1}].", queueItem.UserSerial );
			return;
		}
	
		player->GivePresentToPet(queueItem.PetSerial, queueItem.PresentSellPrice, queueItem.PresentItemCount);		
	}

	void Logic::ProcessPetLevelDecrease(CommandQueue::PetLevelDecreaseQueue& queueItem)
	{
		Player* player = stageData.RetrievePlayer( queueItem.UserSerial );
		if (!player) {
			Logger::GetInstance().Info( L"unknown player [{0}].", queueItem.UserSerial );
			return;
		}
	
		Player::Pets& pets = player->GetPlayerPetData();

		for(size_t i = 0; i < pets.size(); ++i)
		{
			if(pets[i].Pet.PetSerial == queueItem.PetSerial)
			{
				if( ( queueItem.OnlyMaxLevel == 1 && pets[i].Pet.Level >= XRated::Pet::MaxLevel )
					|| ( queueItem.OnlyMaxLevel == 0 && pets[i].Pet.Level < XRated::Pet::MaxLevel ) )
				{

					if(pets[i].Pet.Appear == true)
						player->PetUnsummon(pets[i].Pet);

					if( pets[i].Pet.Level > queueItem.DownLevel )
					{
						pets[i].Pet.Level -= queueItem.DownLevel;
					}
					else
					{
						pets[i].Pet.Level = 1;
					}

					pets[i].Pet.Exp = 0.0;
					pets[i].Pet.FullSum = 0.0f;
					pets[i].Pet.LevelUpPeriod = 0.0f;

					stageData.PetInfoUpdated(player, pets[i].Pet, pets[i].PetNPCSerial, L"PET_PETLEVELDOWN");
					break;
				}
			}
		}
	}

	void Logic::ProcessCreatePet(CommandQueue::CreatePetQueue& queueItem)
	{
		Player* player = stageData.RetrievePlayer( queueItem.UserSerial );
		if (!player) {
			Logger::GetInstance().Info( L"unknown player [{0}].", queueItem.UserSerial );
			return;
		}

		player->AddPet( queueItem.Pet );
	}

	void Logic::ProcessPetEquip(CommandQueue::PetEquipQueue& queueItem)
	{
		Player* player = stageData.RetrievePlayer( queueItem.userSerial );
		if (!player) {
			Logger::GetInstance().Info( L"unknown player [{0}].", queueItem.userSerial );
			return;
		}
		uint32 equipmentOld =0, equipmentId=(queueItem.info?queueItem.info->Hash : 0);
		XRated::Serial petObjectSerial =0;
		bool returnValue = player->PetEquip( queueItem.PetSerial, queueItem.info, queueItem.fields, queueItem.where, equipmentOld, petObjectSerial);
		listener->PetItemEquipped(returnValue, queueItem.PetSerial, petObjectSerial, player, equipmentId, queueItem.fields, equipmentOld);		
	}

	void Logic::ProcessTakeCarePetBySchool(CommandQueue::TakeCarePetBySchoolQueue& queueItem)
	{
		Player* player = stageData.RetrievePlayer( queueItem.UserSerial );
		if (!player) {
			Logger::GetInstance().Info( L"unknown player [{0}].", queueItem.UserSerial );
			return;
		}

		player->TakeCarePetBySchool( queueItem.PetItemHash, queueItem.PetSerial
			                       , queueItem.PetItemInstance, queueItem.PetItemcount
								   , queueItem.Period, queueItem.ExpFactor );
	}

	void Logic::ProcessTakeOutCaredPet(CommandQueue::TakeOutCaredPetQueue& queueItem)
	{
		Player* player = stageData.RetrievePlayer( queueItem.UserSerial );
		if (!player) {
			Logger::GetInstance().Info( L"unknown player [{0}].", queueItem.UserSerial );
			return;
		}

		player->TakeOutCaredPet( queueItem.PetSerial );
	}

	void Logic::ProcessPetRenaming(CommandQueue::PetRenamingQueue& queueItem)
	{
		Player* player = stageData.RetrievePlayer(queueItem.playerSerial);

		if(player == NULL)
		{
			Logger::GetInstance().Info(L"unknown player [{0}].", queueItem.playerSerial);
			return;
		}

		player->PetRenaming(queueItem.petSerial, queueItem.newName);
	}

	void Logic::ProcessUnsummonPet(CommandQueue::PetUnsummonQueue& queueItem)
	{
		Player* player = stageData.RetrievePlayer(queueItem.playerSerial);

		if(player == NULL)
		{
			Logger::GetInstance().Info(L"unknown player [{0}].", queueItem.playerSerial);
			return;
		}

		player->UnsummonPet();
	}

	void Logic::ProcessEquip(CommandQueue::EquipQueue& queueItem)
	{
		Player* player = stageData.RetrievePlayer( queueItem.userSerial);
		if (!player) {
			Logger::GetInstance().Info( L"unknown player [{0}].", queueItem.userSerial );
			return;
		}

		uint32 equipmentOld=0, equipmentId=(queueItem.info?queueItem.info->Hash : 0);
		bool returnValue = player->Equip(queueItem.info, queueItem.fields, queueItem.where, equipmentOld);
		listener->ItemEquipped(returnValue, player, equipmentId, queueItem.fields, equipmentOld);
		if ( returnValue && queueItem.info != NULL ) player->AddEquipSetBonus();
	}

	void Logic::ProcessSwapEquipment(CommandQueue::SwapEquipmentQueue& queueItem )
	{
		Player* player = stageData.RetrievePlayer( queueItem.userSerial);
		if (!player) {
			Logger::GetInstance().Info( L"unknown player [{0}].", queueItem.userSerial );
			return;
		}

		//uint32 equipmentOld=0, equipmentId=(queueItem.info?queueItem.info->Hash : 0);
		bool returnValue = player->SwapEquipment( queueItem.equipments );
		listener->EquipementSwapped(returnValue, player, queueItem.equipments);
		if ( returnValue ) player->AddEquipSetBonus();
	}

	void Logic::ProcessCashSwapEquipment(CommandQueue::SwapCashEquipmentQueue& queueItem )
	{
		Player* player = stageData.RetrievePlayer( queueItem.userSerial);
		if (!player) {
			Logger::GetInstance().Info( L"unknown player [{0}].", queueItem.userSerial );
			return;
		}
		//uint32 equipmentOld=0, equipmentId=(queueItem.info?queueItem.info->Hash : 0);
		bool returnValue = player->SwapEquipment( queueItem.equipments );
		listener->CashEquipementSwapped(returnValue, player, queueItem.equipments);
		if ( returnValue ) player->AddEquipSetBonus();
	}

	void Logic::ProcessChangeEquipToEquip(CommandQueue::ChangeEquipToEquipQueue& queueItem)
	{
		Player* player = stageData.RetrievePlayer( queueItem.userSerial);
		if (!player) {
			Logger::GetInstance().Info( L"unknown player [{0}].", queueItem.userSerial );
			return;
		}

		bool returnValue = player->ChangeEquipToEquip(queueItem.from,queueItem.to);
		listener->ItemEquipToEquipChanged(returnValue,player,queueItem.from,queueItem.to);		
	}
	void Logic::ProcessCreateItem(CommandQueue::CreateItemQueue& queueItem)
	{
		Player* player = stageData.RetrievePlayer( queueItem.userSerial);
		if (!player) {
			Logger::GetInstance().Info( L"unknown player [{0}].", queueItem.userSerial );
			return;
		}

		stageData.CreateItem(queueItem.rewardItem.ItemHash, player->GetPosition(), player->GetDirection(), player->GetSerial(), 
							 queueItem.rewardItem.StackCount, queueItem.rewardItem.InstanceEx, queueItem.isPrivateItem);
	}

	void Logic::ProcessRebirthRollBack(CommandQueue::RebirthQueue& queueItem)
	{
		Player* player = stageData.RetrievePlayer(queueItem.playerSerial);

		if(player == NULL)
		{
			Logger::GetInstance().Info(L"unknown player [{0}]. - at Logic::ProcessRebirth", queueItem.playerSerial);
			return;
		}
		
		player->RebirthRollBack();
	}

	void Logic::ProcessRebirth(CommandQueue::RebirthQueue& queueItem)
	{
		Player* player = stageData.RetrievePlayer(queueItem.playerSerial);

		if(player == NULL)
		{
			Logger::GetInstance().Info(L"unknown player [{0}]. - at Logic::ProcessRebirth", queueItem.playerSerial);
			return;
		}
		
		if(player->Rebirth( queueItem.levelAfterRebirth ) == false)
			return;

        //stageData->
	}

	void Logic::ProcessCommand(CommandQueue::CmdQueue& queueItem)
	{
		Player* player = stageData.RetrievePlayer( queueItem.userSerial );
		if (!player) {
			if ( queueItem.operation == Constants::Command::GameStart ) 
			{
				stageData.GameStart();
				if ( listener->IsPvPRoom() )
					stageData.CheckAbleToPlay();
				return;
			}
			if ( queueItem.operation == Constants::Command::NoticeHolidayEvent )
			{
				stageData.NoticeHolidayEvent( queueItem.param, queueItem.param2 == 1 ? true : false );
				return;
			}
			
			Logger::GetInstance().Info( L"[Logic::ProcessCommand] unknown player [{0}].", queueItem.userSerial  );
			if ( queueItem.operation == Constants::Command::InstantCoinRevive )
			{
				listener->InstantCoinRevived( player, RollbackRequested, (queueItem.param << 32) + queueItem.param2  );
			}
			return;
		}

		//static std::list<XRated::Constants::Command> commandList;
		//static std::list<XRated::Constants::Command> checkCommandList;
		//
		//if(checkCommandList.empty() == true)
		//{
		//	checkCommandList.push_back(XRated::Constants::Command::Move);
		//	checkCommandList.push_back(XRated::Constants::Command::Stop);
		//	checkCommandList.push_back(XRated::Constants::Command::Dash);
		//	checkCommandList.push_back(XRated::Constants::Command::Stop);
		//	checkCommandList.push_back(XRated::Constants::Command::AttackA);
		//	checkCommandList.push_back(XRated::Constants::Command::Move);
		//	checkCommandList.push_back(XRated::Constants::Command::Stop);
		//}

		//commandList.push_back(queueItem.operation);

		//bool findCommand = true;

		//if(commandList.size() > 7)
		//	commandList.pop_front();
		//else if(commandList.size() != 7)
		//	findCommand = false;

		//std::list<XRated::Constants::Command>::iterator itr2 = checkCommandList.begin();
		//
		//for(std::list<XRated::Constants::Command>::iterator itr = commandList.begin(); itr != commandList.end() && itr2 != checkCommandList.end(); ++itr, ++itr2)
		//{
		//	if(*itr != *itr2)
		//	{
		//		findCommand = false;
		//		break;
		//	}
		//}

		//if(findCommand == true)
		//{
		//	Lunia_IMPORTANT((L"--------- Command Bug", static_cast<int>(queueItem.operation)));		
		//}

		//Lunia_IMPORTANT((L"--------- input log {}", static_cast<int>(queueItem.operation)));		

		bool ret = false;

		switch(queueItem.operation) {
		case Constants::Command::GameStart :
			{
				stageData.GameStart();
				if ( listener->IsPvPRoom() )
					stageData.CheckAbleToPlay();
			}
			break;
		case Constants::Command::Move :
			player->Command(Action::Action::CommandType::MOVE, queueItem.param);
			break;
		case Constants::Command::Dash :
			player->StopAutoAction();
			player->Command(Action::Action::CommandType::DASH, queueItem.param);
			break;
		case Constants::Command::Stop :
			player->Command(Action::Action::CommandType::STOP, queueItem.param);
			break;
		case Constants::Command::AttackA :
			player->StopAutoAction();
			player->Command(Action::Action::CommandType::ATTACKA, queueItem.param);
			break;
		case Constants::Command::AttackARelease :
			player->Command(Action::Action::CommandType::ATTACKARELEASE, queueItem.param);
			break;
		case Constants::Command::AttackB :
			player->StopAutoAction();
			player->Command(Action::Action::CommandType::ATTACKB, queueItem.param);
			break;
		case Constants::Command::AttackBRelease :
			player->Command(Action::Action::CommandType::ATTACKBRELEASE, queueItem.param);
			break;
		case Constants::Command::DownAttack :
			player->StopAutoAction();
			player->Command(Action::Action::CommandType::DOWNATTACK, queueItem.param);
			break;
		case Constants::Command::Jump :
			player->Command(Action::Action::CommandType::JUMP, queueItem.param);
			break;
		case Constants::Command::Cast :
			player->StopAutoAction();
			player->UseSkill(queueItem.param);
			break;
		case Constants::Command::Revive :
			{
				if( queueItem.param == 0 ) {
					std::cout << "Revive Player, from Logic.cpp 000" << std::endl;
					player->Revive( Player::ReviveStatus::Basic, false);
				}else {
					std::cout << "Revive Player, from Logic.cpp" << std::endl;
					player->Revive( Player::ReviveStatus::Basic, true);
				}
			}			
			break;
		case Constants::Command::DailyRevive :
			{
				player->DailyRevive();
			}	
			break;
		case Constants::Command::InstantCoinRevive :

			if ( listener->IsPvPRoom() == false )
			{

			}
			else
			{

			}

			break;
		case Constants::Command::IncSkill :
			player->IncSkillLevel(queueItem.param, 1);
			break;
		case Constants::Command::DecSkill :
			player->DecSkillLevel(queueItem.param, 1);
			break;
		case Constants::Command::ResetSkill :
			{
				Lunia::XRated::PlayerData& playerData = player->GetPlayerData();
				std::vector<uint32> skillGroupHash;

				if (queueItem.param==0) /* every skill group of the player */
				{
					for ( std::vector<Skill>::iterator i = playerData.Skills.begin() ; i != playerData.Skills.end() ; ++i )
						skillGroupHash.push_back((*i).Id);
				}
				else
				{
					skillGroupHash.push_back( queueItem.param );
				}

				for ( std::vector<uint32>::iterator is = skillGroupHash.begin(); is != skillGroupHash.end() ; ++is ) {
					player->ResetSkillLevel((*is));
				}
				listener->SkillPointChanged(player, 0, 0);
			}
			break;
		case Constants::Command::AddSkillPointPlus :
			player->AddSkillPointPlus( (uint16)queueItem.param );
			break;
		case Constants::Command::ChangeTeam :
			player->SetTeam( (uint8)queueItem.param );
			break;
		case Constants::Command::Part :
			{
				Serial serial = player->GetSerial();
				uint8 team = player->GetTeam();
				stageData.RemoveTombstone( serial );
				stageData.ObjectDestroyed(player, Constants::ObjectType::Player);
				Player* obj = stageData.RemovePlayer( serial );
				if (obj) {
					obj->NoticeDestroyToFamiliar();
					if ( obj->IsPlayerPvpItem() )
						LogicDBInstance().ReleasePlayerPvpItem(obj);
					else
						LogicDBInstance().ReleasePlayer(obj);
				}

				stageData.PlayerLeft(serial, team);

				if ( stageData.GetPlayerCnt() == 0 ) {
					Logger::GetInstance().Warn( L"[] Logic Stopped. [{0}]", stageCode );
					bActivated = false;
					return;
				}
				//else	
				//{
				//	stageData.PlayerLeft(serial, team);
				//}

				stageData.CheckAbleToPlay();
				//if ( obj )
				//	delete obj;
			}
			break;
		//case Constants::Command::CreateItem :
		//	stageData.CreateItem(queueItem.param, player->GetPosition(), player->GetDirection(), player->GetSerial(), (uint16)(queueItem.subParam >> 16), (queueItem.subParam & 0x0000FFFF));
		//	break;
		case Constants::Command::SetExpFactor :
			player->SetExpFactor((float)queueItem.param / 100.0f);
			break;
		case Constants::Command::GiveUpRevive :
			stageData.GiveUpRevive(player);
			break;
		case Constants::Command::GiveExp :
			player->XpGained(static_cast<XRated::Constants::ExpAcquiredType>(queueItem.subParam), queueItem.param, true);
			break;
		case Constants::Command::GiveExpWithNoFactor :
			player->XpGained(static_cast<XRated::Constants::ExpAcquiredType>(queueItem.subParam), queueItem.param, false);
			break;
		case Constants::Command::GivePvpExp :
			{
				uint32 xpPvp = DatabaseInstance().InfoCollections.Statuses.GetExp(Info::StatusInfoManager::ExpType::Pvp, player->GetPlayerData().PvpLevel-1);
				player->PvpLevelUp(static_cast<XRated::Constants::ExpAcquiredType>(queueItem.subParam), queueItem.param, xpPvp);
			}
			break;
		case Constants::Command::GiveWarExp :
			{
				uint32 xpWar = DatabaseInstance().InfoCollections.Statuses.GetExp(Info::StatusInfoManager::ExpType::War, player->GetPlayerData().WarLevel-1);
				player->WarLevelUp(static_cast<XRated::Constants::ExpAcquiredType>(queueItem.subParam), queueItem.param, xpWar);
			}
			break;
		case Constants::Command::GiveStateBundle :
			{
				Database::Info::StateBundleInfo* info = DatabaseInstance().InfoCollections.StateBundles.Retrieve(queueItem.param);
				if (info)
					player->AddStateBundle(info,0);
			}
			break;		
		case Constants::Command::ControlFamiliar :
			player->ControlFamiliar( queueItem.param, (uint16)(queueItem.param2 >> 16), (Constants::Familiar::Command)(queueItem.param2 & 0x0000FFFF) );
			break;
		case Constants::Command::ChangeFamiliarFormation :
			player->ChangeFamiliarFormation((Constants::Familiar::Formation)queueItem.param);
			break;
		case Constants::Command::ChangeStylePointState:
			player->ChangeStylePointState((XRated::StylePoint::State)queueItem.param);
			break;
		case Constants::Command::ChangeStylePointPlayerCount:
			player->ChangeStylePointPlayerCount((int)queueItem.param);
			break;
		case Constants::Command::AddPassive:
			{
				Database::Info::StateBundleInfo* info = DatabaseInstance().InfoCollections.StateBundles.Retrieve(queueItem.param);
				if (info)
					player->AddPassive(info);
			}
			break;
		case Constants::Command::RemovePassive:
			{
				Database::Info::StateBundleInfo* info = DatabaseInstance().InfoCollections.StateBundles.Retrieve(queueItem.param);
				if (info)
					player->RemovePassive(info);
			}
			break;
		case Constants::Command::CompletedQuest:
			{
				stageData.CompletedQuest(queueItem.userSerial, queueItem.param);				
			}
			break;
		case Constants::Command::AcceptedQuest:
			{
				stageData.AcceptedQuest( queueItem.userSerial, queueItem.param );
			}
			break;

		/* cheat commands */
		case Constants::MissionClearCommand:
			stageData.MissionClear(true, static_cast<uint8>(queueItem.param));
			break;
		
		case Constants::Command::ChangePlayTimePenalty :
			player->SetPlayTimePenalty( static_cast<Constants::PlayTimePenalty::Type>(queueItem.param) );
			break;
		case Constants::Command::AddGuildUser:
			listener->AddedGuildUser(queueItem.userSerial);
			break;
		case Constants::Command::RemoveGuildUser:
			listener->RemovedGuildUser(queueItem.userSerial);
			break;
		case Constants::Command::UpdateGuildExp:
			listener->UpdateGuildExpFactor( static_cast<int>(queueItem.param) );
			break;
		case Constants::Command::ValidateEquippedItems:
			player->ValidateEquippedItems();
			break;
		case Constants::Command::SetPvPRank :
			player->SetUserPvpRank( queueItem.param );
			break;
		default :
			break;
		}
	}

	bool Logic::UpdateObject(double dt)
	{
		remain  = remain + dt;

		/* object update */
		if (remain > resolution)
		{
			stageData.Update((float)resolution);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			while( remain >= resolution) {
				remain = remain - resolution;
			}
			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			++elapsedTick;
			++turnNo;

			elapsedTime+=dt;
			return true;
		}
		elapsedTime+=dt;
		return false;
	}

	void Logic::UpdateCommand()
	{
		commandQueue.SwapQueue();

		updateInfo.cmdCount = commandQueue.GetCmdQueueCnt();

		std::vector<CommandQueue::QueueType>& commands = commandQueue.GetOrderQueue();
		std::vector<CommandQueue::QueueType>::iterator i = commands.begin();
		for ( ; i != commands.end() ; ++i) {
			switch ( (*i) ) {
			case CommandQueue::QueueType::TypeCmd :
				{
					CommandQueue::CmdQueue queueItem;
					commandQueue.GetCmdItem(queueItem);
					ProcessCommand(queueItem);
				}
				break;
			case CommandQueue::QueueType::TypeEquip :
				{
					CommandQueue::EquipQueue queueItem;
					commandQueue.GetEquipItem(queueItem);
					ProcessEquip(queueItem);
				}
				break;
			case CommandQueue::QueueType::TypeChangeEquipToEquip:
				{
					CommandQueue::ChangeEquipToEquipQueue queueItem;
					commandQueue.GetChangeEquipToEquipItem(queueItem);
					ProcessChangeEquipToEquip(queueItem);
				}
				break;
			case CommandQueue::QueueType::TypeJoin :
				{
					CommandQueue::JoinQueue queueItem;
					commandQueue.GetJoinItem(queueItem);
					ProcessJoin(queueItem);
				}
				break;
			case CommandQueue::QueueType::TypeUse :
				{
					CommandQueue::UseQueue queueItem;
					commandQueue.GetUseItem(queueItem);
					ProcessUse(queueItem);
				}
				break;
			case CommandQueue::QueueType::TypePetItemUse :
				{
					CommandQueue::PetItemQueue queueItem;
					commandQueue.GetPetItem(queueItem);
					ProcessPetItem(queueItem);
				}
				break;

			case CommandQueue::QueueType::TypePetFeed :
				{
					CommandQueue::PetFeedQueue queueItem;
					commandQueue.GetPetFeed(queueItem);
					ProcessPetFeed(queueItem);
				}
				break;
			case CommandQueue::QueueType::TypePresentToPet:
				{
					CommandQueue::PresentToPetQueue queueItem;
					commandQueue.GetPresentToPetItem(queueItem);
					ProcessGivePresentToPet(queueItem);
				}
				break;
			case CommandQueue::QueueType::TypeDecreasePetLevel:
				{
					CommandQueue::PetLevelDecreaseQueue queueItem;
					commandQueue.GetPetLevelDecrease(queueItem);
					ProcessPetLevelDecrease(queueItem);
				}
				break;
			case CommandQueue::QueueType::TypeCreatePet:
				{
					CommandQueue::CreatePetQueue queueItem;
					commandQueue.GetCreatePet(queueItem);
					ProcessCreatePet(queueItem);
				}
				break;

			case CommandQueue::QueueType::TypePetEquip:
				{
					CommandQueue::PetEquipQueue queueItem;
					commandQueue.GetPetEquip(queueItem);
					ProcessPetEquip(queueItem);
				}				
				break;
			case CommandQueue::QueueType::TypeTakeCarePetBySchool:
				{
					CommandQueue::TakeCarePetBySchoolQueue queueItem;
					commandQueue.GetTakeCarePetBySchool(queueItem);
					ProcessTakeCarePetBySchool(queueItem);
				}
				break;

			case CommandQueue::QueueType::TypeTakeOutCaredPet:
				{
					CommandQueue::TakeOutCaredPetQueue queueItem;
					commandQueue.GetTakeOutCaredPet(queueItem);
					ProcessTakeOutCaredPet(queueItem);
				}
				break;
			case CommandQueue::QueueType::TypeCreateItem:
				{
					CommandQueue::CreateItemQueue queueItem;
					commandQueue.GetCreateItem(queueItem);
					ProcessCreateItem(queueItem);
				}
				break;
			case CommandQueue::QueueType::TypeRebirth:
				{
					CommandQueue::RebirthQueue queueItem;
					commandQueue.GetRebirth(queueItem);
					ProcessRebirth(queueItem);
				}
				break;
			case CommandQueue::QueueType::TypeRebirthRollBack:
				{
					CommandQueue::RebirthQueue queueItem;
					commandQueue.GetRebirth(queueItem);
					ProcessRebirthRollBack(queueItem);
				}
				break;
			case CommandQueue::QueueType::TypePetRenaming:
				{
					CommandQueue::PetRenamingQueue queueItem;
					commandQueue.GetPetRenaming(queueItem);
					ProcessPetRenaming(queueItem);
				}
				break;
			case CommandQueue::QueueType::TypePetUnsummon:
				{
					CommandQueue::PetUnsummonQueue queueItem;
					commandQueue.GetPetUnsummon(queueItem);
					ProcessUnsummonPet(queueItem);
				}
				break;
			case CommandQueue::QueueType::TypeSwapEquipment:
				{
					CommandQueue::SwapEquipmentQueue queueItem;
					commandQueue.GetSwapEquipment( queueItem );
					ProcessSwapEquipment( queueItem );
				}
				break;
			case CommandQueue::QueueType::TypeSwapCashEquipment:
				{
					CommandQueue::SwapCashEquipmentQueue queueItem;
					commandQueue.GetCashSwapEquipment( queueItem );
					ProcessCashSwapEquipment( queueItem );
				}
				break;
			}
		}
	}

	void Logic::Update(double dt)
	{
		if ( bUpdating || IsLoading() ) {
			Logger::GetInstance().Info(L"Logic is loading now.[{0}/{1}]", bLoading?1:0, bUpdating?1:0);
			return;
		}
		bUpdating = true;

		if (bActivated) {

			timer.GetTimeDelta();

			UpdateCommand();

			updateInfo.SetProcessCommandTime( timer.GetTimeDelta() );

			updateInfo.playerCntBefore = stageData.GetPlayerCnt();
			updateInfo.npcCntBefore = stageData.GetNPCCnt();
			updateInfo.miscCntBefore = stageData.GetMISCCnt();
			updateInfo.projectileCntBefore = stageData.GetProjectileCnt();

			updateInfo.bUpdated = UpdateObject( dt );

			updateInfo.playerCnt = stageData.GetPlayerCnt();
			updateInfo.npcCnt = stageData.GetNPCCnt();
			updateInfo.miscCnt = stageData.GetMISCCnt();
			updateInfo.projectileCnt = stageData.GetProjectileCnt();

			updateInfo.SetProcessTime( timer.GetTimeDelta() );
		} else {// Game paused
			//commandQueue.Clear();
			pauseDuration -= (float)dt;
			if ( pauseDuration <= 0 ) {//������ ������.
				bActivated = true;
				listener->Paused(false);
			}
		}
		bUpdating = false;
	}
} } }
/*
#ifdef _SINGLE
#include "Stage/Preprocessor/Preprocess.h"
#include "Lunia/Resource/Resource.h"
namespace Lunia { namespace XRated {	namespace Logic {
	int BindScript(ScriptLoader::ASModule* module)
	{
		return ScriptLoader::BindScript(module);
	}

	bool Logic::CheckScript()
	{
		Database::Info::StageInfo *stageInfo = DatabaseInstance().InfoCollections.Stages.Retrieve(stageCode);
		com_ptr<Lunia::Resource::IStream> stream= Lunia::Resource::ResourceSystemInstance().OpenStream( stageInfo->ScriptName );
		std::vector<char> ScriptSource;
		int len = stream->GetSize();
		ScriptSource.resize(len);
		stream->Read( (unsigned char*)(&ScriptSource[0]), len );

		ScriptLoader::ASModule* module;
		module = new ScriptLoader::ASModule;
		//Create script engine and context
		module->engine=asCreateScriptEngine(ANGELSCRIPT_VERSION);
		if ( !module->engine ) {
			throw( Lunia_EXCEPTION((L"[CheckScript] unable to create script engine.")) );
		}

		int r;

		std::string name = StringUtil::ToASCII(stageInfo->Id);
		ScriptLoader::Output out;
		out.SetStageCode(name);
		module->engine->SetCommonMessageStream(&out);

		//��ó����
		Preprocessor::VectorOutStream VOS;
		Preprocessor::NullOutStream nullStream;
		HeadLoader fs(ScriptSource);
		Preprocessor::preprocess(name,fs,VOS, nullStream, out.GetTranslator());

		r = module->engine->AddScriptSection(0, name.c_str(), VOS.data(), (int)VOS.size(), 0);
		if ( r < 0 ) {
			throw( Lunia_EXCEPTION((L"[CheckScript] Unable to add scriptsection to engine.")) );
		}

		r = BindScript(module);
		if ( r < 0 ) {
			throw( Lunia_EXCEPTION((L"[CheckScript] Cannot bind script.")) );
		}

		r = module->engine->Build(0);

		module->engine->Release();
		delete module;
		if ( r < 0 )
			return false;
		return true;
	}

	void Logic::ReloadScript()
	{
		DatabaseInstance().InfoCollections.Stages.Retrieve(stageCode)->LoadScript();
        stageData.Reload();
	}

	
}	}	}
#endif
*/
namespace Lunia { namespace XRated {	namespace Logic {
	ILogic* CreateLogic()
	{
		return new Logic;
	}
} } }
