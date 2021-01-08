#include "ItemRequirementQueue.h"

namespace Lunia { namespace XRated { namespace StageServer {


	ItemRequirementQue::ItemRequirementQue()
	{
		Clear();
	}


	ItemRequirementQue::~ItemRequirementQue()
	{
	}

	
	bool ItemRequirementQue::RequireItemUse(uint32 bag, uint32 pos)
	{
		if(requirement != NONE)
			return false;

		requirement = USEITEM;
		useRequirement.bag_ = bag;
		useRequirement.pos_ = pos;
		return true;
	}


	bool ItemRequirementQue::RequireDropEquip(uint32 pos)
	{
		if(requirement != NONE)
			return false;

		requirement = DROPEQUIP;
		dropEquipRequirement.pos_ = pos;
		return true;
	}


	bool ItemRequirementQue::RequireMoveEquip(bool isEquip, uint32 equipID, int equipPos, uint32 invenID, int invenPos, int bag)
	{
		if(requirement != NONE)
			return false;

		requirement = MOVEEQUIP;
		moveEquipRequirement.isEquip_	= isEquip;
		moveEquipRequirement.equipID_	= equipID;
		moveEquipRequirement.equipPos_	= equipPos;
		moveEquipRequirement.invenID_	= invenID;
		moveEquipRequirement.invenPos_	= invenPos;
		moveEquipRequirement.bag_		= bag;
		return true;
	}

	bool ItemRequirementQue::RequireEquipToEquip(int fromEquipPos, int toEquipPos)
	{
		if(requirement != NONE)
			return false;

		requirement = EQUIPTOEQUIP;				
		equipToEquipRequirement.fromEquipPos_	= fromEquipPos;		
		equipToEquipRequirement.toEquipPos_	= toEquipPos;		
		return true;
	}

	bool ItemRequirementQue::RequireSwapEquip(uint8 newEquipSet)
	{
		if(requirement != NONE)
			return false;

		requirement = SWAPEQUIP;	
		swapEquipRequirement.newEquipset = newEquipSet;
		
		return true;
	}

	bool ItemRequirementQue::RequireSwapCashEquip(uint8 newEquipSet)
	{
		if(requirement != NONE)
			return false;

		requirement = SWAPCASHEQUIP;	
		swapCashEquipRequirement.newEquipset = newEquipSet;
		
		return true;
	}

	bool ItemRequirementQue::RequireDirectlyEquip(uint32 equipID, uint64 equipIns, uint64 equipSerial, int32 pos)
	{
		if(requirement != NONE)
			return false;

		requirement = DIRECTLYEQUIP;
		directlyEquipRequirement.equipID_ = equipID;
		directlyEquipRequirement.equipIns_ = equipIns;
		directlyEquipRequirement.equipSerial_ = equipSerial;
		directlyEquipRequirement.pos_ = pos;

		return true;
	}
	//bool ItemRequirementQue::RequireDirectlyEquip(uint32 equipID, int pos)
	//{
	//	if(requirement != NONE)
	//		return false;

	//	requirement = DIRECTLYEQUIP;
	//	directlyEquipRequirement.equipID_ = equipID;
	//	directlyEquipRequirement.pos_ = pos;

	//	return true;
	//}
	bool ItemRequirementQue::RequirePetMoveEquip(bool isEquip, XRated::GlobalSerial petSerial, uint32 equipID, int equipPos, uint32 invenID, int invenPos, int bag)
	{
		if(requirement != NONE)
			return false;

		requirement = PETMOVEEQUIP;
		petMoveEquipRequirement.petSerial = petSerial;
		petMoveEquipRequirement.isEquip_	= isEquip;
		petMoveEquipRequirement.equipID_	= equipID;
		petMoveEquipRequirement.equipPos_	= equipPos;
		petMoveEquipRequirement.invenID_	= invenID;
		petMoveEquipRequirement.invenPos_	= invenPos;
		petMoveEquipRequirement.bag_		= bag;
		return true;
	}

	bool ItemRequirementQue::RequireMovePetEquipInPetInven(bool isEquip, XRated::GlobalSerial petSerial, uint32 equipID, int equipPos, uint32 itemIdInPetInven, int invenPos)
	{
		if(requirement != NONE)
			return false;

		requirement = PETEQUIPMOVEINPETINVEN;
		movePetEquipInPetInvenRequirement.PetSerial = petSerial;
		movePetEquipInPetInvenRequirement.IsEquip = isEquip;
		movePetEquipInPetInvenRequirement.EquipmentItemId = equipID;
		movePetEquipInPetInvenRequirement.EquipPos = equipPos;
		movePetEquipInPetInvenRequirement.ItemIdInPetInven = itemIdInPetInven;
		movePetEquipInPetInvenRequirement.InvenPos = invenPos;

		return true;
	}

	bool ItemRequirementQue::RequireUseItemInPetInven(XRated::GlobalSerial petSerial, uint32 positionType, uint32 position)
	{
		if(requirement != NONE)
			return false;

		requirement = USEITEMINPETINVEN;
		useItemInPetInvenRequirement.PetSerial = petSerial;
		useItemInPetInvenRequirement.Position = position;
		useItemInPetInvenRequirement.PositionType = positionType;

		return true;
	}

	PetMoveEquipRequirement* ItemRequirementQue::PetEquipResult()
	{
		if( requirement == PETMOVEEQUIP ) {
			return &petMoveEquipRequirement;
		}
		return NULL;
	}
	bool ItemRequirementQue::IsEmpty()
	{
		return (requirement == NONE)?true:false;
	}


	IItemResult* ItemRequirementQue::Result()
	{
		switch(requirement)
		{
		case NONE:		return NULL;
		case USEITEM:	return &useRequirement;
		case DROPEQUIP:	return &dropEquipRequirement;
		case MOVEEQUIP:	return &moveEquipRequirement;
		case EQUIPTOEQUIP: return &equipToEquipRequirement;
		case DIRECTLYEQUIP: return &directlyEquipRequirement;
		case PETMOVEEQUIP: return &petMoveEquipRequirement;
		case PETEQUIPMOVEINPETINVEN: return &movePetEquipInPetInvenRequirement;
		case USEITEMINPETINVEN: return &useItemInPetInvenRequirement;
		case SWAPEQUIP:			return &swapEquipRequirement;
		case SWAPCASHEQUIP:		return &swapCashEquipRequirement;
		default:		return NULL;
		}
	}


	void ItemRequirementQue::Clear()
	{
		requirement = NONE;
	}


} } }