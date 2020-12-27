#include "SectorManager.h"
#include "../Object/Player.h"
#include "../Object/NonPlayer.h"


namespace Lunia { namespace XRated { namespace Logic {

	//SectorManager::SectorManager(int mapW, int mapH, int secW, int secH)
	//{
	//	Init(mapW, mapH, secW, secH);
	//}


	void SectorManager::Init( int mapW, int mapH, int secW, int secH )
	{
		if (!sectors.empty()) {
			std::vector<Sector*>::iterator i, end = sectors.end();
			for (i = sectors.begin() ; i != end ; ++i)
				delete (*i);
			sectors.clear();
		}

		MakeSector(mapW, mapH, secW, secH);
		std::vector<Sector*>::iterator it;
		for( it = sectors.begin(); it != sectors.end(); ++it ) {
			LinkSector((*it));
			//(*it)->CalculateLastSector();	//나중에 object들의 list를 얻어올 때 마지막 iterator를 찾기 위해서
		}

		//방향값 계산
		direction.ld = cols - 1;
		direction.d = cols;
		direction.rd = cols + 1;
		direction.r = 1;
		direction.ru = -cols + 1;
		direction.u = -cols;
		direction.lu = -cols - 1;
		direction.l = -1;
	}

	SectorManager::~SectorManager()
	{
		if (!sectors.empty()) {
			std::vector<Sector*>::iterator i, end = sectors.end();
			for (i = sectors.begin() ; i != end ; ++i)
				delete (*i);
			sectors.clear();
		}
	}

	void SectorManager::Clear()
	{
		std::vector<Sector*>::iterator i = sectors.begin();
		std::vector<Sector*>::iterator end = sectors.end();
		for (; i != end ; ++i)
			(*i)->Clear();
	}

	void SectorManager::MakeSector( int mapW, int mapH, int secW, int secH )
	{
		secWidth = secW; secHeight = secH;

		cols = (int)ceil( double(mapW) / secW );
		rows = (int)ceil( double(mapH) / secH );

		sectors.reserve( cols * rows );

		for( int row=0; row<rows; row++ )
		{
			for( int col=0; col<cols; col++ )
			{
				int index = ( row * cols ) + col;
				//int xPos  =   col * secW;
				//int yPos  =	  row * secH;
				
				Sector* sector = new Sector( index/*, xPos, yPos, xPos+secW, yPos+secH*/ );

				//TRACE("Sector : {}, xPos = {}, yPos = {}, cxPos = {}, cyPos= {}", index, xPos, yPos, xPos+secW, yPos+secH);
				sectors.push_back( sector );
			}
		}
	}

	void SectorManager::LinkSector( Sector* sector  )
	{
		for( int i=0; i<Sector::InterestZoneCount; i++ )
		{
			switch( i )
			{
			case Sector::LeftUP:

				if( (sector->GetIndex() % cols) == 0 || (sector->GetIndex() - cols) < 0)
				{
					sector->interestZone[Sector::LeftUP] = &invalidSector;
				}else{

					int leftUp = (sector->GetIndex() - cols) - 1;

					Sector* leftUpSector  = FindSector( leftUp );
					if ( !leftUpSector ) {
						Logger::GetInstance().Exception(L"[SectorManager::LinkSector] leftUpSector not found.");
					}

					sector->interestZone[Sector::LeftUP] = leftUpSector;
				}
				break;
			case Sector::UP:

				if( (sector->GetIndex() - cols) < 0 )
					sector->interestZone[Sector::UP] = &invalidSector;
				else{
					int up = sector->GetIndex() - cols;

					Sector* upSector  = FindSector( up );
					if ( !upSector ) {
						Logger::GetInstance().Exception(L"[SectorManager::LinkSector] upSector not found.");
					}

					sector->interestZone[Sector::UP] = upSector;
				}

				break;
			case Sector::RightUP:

				if( (sector->GetIndex() % cols ) == (cols - 1) || (sector->GetIndex() - cols) < 0)
					sector->interestZone[Sector::RightUP] = &invalidSector;
				else{
					int rightUp = (sector->GetIndex() - cols) + 1;

					Sector* rightUpSector = FindSector( rightUp );
					if ( !rightUpSector ) {
						Logger::GetInstance().Exception( L"[SectorManager::LinkSector] rightUpSector not found.");
					}

					sector->interestZone[Sector::RightUP] = rightUpSector;
				}
				break;
			case Sector::Right:
				if( (sector->GetIndex() % cols ) == (cols - 1) )
					sector->interestZone[Sector::Right] = &invalidSector;
				else{
					int right = sector->GetIndex() + 1;

					Sector* rightSector  = FindSector( right );
					if ( !rightSector ) {
						Logger::GetInstance().Exception(L"[SectorManager::LinkSector] rightSector not found.");
					}

					sector->interestZone[Sector::Right] = rightSector;
				}
				break;
			case Sector::RightDown:
				if( (sector->GetIndex() % cols ) == (cols - 1) || (sector->GetIndex() +cols) >= (cols * rows) )
					sector->interestZone[Sector::RightDown] = &invalidSector;
				else{
					int rightDown = (sector->GetIndex() + cols) + 1;

					Sector* rightDownSector  = FindSector( rightDown );
					if ( !rightDownSector ) {
						Logger::GetInstance().Exception(L"[SectorManager::LinkSector] rightDownSector not found.");
					}

					sector->interestZone[Sector::RightDown] = rightDownSector;
				}
				break;
			case Sector::Down:
				if( (sector->GetIndex() + cols) >= (cols * rows) )
					sector->interestZone[Sector::Down] = &invalidSector;
				else{
					int down = (sector->GetIndex() + cols);

					Sector* downSector = FindSector( down );
					if ( !downSector ) {
						Logger::GetInstance().Exception(L"[SectorManager::LinkSector] downSector not found." );
					}

					sector->interestZone[Sector::Down] = downSector;
				}
				break;
			case Sector::LeftDown:
				if( (sector->GetIndex() % cols) == 0 || (sector->GetIndex() +cols) >= (cols * rows) )
				{
					sector->interestZone[Sector::LeftDown] = &invalidSector;
				}else{

					int leftDown = (sector->GetIndex() + cols) - 1;

					Sector* leftDownSector  = FindSector( leftDown );
					if ( !leftDownSector ) {
						Logger::GetInstance().Exception(L"[SectorManager::LinkSector] leftDownSector not found.");
					}

					sector->interestZone[Sector::LeftDown] = leftDownSector;
				}
				break;
			case Sector::Left:
				if( (sector->GetIndex() % cols) == 0 )
				{
					sector->interestZone[Sector::Left] = &invalidSector;
				}else{

					int left = sector->GetIndex() - 1;

					Sector* leftSector  = FindSector( left );
					if ( !leftSector ) {
						Logger::GetInstance().Exception(L"[SectorManager::LinkSector] leftSector not found.");
					}

					sector->interestZone[Sector::Left] = leftSector;
				}
				break;
			}
		}
	}

	Sector* SectorManager::GetSector( const float3& pos )
	{
		float xPos = pos.x;
		float yPos = pos.z;

		int col  = (int)floor( (double)xPos / secWidth  );
		int row  = (int)floor( (double)yPos / secHeight );

		
		
		if( (col < 0) || (col >= cols) || ( row< 0 ) || ( row >= rows ) ) {
			return &invalidSector;
		}
		int idx  = (row * cols) + col;
		return sectors.at(idx);		
	}

	Sector* SectorManager::GetSector( const float2& pos )
	{
		float xPos = pos.x;
		float yPos = pos.y;

		int col  = (int)floor( (double)xPos / secWidth  );
		int row  = (int)floor( (double)yPos / secHeight );

		if( (col < 0) || (col >= cols) || ( row< 0 ) || ( row >= rows ) ) {
			return &invalidSector;
		}
		int idx  = (row * cols) + col;
		
		return sectors.at(idx);
	}
	void SectorManager::GetSectorForCircle( const float3& pos,float /*range*/, std::vector<Sector*>& out )
	{
		float xPos = pos.x;
		float yPos = pos.z;

		int col  = (int)floor( (double)xPos / secWidth  );
		int row  = (int)floor( (double)yPos / secHeight );


		bool addedInvlidSector = false;
		//center
		Sector* sector = FindSector(col, row);
		if( sector != &invalidSector ) {
			out.push_back( sector );
		}else {
			if( addedInvlidSector == false ) {
				addedInvlidSector = true;
				out.push_back( sector );
			}
		}

		//left
		sector = FindSector(col - 1, row);
		if( sector != &invalidSector ) {
			out.push_back( sector );
		}else {
			if( addedInvlidSector == false ) {
				addedInvlidSector = true;
				out.push_back( sector );
			}
		}

		//left up
		sector = FindSector(col - 1, row - 1);
		if( sector != &invalidSector ) {
			out.push_back( sector );
		}else {
			if( addedInvlidSector == false ) {
				addedInvlidSector = true;
				out.push_back( sector );
			}
		}

		//left down
		sector = FindSector(col - 1, row + 1);
		if( sector != &invalidSector ) {
			out.push_back( sector );
		}else {
			if( addedInvlidSector == false ) {
				addedInvlidSector = true;
				out.push_back( sector );
			}
		}

		//up
		sector = FindSector(col, row - 1);
		if( sector != &invalidSector ) {
			out.push_back( sector );
		}else {
			if( addedInvlidSector == false ) {
				addedInvlidSector = true;
				out.push_back( sector );
			}
		}

		//down
		sector = FindSector(col, row + 1);
		if( sector != &invalidSector ) {
			out.push_back( sector );
		}else {
			if( addedInvlidSector == false ) {
				addedInvlidSector = true;
				out.push_back( sector );
			}
		}

		//right up
		sector = FindSector(col + 1, row - 1);
		if( sector != &invalidSector ) {
			out.push_back( sector );
		}else {
			if( addedInvlidSector == false ) {
				addedInvlidSector = true;
				out.push_back( sector );
			}
		}

		//right
		sector = FindSector(col + 1, row);
		if( sector != &invalidSector ) {
			out.push_back( sector );
		}else {
			if( addedInvlidSector == false ) {
				addedInvlidSector = true;
				out.push_back( sector );
			}
		}

		//right down
		sector = FindSector(col + 1, row + 1);
		if( sector != &invalidSector ) {
			out.push_back( sector );
		}else {
			if( addedInvlidSector == false ) {
				addedInvlidSector = true;
				out.push_back( sector );
			}
		}
	}

	Sector* SectorManager::FindSector( int sectorIndex )
	{
		return sectors.at(sectorIndex);
	}
	Sector* SectorManager::FindSector( int indexX,int indexY )
	{
		if( (indexX < 0) || (indexX >= cols) || ( indexY< 0 ) || ( indexY >= rows ) ) {
			return &invalidSector;
		}
		int idx  = (indexY * cols) + indexX;
		
		return sectors.at(idx);
	}

	Constants::Direction SectorManager::GetDirection(int index)
	{
		if (index == direction.ld)
			return Constants::Direction::LeftDown;
		else if (index == direction.d)
			return Constants::Direction::Down;
		else if (index == direction.rd)
			return Constants::Direction::RightDown;
		else if (index == direction.r)
			return Constants::Direction::Right;
		else if (index == direction.ru)
			return Constants::Direction::RightUp;
		else if (index == direction.u)
			return Constants::Direction::Up;
		else if (index == direction.lu)
			return Constants::Direction::LeftUp;
		else if (index == direction.l)
			return Constants::Direction::Left;
		else {
			//Lunia_INFO((L"[SectorManager::GetDirection] Wrong direction."));
		}
		return Constants::Direction::None;
	}

	Sector* SectorManager::ChangeSector(Object* obj, Sector* sector)
	{
		float3 pos = obj->GetPosition();
		Sector* newSector = GetSector(float2(pos.x, pos.z));
		if (sector != newSector) {//sector changed
			switch (obj->GetType()) {
			case Constants::ObjectType::Player :
				sector->RemovePlayer((Player*)obj);
				newSector->InsertPlayer((Player*)obj);
				break;
			case Constants::ObjectType::NonPlayer :
				sector->RemoveNpc((NonPlayer*)obj);
				newSector->InsertNpc((NonPlayer*)obj);
				break;
			case Constants::ObjectType::Misc:
			case Constants::ObjectType::TombStone :
				sector->RemoveObject((Actor*)obj);
				newSector->InsertObject((Actor*)obj);
				break;
			default :
				break;
			}
			return newSector;
		}
		return NULL;
	}

	Sector* SectorManager::ObjectCreated(Actor* obj)
	{
		Sector* sector = GetSector(float2(obj->GetPosition().x, obj->GetPosition().z));
		sector->InsertObject(obj);
		return sector;
	}

	void SectorManager::ObjectDestroyed(Actor* obj, Sector* sector)
	{
		sector->RemoveObject(obj);
	}

	Sector* SectorManager::NonPlayerCreated(NonPlayer* obj)
	{
		Sector* sector = GetSector(float2(obj->GetPosition().x, obj->GetPosition().z));
		sector->InsertNpc(obj);
		return sector;
	}

	void SectorManager::NonPlayerDestroyed(NonPlayer* obj, Sector* sector)
	{
		sector->RemoveNpc(obj);
	}

	Sector* SectorManager::PlayerCreated(Player* obj)
	{
		Sector* sector = GetSector(float2(obj->GetPosition().x, obj->GetPosition().z));
		sector->InsertPlayer(obj);
		return sector;
	}

	void SectorManager::PlayerDestroyed(Player* obj, Sector* sector)
	{
		sector->RemovePlayer(obj);
	}

} } }

