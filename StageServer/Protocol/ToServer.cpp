#include <StageServer/Protocol/ToServer.h>
namespace Lunia
{
	namespace XRated
	{
		namespace StageServer
		{
			namespace Protocol
			{
                namespace ToServer {

                    // Stage
                    // //////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Stage::TypeName = L"Stage";
                    const HashType Stage::TypeHash = StringUtil::Hash(Stage::TypeName);
                    void Stage::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"secukey", secukey);
                        out.Write(L"Version", Version);
                        out.Write(L"Locale", Locale);
                    }

                    void Stage::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"secukey", secukey);
                        in.Read(L"Version", Version);
                        in.Read(L"Locale", Locale);
                    }

                    //////////////////////////////////////////////////////////////////////////////////
                    ///Stage //

                    // CharacterInfo
                    // //////////////////////////////////////////////////////////////////////////
                    // Don't have to use these packets immediately : by rapperkjm
                    const wchar_t* CharacterInfo::TypeName = L"CharacterInfo";
                    const HashType CharacterInfo::TypeHash =
                        StringUtil::Hash(CharacterInfo::TypeName);
                    void CharacterInfo::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }

                    void CharacterInfo::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    //////////////////////////////////////////////////////////////////////////
                    ///CharacterInfo //

                    // ListQuickSlot
                    // //////////////////////////////////////////////////////////////////////////
                    // Don't have to use these packets immediately : by rapperkjm
                    const wchar_t* ListQuickSlot::TypeName = L"ListQuickSlot";
                    const HashType ListQuickSlot::TypeHash =
                        StringUtil::Hash(ListQuickSlot::TypeName);
                    void ListQuickSlot::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }

                    void ListQuickSlot::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    //////////////////////////////////////////////////////////////////////////
                    ///ListQuickSlot //

                    // ListSkill
                    // //////////////////////////////////////////////////////////////////////////////
                    // Don't have to use these packets immediately : by rapperkjm
                    const wchar_t* ListSkill::TypeName = L"ListSkill";
                    const HashType ListSkill::TypeHash =
                        StringUtil::Hash(ListSkill::TypeName);
                    void ListSkill::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    };

                    void ListSkill::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///ListSkill //

                    // ListItem
                    // ///////////////////////////////////////////////////////////////////////////////
                    // Don't have to use these packets immediately : by rapperkjm
                    const wchar_t* ListItem::TypeName = L"ListItem";
                    const HashType ListItem::TypeHash =
                        StringUtil::Hash(ListItem::TypeName);
                    void ListItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    };

                    void ListItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }
                    ///////////////////////////////////////////////////////////////////////////////
                    ///ListItem //

                    // Chat
                    // ///////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Chat::TypeName = L"Chat";
                    const HashType Chat::TypeHash = StringUtil::Hash(Chat::TypeName);
                    void Chat::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"chattype", (int)(chattype));
                        out.Write(L"message", message);
                    }

                    void Chat::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"chattype", (int&)(chattype));
                        in.Read(L"message", message);
                    }

                    ///////////////////////////////////////////////////////////////////////////////////
                    ///Chat //

                    // Voice
                    // //////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Voice::TypeName = L"Voice";
                    const HashType Voice::TypeHash = StringUtil::Hash(Voice::TypeName);
                    void Voice::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"chattype", chattype);
                        out.Write(L"messageid", messageid);
                    }

                    void Voice::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"chattype", chattype);
                        in.Read(L"messageid", messageid);
                    }
                    //////////////////////////////////////////////////////////////////////////////////
                    ///Voice //

                    // Join
                    // ///////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Join::TypeName = L"Join";
                    const HashType Join::TypeHash = StringUtil::Hash(Join::TypeName);
                    void Join::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }

                    void Join::Deserialize(Serializer::IStreamReader& in) { in.Begin(TypeName); }
                    ///////////////////////////////////////////////////////////////////////////////////
                    ///Join //

                    // CheckStage
                    // /////////////////////////////////////////////////////////////////////////////
                    const wchar_t* CheckJoinToSecretStage::TypeName = L"CheckJoinToSecretStage";
                    const HashType CheckJoinToSecretStage::TypeHash =
                        StringUtil::Hash(CheckJoinToSecretStage::TypeName);
                    void CheckJoinToSecretStage::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"password", password);
                    }

                    void CheckJoinToSecretStage::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"password", password);
                    }
                    ///////////////////////////////////////////////////////////////////////////////////
                    ///Join //

                    // LoadEnd
                    // ////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* LoadEnd::TypeName = L"LoadEnd";
                    const HashType LoadEnd::TypeHash =
                        StringUtil::Hash(LoadEnd::TypeName);
                    void LoadEnd::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"progress", progress);
                    }

                    void LoadEnd::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"progress", progress, -1.0f);
                    }
                    ////////////////////////////////////////////////////////////////////////////////
                    ///LoadEnd //

                    //// Part
                    //////////////////////////////////////////////////////////////////////////////////////
                    // const wchar_t* Part::TypeName = L"Part";
                    // void Part::Serialize(Serializer::IStreamWriter& out) const
                    //{
                    //	out.Begin( TypeName );
                    //}

                    // void Part::Deserialize(Serializer::IStreamReader& in)
                    //{
                    //	in.Begin( TypeName );
                    //}
                    /////////////////////////////////////////////////////////////////////////////////////
                    ///Part //

                    // Command
                    // ////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Command::TypeName = L"Command";
                    const HashType Command::TypeHash =
                        StringUtil::Hash(Command::TypeName);
                    void Command::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"command", command);
                        out.Write(L"direction", (int)(direction));
                    }

                    void Command::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"command", command);
                        in.Read(L"direction", (int&)(direction));
                    }
                    ////////////////////////////////////////////////////////////////////////////////
                    ///Command //

                    // Cast
                    // ///////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Cast::TypeName = L"Cast";
                    const HashType Cast::TypeHash = StringUtil::Hash(Cast::TypeName);
                    void Cast::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"skillgroupid", skillgroupid);
                    }

                    void Cast::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"skillgroupid", skillgroupid);
                    }

                    ////////////////////////////////////////////////////////////////////////////////
                    ///Command //

                    // SetSkillLevel
                    // //////////////////////////////////////////////////////////////////////////
                    const wchar_t* SetSkillLevel::TypeName = L"SetSkillLevel";
                    const HashType SetSkillLevel::TypeHash =
                        StringUtil::Hash(SetSkillLevel::TypeName);
                    void SetSkillLevel::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"skillgroupid", skillgroupid);
                        out.Write(L"delta", delta);
                    }
                    void SetSkillLevel::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"skillgroupid", skillgroupid);
                        in.Read(L"delta", delta);
                    }

                    //////////////////////////////////////////////////////////////////////////
                    ///SetSkillLevel //

                    // Revive
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Revive::TypeName = L"Revive";
                    const HashType Revive::TypeHash = StringUtil::Hash(Revive::TypeName);
                    void Revive::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }

                    void Revive::Deserialize(Serializer::IStreamReader& in) { in.Begin(TypeName); }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///Revive //

                    // DailyRevive
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* DailyRevive::TypeName = L"DailyRevive";
                    const HashType DailyRevive::TypeHash =
                        StringUtil::Hash(DailyRevive::TypeName);
                    void DailyRevive::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }

                    void DailyRevive::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///DailyRevive //

                    // EquipSwap
                    // ///////////////////////////////////////////////////////////////////////////////
                    const wchar_t* EquipSwap::TypeName = L"EquipSwap";
                    const HashType EquipSwap::TypeHash =
                        StringUtil::Hash(EquipSwap::TypeName);
                    void EquipSwap::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Set", Set);
                    }

                    void EquipSwap::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Set", Set);
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///EquipSwap //

                    // CashEquipSwap
                    // ///////////////////////////////////////////////////////////////////////////////
                    const wchar_t* CashEquipSwap::TypeName = L"CashEquipSwap";
                    const HashType CashEquipSwap::TypeHash =
                        StringUtil::Hash(CashEquipSwap::TypeName);
                    void CashEquipSwap::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Set", Set);
                    }

                    void CashEquipSwap::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Set", Set);
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///EquipSwap //

                    // MoveItem
                    // ///////////////////////////////////////////////////////////////////////////////
                    const wchar_t* MoveItem::TypeName = L"MoveItem";
                    const HashType MoveItem::TypeHash =
                        StringUtil::Hash(MoveItem::TypeName);
                    void MoveItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"pbag", pbag);
                        out.Write(L"ppos", ppos);
                        out.Write(L"nbag", nbag);
                        out.Write(L"npos", npos);
                    }

                    void MoveItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"pbag", pbag);
                        in.Read(L"ppos", ppos);
                        in.Read(L"nbag", nbag);
                        in.Read(L"npos", npos);
                    }
                    ///////////////////////////////////////////////////////////////////////////////
                    ///MoveItem //

                    // SortInventory
                    // ///////////////////////////////////////////////////////////////////////////////
                    const wchar_t* SortInventory::TypeName = L"SortInventory";
                    const HashType SortInventory::TypeHash =
                        StringUtil::Hash(SortInventory::TypeName);
                    void SortInventory::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }

                    void SortInventory::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }
                    ///////////////////////////////////////////////////////////////////////////////
                    ///SortInventory //

                    // StackItem
                    // ///////////////////////////////////////////////////////////////////////////////
                    const wchar_t* StackItem::TypeName = L"StackItem";
                    const HashType StackItem::TypeHash =
                        StringUtil::Hash(StackItem::TypeName);
                    void StackItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"pbag", pbag);
                        out.Write(L"ppos", ppos);
                        out.Write(L"nbag", nbag);
                        out.Write(L"npos", npos);
                        out.Write(L"count", count);
                    }

                    void StackItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"pbag", pbag);
                        in.Read(L"ppos", ppos);
                        in.Read(L"nbag", nbag);
                        in.Read(L"npos", npos);
                        in.Read(L"count", count);
                    }
                    ///////////////////////////////////////////////////////////////////////////////
                    ///StackItem //

                    // GivePresentToPet
                    // ////////////////////////////////////////////////////////////////////////
                    const wchar_t* GivePresentToPet::TypeName = L"GivePresentToPet";
                    const HashType GivePresentToPet::TypeHash =
                        StringUtil::Hash(GivePresentToPet::TypeName);
                    void GivePresentToPet::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PetItemPosition", PetItemPosition);
                        out.Write(L"PresentItemPack", PresentItemPack);
                    }

                    void GivePresentToPet::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PetItemPosition", PetItemPosition);
                        in.Read(L"PresentItemPack", PresentItemPack);
                    }
                    ////////////////////////////////////////////////////////////////////////
                    ///GivePresentToPet //

                    // MoveInventoryToPetItem
                    // ////////////////////////////////////////////////////////////////////////
                    const wchar_t* MoveItemInventoryToPet::TypeName = L"MoveItemInventoryToPet";
                    const HashType MoveItemInventoryToPet::TypeHash =
                        StringUtil::Hash(MoveItemInventoryToPet::TypeName);
                    void MoveItemInventoryToPet::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PetSerial", PetSerial);
                        out.Write(L"Inventory", Inventory);
                        out.WriteEnum(L"PetSlotType", PetSlotType);
                        out.Write(L"PetSlotPosition", PetSlotPosition);
                    }

                    void MoveItemInventoryToPet::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PetSerial", PetSerial);
                        in.Read(L"Inventory", Inventory);
                        in.ReadEnum(L"PetSlotType", PetSlotType);
                        in.Read(L"PetSlotPosition", PetSlotPosition);
                    }
                    ////////////////////////////////////////////////////////////////////////
                    ///MoveInventoryToPetItem //

                    // MoveItemInPetInven
                    // ////////////////////////////////////////////////////////////////////////
                    const wchar_t* MoveItemInPetInven::TypeName = L"MoveItemInPetInven";
                    const HashType MoveItemInPetInven::TypeHash =
                        StringUtil::Hash(MoveItemInPetInven::TypeName);
                    void MoveItemInPetInven::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PetSerial", PetSerial);
                        out.WriteEnum(L"SourcePositionType", SourcePositionType);
                        out.Write(L"SourcePosition", SourcePosition);
                        out.WriteEnum(L"TargetPositionType", TargetPositionType);
                        out.Write(L"TargetPosition", TargetPosition);
                    }

                    void MoveItemInPetInven::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PetSerial", PetSerial);
                        in.ReadEnum(L"SourcePositionType", SourcePositionType);
                        in.Read(L"SourcePosition", SourcePosition);
                        in.ReadEnum(L"TargetPositionType", TargetPositionType);
                        in.Read(L"TargetPosition", TargetPosition);
                    }
                    ////////////////////////////////////////////////////////////////////////
                    ///MoveItemInPetInven //

                    // RequestTakeOutCaredPet
                    // ////////////////////////////////////////////////////////////////////////
                    const wchar_t* RequestTakeOutCaredPet::TypeName = L"RequestTakeOutCaredPet";
                    const HashType RequestTakeOutCaredPet::TypeHash =
                        StringUtil::Hash(RequestTakeOutCaredPet::TypeName);
                    void RequestTakeOutCaredPet::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PetSerial", PetSerial);
                    }

                    void RequestTakeOutCaredPet::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PetSerial", PetSerial);
                    }
                    ////////////////////////////////////////////////////////////////////////
                    ///RequestTakeOutCaredPet //

                    // TakeOutAllPetItems
                    // ////////////////////////////////////////////////////////////////////////
                    const wchar_t* TakeOutAllPetItems::TypeName = L"TakeOutAllPetItems";
                    const HashType TakeOutAllPetItems::TypeHash =
                        StringUtil::Hash(TakeOutAllPetItems::TypeName);
                    void TakeOutAllPetItems::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PetSerial", PetSerial);
                    }

                    void TakeOutAllPetItems::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PetSerial", PetSerial);
                    }
                    ////////////////////////////////////////////////////////////////////////
                    ///TakeOutAllPetItems //

                    // Use
                    // ////////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Use::TypeName = L"Use";
                    const HashType Use::TypeHash = StringUtil::Hash(Use::TypeName);
                    void Use::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"itemposotion", itemposition);
                    }

                    void Use::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"itemposotion", itemposition);
                    }
                    ////////////////////////////////////////////////////////////////////////////////////
                    ///Use //

                    // UseItemInPetInven
                    // ////////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* UseItemInPetInven::TypeName = L"UseItemInPetInven";
                    const HashType UseItemInPetInven::TypeHash =
                        StringUtil::Hash(UseItemInPetInven::TypeName);
                    void UseItemInPetInven::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PetSerial", PetSerial);
                        out.WriteEnum(L"PositionType", PositionType);
                        out.Write(L"Position", Position);
                    }

                    void UseItemInPetInven::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PetSerial", PetSerial);
                        in.ReadEnum(L"PositionType", PositionType);
                        in.Read(L"Position", Position);
                    }
                    ////////////////////////////////////////////////////////////////////////////////////
                    ///UseItemInPetInven //

                    // DropItem
                    // ///////////////////////////////////////////////////////////////////////////////
                    const wchar_t* DropItem::TypeName = L"DropItem";
                    const HashType DropItem::TypeHash =
                        StringUtil::Hash(DropItem::TypeName);
                    void DropItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"position", position);
                        out.Write(L"count", count);
                    }

                    void DropItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"position", position);
                        in.Read(L"count", count);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///Drop Item //

                    // AddMinimapPing
                    // /////////////////////////////////////////////////////////////////////////

                    const wchar_t* AddMinimapPing::TypeName = L"AddMinimapPing";
                    const HashType AddMinimapPing::TypeHash =
                        StringUtil::Hash(AddMinimapPing::TypeName);
                    void AddMinimapPing::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"position", position);
                    }

                    void AddMinimapPing::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"position", position);
                    }

                    /////////////////////////////////////////////////////////////////////////
                    ///AddMinimapPing //

                    // MoveQuickSlot
                    // //////////////////////////////////////////////////////////////////////////
                    const wchar_t* MoveQuickSlot::TypeName = L"MoveQuickSlot";
                    const HashType MoveQuickSlot::TypeHash =
                        StringUtil::Hash(MoveQuickSlot::TypeName);
                    void MoveQuickSlot::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"ppos", ppos);
                        out.Write(L"npos", npos);
                    }

                    void MoveQuickSlot::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"ppos", ppos);
                        in.Read(L"npos", npos);
                    }
                    //////////////////////////////////////////////////////////////////////////
                    ///MoveQuickSlot //

                    // RequestVoting
                    // //////////////////////////////////////////////////////////////////////////
                    const wchar_t* RequestVoting::TypeName = L"RequestVoting";
                    const HashType RequestVoting::TypeHash =
                        StringUtil::Hash(RequestVoting::TypeName);
                    void RequestVoting::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"type", (int)(type));
                        out.Write(L"targetStage", targetStage);
                        out.Write(L"charName", charName);
                    }

                    void RequestVoting::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"type", (int&)type);
                        in.Read(L"targetStage", targetStage);
                        in.Read(L"charName", charName);
                    };

                    //////////////////////////////////////////////////////////////////////////
                    ///RequestVoting //

                    // AnswerPersonalVoting
                    // //////////////////////////////////////////////////////////////////////////
                    const wchar_t* AnswerPersonalVoting::TypeName = L"AnswerPersonalVoting";
                    const HashType AnswerPersonalVoting::TypeHash =
                        StringUtil::Hash(AnswerPersonalVoting::TypeName);
                    void AnswerPersonalVoting::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"type", (int)(type));
                    }

                    void AnswerPersonalVoting::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"type", (int&)type);
                    };

                    //////////////////////////////////////////////////////////////////////////
                    ///AnswerPersonalVoting //

                    // RequestPersonalVoting
                    // //////////////////////////////////////////////////////////////////////////
                    const wchar_t* RequestPersonalVoting::TypeName = L"RequestPersonalVoting";
                    const HashType RequestPersonalVoting::TypeHash =
                        StringUtil::Hash(RequestPersonalVoting::TypeName);
                    void RequestPersonalVoting::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"type", (int)(type));
                        out.Write(L"charName", charName);
                    }

                    void RequestPersonalVoting::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"type", (int&)type);
                        in.Read(L"charName", charName);
                    };

                    //////////////////////////////////////////////////////////////////////////
                    ///RequestPersonalVoting //

                    // Vote
                    // ///////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Vote::TypeName = L"Vote";
                    const HashType Vote::TypeHash = StringUtil::Hash(Vote::TypeName);
                    void Vote::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"result", (int)(result));
                    }

                    void Vote::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"result", (int&)result);
                    };
                    ///////////////////////////////////////////////////////////////////////////////////
                    ///Vote //

                    // EnterShop
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* EnterShop::TypeName = L"EnterShop";
                    const HashType EnterShop::TypeHash =
                        StringUtil::Hash(EnterShop::TypeName);
                    void EnterShop::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"shopnumber", (int)(shopnumber));
                        out.Write(L"param", param);
                    }
                    void EnterShop::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"shopnumber", (int&)(shopnumber));
                        in.Read(L"param", param);
                    }

                    //////////////////////////////////////////////////////////////////////////////
                    ///EnterShop //

                    // LeaveShop
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* LeaveShop::TypeName = L"LeaveShop";
                    const HashType LeaveShop::TypeHash =
                        StringUtil::Hash(LeaveShop::TypeName);
                    void LeaveShop::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void LeaveShop::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    //////////////////////////////////////////////////////////////////////////////
                    ///LeaveShop //

                    // Buy
                    // ////////////////////////////////////////////////////////////////////////////////////

                    const wchar_t* Buy::TypeName = L"Buy";
                    const HashType Buy::TypeHash = StringUtil::Hash(Buy::TypeName);
                    void Buy::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"itemid", itemid);
                        out.Write(L"count", count);
                    }

                    void Buy::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"itemid", itemid);
                        in.Read(L"count", count);
                    }
                    // 2
                    ////////////////////////////////////////////////////////////////////////////////////
                    ///Buy //

                    // BuyBack
                    // ////////////////////////////////////////////////////////////////////////////////////
                    // // 3.1 BY ULTIMATE

                    const wchar_t* BuyBack::TypeName = L"BuyBack";
                    const HashType BuyBack::TypeHash =
                        StringUtil::Hash(BuyBack::TypeName);
                    void BuyBack::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"itemid", itemid);
                        out.Write(L"instanceEx", instanceEx);
                        out.Write(L"count", count);
                    }

                    void BuyBack::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"itemid", itemid);
                        in.Read(L"instanceEx", instanceEx);
                        in.Read(L"count", count);
                    }
                    // 2
                    ////////////////////////////////////////////////////////////////////////////////////
                    ///BuyBack //

                    // BuyBarterItem
                    // /////////////////////////////////////////////////////////////////// // 3.1 BY
                    // ULTIMATE

                    const wchar_t* BuyBarterItem::TypeName = L"BuyBarterItem";
                    const HashType BuyBarterItem::TypeHash =
                        StringUtil::Hash(BuyBarterItem::TypeName);
                    void BuyBarterItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"barterItemId", barterItemId);
                        out.Write(L"count", count);
                    }

                    void BuyBarterItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"barterItemId", barterItemId);
                        in.Read(L"count", count);
                    }
                    // 2
                    ////////////////////////////////////////////////////////////////////////////////////
                    ///BuyBack //
                    // Buy
                    // ////////////////////////////////////////////////////////////////////////////////////

                    const wchar_t* BuyToGuildShop::TypeName = L"BuyToGuildShop";
                    const HashType BuyToGuildShop::TypeHash =
                        StringUtil::Hash(BuyToGuildShop::TypeName);
                    void BuyToGuildShop::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"itemid", itemid);
                        out.Write(L"count", count);
                    }

                    void BuyToGuildShop::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"itemid", itemid);
                        in.Read(L"count", count);
                    }
                    ////////////////////////////////////////////////////////////////////////////////////
                    ///Buy //

                    // Sell
                    // ///////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Sell::TypeName = L"Sell";
                    const HashType Sell::TypeHash = StringUtil::Hash(Sell::TypeName);
                    void Sell::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"bag", bag);
                        out.Write(L"pos", pos);
                    }

                    void Sell::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"bag", bag);
                        in.Read(L"pos", pos);
                    }
                    ///////////////////////////////////////////////////////////////////////////////////
                    ///Sell //

                    // RequestTrade
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* RequestTrade::TypeName = L"RequestTrade";
                    const HashType RequestTrade::TypeHash =
                        StringUtil::Hash(RequestTrade::TypeName);
                    void RequestTrade::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"serial", serial);
                    }

                    void RequestTrade::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"serial", serial);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///RequestTrade //

                    // AddTradeItem
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* AddTradeItem::TypeName = L"AddTradeItem";
                    const HashType AddTradeItem::TypeHash =
                        StringUtil::Hash(AddTradeItem::TypeName);
                    void AddTradeItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"itemposition", itemposition);
                    }

                    void AddTradeItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"itemposition", itemposition);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///AddTradeItem //

                    // AddTradeMoney
                    // //////////////////////////////////////////////////////////////////////////
                    const wchar_t* AddTradeMoney::TypeName = L"AddTradeMoeny";
                    const HashType AddTradeMoney::TypeHash =
                        StringUtil::Hash(AddTradeMoney::TypeName);
                    void AddTradeMoney::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"money", money);
                    }

                    void AddTradeMoney::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"money", money);
                    }

                    //////////////////////////////////////////////////////////////////////////
                    ///AddTradeMoney //

                    // ReadyToTrade
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* ReadyToTrade::TypeName = L"ReadyToTrade";
                    const HashType ReadyToTrade::TypeHash =
                        StringUtil::Hash(ReadyToTrade::TypeName);
                    void ReadyToTrade::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }

                    void ReadyToTrade::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    ///////////////////////////////////////////////////////////////////////////
                    ///ReadyToTrade //

                    // CancelTrade
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* CancelTrade::TypeName = L"CancelTrade";
                    const HashType CancelTrade::TypeHash =
                        StringUtil::Hash(CancelTrade::TypeName);
                    void CancelTrade::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }

                    void CancelTrade::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    ////////////////////////////////////////////////////////////////////////////
                    ///CancelTrade //

                    // ConfirmTrade
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* ConfirmTrade::TypeName = L"ConfirmTrade";
                    const HashType ConfirmTrade::TypeHash =
                        StringUtil::Hash(ConfirmTrade::TypeName);
                    void ConfirmTrade::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"conFirm", conFirm);
                    }

                    void ConfirmTrade::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"conFirm", conFirm);
                    }

                    ////////////////////////////////////////////////////////////////////////////
                    ///ConfirmTrade //

                    // DropBankItem
                    // //////////////////////////////////////////////////////////////////////////
                    const wchar_t* DropBankItem::TypeName = L"DropBankItem";
                    const HashType DropBankItem::TypeHash =
                        StringUtil::Hash(DropBankItem::TypeName);
                    void DropBankItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"position", position);
                    }

                    void DropBankItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"position", position);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///DropBankItem //

                    // BankMoneyIn
                    // ////////////////////////////////////////////////////////////////////////////
                    const wchar_t* BankMoneyIn::TypeName = L"BankMoneyIn";
                    const HashType BankMoneyIn::TypeHash =
                        StringUtil::Hash(BankMoneyIn::TypeName);
                    void BankMoneyIn::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"BankMoney", BankMoney);
                    }

                    void BankMoneyIn::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"BankMoney", BankMoney);
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    ///BankMoneyIn //

                    // BankMoneyOut
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* BankMoneyOut::TypeName = L"BankMoneyOut";
                    const HashType BankMoneyOut::TypeHash =
                        StringUtil::Hash(BankMoneyOut::TypeName);
                    void BankMoneyOut::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"BankMoney", BankMoney);
                    }

                    void BankMoneyOut::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"BankMoney", BankMoney);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///BankMoneyOut //

                    // QuickSlot
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* QuickSlot::TypeName = L"QuickSlot";
                    const HashType QuickSlot::TypeHash =
                        StringUtil::Hash(QuickSlot::TypeName);
                    void QuickSlot::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"number", number);
                        out.Write(L"id", id);
                        out.Write(L"isskill", (int)(isskill));
                        out.Write(L"instanceEx", instanceEx);
                    }
                    void QuickSlot::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"number", number);
                        in.Read(L"id", id);
                        in.Read(L"isskill", (int&)(isskill));
                        in.Read(L"instanceEx", instanceEx);
                    };
                    /////////////////////////////////////////////////////////////////////////////
                    ///QuickSlot //

                    // Alive
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Alive::TypeName = L"Alive";
                    const HashType Alive::TypeHash = StringUtil::Hash(Alive::TypeName);
                    void Alive::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"EncryptKey", EncryptKey);
                        out.Write(L"RequestTime", RequestTime);
                    }

                    void Alive::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"EncryptKey", EncryptKey);
                        in.Read(L"RequestTime", RequestTime);
                    }

                    // Identify
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Identify::TypeName = L"Identify";
                    const HashType Identify::TypeHash =
                        StringUtil::Hash(Identify::TypeName);
                    void Identify::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"position", Position);
                        // out.Write( L"Position", Position );
                        // out.Write( L"IdentifyIndex", IdentifyIndex );
                    }

                    void Identify::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"position", Position);
                        // in.Read( L"Position", Position );
                        // in.Read( L"IdentifyIndex", IdentifyIndex );
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///Identify //

                    // Reinforce
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Reinforce::TypeName = L"Reinforce";
                    const HashType Reinforce::TypeHash =
                        StringUtil::Hash(Reinforce::TypeName);
                    void Reinforce::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Position", Position);
                        out.Write(L"Stones", Stones);
                        out.Write(L"Catalysts", Catalysts);
                    }

                    void Reinforce::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Position", Position);
                        in.Read(L"Stones", Stones);
                        in.Read(L"Catalysts", Catalysts);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///Reinforce //

                    // LightReinforce
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* LightReinforce::TypeName = L"LightReinforce";
                    const HashType LightReinforce::TypeHash =
                        StringUtil::Hash(LightReinforce::TypeName);
                    void LightReinforce::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Position", Position);
                        out.Write(L"Jewels", Jewels);
                        out.Write(L"Catalysts", Catalysts);
                    }

                    void LightReinforce::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Position", Position);
                        in.Read(L"Jewels", Jewels);
                        in.Read(L"Catalysts", Catalysts);
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///LightReinforce //

                    // AttachMagic
                    // ////////////////////////////////////////////////////////////////////////////
                    const wchar_t* AttachMagic::TypeName = L"AttachMagic";
                    const HashType AttachMagic::TypeHash =
                        StringUtil::Hash(AttachMagic::TypeName);
                    void AttachMagic::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Position", Position);
                        out.Write(L"Scrolls", Scrolls);
                        out.Write(L"Components", Components);
                    }

                    void AttachMagic::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Position", Position);
                        in.Read(L"Scrolls", Scrolls);
                        in.Read(L"Components", Components);
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    ///AttachMagic //

                    /*   3.1 BY ULTIMATE */
                    // Compose
                    // ////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Compose::TypeName = L"Compose";
                    const HashType Compose::TypeHash =
                        StringUtil::Hash(Compose::TypeName);
                    void Compose::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Type", Type);
                        out.Write(L"Item1", Item1);
                        out.Write(L"Item2", Item2);
                    }

                    void Compose::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Type", Type);
                        in.Read(L"Item1", Item1);
                        in.Read(L"Item2", Item2);
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    ///Compose //

                    // Extract
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Extract::TypeName = L"Extract";
                    const HashType Extract::TypeHash =
                        StringUtil::Hash(Extract::TypeName);
                    void Extract::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Position", Position);
                    }

                    void Extract::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Position", Position);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///Extract //

                    // RestoreBelonging
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* RestoreBelonging::TypeName = L"RestoreBelonging";
                    const HashType RestoreBelonging::TypeHash =
                        StringUtil::Hash(RestoreBelonging::TypeName);
                    void RestoreBelonging::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);

                        out.Write(L"Position", Position);
                        /*out.Write( L"Scrolls", Scrolls );
                        out.Write( L"Components", Components );*/
                    }

                    void RestoreBelonging::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Position", Position);
                        /*in.Read( L"Scrolls", Scrolls );
                        in.Read( L"Components", Components );*/
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///RestoreBelonging //

                    // RecoverReinforcement
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* RecoverReinforcement::TypeName = L"RecoverReinforcement";
                    const HashType RecoverReinforcement::TypeHash =
                        StringUtil::Hash(RecoverReinforcement::TypeName);
                    void RecoverReinforcement::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);

                        out.Write(L"Target", Target);
                        out.Write(L"Stones", Stones);
                    }

                    void RecoverReinforcement::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Target", Target);
                        in.Read(L"Stones", Stones);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///RecoverReinforcement //

                    // RecoverLightReinforcement
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* RecoverLightReinforcement::TypeName =
                        L"RecoverLightReinforcement";
                    const HashType RecoverLightReinforcement::TypeHash =
                        StringUtil::Hash(RecoverLightReinforcement::TypeName);
                    void RecoverLightReinforcement::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);

                        out.Write(L"Target", Target);
                        out.Write(L"Stones", Stones);
                    }

                    void RecoverLightReinforcement::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Target", Target);
                        in.Read(L"Stones", Stones);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///RecoverLightReinforcement //

                    // ResetIndentification
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* ResetIndentification::TypeName = L"ResetIndentification";
                    const HashType ResetIndentification::TypeHash =
                        StringUtil::Hash(ResetIndentification::TypeName);
                    void ResetIndentification::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);

                        out.Write(L"Target", Target);
                        out.Write(L"Scrolls", Scrolls);
                    }

                    void ResetIndentification::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Target", Target);
                        in.Read(L"Scrolls", Scrolls);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///ResetIndentification //

                    // ToServer::Terminate
                    const wchar_t* Terminate::TypeName = L"Terminate";
                    const HashType Terminate::TypeHash =
                        StringUtil::Hash(Terminate::TypeName);
                    void Terminate::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void Terminate::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    // ListCashItems
                    // //////////////////////////////////////////////////////////////////////////
                    const wchar_t* ListCashItems::TypeName = L"ListCashItems";
                    const HashType ListCashItems::TypeHash =
                        StringUtil::Hash(ListCashItems::TypeName);
                    void ListCashItems::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PageNumber", PageNumber);
                        out.Write(L"IsPresent", IsPresent);
                        out.Write(L"Language", Language);
                    }

                    void ListCashItems::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PageNumber", PageNumber);
                        in.Read(L"IsPresent", IsPresent);
                        in.Read(L"Language", Language);
                    }
                    //////////////////////////////////////////////////////////////////////////
                    ///ListCashItems //

                    // PurchaseCashItem
                    // //////////////////////////////////////////////////////////////////////////
                    const wchar_t* PurchaseCashItem::TypeName = L"PurchaseCashItem";
                    const HashType PurchaseCashItem::TypeHash =
                        StringUtil::Hash(PurchaseCashItem::TypeName);
                    void PurchaseCashItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"ProductID", ProductID);
                    }

                    void PurchaseCashItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"ProductID", ProductID);
                    }
                    //////////////////////////////////////////////////////////////////////////
                    ///PurchaseCashItem //

                    // CashItemMove
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* CashItemMove::TypeName = L"CashItemMove";
                    const HashType CashItemMove::TypeHash =
                        StringUtil::Hash(CashItemMove::TypeName);
                    void CashItemMove::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"OrderNumber", OrderNumber);
                        out.Write(L"ProductNumber", ProductNumber);
                        out.Write(L"Quantity", Quantity);
                    }

                    void CashItemMove::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"OrderNumber", OrderNumber);
                        in.Read(L"ProductNumber", ProductNumber);
                        in.Read(L"Quantity", Quantity);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///CashItemMove //

                    // CashItemRefund
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* CashItemRefund::TypeName = L"CashItemRefund";
                    const HashType CashItemRefund::TypeHash =
                        StringUtil::Hash(CashItemRefund::TypeName);
                    void CashItemRefund::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"OrderNumber", OrderNumber);
                    }

                    void CashItemRefund::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"OrderNumber", OrderNumber);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///CashItemMove //

                    // CashItemView
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* CashItemView::TypeName = L"CashItemView";
                    const HashType CashItemView::TypeHash =
                        StringUtil::Hash(CashItemView::TypeName);
                    void CashItemView::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Flag", Flag);
                        out.Write(L"WingType", WingType);  // 3.1 by Robotex
                    }

                    void CashItemView::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Flag", Flag);
                        in.Read(L"WingType", WingType);  // 3.1 by Robotex
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///CashItemView //

                    // UseItemEx
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* UseItemEx::TypeName = L"UseItemEx";
                    const HashType UseItemEx::TypeHash =
                        StringUtil::Hash(UseItemEx::TypeName);
                    void UseItemEx::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Position", Position);
                        out.Write(L"Parameters", Parameters);
                    }

                    void UseItemEx::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Position", Position);
                        in.Read(L"Parameters", Parameters);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///UseItemEx //

                    // Alchemy
                    // ////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Alchemy::TypeName = L"Alchemy";
                    const HashType Alchemy::TypeHash =
                        StringUtil::Hash(Alchemy::TypeName);
                    void Alchemy::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Position", Position);
                        out.Write(L"Components", Components);
                    }

                    void Alchemy::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Position", Position);
                        in.Read(L"Components", Components);
                    }
                    ////////////////////////////////////////////////////////////////////////////////
                    ///Alchemy //

                    // GiveUpRevive
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* GiveUpRevive::TypeName = L"GiveUpRevive";
                    const HashType GiveUpRevive::TypeHash =
                        StringUtil::Hash(GiveUpRevive::TypeName);
                    void GiveUpRevive::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }

                    void GiveUpRevive::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///GiveUpRevive //

                    // InstantCoinRevive
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* InstantCoinRevive::TypeName = L"InstantCoinRevive";
                    const HashType InstantCoinRevive::TypeHash =
                        StringUtil::Hash(InstantCoinRevive::TypeName);
                    void InstantCoinRevive::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }

                    void InstantCoinRevive::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///InstantCoinRevive //

                    // Random
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Random::TypeName = L"Random";
                    const HashType Random::TypeHash = StringUtil::Hash(Random::TypeName);
                    void Random::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"RandomName", RandomName);
                        out.Write(L"MinValue", MinValue);
                        out.Write(L"MaxValue", MaxValue);
                    }

                    void Random::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"RandomName", RandomName);
                        in.Read(L"MinValue", MinValue);
                        in.Read(L"MaxValue", MaxValue);
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///Random //

                    // InstantCoinReviveRollBack
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* InstantCoinReviveRollBack::TypeName =
                        L"InstantCoinReviveRollBack";
                    const HashType InstantCoinReviveRollBack::TypeHash =
                        StringUtil::Hash(InstantCoinReviveRollBack::TypeName);
                    void InstantCoinReviveRollBack::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }

                    void InstantCoinReviveRollBack::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///InstantCoinReviveRollBack //

                    /* Familiar
                     * *******************************************************************************/
                    const wchar_t* Familiar::ChangeFormation::TypeName =
                        L"Familiar::ChangeFormation";
                    const HashType Familiar::ChangeFormation::TypeHash =
                        StringUtil::Hash(Familiar::ChangeFormation::TypeName);
                    void Familiar::ChangeFormation::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"FormationTo", FormationTo);
                    }

                    void Familiar::ChangeFormation::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"FormationTo", FormationTo);
                    }

                    const wchar_t* Familiar::Command::TypeName = L"Familiar::Command";
                    const HashType Familiar::Command::TypeHash =
                        StringUtil::Hash(Familiar::Command::TypeName);
                    void Familiar::Command::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"TargetIndex", TargetIndex);
                        out.Write(L"TargetSerial", TargetSerial);
                        out.WriteEnum(L"Sign", Sign);
                    }

                    void Familiar::Command::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"TargetIndex", TargetIndex);
                        in.Read(L"TargetSerial", TargetSerial);
                        in.ReadEnum(L"Sign", Sign);
                    }
                    /*******************************************************************************
                     * Familiar */

                     /* RequestReward
                      * ***********************************************************************/
                    const wchar_t* RequestReward::TypeName = L"RequestReward";
                    const HashType RequestReward::TypeHash =
                        StringUtil::Hash(RequestReward::TypeName);
                    void RequestReward::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"RewardType", RewardType);
                        out.Write(L"SelectType", SelectType);
                    }
                    void RequestReward::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"RewardType", RewardType);
                        in.Read(L"SelectType", SelectType);
                    }
                    /***********************************************************************
                     * RequestReward */

                     /* Upgrade
                      * ***********************************************************************/
                    const wchar_t* Upgrade::TypeName = L"Upgrade";
                    const HashType Upgrade::TypeHash =
                        StringUtil::Hash(Upgrade::TypeName);
                    void Upgrade::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PotionPosition", PotionPosition);
                        out.Write(L"TargetPosition", TargetPosition);
                    }
                    void Upgrade::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PotionPosition", PotionPosition);
                        in.Read(L"TargetPosition", TargetPosition);
                    }
                    /*********************************************************************** Upgrade
                     */

                     /* Rename
                      * ***********************************************************************/
                    const wchar_t* Rename::TypeName = L"Rename";
                    const HashType Rename::TypeHash = StringUtil::Hash(Rename::TypeName);
                    void Rename::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"TargetType", TargetType);
                        out.Write(L"NewName", NewName);
                        out.Write(L"RenamingItemPositoin", RenamingItemPositoin);
                    }
                    void Rename::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"TargetType", TargetType);
                        in.Read(L"NewName", NewName);
                        in.Read(L"RenamingItemPositoin", RenamingItemPositoin);
                    }
                    /*********************************************************************** Rename
                     */

                     /* AllMGuild
                      * *********************************************************************************/
                    const wchar_t* AllMGuild::Create::TypeName = L"AllMGuild::Create";
                    const HashType AllMGuild::Create::TypeHash =
                        StringUtil::Hash(AllMGuild::Create::TypeName);
                    void AllMGuild::Create::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Alias", Alias);
                        out.Write(L"GuildName", GuildName);
                        out.Write(L"Message", Message);
                    }
                    void AllMGuild::Create::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Alias", Alias);
                        in.Read(L"GuildName", GuildName);
                        in.Read(L"Message", Message);
                    }

                    const wchar_t* AllMGuild::InviteReply::TypeName = L"AllMGuild::InviteReply";
                    const HashType AllMGuild::InviteReply::TypeHash =
                        StringUtil::Hash(AllMGuild::InviteReply::TypeName);
                    void AllMGuild::InviteReply::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"TargetCharacterName", TargetCharacterName);
                        out.Write(L"ReplyCode", ReplyCode);
                    }
                    void AllMGuild::InviteReply::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"TargetCharacterName", TargetCharacterName);
                        in.Read(L"ReplyCode", ReplyCode);
                    }

                    const wchar_t* AllMGuild::Invite::TypeName = L"AllMGuild::Invite";
                    const HashType AllMGuild::Invite::TypeHash =
                        StringUtil::Hash(AllMGuild::Invite::TypeName);
                    void AllMGuild::Invite::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"TargetCharacterName", TargetCharacterName);
                    }
                    void AllMGuild::Invite::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"TargetCharacterName", TargetCharacterName);
                    }

                    const wchar_t* AllMGuild::Join::TypeName = L"AllMGuild::Join";
                    const HashType AllMGuild::Join::TypeHash =
                        StringUtil::Hash(AllMGuild::Join::TypeName);
                    void AllMGuild::Join::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"GuildId", GuildId);
                        out.Write(L"Grade", Grade);
                    }
                    void AllMGuild::Join::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"GuildId", GuildId);
                        in.Read(L"Grade", Grade);
                    }

                    const wchar_t* AllMGuild::Kick::TypeName = L"AllMGuild::Kick";
                    const HashType AllMGuild::Kick::TypeHash =
                        StringUtil::Hash(AllMGuild::Kick::TypeName);
                    void AllMGuild::Kick::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"TargetCharacterName", TargetCharacterName);
                    }
                    void AllMGuild::Kick::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"TargetCharacterName", TargetCharacterName);
                    }

                    const wchar_t* AllMGuild::Leave::TypeName = L"AllMGuild::Leave";
                    const HashType AllMGuild::Leave::TypeHash =
                        StringUtil::Hash(AllMGuild::Leave::TypeName);
                    void AllMGuild::Leave::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void AllMGuild::Leave::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    const wchar_t* AllMGuild::ListMembers::TypeName = L"AllMGuild::ListMembers";
                    const HashType AllMGuild::ListMembers::TypeHash =
                        StringUtil::Hash(AllMGuild::ListMembers::TypeName);
                    void AllMGuild::ListMembers::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void AllMGuild::ListMembers::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    const wchar_t* AllMGuild::Remove::TypeName = L"AllMGuild::Remove";
                    const HashType AllMGuild::Remove::TypeHash =
                        StringUtil::Hash(AllMGuild::Remove::TypeName);
                    void AllMGuild::Remove::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void AllMGuild::Remove::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    const wchar_t* AllMGuild::ValidateName::TypeName = L"AllMGuild::ValidateName";
                    const HashType AllMGuild::ValidateName::TypeHash =
                        StringUtil::Hash(AllMGuild::ValidateName::TypeName);
                    void AllMGuild::ValidateName::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Name", Name);
                    }
                    void AllMGuild::ValidateName::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Name", Name);
                    }

                    const wchar_t* AllMGuild::ValidateAlias::TypeName = L"AllMGuild::ValidateAlias";
                    const HashType AllMGuild::ValidateAlias::TypeHash =
                        StringUtil::Hash(AllMGuild::ValidateAlias::TypeName);
                    void AllMGuild::ValidateAlias::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Alias", Alias);
                    }
                    void AllMGuild::ValidateAlias::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Alias", Alias);
                    }

                    const wchar_t* AllMGuild::AddExp::TypeName = L"AllMGuild::AddExp";
                    const HashType AllMGuild::AddExp::TypeHash =
                        StringUtil::Hash(AllMGuild::AddExp::TypeName);
                    void AllMGuild::AddExp::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void AllMGuild::AddExp::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }
                    const wchar_t* AllMGuild::AddMaintenancePoint::TypeName =
                        L"AllMGuild::AddMaintenancePoint";
                    const HashType AllMGuild::AddMaintenancePoint::TypeHash =
                        StringUtil::Hash(AllMGuild::AddMaintenancePoint::TypeName);
                    void AllMGuild::AddMaintenancePoint::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Point", Point);
                        out.Write(L"ItemHash", ItemHash);
                    }
                    void AllMGuild::AddMaintenancePoint::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Point", Point);
                        in.Read(L"ItemHash", ItemHash);
                    }

                    const wchar_t* AllMGuild::MessageChange::TypeName = L"AllMGuild::MessageChange";
                    const HashType AllMGuild::MessageChange::TypeHash =
                        StringUtil::Hash(AllMGuild::MessageChange::TypeName);
                    void AllMGuild::MessageChange::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Message", Message);
                    }
                    void AllMGuild::MessageChange::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Message", Message);
                    }

                    const wchar_t* AllMGuild::ChangeGrade::TypeName = L"AllMGuild::ChangeGrade";
                    const HashType AllMGuild::ChangeGrade::TypeHash =
                        StringUtil::Hash(AllMGuild::ChangeGrade::TypeName);
                    void AllMGuild::ChangeGrade::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"TargetCharacterName", TargetCharacterName);
                        out.Write(L"Grade", Grade);
                    }
                    void AllMGuild::ChangeGrade::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"TargetCharacterName", TargetCharacterName);
                        in.Read(L"Grade", Grade);
                    }

                    const wchar_t* AllMGuild::ChangeGradeName::TypeName =
                        L"AllMGuild::ChangeGradeName";
                    const HashType AllMGuild::ChangeGradeName::TypeHash =
                        StringUtil::Hash(AllMGuild::ChangeGradeName::TypeName);
                    void AllMGuild::ChangeGradeName::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"TargetGrade", TargetGrade);
                        out.Write(L"TargetGradeName", TargetGradeName);
                    }
                    void AllMGuild::ChangeGradeName::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"TargetGrade", TargetGrade);
                        in.Read(L"TargetGradeName", TargetGradeName);
                    }

                    const wchar_t* AllMGuild::ChangeGradeAuth::TypeName =
                        L"AllMGuild::ChangeGradeAuth";
                    const HashType AllMGuild::ChangeGradeAuth::TypeHash =
                        StringUtil::Hash(AllMGuild::ChangeGradeAuth::TypeName);
                    void AllMGuild::ChangeGradeAuth::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"TargetGrade", TargetGrade);
                        out.Write(L"Authority", Authority);
                    }
                    void AllMGuild::ChangeGradeAuth::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"TargetGrade", TargetGrade);
                        in.Read(L"Authority", Authority);
                    }

                    const wchar_t* AllMGuild::ChangeGuildMaster::TypeName =
                        L"AllMGuild::ChangeGuildMaster";
                    const HashType AllMGuild::ChangeGuildMaster::TypeHash =
                        StringUtil::Hash(AllMGuild::ChangeGuildMaster::TypeName);
                    void AllMGuild::ChangeGuildMaster::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"TargetCharacterName", TargetCharacterName);
                    }
                    void AllMGuild::ChangeGuildMaster::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"TargetCharacterName", TargetCharacterName);
                    }

                    const wchar_t* AllMGuild::GetMyInfo::TypeName = L"AllMGuild::GetMyInfo";
                    const HashType AllMGuild::GetMyInfo::TypeHash =
                        StringUtil::Hash(AllMGuild::GetMyInfo::TypeName);
                    void AllMGuild::GetMyInfo::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void AllMGuild::GetMyInfo::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    const wchar_t* AllMGuild::RefreshMyInfo::TypeName = L"AllMGuild::RefreshMyInfo";
                    const HashType AllMGuild::RefreshMyInfo::TypeHash =
                        StringUtil::Hash(AllMGuild::RefreshMyInfo::TypeName);
                    void AllMGuild::RefreshMyInfo::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void AllMGuild::RefreshMyInfo::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    const wchar_t* AllMGuild::LevelUp::TypeName = L"AllMGuild::LevelUp";
                    const HashType AllMGuild::LevelUp::TypeHash =
                        StringUtil::Hash(AllMGuild::LevelUp::TypeName);
                    void AllMGuild::LevelUp::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Level", level);
                    }
                    void AllMGuild::LevelUp::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Level", level);
                    }

                    const wchar_t* AllMGuild::ReflashGuildLevel::TypeName =
                        L"AllMGuild::ReflashGuildLevel";
                    const HashType AllMGuild::ReflashGuildLevel::TypeHash =
                        StringUtil::Hash(AllMGuild::ReflashGuildLevel::TypeName);
                    void AllMGuild::ReflashGuildLevel::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Level", level);
                    }
                    void AllMGuild::ReflashGuildLevel::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Level", level);
                    }

                    const wchar_t* AllMGuild::ReflashGuildProfitDate::TypeName =
                        L"AllMGuild::ReflashGuildProfitDate";
                    const HashType AllMGuild::ReflashGuildProfitDate::TypeHash =
                        StringUtil::Hash(AllMGuild::ReflashGuildProfitDate::TypeName);
                    void AllMGuild::ReflashGuildProfitDate::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"OpenDate", OpenDate);
                        out.Write(L"CloseDate", CloseDate);
                    }
                    void AllMGuild::ReflashGuildProfitDate::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"OpenDate", OpenDate);
                        in.Read(L"CloseDate", CloseDate);
                    }

                    const wchar_t* AllMGuild::ReflashGuildKicked::TypeName =
                        L"AllMGuild::ReflashGuildKicked";
                    const HashType AllMGuild::ReflashGuildKicked::TypeHash =
                        StringUtil::Hash(AllMGuild::ReflashGuildKicked::TypeName);
                    void AllMGuild::ReflashGuildKicked::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void AllMGuild::ReflashGuildKicked::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    const wchar_t* AllMGuild::GetTax::TypeName = L"AllMGuild::GetTax";
                    const HashType AllMGuild::GetTax::TypeHash =
                        StringUtil::Hash(AllMGuild::GetTax::TypeName);
                    void AllMGuild::GetTax::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Tax", tax);
                    }
                    void AllMGuild::GetTax::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Tax", tax);
                    }
                    const wchar_t* AllMGuild::ReflashGuildShopItem::TypeName =
                        L"AllMGuild::ReflashGuildShopItem";
                    const HashType AllMGuild::ReflashGuildShopItem::TypeHash =
                        StringUtil::Hash(AllMGuild::ReflashGuildShopItem::TypeName);
                    void AllMGuild::ReflashGuildShopItem::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Item", Item);
                    }
                    void AllMGuild::ReflashGuildShopItem::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Item", Item);
                    }

                    /*********************************************************************************
                     * AllMGuild */

                     /* Quest
                      * *********************************************************************************/
                    const wchar_t* Quest::Accept::TypeName = L"Quest::Accept";
                    const HashType Quest::Accept::TypeHash =
                        StringUtil::Hash(Quest::Accept::TypeName);
                    void Quest::Accept::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"QuestHash", QuestHash);
                        out.Write(L"SelectRewardIndex", SelectRewardIndex);
                    }
                    void Quest::Accept::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"QuestHash", QuestHash);
                        in.Read(L"SelectRewardIndex", SelectRewardIndex);
                    }

                    const wchar_t* Quest::Complete::TypeName = L"Quest::Complete";
                    const HashType Quest::Complete::TypeHash =
                        StringUtil::Hash(Quest::Complete::TypeName);
                    void Quest::Complete::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"QuestHash", QuestHash);
                        out.Write(L"SelectRewardIndex", SelectRewardIndex);
                        out.Write(L"count", count);
                    }
                    void Quest::Complete::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"QuestHash", QuestHash);
                        in.Read(L"SelectRewardIndex", SelectRewardIndex);
                        in.Read(L"count", count);
                    }

                    const wchar_t* Quest::Drop::TypeName = L"Quest::Drop";
                    const HashType Quest::Drop::TypeHash =
                        StringUtil::Hash(Quest::Drop::TypeName);
                    void Quest::Drop::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"QuestHash", QuestHash);
                    }
                    void Quest::Drop::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"QuestHash", QuestHash);
                    }

                    const wchar_t* Quest::ActivityItem::TypeName = L"Quest::ActivityItem";
                    const HashType Quest::ActivityItem::TypeHash =
                        StringUtil::Hash(Quest::ActivityItem::TypeName);
                    void Quest::ActivityItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"QuestHash", QuestHash);
                    }
                    void Quest::ActivityItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"QuestHash", QuestHash);
                    }

                    const wchar_t* Quest::ShareToPartyPlayer::TypeName =
                        L"Quest::ShareToPartyPlayer";
                    const HashType Quest::ShareToPartyPlayer::TypeHash =
                        StringUtil::Hash(Quest::ShareToPartyPlayer::TypeName);
                    void Quest::ShareToPartyPlayer::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"QuestHash", QuestHash);
                    }
                    void Quest::ShareToPartyPlayer::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"QuestHash", QuestHash);
                    }

                    const wchar_t* Quest::SharedAnswer::TypeName = L"Quest::SharedAnswer";
                    const HashType Quest::SharedAnswer::TypeHash =
                        StringUtil::Hash(Quest::SharedAnswer::TypeName);
                    void Quest::SharedAnswer::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"QuestHash", QuestHash);
                        out.Write(L"SelectRewardIndex", SelectRewardIndex);
                        out.Write(L"Accept", Accept);
                    }
                    void Quest::SharedAnswer::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"QuestHash", QuestHash);
                        in.Read(L"SelectRewardIndex", SelectRewardIndex);
                        in.Read(L"Accept", Accept);
                    }
                    /*********************************************************************************
                     * Quest */

                     /* PlayerStore
                      * ***************************************************************************/
                    const wchar_t* PlayerStore::List::TypeName = L"PlayerStore::List";
                    const HashType PlayerStore::List::TypeHash =
                        StringUtil::Hash(PlayerStore::List::TypeName);
                    void PlayerStore::List::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PlayerSerial", PlayerSerial);
                    }
                    void PlayerStore::List::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PlayerSerial", PlayerSerial);
                    }

                    const wchar_t* PlayerStore::Buy::TypeName = L"PlayerStore::Buy";
                    const HashType PlayerStore::Buy::TypeHash =
                        StringUtil::Hash(PlayerStore::Buy::TypeName);
                    void PlayerStore::Buy::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PlayerSerial", PlayerSerial);
                        out.Write(L"Item", Item);
                    }
                    void PlayerStore::Buy::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PlayerSerial", PlayerSerial);
                        in.Read(L"Item", Item);
                    }

                    const wchar_t* PlayerStore::BuyPet::TypeName = L"PlayerStore::BuyPet";
                    const HashType PlayerStore::BuyPet::TypeHash =
                        StringUtil::Hash(PlayerStore::BuyPet::TypeName);
                    void PlayerStore::BuyPet::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PlayerSerial", PlayerSerial);
                        out.Write(L"Item", Item);
                        out.Write(L"PetSerial", PetSerial);
                    }
                    void PlayerStore::BuyPet::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PlayerSerial", PlayerSerial);
                        in.Read(L"Item", Item);
                        in.Read(L"PetSerial", PetSerial);
                    }

                    const wchar_t* PlayerStore::Close::TypeName = L"PlayerStore::Close";
                    const HashType PlayerStore::Close::TypeHash =
                        StringUtil::Hash(PlayerStore::Close::TypeName);
                    void PlayerStore::Close::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void PlayerStore::Close::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }
                    /***************************************************************************
                     * PlayerStore */

                     /* Bank ***********************************************************************/
                    const wchar_t* Bank::List::TypeName = L"Bank::List";
                    const HashType Bank::List::TypeHash =
                        StringUtil::Hash(Bank::List::TypeName);
                    void Bank::List::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"BagNumber", BagNumber);
                    }
                    void Bank::List::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"BagNumber", BagNumber);
                    }

                    const wchar_t* Bank::Push::TypeName = L"Bank::Push";
                    const HashType Bank::Push::TypeHash =
                        StringUtil::Hash(Bank::Push::TypeName);
                    void Bank::Push::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"SourceInventory", SourceInventory);
                        out.Write(L"TargetStorage", TargetStorage);
                        out.Write(L"Count", Count);
                    }
                    void Bank::Push::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"SourceInventory", SourceInventory);
                        in.Read(L"TargetStorage", TargetStorage);
                        in.Read(L"Count", Count);
                    }

                    const wchar_t* Bank::Pop::TypeName = L"Bank::Pop";
                    const HashType Bank::Pop::TypeHash =
                        StringUtil::Hash(Bank::Pop::TypeName);
                    void Bank::Pop::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"SourceStorage", SourceStorage);
                        out.Write(L"TargetInventory", TargetInventory);
                        out.Write(L"ItemHash", ItemHash);  // 3.1 by ultimate
                        out.Write(L"Count", Count);
                    }
                    void Bank::Pop::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"SourceStorage", SourceStorage);
                        in.Read(L"TargetInventory", TargetInventory);
                        in.Read(L"ItemHash", ItemHash);  // 3.1 by ultimate
                        in.Read(L"Count", Count);
                    }

                    const wchar_t* Bank::SwapItemInStorage::TypeName = L"Bank::SwapItemInStorage";
                    const HashType Bank::SwapItemInStorage::TypeHash =
                        StringUtil::Hash(Bank::SwapItemInStorage::TypeName);
                    void Bank::SwapItemInStorage::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"SourceStorage", SourceStorage);
                        out.Write(L"TargetStorage", TargetStorage);
                        out.Write(L"ItemHash", ItemHash);
                        out.Write(L"Count", Count);
                    }
                    void Bank::SwapItemInStorage::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"SourceStorage", SourceStorage);
                        in.Read(L"TargetStorage", TargetStorage);
                        in.Read(L"ItemHash", ItemHash);
                        in.Read(L"Count", Count);
                    }

                    const wchar_t* Bank::Move::TypeName = L"Bank::Move";
                    const HashType Bank::Move::TypeHash =
                        StringUtil::Hash(Bank::Move::TypeName);
                    void Bank::Move::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"SourceInventory", SourceInventory);
                        out.Write(L"TargetStorage", TargetStorage);
                    }
                    void Bank::Move::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"SourceInventory", SourceInventory);
                        in.Read(L"TargetStorage", TargetStorage);
                    }
                    /*********************************************************************** Bank */

                    /* CashItemStorage
                     * ***********************************************************************/
                    const wchar_t* CashItemStorage::List::TypeName = L"CashItemStorage::List";
                    const HashType CashItemStorage::List::TypeHash =
                        StringUtil::Hash(CashItemStorage::List::TypeName);
                    void CashItemStorage::List::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PageIndex", PageIndex);
                    }
                    void CashItemStorage::List::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PageIndex", PageIndex);
                    }

                    const wchar_t* CashItemStorage::Push::TypeName = L"CashItemStorage::Push";
                    const HashType CashItemStorage::Push::TypeHash =
                        StringUtil::Hash(CashItemStorage::Push::TypeName);
                    void CashItemStorage::Push::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"SourceInventory", SourceInventory);
                        out.Write(L"TargetStorage", TargetStorage);
                    }
                    void CashItemStorage::Push::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"SourceInventory", SourceInventory);
                        in.Read(L"TargetStorage", TargetStorage);
                    }

                    const wchar_t* CashItemStorage::Pop::TypeName = L"CashItemStorage::Pop";
                    const HashType CashItemStorage::Pop::TypeHash =
                        StringUtil::Hash(CashItemStorage::Pop::TypeName);
                    void CashItemStorage::Pop::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"SourceStorage", SourceStorage);
                        out.Write(L"TargetInventory", TargetInventory);
                    }
                    void CashItemStorage::Pop::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"SourceStorage", SourceStorage);
                        in.Read(L"TargetInventory", TargetInventory);
                    }

                    const wchar_t* CashItemStorage::MoveItemInStorage::TypeName =
                        L"CashItemStorage::MoveItemInStorage";
                    const HashType CashItemStorage::MoveItemInStorage::TypeHash =
                        StringUtil::Hash(CashItemStorage::MoveItemInStorage::TypeName);
                    void CashItemStorage::MoveItemInStorage::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"SourceStorage", SourceStorage);
                        out.Write(L"TargetStorage", TargetStorage);
                    }
                    void CashItemStorage::MoveItemInStorage::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"SourceStorage", SourceStorage);
                        in.Read(L"TargetStorage", TargetStorage);
                    }

                    const wchar_t* CashItemStorage::Stack::TypeName = L"CashItemStorage::Stack";
                    const HashType CashItemStorage::Stack::TypeHash =
                        StringUtil::Hash(CashItemStorage::Stack::TypeName);
                    void CashItemStorage::Stack::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"SourceInventory", SourceInventory);
                        out.Write(L"TargetStorage", TargetStorage);
                        out.Write(L"Count", Count);
                    }
                    void CashItemStorage::Stack::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"SourceInventory", SourceInventory);
                        in.Read(L"TargetStorage", TargetStorage);
                        in.Read(L"Count", Count);
                    }
                    /***********************************************************************
                     * CashItemStorage */

                     /* PVP ***********************************************************************/
                    const wchar_t* Pvp::PvpLeaveStage::TypeName = L"Pvp::PvpLeaveStage";
                    const HashType Pvp::PvpLeaveStage::TypeHash =
                        StringUtil::Hash(Pvp::PvpLeaveStage::TypeName);
                    void Pvp::PvpLeaveStage::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void Pvp::PvpLeaveStage::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }
                    /*********************************************************************** PVP */

                    /* Mail ***********************************************************************/
                    const wchar_t* Mail::RequestMailList::TypeName = L"Mail::RequestMailList";
                    const HashType Mail::RequestMailList::TypeHash =
                        StringUtil::Hash(Mail::RequestMailList::TypeName);
                    void Mail::RequestMailList::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void Mail::RequestMailList::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    const wchar_t* Mail::ReadMail::TypeName = L"Mail::ReadMail";
                    const HashType Mail::ReadMail::TypeHash =
                        StringUtil::Hash(Mail::ReadMail::TypeName);
                    void Mail::ReadMail::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Index", Index);
                    }
                    void Mail::ReadMail::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Index", Index);
                    }

                    const wchar_t* Mail::SendMail::TypeName = L"Mail::SendMail";
                    const HashType Mail::SendMail::TypeHash =
                        StringUtil::Hash(Mail::SendMail::TypeName);
                    void Mail::SendMail::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Receiver", Receiver);
                        out.Write(L"Message", Message);
                        out.Write(L"AttachedMoney", AttachedMoney);
                        out.Write(L"AttachedItems", AttachedItems);
                        out.Write(L"StampItemPosition", StampItemPosition);
                    }
                    void Mail::SendMail::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Receiver", Receiver);
                        in.Read(L"Message", Message);
                        in.Read(L"AttachedMoney", AttachedMoney);
                        in.Read(L"AttachedItems", AttachedItems);
                        in.Read(L"StampItemPosition", StampItemPosition);
                    }

                    const wchar_t* Mail::DisposeMail::TypeName = L"Mail::DisposeMail";
                    const HashType Mail::DisposeMail::TypeHash =
                        StringUtil::Hash(Mail::DisposeMail::TypeName);
                    void Mail::DisposeMail::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Index", Index);
                    }
                    void Mail::DisposeMail::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Index", Index);
                    }

                    const wchar_t* Mail::DisposeReadMails::TypeName = L"Mail::DisposeReadMails";
                    const HashType Mail::DisposeReadMails::TypeHash =
                        StringUtil::Hash(Mail::DisposeReadMails::TypeName);
                    void Mail::DisposeReadMails::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void Mail::DisposeReadMails::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    const wchar_t* Mail::RetrieveAttached::TypeName = L"Mail::RetrieveAttached";
                    const HashType Mail::RetrieveAttached::TypeHash =
                        StringUtil::Hash(Mail::RetrieveAttached::TypeName);
                    void Mail::RetrieveAttached::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Index", Index);
                    }
                    void Mail::RetrieveAttached::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Index", Index);
                    }
                    /*********************************************************************** Mail */

                    /* Fishing
                     * ***********************************************************************/
                    const wchar_t* Fishing::Start::TypeName = L"Fishing::Start";
                    const HashType Fishing::Start::TypeHash =
                        StringUtil::Hash(Fishing::Start::TypeName);
                    void Fishing::Start::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void Fishing::Start::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    const wchar_t* Fishing::End::TypeName = L"Fishing::End";
                    const HashType Fishing::End::TypeHash =
                        StringUtil::Hash(Fishing::End::TypeName);
                    void Fishing::End::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void Fishing::End::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    const wchar_t* Fishing::Do::TypeName = L"Fishing::Do";
                    const HashType Fishing::Do::TypeHash =
                        StringUtil::Hash(Fishing::Do::TypeName);
                    void Fishing::Do::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"BaitHash", BaitHash);
                        out.Write(L"BaitPosition", BaitPosition);
                        // out.Write(L"BaitCount", BaitCount);
                    }
                    void Fishing::Do::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"BaitHash", BaitHash);
                        in.Read(L"BaitPosition", BaitPosition);
                        // in.Read(L"BaitCount", BaitCount);
                    }

                    const wchar_t* Fishing::ChangeBaitCount::TypeName = L"Fishing::ChangeBaitCount";
                    const HashType Fishing::ChangeBaitCount::TypeHash =
                        StringUtil::Hash(Fishing::ChangeBaitCount::TypeName);
                    void Fishing::ChangeBaitCount::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Count", Count);
                        // out.Write(L"BaitCount", BaitCount);
                    }
                    void Fishing::ChangeBaitCount::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Count", Count);
                        // in.Read(L"BaitCount", BaitCount);
                    }

                    const wchar_t* Fishing::List::TypeName = L"Fishing::List";
                    const HashType Fishing::List::TypeHash =
                        StringUtil::Hash(Fishing::List::TypeName);
                    void Fishing::List::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void Fishing::List::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }
                    /*********************************************************************** Fishing
                     */

                     /* Pvp
                      * ***********************************************************************************/
                      // const wchar_t* Pvp::CheckLatency::TypeName=L"Pvp::CheckLatency";
                      // const HashType
                      // Pvp::CheckLatency::TypeHash=StringUtil::Hash(Pvp::CheckLatency::TypeName); void
                      // Pvp::CheckLatency::Serialize(Serializer::IStreamWriter& out) const
                      //{
                      //	out.Begin(TypeName);
                      //}
                      // void Pvp::CheckLatency::Deserialize(Serializer::IStreamReader& in)
                      //{
                      //	in.Begin(TypeName);
                      //}
                      /***********************************************************************************
                       * Pvp */

                       /* Gamble
                        * ***********************************************************************************/
                    const wchar_t* Gamble::SlimeRace::Bet::TypeName = L"Gamble::SlimeRace::Bet";
                    const HashType Gamble::SlimeRace::Bet::TypeHash =
                        StringUtil::Hash(Gamble::SlimeRace::Bet::TypeName);
                    void Gamble::SlimeRace::Bet::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Position", Position);
                        out.Write(L"Chips", Chips);
                    }
                    void Gamble::SlimeRace::Bet::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Position", Position);
                        in.Read(L"Chips", Chips);
                    }

                    const wchar_t* Gamble::SlimeRace::ClearBet::TypeName =
                        L"Gamble::SlimeRace::ClearBet";
                    const HashType Gamble::SlimeRace::ClearBet::TypeHash =
                        StringUtil::Hash(Gamble::SlimeRace::ClearBet::TypeName);
                    void Gamble::SlimeRace::ClearBet::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Position", Position);
                    }
                    void Gamble::SlimeRace::ClearBet::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Position", Position);
                    }

                    const wchar_t* Gamble::SlimeRace::RequestBettingState::TypeName =
                        L"Gamble::SlimeRace::RequestBettingState";
                    const HashType Gamble::SlimeRace::RequestBettingState::TypeHash =
                        StringUtil::Hash(Gamble::SlimeRace::RequestBettingState::TypeName);
                    void Gamble::SlimeRace::RequestBettingState::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void Gamble::SlimeRace::RequestBettingState::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    const wchar_t* Gamble::SlimeRace::RequestRecentResults::TypeName =
                        L"Gamble::SlimeRace::RequestRecentResults";
                    const HashType Gamble::SlimeRace::RequestRecentResults::TypeHash =
                        StringUtil::Hash(Gamble::SlimeRace::RequestRecentResults::TypeName);
                    void Gamble::SlimeRace::RequestRecentResults::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void Gamble::SlimeRace::RequestRecentResults::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }
                    /***********************************************************************************
                     * Gamble */

                    const wchar_t* SaveKeySetting::TypeName = L"SaveKeySetting";
                    const HashType SaveKeySetting::TypeHash =
                        StringUtil::Hash(SaveKeySetting::TypeName);
                    void SaveKeySetting::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Account", Account);
                        std::wstring ConvertedCodes =
                            StringUtil::Replace(Keycodes, L"K_INPUT", L"$K");
                        ConvertedCodes =
                            StringUtil::Replace(ConvertedCodes, L"UI_INPUT", L"$U");
                        ConvertedCodes =
                            StringUtil::Replace(ConvertedCodes, L"pressed", L"$p");
                        ConvertedCodes =
                            StringUtil::Replace(ConvertedCodes, L"KEY_ARROW", L"$A");
                        ConvertedCodes =
                            StringUtil::Replace(ConvertedCodes, L"KEY_PAD", L"$P");
                        ConvertedCodes =
                            StringUtil::Replace(ConvertedCodes, L"KEY_NUM", L"$N");
                        ConvertedCodes =
                            StringUtil::Replace(ConvertedCodes, L"JOPAD_BUTTON", L"$J");
                        ConvertedCodes =
                            StringUtil::Replace(ConvertedCodes, L"QuickSlot", L"$Q");
                        out.Write(L"Keycodes", ConvertedCodes);
                    }
                    void SaveKeySetting::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Account", Account);
                        in.Read(L"Keycodes", Keycodes);
                        // Keycodes = AllM::StringUtil::Replace( Keycodes, L"$Q", L"QuickSlot" );
                        // Keycodes = AllM::StringUtil::Replace( Keycodes, L"$J", L"JOPAD_BUTTON" );
                        // Keycodes = AllM::StringUtil::Replace( Keycodes, L"$N", L"KEY_NUM" );
                        // Keycodes = AllM::StringUtil::Replace( Keycodes, L"$P", L"KEY_PAD" );
                        // Keycodes = AllM::StringUtil::Replace( Keycodes, L"$A", L"KEY_ARROW" );
                        // Keycodes = AllM::StringUtil::Replace( Keycodes, L"$p", L"pressed" );
                        // Keycodes = AllM::StringUtil::Replace( Keycodes, L"$U", L"UI_INPUT" );
                        // Keycodes = AllM::StringUtil::Replace( Keycodes, L"$K", L"K_INPUT" );
                        // /// by kpongky( 09.07.28 ) for DB Error
                    }

                    //////////////////////////////////////////////////////////////////////////////////////////////////
                    // Family
                    // Invite
                    const wchar_t* Family::Invite::TypeName = L"Family::Invite";
                    const HashType Family::Invite::TypeHash =
                        StringUtil::Hash(Family::Invite::TypeName);
                    void Family::Invite::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"TargetSerial", TargetSerial);
                        out.Write(L"Target", Target);
                    }
                    void Family::Invite::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"TargetSerial", TargetSerial);
                        in.Read(L"Target", Target);
                    }

                    // Leave
                    const wchar_t* Family::Leave::TypeName = L"Family::Leave";
                    const HashType Family::Leave::TypeHash =
                        StringUtil::Hash(Family::Leave::TypeName);
                    void Family::Leave::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void Family::Leave::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    // InviteAnwer
                    const wchar_t* Family::InviteAnwer::TypeName = L"Family::InviteAnwer";
                    const HashType Family::InviteAnwer::TypeHash =
                        StringUtil::Hash(Family::InviteAnwer::TypeName);
                    void Family::InviteAnwer::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"IsJoin", IsJoin);
                    }
                    void Family::InviteAnwer::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"IsJoin", IsJoin);
                    }

                    // ReflashInfo
                    const wchar_t* Family::RefreshInfo::TypeName = L"Family::RefreshInfo";
                    const HashType Family::RefreshInfo::TypeHash =
                        StringUtil::Hash(Family::RefreshInfo::TypeName);
                    void Family::RefreshInfo::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void Family::RefreshInfo::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    // RequestPlayTimePresent
                    const wchar_t* Family::RequestPresent::TypeName = L"Family::RequestPresent";
                    const HashType Family::RequestPresent::TypeHash =
                        StringUtil::Hash(Family::RequestPresent::TypeName);
                    void Family::RequestPresent::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Type", Type);
                    }
                    void Family::RequestPresent::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Type", Type);
                    }

                    // RequestPlayTimePresent
                    const wchar_t* Family::Kick::TypeName = L"Family::Kick";
                    const HashType Family::Kick::TypeHash =
                        StringUtil::Hash(Family::Kick::TypeName);
                    void Family::Kick::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"TargetCharacterName", TargetCharacterName);
                    }
                    void Family::Kick::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"TargetCharacterName", TargetCharacterName);
                    }

                    // PartyLog
                    const wchar_t* PartyLog::TypeName = L"PartyLog";
                    const HashType PartyLog::TypeHash =
                        StringUtil::Hash(PartyLog::TypeName);
                    void PartyLog::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"LogType", LogType);
                        out.Write(L"PartyType", PartyType);
                        out.Write(L"StageGroupHash", StageGroupHash);
                        out.Write(L"AccessLevel", AccessLevel);
                        out.Write(L"Recommend", Recommend);

                        out.Write(L"BannedCharacterName", BannedCharacterName);
                        out.Write(L"PartyStartDuration", PartyStartDuration);
                        out.Write(L"PlayerCount", PlayerCount);
                    }

                    void PartyLog::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"LogType", LogType);
                        in.Read(L"PartyType", PartyType);
                        in.Read(L"StageGroupHash", StageGroupHash);
                        in.Read(L"AccessLevel", AccessLevel);
                        in.Read(L"Recommend", Recommend);

                        in.Read(L"BannedCharacterName", BannedCharacterName);
                        in.Read(L"PartyStartDuration", PartyStartDuration);
                        in.Read(L"PlayerCount", PlayerCount);
                    }

                    //////////////////////////////////////////////////////////////////////////////////////////////////
                    // OpenMarket
                    // RequestProductList
                    const wchar_t* OpenMarket::RequestProductList::TypeName =
                        L"OpenMarket::RequestProductList";
                    const HashType OpenMarket::RequestProductList::TypeHash =
                        StringUtil::Hash(OpenMarket::RequestProductList::TypeName);
                    void OpenMarket::RequestProductList::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"MinLevel", MinLevel);
                        out.Write(L"MaxLevel", MaxLevel);
                        out.Write(L"Keyword", Keyword);
                        out.Write(L"Category", Category);
                        out.WriteEnum(L"OrderType", OrderType);
                        out.WriteEnum(L"ClassType", ClassType);
                        out.Write(L"PageNumber", PageNumber);
                        out.Write(L"IsWearable", IsWearable);
                    }
                    void OpenMarket::RequestProductList::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"MinLevel", MinLevel);
                        in.Read(L"MaxLevel", MaxLevel);
                        in.Read(L"Keyword", Keyword);
                        in.Read(L"Category", Category);
                        in.ReadEnum(L"OrderType", OrderType);
                        in.ReadEnum(L"ClassType", ClassType);
                        in.Read(L"PageNumber", PageNumber);
                        in.Read(L"IsWearable", IsWearable);
                    }

                    // RequestProductInfo
                    const wchar_t* OpenMarket::RequestProductInfo::TypeName =
                        L"OpenMarket::RequestProductInfo";
                    const HashType OpenMarket::RequestProductInfo::TypeHash =
                        StringUtil::Hash(OpenMarket::RequestProductInfo::TypeName);
                    void OpenMarket::RequestProductInfo::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"RegistrationNumber", RegistrationNumber);
                    }
                    void OpenMarket::RequestProductInfo::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"RegistrationNumber", RegistrationNumber);
                    }

                    // RequestItemInfo
                    const wchar_t* OpenMarket::RequestItemInfo::TypeName =
                        L"OpenMarket::RequestItemInfo";
                    const HashType OpenMarket::RequestItemInfo::TypeHash =
                        StringUtil::Hash(OpenMarket::RequestItemInfo::TypeName);
                    void OpenMarket::RequestItemInfo::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"ItemPosition", ItemPosition);
                    }
                    void OpenMarket::RequestItemInfo::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"ItemPosition", ItemPosition);
                    }

                    // RequestItemInfo
                    const wchar_t* OpenMarket::BuyProduct::TypeName = L"OpenMarket::BuyProduct";
                    const HashType OpenMarket::BuyProduct::TypeHash =
                        StringUtil::Hash(OpenMarket::BuyProduct::TypeName);
                    void OpenMarket::BuyProduct::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"RegistrationNumber", RegistrationNumber);
                    }
                    void OpenMarket::BuyProduct::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"RegistrationNumber", RegistrationNumber);
                    }

                    // RegisterItem
                    const wchar_t* OpenMarket::RegisterItem::TypeName = L"OpenMarket::RegisterItem";
                    const HashType OpenMarket::RegisterItem::TypeHash =
                        StringUtil::Hash(OpenMarket::RegisterItem::TypeName);
                    void OpenMarket::RegisterItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"ItemName", ItemName);
                        out.Write(L"ItemPosition", ItemPosition);
                        out.Write(L"PremiumItemPosition", PremiumItemPosition);
                        out.Write(L"Price", Price);
                        out.Write(L"RegistrationPeriod", RegistrationPeriod);
                    }
                    void OpenMarket::RegisterItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"ItemName", ItemName);
                        in.Read(L"ItemPosition", ItemPosition);
                        in.Read(L"PremiumItemPosition", PremiumItemPosition);
                        in.Read(L"Price", Price);
                        in.Read(L"RegistrationPeriod", RegistrationPeriod);
                    }

                    // RegistrationCancel
                    const wchar_t* OpenMarket::RegistrationCancel::TypeName =
                        L"OpenMarket::RegistrationCancel";
                    const HashType OpenMarket::RegistrationCancel::TypeHash =
                        StringUtil::Hash(OpenMarket::RegistrationCancel::TypeName);
                    void OpenMarket::RegistrationCancel::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"RegistrationNumber", RegistrationNumber);
                    }
                    void OpenMarket::RegistrationCancel::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"RegistrationNumber", RegistrationNumber);
                    }

                    // RequestMyProductList
                    const wchar_t* OpenMarket::RequestMyProductList::TypeName =
                        L"OpenMarket::RequestMyProductList";
                    const HashType OpenMarket::RequestMyProductList::TypeHash =
                        StringUtil::Hash(OpenMarket::RequestMyProductList::TypeName);
                    void OpenMarket::RequestMyProductList::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void OpenMarket::RequestMyProductList::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    const wchar_t* CashShop::RequestCashAmount::TypeName =
                        L"CashShop::RequestCashAmount";
                    const HashType CashShop::RequestCashAmount::TypeHash =
                        StringUtil::Hash(CashShop::RequestCashAmount::TypeName);
                    void CashShop::RequestCashAmount::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void CashShop::RequestCashAmount::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    const wchar_t* CashShop::Purchase::TypeName = L"CashShop::Purchase";
                    const HashType CashShop::Purchase::TypeHash =
                        StringUtil::Hash(CashShop::Purchase::TypeName);
                    void CashShop::Purchase::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"OidProduct", oidProduct);
                        out.Write(L"OrderQuantity", orderQuantity);
                        out.Write(L"OidCouponUser", oidCouponUser);
                    }
                    void CashShop::Purchase::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"OidProduct", oidProduct);
                        in.Read(L"OrderQuantity", orderQuantity);
                        in.Read(L"OidCouponUser", oidCouponUser);
                    }

                    const wchar_t* CashShop::Present::TypeName = L"CashShop::Present";
                    const HashType CashShop::Present::TypeHash =
                        StringUtil::Hash(CashShop::Present::TypeName);
                    void CashShop::Present::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"OidProduct", oidProduct);
                        out.Write(L"OrderQuantity", orderQuantity);
                        out.Write(L"OidCouponUser", oidCouponUser);
                        out.Write(L"Receiver", receiver);
                        out.Write(L"Message", message);
                    }
                    void CashShop::Present::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"OidProduct", oidProduct);
                        in.Read(L"OrderQuantity", orderQuantity);
                        in.Read(L"OidCouponUser", oidCouponUser);
                        in.Read(L"Receiver", receiver);
                        in.Read(L"Message", message);
                    }

                    const wchar_t* CashShop::RegistCoupon::TypeName = L"CashShop::RegistCoupon";
                    const HashType CashShop::RegistCoupon::TypeHash =
                        StringUtil::Hash(CashShop::RegistCoupon::TypeName);
                    void CashShop::RegistCoupon::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Serial", serial);
                    }
                    void CashShop::RegistCoupon::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Serial", serial);
                    }
                }  // namespace ToServer
			}
		}
	}
}