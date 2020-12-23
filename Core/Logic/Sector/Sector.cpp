#include "Sector.h"


namespace Lunia {
	namespace XRated {
		namespace Logic {

			Sector::Sector(void)
			{
				players.reserve(4);
				npcs.reserve(10);
				objects.reserve(5);
			}

			Sector::Sector(int idx/*, int x, int y, int cx, int cy*/)
			{
				index = idx;

				//rt.left		= x;
				//rt.top		= y;
				//rt.right	= cx;
				//rt.bottom	= cy;
			}

			Sector::~Sector(void)
			{
			}

			void Sector::Clear()
			{
				players.clear();
				npcs.clear();
				objects.clear();
			}

			template<> std::vector<Actor*>& Sector::GetObjects() { return objects; }
			template<> std::vector<NonPlayer*>& Sector::GetObjects() { return npcs; }
			template<> std::vector<Player*>& Sector::GetObjects() { return players; }


			void Sector::InsertPlayer(Player* player)
			{
				players.push_back(player);
			}

			void Sector::RemovePlayer(Player* player)
			{
				int cnt = players.size();
				while (cnt--) {
					if (players[cnt] == player) {
						players[cnt] = players.back();
						players.pop_back();
						return;
					}
				}
			}

			void Sector::InsertNpc(NonPlayer* npc)
			{
				npcs.push_back(npc);
			}

			void Sector::RemoveNpc(NonPlayer* npc)
			{
				int cnt = npcs.size();
				while (cnt--) {
					if (npcs[cnt] == npc) {
						npcs[cnt] = npcs.back();
						npcs.pop_back();
						return;
					}
				}
			}

			void Sector::InsertObject(Actor* object)
			{
				objects.push_back(object);
			}

			void Sector::RemoveObject(Actor* object)
			{
				int cnt = objects.size();
				while (cnt--) {
					if (objects[cnt] == object) {
						objects[cnt] = objects.back();
						objects.pop_back();
						return;
					}
				}
			}

		}
	}
}
