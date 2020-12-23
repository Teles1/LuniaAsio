#pragma once

namespace Lunia {
	namespace XRated {
		namespace Logic {
			class ProjectileManager;
			class Object;
			class Projectile;
			class Actor;
			class Player;
			class NonPlayer;

			typedef std::vector<Actor*> ObjectList;
			typedef std::vector<NonPlayer*> NPCList;
			typedef std::vector<Player*> PlayerList;

			class Sector {
			public:
				enum InterestZone { LeftUP = 0, UP, RightUP, Right, RightDown, Down, LeftDown, Left, InterestZoneCount };

				template<class T> struct ObjectIterator {
					typename std::vector<T*>::iterator current;
					std::vector<T*>* objects;
					Sector* mySector;
					int cnt;
					bool bEnd;
					ObjectIterator(Sector* sector, typename std::vector<T*>::iterator i)
						: mySector(sector), objects(&sector->GetObjects<T>()), current(i), cnt(InterestZone::InterestZoneCount), bEnd(true)
					{
						if (current == objects->end()) {
							++(*this);
						}
					}

					void operator++ () {
						if (current == objects->end() || (++current) == objects->end()) {
							while (cnt--) {
								Sector* sector = mySector->GetInterestZone(cnt);
								if (sector) {
									objects = &sector->GetObjects<T>();
									if (!objects->empty()) { 
										current = objects->begin();
										return; 
									}
								}
							}
							bEnd = false;
							return;
						}
					}

					T* operator* () { return *current; }
					bool IsFinished() { return bEnd; }
				};


				friend class SectorManager;
			private:
				Sector* interestZone[InterestZoneCount];
				int index;

				//RECT rt;

				PlayerList players;
				NPCList npcs;
				ObjectList objects;

				//void CalculateLastSector();

			public:
				Sector(void);
				Sector(int idx/*, int x, int y, int cx, int cy*/);
				virtual ~Sector(void);

				void Clear();

				template<class T> ObjectIterator<T> begin() {
					return ObjectIterator<T>(this, GetObjects<T>().begin()); //objects.begin());
				}
				//template<class T> T end();

				int GetIndex() const { return index; }
				Sector* GetInterestZone(int index) const { return interestZone[index]; }

				void InsertPlayer(Player* player);
				void RemovePlayer(Player* player);
				//PlayerList& GetPlayers() { return players; }

				void InsertNpc(NonPlayer* npc);
				void RemoveNpc(NonPlayer* npc);
				//NPCList& GetNonPlayers() { return npcs; }

				void InsertObject(Actor* object);
				void RemoveObject(Actor* object);
				template<class T> std::vector<T*>& GetObjects();
			};
		}
	}
}