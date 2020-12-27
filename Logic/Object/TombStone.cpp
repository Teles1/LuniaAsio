#include "TombStone.h"
#include "Player.h"
#include "../GameState.h"

namespace Lunia { namespace XRated {	namespace Logic {

	const float TombStone::ReviveRange = 60;

	TombStone::TombStone(const wchar_t* name, Lunia::XRated::Serial serial)
		: Actor(Lunia::XRated::Constants::ObjectType::TombStone), whos(name), ownerSerial(serial)/*, bInSighted(true)*/
	{
	}

	bool TombStone::Update(float /*dt*/, IGameStateDatabase* /*db*/)
	{
		//Player* player = db->RetrievePlayer(ownerSerial);
		//if (!player) {//������ player�� ������ �����ɷ� �ν��ؼ� ��������.
		//	return true;
		//}

		//if (!player->GetPlayerData().Life)
		//	return false;

		//float len = Lunia::Math::Length<float>(objectData.Position - player->GetPosition());
		//if ( !bInSighted &&  len <= ReviveRange ) {
		//	bInSighted = true;
		//	db->Revivable(player, true);
		//} else if ( bInSighted && len > ReviveRange ) {
		//	bInSighted = false;
		//	db->Revivable(player, false);
		//}
		return false;
	}

}	}	}
