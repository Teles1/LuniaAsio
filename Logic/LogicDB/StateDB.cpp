#include "StateDB.h"
#include <Info/Info/Database.h>
#include "../State/States.h"
#include "../Object/Actor.h"
#include "../State/StateDamage.h"
#include "../State/StateBundle/StateBundle.h"


namespace Lunia { namespace XRated { namespace Logic {

	StateDB::~StateDB()
	{
		Clear();
	}

	void StateDB::Clear()
	{
		StateMap::iterator i = stateNPC.begin();
		std::vector<Stat::IState*>::iterator is;
		while ( i != stateNPC.end() ) {
			is = (*i).second.begin();
			while ( is != (*i).second.end() ) {
				delete *is;
				++is;
			}
			i->second.clear();
			++i;
		}

		i = stateItem.begin();
		while ( i != stateItem.end() ) {
			is = (*i).second.begin();
			while ( is != (*i).second.end() ) {
				delete *is;
				++is;
			}
			i->second.clear();
			++i;
		}

		i = stateStateBundle.begin();
		while ( i != stateStateBundle.end() ) {
			is = (*i).second.begin();
			while ( is != (*i).second.end() ) {
				delete *is;
				++is;
			}
			i->second.clear();
			++i;
		}

		i = stateProjectile.begin();
		while ( i != stateProjectile.end() ) {
			is = (*i).second.begin();
			while ( is != (*i).second.end() ) {
				delete *is;
				++is;
			}
			i->second.clear();
			++i;
		}
	}

	void StateDB::InitNPC()
	{
		//NPC state들을 만들어 준다.
		std::vector<std::wstring> l;
		Database::DatabaseInstance().InfoCollections.Npcs.GetNPCList(l); //NPC list를 얻어온다.
		Database::Info::NonPlayerInfo* npcInfo;
		for ( std::vector<std::wstring>::iterator i = l.begin() ; i != l.end() ; ++i ) {//NPC리스트를 돌면서 state list를 얻어온다.
			npcInfo = Database::DatabaseInstance().InfoCollections.Npcs.Retrieve((*i).c_str());
			if ( !npcInfo ) {
				Logger::GetInstance().Warn( L"[StateDB::InitNPC] unable to find npc info [{0}]", (*i).c_str() );
				continue;
			}

			//npc map
			stateNPC[ npcInfo->HashcodeName ] = std::vector<Stat::IState*>();
			std::vector<Stat::IState*>& iList = stateNPC[ npcInfo->HashcodeName ];

			//StateList
			std::vector<Database::Info::ActionInfo::State>::iterator iState = npcInfo->States.begin(); //Database::Info::NonPlayerInfo::States
			while( iState != npcInfo->States.end() ) { //State들을 만든다.
				Stat::IState* state = Stat::CreateState( (*iState).type, (Player*)NULL, &(*iState).params, &(*iState).optionalStates);
				if ( !state ) {
					Logger::GetInstance().Warn( L"[StateDB::InitNPC] unable to create state template. [{0}]", npcInfo->Name.c_str() );
				} else {
					iList.push_back( state ); //만들어진 state를 list에 추가한다.
				}

				++iState;
			}
		}
	}

	void StateDB::InitItem()
	{
		//Item state들을 만들어 준다.
		std::vector<std::wstring> l;
		Database::DatabaseInstance().InfoCollections.Items.GetItemList(l); //Item list를 얻어온다.
		Database::Info::ItemInfo* itemInfo;
		for ( std::vector<std::wstring>::iterator i = l.begin() ; i != l.end() ; ++i ) {//Item 리스트를 돌면서 state list를 얻어온다.
			itemInfo = Database::DatabaseInstance().InfoCollections.Items.Retrieve((*i).c_str());
			if ( !itemInfo ) {
				Logger::GetInstance().Warn( L"[StateDB::InitItem] unable to find item info [{0}]", (*i).c_str() );
				continue;
			}

			//item map 
			stateItem[ itemInfo->Hash ] = std::vector<Stat::IState*>();
			std::vector<Stat::IState*>& iList = stateItem[ itemInfo->Hash ];

			std::vector<Database::Info::ActionInfo::State>::iterator iState = itemInfo->States.begin(); //Database::Info::ItemInfo::States
			while( iState != itemInfo->States.end() ) { 
				if ((*iState).type != Database::Info::StateInfo::NOLOGIC && (*iState).type != Database::Info::StateInfo::AUTOACTION)
				{
					Stat::IState* state = Stat::CreateState( (*iState).type, (Player*)NULL, &(*iState).params, &(*iState).optionalStates);
					if ( !state ) {
						Logger::GetInstance().Warn( L"[StateDB::InitItem] unable to create state template. [{0}]", itemInfo->Id.c_str() );
					} else {
						iList.push_back( state );
					}
				}
				++iState;
			}
		}
	}

	void StateDB::InitStateBundle()
	{
		//StateBundle state
		std::vector<std::wstring> l;
		Database::DatabaseInstance().InfoCollections.StateBundles.GetList(l);
		Database::Info::StateBundleInfo* info;
		for ( std::vector<std::wstring>::iterator i = l.begin() ; i != l.end() ; ++i ) {
			info = Database::DatabaseInstance().InfoCollections.StateBundles.Retrieve( (*i).c_str() );
			if ( !info ) {
				Logger::GetInstance().Warn( L"[StateDB::InitStateBundle] unable to find statebundle info [{0}]", (*i).c_str() );
				continue;
			}

			//map에 statebundle
			stateStateBundle[ info->hash ] = std::vector<Stat::IState*>();
			std::vector<Stat::IState*>& iList = stateStateBundle[ info->hash ];

			//StateList
			std::vector<Database::Info::StateBundleInfo::State>::iterator iState = info->states.begin();
			while( iState != info->states.end() ) {
				if ( ValidateBundleState((*iState).type) ) {
					Stat::IState* state = Stat::CreateState( (*iState).type, (Player*)NULL, &(*iState).params, NULL);
					if ( !state ) {
						Logger::GetInstance().Warn( L"[StateDB::InitStateBundle] unable to create state template. [{0}]", info->name.c_str() );
					} else {
						iList.push_back( state );
					}
				}
				++iState;
			}
		}
	}

	void StateDB::InitProjectile()
	{
		//Projectile state들을 만들어 준다.
		std::vector<std::wstring> l;
		Database::DatabaseInstance().InfoCollections.Projectiles.GetList(l);
		Database::Info::ProjectileInfo* info;
		for ( std::vector<std::wstring>::iterator i = l.begin() ; i != l.end() ; ++i ) {
			info = Database::DatabaseInstance().InfoCollections.Projectiles.Retrieve( (*i).c_str() );
			if ( !info ) {
				Logger::GetInstance().Warn( L"[StateDB::InitProjectile] unable to find projectile info [{0}]", (*i).c_str() );
				continue;
			}

			stateProjectile[ info->Hash ] = std::vector<Stat::IState*>();
			std::vector<Stat::IState*>& iList = stateProjectile[ info->Hash ];

			//StateList
			std::vector<Database::Info::ActionInfo::State>::iterator iState = info->States.begin();
			while( iState != info->States.end() ) {
				Stat::IState* state = Stat::CreateState( (*iState).type, (Projectile*)NULL, (Actor*)NULL, &(*iState).params );
				if ( !state ) {
					Logger::GetInstance().Warn( L"[StateDB::InitProjectile] unable to create state template. [{0}]", info->Name.c_str() );
				} else {
					iList.push_back( state );
				}
				++iState;
			}
		}
	}

	void StateDB::InitCashItem()
	{
		//Projectile state
		std::vector<uint32> l;
		Database::DatabaseInstance().InfoCollections.EnchantTables.GetCashEnchantList(l);

		for ( std::vector<uint32>::iterator i = l.begin() ; i != l.end() ; ++i ) {
			std::pair< Database::Enchant::CashEnchantInfo*, Database::Enchant::CashEnchantInfo* > info = Database::DatabaseInstance().InfoCollections.EnchantTables.GetCashEnchantInfo( (*i), 0 );
			if ( !info.first ) {
				Logger::GetInstance().Warn( L"[StateDB::InitCashItem] unable to find cash enchant info [{0}]", (*i) );
				continue;
			}

			//stateCashItems.insert( std::map<uint32, ItemStates>::value_type((*i), ItemStates(&info->StateBundles)) );
			ItemStates& item = stateCashItems[(*i)];

			//StateList를 돌면서 만들어 주자.
			std::vector<Database::Info::ActionInfo::State>::iterator iState = info.first->States.begin();
			while( iState != info.first->States.end() ) {
				Stat::IState* state = Stat::CreateState( (*iState).type, (Player*)NULL, &(*iState).params, &(*iState).optionalStates );
				if ( !state ) {
					Logger::GetInstance().Warn( L"[StateDB::InitCashItem] unable to create state template. [{0}]", (*i) );
				} else {
					if ( iState->startTime == -1 )
						item.useStates.push_back( state ); 
					else
						item.equipStates.push_back( state );
				}
				++iState;
			}
		}
	}


	void StateDB::Init()
	{
		Clear();

		InitNPC();
		InitItem();
		InitStateBundle();
		InitProjectile();
		InitCashItem();
	}

	bool StateDB::ValidateBundleState(Database::Info::StateInfo::Type type) const
	{
		switch ( type ) {
		case Database::Info::StateInfo::Type::AREAHEAL :
		case Database::Info::StateInfo::Type::ATTACK :
		case Database::Info::StateInfo::Type::CHANGEACTION :
		case Database::Info::StateInfo::Type::CHARGE :
		case Database::Info::StateInfo::Type::CLEAR :
		case Database::Info::StateInfo::Type::CREATENPC :
		case Database::Info::StateInfo::Type::CREATEOBJECT :
		case Database::Info::StateInfo::Type::CREATEPROJECTILE :
		case Database::Info::StateInfo::Type::DAMAGE :
		case Database::Info::StateInfo::Type::DESTROYENEMY :
		case Database::Info::StateInfo::Type::INCLIFE :
		case Database::Info::StateInfo::Type::KNOCKBACK :
		case Database::Info::StateInfo::Type::MAKESTUN:
		case Database::Info::StateInfo::Type::RESETCOOLDOWN :
		case Database::Info::StateInfo::Type::RESURRECTION :
		case Database::Info::StateInfo::Type::SELFHEAL :
		case Database::Info::StateInfo::Type::SLEEP :
		case Database::Info::StateInfo::Type::TAUNT :
			Logger::GetInstance().Warn(L"[StateDB::CheckBundleState] [{0}] This state type cannot be added to StateBundle", (int)type);
			return false;
		}
		return true;
	}

	void StateDB::AddNPCState(uint32 code, Object* obj)
	{
#ifdef _SINGLE
		{
			StateMap::const_iterator i=stateNPC.find(code);
			if (i==stateNPC.end() )	{
				InitNPCObject(code);				
			}
		}
#endif
		StateMap::const_iterator i=stateNPC.find(code);
		if (i==stateNPC.end())
		{
			Logger::GetInstance().Exception(L"unable to create NPC state({0})", code);
		}

		for(std::vector<Stat::IState*>::const_iterator j=i->second.begin(), end=i->second.end(); j!=end; ++j)
			obj->AddState( Stat::CreateState( *j, obj ) );
	}

	void StateDB::AddItemState(uint32 code, Object* obj)
	{
#ifdef _SINGLE
		{
			StateMap::const_iterator i=stateItem.find(code);
			if (i==stateItem.end())
			{
				InitItemObject(code);
			}
		}
#endif
		StateMap::const_iterator i=stateItem.find(code);
		if (i==stateItem.end())
		{
			Logger::GetInstance().Exception(L"unable to create Item state({0})", code);
		}

		//StateList
		for(std::vector<Stat::IState*>::const_iterator j=i->second.begin(), end=i->second.end(); j!=end; ++j)
			obj->AddState( Stat::CreateState( *j, obj ) );
	}

	void StateDB::AddItemState(uint32 code, Object* obj, std::vector<Stat::IState*>& l)
	{
#ifdef _SINGLE
		{
			StateMap::const_iterator i=stateItem.find(code);
			if (i==stateItem.end())
			{
				InitItemObject(code);
			}
		}
#endif
		StateMap::const_iterator i=stateItem.find(code);
		if (i==stateItem.end())
		{
			Logger::GetInstance().Exception(L"unable to create Item state({0})", code);
		}

		//StateList
		for(std::vector<Stat::IState*>::const_iterator j=i->second.begin(), end=i->second.end(); j!=end; ++j)
		{
			Stat::IState* state=Stat::CreateState( *j, obj );
			obj->AddState( state );
			l.push_back(state);
		}
	}

	void StateDB::AddCashItemState(uint32 id, Object* obj)
	{
#ifdef _SINGLE
		{
			std::map<uint32, ItemStates>::const_iterator i=stateCashItems.find(id);
			if (i==stateCashItems.end())
			{
				InitCashItemObject(id);
			}
		}
#endif
		std::map<uint32, ItemStates>::const_iterator i=stateCashItems.find(id);
		if (i==stateCashItems.end())
		{
			Logger::GetInstance().Exception(L"unable to create cash Item state({0})", id);
		}

		//StateList를 돌면서 만들어 주자.
		for(std::vector<Stat::IState*>::const_iterator j=i->second.useStates.begin(), end=i->second.useStates.end(); j!=end; ++j)
			obj->AddState( Stat::CreateState( *j, obj ) );
	}

	void StateDB::AddCashItemState(uint32 id, Object* obj, std::vector<Stat::IState*>& l)
	{
#ifdef _SINGLE
		{
			std::map<uint32, ItemStates>::const_iterator i=stateCashItems.find(id);
			if (i==stateCashItems.end())
			{
				InitCashItemObject(id);
			}
		}
#endif
		std::map<uint32, ItemStates>::const_iterator i=stateCashItems.find(id);
		if (i==stateCashItems.end())
		{
			Logger::GetInstance().Exception(L"unable to create cash Item state({0})", id);
		}

		//StateList를 돌면서 만들어 주자.
		for(std::vector<Stat::IState*>::const_iterator j=i->second.equipStates.begin(), end=i->second.equipStates.end(); j!=end; ++j)
		{
			Stat::IState* state = Stat::CreateState( *j, obj );
			obj->AddState( state );
            l.push_back(state);
		}
	}

	void StateDB::AddStateBundleState(uint32 code, Object* obj, Stat::StateBundle* stateBundle, Lunia::XRated::Serial creator)
	{
#ifdef _SINGLE
		{
			StateMap::const_iterator i=stateStateBundle.find(code);
			if (i==stateStateBundle.end())
			{
				InitStateBundleObject(code);
			}
		}
#endif
		StateMap::const_iterator i=stateStateBundle.find(code);
		if (i==stateStateBundle.end())
		{
			Logger::GetInstance().Error(L"unable to create state bundle state({0})", code);
			return;
		}

		//StateList를 돌면서 만들어 주자.
		for(std::vector<Stat::IState*>::const_iterator j=i->second.begin(), end=i->second.end(); j!=end; ++j)
		{
			Stat::IState* state = Stat::CreateState( *j, obj, creator );
			obj->AddState( state );
            stateBundle->AddState(state);
		}
	}

	Stat::IState* StateDB::CreateState(Type type, uint32 /*code*/, Object* /*obj*/, int /*cnt*/)
	{
		switch (type) {
		case Type::TypeNpc :
			return NULL;
		case Type::TypeItem :
			return NULL;
		case Type::TypeStateBundle :
			return NULL;
		case Type::TypeProjectile :
			return NULL;
		}
		return NULL;
	}

	Stat::IState* StateDB::CreateProjectileState(uint32 code, Projectile* obj, Actor* target,int cnt, IGameStateDatabase* db, uint32 damageStateTypeUniqueSerial)
	{
#ifdef _SINGLE
		{
			StateMap::const_iterator i=stateProjectile.find(code);
			if (i==stateProjectile.end())
			{
				InitProjectileObject(code);
			}
		}
#endif
		StateMap::const_iterator i=stateProjectile.find(code);
		if (i==stateProjectile.end())
		{
			Logger::GetInstance().Exception(L"unable to create projectile state({})", code);
		}
		if (cnt >= (int)i->second.size()) {
			Logger::GetInstance().Info( L"[StateDB::CreateProjectileState] wrong index. [{0}/{1}]", code, cnt );
			return NULL;
		}

		Stat::IState* state = i->second[cnt];

		switch ( state->GetType() ) {
		case Database::Info::StateInfo::Type::ATTACK :
			return Stat::CreateState(state, obj);
		case Database::Info::StateInfo::Type::CREATEPROJECTILE :
			return Stat::CreateState(state, obj);
		case Database::Info::StateInfo::Type::DAMAGE :
			{
				Stat::IState* newState = Stat::CreateState(state, target);
				((Stat::Damage*)newState)->Initialize(obj, db, damageStateTypeUniqueSerial );
				return newState;
			}
			break;
		case Database::Info::StateInfo::Type::AREAHEAL:
			return Stat::CreateState(state, obj);
		}
		return Stat::CreateState(state, target);
	}
#ifdef _SINGLE
	void StateDB::InitNPCObject(uint32 hash)
	{
		Database::Info::NonPlayerInfo* npcInfo = Database::DatabaseInstance().InfoCollections.Npcs.Retrieve(hash);
		if ( !npcInfo ) {
			Lunia_EXCEPTION(( L"[StateDB::InitNPC] unable to find npc info [{}]", hash ));
			return;
		}

		//npc map 에 npc를 추가 하고 참조를 얻어온다.
		stateNPC[ npcInfo->HashcodeName ] = std::vector<Stat::IState*>();
		std::vector<Stat::IState*>& iList = stateNPC[ npcInfo->HashcodeName ];

		//StateList를 돌면서 만들어 주자.
		std::vector<Database::Info::ActionInfo::State>::iterator iState = npcInfo->States.begin(); //Database::Info::NonPlayerInfo::States
		while( iState != npcInfo->States.end() ) { //State들을 만든다.
			Stat::IState* state = Stat::CreateState( (*iState).type, (Player*)NULL, &(*iState).params, &(*iState).optionalStates);
			if ( !state ) {
				Lunia_IMPORTANT(( L"[StateDB::InitNPC] unable to create state template. [{}]", npcInfo->Name.c_str() ));
			} else {
				iList.push_back( state ); //만들어진 state를 list에 추가한다.
			}

			++iState;
		}
	}
	void StateDB::InitItemObject(uint32 hash)
	{
		Database::Info::ItemInfo* itemInfo = Database::DatabaseInstance().InfoCollections.Items.Retrieve(hash);
		if ( !itemInfo ) {
			Lunia_EXCEPTION(( L"[StateDB::InitItem] unable to find item info [{}]", hash ));
			return;
		}

		//item map 에 item을 추가 하고 참조를 얻어온다.
		stateItem[ itemInfo->Hash ] = std::vector<Stat::IState*>();
		std::vector<Stat::IState*>& iList = stateItem[ itemInfo->Hash ];

		//StateList를 돌면서 만들어 주자.
		std::vector<Database::Info::ActionInfo::State>::iterator iState = itemInfo->States.begin(); //Database::Info::ItemInfo::States
		while( iState != itemInfo->States.end() ) { //State들을 만든다.
			if ((*iState).type != Database::Info::StateInfo::NOLOGIC)
			{
				Stat::IState* state = Stat::CreateState( (*iState).type, (Player*)NULL, &(*iState).params, &(*iState).optionalStates);
				if ( !state ) {
					Lunia_IMPORTANT(( L"[StateDB::InitItem] unable to create state template. [{}]", itemInfo->Id.c_str() ));
				} else {
					iList.push_back( state ); //만들어진 state를 list에 추가한다.
				}
			}
			++iState;
		}
	}
	void StateDB::InitStateBundleObject(uint32 hash)
	{
		Database::Info::StateBundleInfo* info = Database::DatabaseInstance().InfoCollections.StateBundles.Retrieve( hash );
		if ( !info ) {
			Lunia_EXCEPTION(( L"[StateDB::InitStateBundle] unable to find statebundle info [{}]", hash ));
			return;
		}

		//map에 statebundle을 추가 하고 참조를 얻어온다.
		stateStateBundle[ info->hash ] = std::vector<Stat::IState*>();
		std::vector<Stat::IState*>& iList = stateStateBundle[ info->hash ];

		//StateList를 돌면서 만들어 주자.
		std::vector<Database::Info::StateBundleInfo::State>::iterator iState = info->states.begin();
		while( iState != info->states.end() ) { //State들을 만든다.
			if ( ValidateBundleState((*iState).type) ) { //들어갈 수 있는 state인가를 검사
				Stat::IState* state = Stat::CreateState( (*iState).type, (Player*)NULL, &(*iState).params, NULL);
				if ( !state ) {
					Lunia_IMPORTANT(( L"[StateDB::InitStateBundle] unable to create state template. [{}]", info->name.c_str() ));
				} else {
					iList.push_back( state ); //만들어진 state를 list에 추가한다.
				}
			}
			++iState;
		}
	}
	void StateDB::InitProjectileObject(uint32 hash)
	{
		Database::Info::ProjectileInfo* info = Database::DatabaseInstance().InfoCollections.Projectiles.Retrieve( hash );
		if ( !info ) {
			Lunia_EXCEPTION(( L"[StateDB::InitProjectile] unable to find projectile info [{}]", hash ));
			return;
		}

		//map에 Projectile을 추가 하고 참조를 얻어온다.
		stateProjectile[ info->Hash ] = std::vector<Stat::IState*>();
		std::vector<Stat::IState*>& iList = stateProjectile[ info->Hash ];

		//StateList를 돌면서 만들어 주자.
		std::vector<Database::Info::ActionInfo::State>::iterator iState = info->States.begin();
		while( iState != info->States.end() ) { //State들을 만든다.
			Stat::IState* state = Stat::CreateState( (*iState).type, (Projectile*)NULL, (Actor*)NULL, &(*iState).params );
			if ( !state ) {
				Lunia_IMPORTANT(( L"[StateDB::InitProjectile] unable to create state template. [{}]", info->Name.c_str() ));
			} else {
				iList.push_back( state ); //만들어진 state를 list에 추가한다.
			}
			++iState;
		}
	}
	void StateDB::InitCashItemObject(uint32 hash)
	{
		std::pair< Database::Enchant::CashEnchantInfo*, Database::Enchant::CashEnchantInfo* > info = Database::DatabaseInstance().InfoCollections.EnchantTables.GetCashEnchantInfo( hash, 0 );
		if ( !info.first ) {
			Lunia_IMPORTANT(( L"[StateDB::InitCashItem] unable to find cash enchant info [{}]", hash ));
			return;
		}

		//stateCashItems.insert( std::map<uint32, ItemStates>::value_type((*i), ItemStates(&info->StateBundles)) );
		ItemStates& item = stateCashItems[hash];

		//StateList를 돌면서 만들어 주자.
		std::vector<Database::Info::ActionInfo::State>::iterator iState = info.first->States.begin();
		while( iState != info.first->States.end() ) { //State들을 만든다.
			Stat::IState* state = Stat::CreateState( (*iState).type, (Player*)NULL, &(*iState).params, &(*iState).optionalStates );
			if ( !state ) {
				Lunia_EXCEPTION(( L"[StateDB::InitCashItem] unable to create state template. [{}]", hash ));
			} else {
				//만들어진 state를 list에 추가한다.
				if ( iState->startTime == -1 )
					item.useStates.push_back( state ); 
				else
					item.equipStates.push_back( state );
			}
			++iState;
		}
	}
#endif

}	}	}





