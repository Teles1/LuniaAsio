#pragma once
#include "Sector.h"

namespace Lunia { namespace XRated { namespace Logic {

	class SectorManager {
		std::vector<Sector*> sectors;
		Sector				invalidSector;

		// Sector
		int cols;
		int rows;
		int secWidth;
		int secHeight;

		struct Direction {
			int ld, d, rd, r, ru, u, lu, l;
		} direction;

	private :
		void MakeSector( int mapW, int mapH, int secW, int secH );
		void LinkSector( Sector* sector  );
		Sector* FindSector( int sectorIndex );
		Sector* FindSector( int indexX,int indexY );
	public :
		SectorManager() :invalidSector(0xFFFFFFFF) {			
			for( size_t i =0; i < Sector::InterestZoneCount; ++i ) {
				invalidSector.interestZone[i] = NULL;
			}
		};
		//SectorManager(int mapW, int mapH, int secW, int secH);
		~SectorManager();

		void Init( int mapW, int mapH, int secW, int secH );
		void Clear();

		int GetColCnt() const { return cols; }
		Constants::Direction GetDirection(int index);
		Sector* GetSector( const float2& pos );
		Sector* GetSector( const float3& pos );
		void GetSectorForCircle( const float3& pos,float range, std::vector<Sector*>& sectors );
		Sector* ChangeSector(Object* obj, Sector* sector=NULL);

		Sector* ObjectCreated(Actor*);
		void ObjectDestroyed(Actor*, Sector*);
		Sector* NonPlayerCreated(NonPlayer*);
		void NonPlayerDestroyed(NonPlayer*, Sector*);
		Sector* PlayerCreated(Player*);
		void PlayerDestroyed(Player*, Sector*);

	private:
		SectorManager(const SectorManager&); // unable to copy, because of potential problem of double delete at destructor
	};

} } }
