#pragma once
#include <Core/GameConstants.h>
namespace Lunia { namespace XRated { namespace StageServer {

	enum Requirement { NONE = 0, USEITEM, DROPEQUIP, MOVEEQUIP, EQUIPTOEQUIP, DIRECTLYEQUIP, PETMOVEEQUIP, PETEQUIPMOVEINPETINVEN, USEITEMINPETINVEN, SWAPEQUIP, SWAPCASHEQUIP };

	struct IItemResult
	{
		virtual Requirement	Kind()		{ return NONE; }
		// UseRequirement
		virtual uint32	Used_Bag()		{ return 0; }
		virtual uint32	Used_Pos()		{ return 0; }
		// DropEquipRequirement
		virtual uint32	Dropd_Pos()		{ return 0; }
		// EquipRequirement
		virtual bool	Equiped_Equip()	{ return false; }
		virtual uint32	Equiped_EID()	{ return 0; }
		virtual uint32	Equiped_IID()	{ return 0; }
		virtual int		Equiped_EPos()	{ return 0; }
		virtual int		Equiped_IPos()	{ return 0; }
		virtual int		Equiped_IBag()	{ return 0; }
		virtual uint64	Used_ExInfo()	{ return 0; }
		virtual uint8	Equiped_SetIndex() { return 0; }
		virtual uint64	Equiped_EIns() { return 0; }
		virtual uint64	Equiped_ESerial() { return 0; }
	};


	struct UseRequirement : public IItemResult
	{
		uint32 bag_;
		uint32 pos_;

		Requirement	Kind()	{ return USEITEM; }
		uint32 Used_Bag()	{ return bag_; }
		uint32 Used_Pos()	{ return pos_; }
	};


	struct DropEquipRequirement : public IItemResult
	{
		uint32 pos_;

		Requirement	Kind()	{ return DROPEQUIP; }
		uint32 Dropd_Pos()	{ return pos_; }
	};


	struct MoveEquipRequirement : public IItemResult
	{
		bool	isEquip_; // inventory -> equip
		uint32	equipID_;
		uint32	invenID_;
		int		equipPos_;
		int		invenPos_;
		int		bag_;

		Requirement	Kind()		{ return MOVEEQUIP; }
		bool	Equiped_Equip()	{ return isEquip_; }
		uint32	Equiped_EID()	{ return equipID_; }
		uint32	Equiped_IID()	{ return invenID_; }
		int		Equiped_EPos()	{ return equipPos_; }
		int		Equiped_IPos()	{ return invenPos_; }
		int		Equiped_IBag()	{ return bag_; }
	};

	struct EquipToEquipRequirement : public IItemResult
	{		
		int		fromEquipPos_;
		int		toEquipPos_;		

		Requirement	Kind()		{ return EQUIPTOEQUIP; }
		bool	Equiped_Equip()	{ return true; }		
		int		Equiped_EPos()	{ return fromEquipPos_; }
		int		Equiped_IPos()	{ return toEquipPos_; }		
	};

	struct SwapEquipRequirement : public IItemResult
	{
		uint8 newEquipset;

		Requirement	Kind()		{ return SWAPEQUIP; }
		uint8		Equiped_SetIndex() { return newEquipset; }
	};

	struct SwapCashEquipRequirement : public IItemResult
	{
		uint8 newEquipset;

		Requirement Kind()		{ return SWAPCASHEQUIP; }
		uint8		Equiped_SetIndex() { return newEquipset; }
	};

	struct DirectlyEquipRequirement : public IItemResult
	{
		int pos_;
		uint32	equipID_;
		uint64	equipIns_;
		uint64	equipSerial_;

		Requirement	Kind()	{ return DIRECTLYEQUIP; }
		uint32	Equiped_EID()	{ return equipID_; }
		uint64	Equiped_EIns()	{ return equipIns_; }
		uint64	Equiped_ESerial()	{ return equipSerial_; }
		int		Equiped_EPos()	{ return pos_; }
	};
	
	struct PetMoveEquipRequirement : public IItemResult
	{
		XRated::GlobalSerial petSerial;
		bool	isEquip_; // inventory -> equip
		uint32	equipID_;
		uint32	invenID_;
		int		equipPos_;
		int		invenPos_;
		int		bag_;

		Requirement	Kind()		{ return PETMOVEEQUIP; }
		bool	Equiped_Equip()	{ return isEquip_; }
		uint32	Equiped_EID()	{ return equipID_; }
		uint32	Equiped_IID()	{ return invenID_; }
		int		Equiped_EPos()	{ return equipPos_; }
		int		Equiped_IPos()	{ return invenPos_; }
		int		Equiped_IBag()	{ return bag_; }
		virtual uint64	Used_ExInfo()	{ return petSerial; }
	};

	struct MovePetEquipInPetInvenRequirement : public IItemResult
	{
		XRated::GlobalSerial PetSerial;
		bool IsEquip;
		uint32 EquipmentItemId;
		uint32 ItemIdInPetInven;
		int EquipPos;
		int InvenPos;
		
		virtual Requirement	Kind()		{ return PETEQUIPMOVEINPETINVEN; }
		
		// EquipRequirement
		virtual bool	Equiped_Equip()	{ return IsEquip; }
		virtual uint32	Equiped_EID()	{ return EquipmentItemId; }
		virtual uint32	Equiped_IID()	{ return ItemIdInPetInven; }
		virtual int		Equiped_EPos()	{ return EquipPos; }
		virtual int		Equiped_IPos()	{ return InvenPos; }
		virtual uint64	Used_ExInfo()	{ return PetSerial; }
	};

	struct UseItemInPetInvenRequirement : public IItemResult
	{
		XRated::GlobalSerial PetSerial;
		uint32 Position;
		uint32 PositionType;
		
		virtual Requirement	Kind()		{ return USEITEMINPETINVEN; }
		
		virtual uint32	Used_Bag()		{ return PositionType; }
		virtual uint32	Used_Pos()		{ return Position; }
		virtual uint64	Used_ExInfo()	{ return PetSerial; }
	};

	

	class ItemRequirementQue
	{
	public:
		ItemRequirementQue();
		~ItemRequirementQue();

		bool RequireItemUse(uint32 bag, uint32 pos);
		bool RequireDropEquip(uint32 pos);
		bool RequireMoveEquip(bool isEquip, uint32 equipID, int equipPos, uint32 invenID, int invenPos, int bag);
		bool RequireEquipToEquip(int fromEquipPos, int toEquipPos);
		bool RequireDirectlyEquip(uint32 equipID, uint64 equipIns, uint64 equipSerial, int32 pos);
		bool RequirePetMoveEquip(bool isEquip, XRated::GlobalSerial petSerial, uint32 equipID, int equipPos, uint32 invenID, int invenPos, int bag);
		bool RequireMovePetEquipInPetInven(bool isEquip, XRated::GlobalSerial petSerial, uint32 equipID, int equipPos, uint32 itemIdInPetInven, int invenPos);
		bool RequireUseItemInPetInven(XRated::GlobalSerial petSerial, uint32 positionType, uint32 position);
		bool RequireSwapEquip( uint8 newEquipSet );
		bool RequireSwapCashEquip( uint8 newEquipSet );

		bool IsEmpty();
		IItemResult* Result();
		void Clear();

	public:
		PetMoveEquipRequirement* PetEquipResult();
	private:
		Requirement requirement;
		UseRequirement useRequirement;
		DropEquipRequirement dropEquipRequirement;
		MoveEquipRequirement moveEquipRequirement;
		EquipToEquipRequirement equipToEquipRequirement;
		SwapEquipRequirement	swapEquipRequirement;
		SwapCashEquipRequirement swapCashEquipRequirement;
		DirectlyEquipRequirement directlyEquipRequirement;
		PetMoveEquipRequirement	petMoveEquipRequirement;
		MovePetEquipInPetInvenRequirement movePetEquipInPetInvenRequirement;
		UseItemInPetInvenRequirement useItemInPetInvenRequirement;
	};


} } } 