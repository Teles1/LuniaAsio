#include <StageServer/Protocol/FromServer.h>

namespace Lunia
{
	namespace XRated
	{
		namespace StageServer
		{
			namespace Protocol
			{
                namespace FromServer {
                    // Error
                    // //////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Error::TypeName = L"Error";
                    const HashType Error::TypeHash = StringUtil::Hash(Error::TypeName);
                    void Error::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"errorcode", errorcode);
                        out.Write(L"errorstring", errorstring);
                    }

                    void Error::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"errorcode", errorcode);
                        in.Read(L"errorstring", errorstring);
                    }
                    //////////////////////////////////////////////////////////////////////////////////
                    ///Error //

                    // Way
                    // ////////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Way::TypeName = L"Way";
                    const HashType Way::TypeHash = StringUtil::Hash(Way::TypeName);
                    void Way::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"EncryptKey", EncryptKey);
                    }
                    void Way::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"EncryptKey", EncryptKey);
                    }
                    ////////////////////////////////////////////////////////////////////////////////////
                    ///Way //

                    // Alive
                    // ////////////////////////////////////////////////////////////////////////////////////
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
                    ////////////////////////////////////////////////////////////////////////////////////
                    ///Alive //

                    // stage
                    // //////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Stage::TypeName = L"Stage";
                    const HashType Stage::TypeHash = StringUtil::Hash(Stage::TypeName);
                    void Stage::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"charactername", charactername);
                        out.Write(L"targetStage", targetStage);
                    }

                    void Stage::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"charactername", charactername);
                        in.Read(L"targetStage", targetStage);
                    }
                    //////////////////////////////////////////////////////////////////////////////////
                    ///Stage //

                    // Floor
                    // //////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Floor::TypeName = L"Floor";
                    const HashType Floor::TypeHash = StringUtil::Hash(Floor::TypeName);
                    void Floor::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"floor", floor);
                    }

                    void Floor::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"floor", floor);
                    }
                    //////////////////////////////////////////////////////////////////////////////////
                    ///Floor //

                    // Granted
                    // ////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Granted::TypeName = L"Granted";
                    const HashType Granted::TypeHash =
                        StringUtil::Hash(Granted::TypeName);
                    void Granted::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"charactername", charactername);
                        out.Write(L"targetStage", targetStage);
                    }

                    void Granted::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"charactername", charactername);
                        in.Read(L"targetStage", targetStage);
                    }
                    ////////////////////////////////////////////////////////////////////////////////
                    ///Granted //

                    // CharacterInfo
                    // //////////////////////////////////////////////////////////////////////////
                    const wchar_t* CharacterInfo::TypeName = L"CharacterInfo";
                    const HashType CharacterInfo::TypeHash =
                        StringUtil::Hash(CharacterInfo::TypeName);
                    void CharacterInfo::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"classtype", classtype);
                        out.Write(L"level", level);
                        out.Write(L"xp", xp);
                        out.Write(L"pvpLevel", pvpLevel);
                        out.Write(L"pvpXp", pvpXp);
                        out.Write(L"warLevel", warLevel);
                        out.Write(L"warXp", warXp);
                        out.Write(L"storedLevel", storedLevel);
                        out.Write(L"rebirthCount", rebirthCount);
                        out.Write(L"money", money);
                        out.Write(L"bankMoney", bankMoney);
                        out.Write(L"life", life);
                        out.Write(L"skillpoint", skillpoint);
                        out.Write(L"addedSkillPointPlus", addedSkillPointPlus);
                        out.Write(L"storedSkillPoint", storedSkillPoint);
                        out.Write(L"ladderPoint", ladderPoint);
                        out.Write(L"ladderMatchCount", ladderMatchCount);
                        out.Write(L"ladderWinCount", ladderWinCount);
                        out.Write(L"ladderLoseCount", ladderLoseCount);
                        out.Write(L"achievementScore", achievementScore);  // 3.1 by Robotex
                        out.Write(L"IsSpectator", IsSpectator);
                    }

                    void CharacterInfo::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"classtype", classtype);
                        in.Read(L"level", level);
                        in.Read(L"xp", xp);
                        in.Read(L"pvpLevel", pvpLevel);
                        in.Read(L"pvpXp", pvpXp);
                        in.Read(L"warLevel", warLevel);
                        in.Read(L"warXp", warXp);
                        in.Read(L"storedLevel", storedLevel);
                        in.Read(L"rebirthCount", rebirthCount);
                        in.Read(L"money", money);
                        in.Read(L"bankMoney", bankMoney);
                        in.Read(L"life", life);
                        in.Read(L"skillpoint", skillpoint);
                        in.Read(L"addedSkillPointPlus", addedSkillPointPlus);
                        in.Read(L"storedSkillPoint", storedSkillPoint);
                        in.Read(L"ladderPoint", ladderPoint);
                        in.Read(L"ladderMatchCount", ladderMatchCount);
                        in.Read(L"ladderWinCount", ladderWinCount);
                        in.Read(L"ladderLoseCount", ladderLoseCount);
                        in.Read(L"achievementScore", achievementScore);  // 3.1 by Robotex
                        in.Read(L"IsSpectator", IsSpectator);
                    }
                    //////////////////////////////////////////////////////////////////////////
                    ///CharacterInfo //

                    // BonusLife
                    // //////////////////////////////////////////////////////////////////////////
                    const wchar_t* BonusLife::TypeName = L"BonusLife";
                    const HashType BonusLife::TypeHash =
                        StringUtil::Hash(BonusLife::TypeName);
                    void BonusLife::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"bonusLife", bonusLife);
                        out.Write(L"usableBonusLife", usableBonusLife);
                    }

                    void BonusLife::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"bonusLife", bonusLife);
                        in.Read(L"usableBonusLife", usableBonusLife);
                    }
                    //////////////////////////////////////////////////////////////////////////
                    ///BonusLife //

                    // ListQuickSlot
                    // //////////////////////////////////////////////////////////////////////////
                    const wchar_t* ListQuickSlot::TypeName = L"ListQuickSlot";
                    const HashType ListQuickSlot::TypeHash =
                        StringUtil::Hash(ListQuickSlot::TypeName);
                    void ListQuickSlot::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"quickslotlist", quickslotlist);
                    }

                    void ListQuickSlot::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"quickslotlist", quickslotlist);
                    }

                    //////////////////////////////////////////////////////////////////////////
                    ///ListQuickSlot //

                    // ListSkill
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* ListSkill::TypeName = L"ListSkill";
                    const HashType ListSkill::TypeHash =
                        StringUtil::Hash(ListSkill::TypeName);
                    void ListSkill::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"listskill", listskill);
                    };

                    void ListSkill::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"listskill", listskill);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///ListSkill //

                    // ListSkillLicenses
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* ListSkillLicenses::TypeName = L"ListSkillLicenses";
                    const HashType ListSkillLicenses::TypeHash =
                        StringUtil::Hash(ListSkillLicenses::TypeName);
                    void ListSkillLicenses::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"listSkillLicenses", listSkillLicenses);
                    };

                    void ListSkillLicenses::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"listSkillLicenses", listSkillLicenses);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///ListSkillLicenses //

                    // ListItem
                    // ///////////////////////////////////////////////////////////////////////////////
                    const wchar_t* ListItem::TypeName = L"ListItem";
                    const HashType ListItem::TypeHash =
                        StringUtil::Hash(ListItem::TypeName);
                    void ListItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"listitem", listitem);
                    };

                    void ListItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"listitem", listitem);
                    }
                    ///////////////////////////////////////////////////////////////////////////////
                    ///ListItem //

                    // LoadEnd
                    // ////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* LoadEnd::TypeName = L"LoadEnd";
                    const HashType LoadEnd::TypeHash =
                        StringUtil::Hash(LoadEnd::TypeName);
                    void LoadEnd::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"charName", charName);
                        out.Write(L"progress", progress);
                    };

                    void LoadEnd::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"charName", charName);
                        in.Read(L"progress", progress);
                    }
                    ////////////////////////////////////////////////////////////////////////////////
                    ///LoadEnd //

                    // Chat
                    // ///////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Chat::TypeName = L"Chat";
                    const HashType Chat::TypeHash = StringUtil::Hash(Chat::TypeName);
                    void Chat::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"playerserial", playerserial);
                        out.WriteEnum(L"chattype", chattype);
                        out.Write(L"message", message);
                    }

                    void Chat::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"playerserial", playerserial);
                        in.ReadEnum(L"chattype", chattype);
                        in.Read(L"message", message);
                    }
                    ///////////////////////////////////////////////////////////////////////////////////
                    ///Chat //

                    // SpectatorChat
                    // ///////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* SpectatorChat::TypeName = L"SpectatorChat";
                    const HashType SpectatorChat::TypeHash =
                        StringUtil::Hash(SpectatorChat::TypeName);
                    void SpectatorChat::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"characterName", characterName);
                        out.WriteEnum(L"chattype", chattype);
                        out.Write(L"message", message);
                    }

                    void SpectatorChat::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"characterName", characterName);
                        in.ReadEnum(L"chattype", chattype);
                        in.Read(L"message", message);
                    }
                    ///////////////////////////////////////////////////////////////////////////////////
                    ///SpectatorChat //

                    // Voice
                    // //////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Voice::TypeName = L"Voice";
                    const HashType Voice::TypeHash = StringUtil::Hash(Voice::TypeName);
                    void Voice::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"playerserial", playerserial);
                        out.Write(L"chattype", chattype);
                        out.Write(L"messageid", messageid);
                    }

                    void Voice::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"playerserial", playerserial);
                        in.Read(L"chattype", chattype);
                        in.Read(L"messageid", messageid);
                    }
                    //////////////////////////////////////////////////////////////////////////////////
                    ///Voice //

                    // StageMove
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* StageMove::TypeName = L"StageMove";
                    const HashType StageMove::TypeHash =
                        StringUtil::Hash(StageMove::TypeName);
                    void StageMove::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"serverAddress", serverAddress);
                        out.Write(L"portNumber", portNumber);
                        out.Write(L"keyCode", keyCode);
                    }

                    void StageMove::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"serverAddress", serverAddress);
                        in.Read(L"portNumber", portNumber);
                        in.Read(L"keyCode", keyCode);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///StageMove //

                    // Cast
                    // ///////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Cast::TypeName = L"Cast";
                    const HashType Cast::TypeHash = StringUtil::Hash(Cast::TypeName);
                    void Cast::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"result", result);
                        out.Write(L"playerserial", playerserial);
                        out.Write(L"skillgroupid", skillgroupid);
                        out.Write(L"skilllevel", skilllevel);
                    }

                    void Cast::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"result", result);
                        in.Read(L"playerserial", playerserial);
                        in.Read(L"skillgroupid", skillgroupid);
                        in.Read(L"skilllevel", skilllevel);
                    }
                    ///////////////////////////////////////////////////////////////////////////////////
                    ///Cast //

                    // SetSkillLevel
                    // //////////////////////////////////////////////////////////////////////////
                    const wchar_t* SetSkillLevel::TypeName = L"SetSkillLevel";
                    const HashType SetSkillLevel::TypeHash =
                        StringUtil::Hash(SetSkillLevel::TypeName);
                    void SetSkillLevel::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"skillgoupid", skillgroupid);
                        out.Write(L"skilllevel", skilllevel);
                        out.Write(L"LeftSkillPoint", LeftSkillPoint);
                    }

                    void SetSkillLevel::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"skillgoupid", skillgroupid);
                        in.Read(L"skilllevel", skilllevel);
                        in.Read(L"LeftSkillPoint", LeftSkillPoint);
                    }
                    //////////////////////////////////////////////////////////////////////////
                    ///SetSkillLevel //

                    // CreatePlayer
                    // ///////////////////////////////////////////////////////////////////////////
                    void CreatePlayer::Equipment::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(L"CreatePlayer::Equipment");
                        out.Write(L"Position", Position);
                        out.Write(L"ItemHash", ItemHash);
                        out.Write(L"instanceEx", instanceEx);
                    }

                    void CreatePlayer::Equipment::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(L"CreatePlayer::Equipment");
                        in.Read(L"Position", Position);
                        in.Read(L"ItemHash", ItemHash);
                        in.Read(L"instanceEx", instanceEx);
                    }

                    const wchar_t* CreatePlayer::TypeName = L"CreatePlayer";
                    const HashType CreatePlayer::TypeHash =
                        StringUtil::Hash(CreatePlayer::TypeName);
                    void CreatePlayer::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"playerserial", playerserial);
                        out.WriteEnum(L"classtype", classtype);
                        out.Write(L"charactername", charactername);
                        out.Write(L"level", level);
                        out.Write(L"pvpLevel", pvpLevel);
                        out.Write(L"warLevel", warLevel);
                        out.Write(L"storedLevel", storedLevel);
                        out.Write(L"rebirthCount", rebirthCount);
                        out.Write(L"ladderPoint", ladderPoint);
                        out.Write(L"ladderMatchCount", ladderMatchCount);
                        out.Write(L"ladderWinCount", ladderWinCount);
                        out.Write(L"ladderLoseCount", ladderLoseCount);
                        out.Write(L"achievementScore", achievementScore);  // 3.1 by Robotex
                        out.Write(L"addedSkillPointByRebirth", addedSkillPointByRebirth);
                        out.Write(L"position", position);
                        out.Write(L"direction", direction);
                        out.Write(L"hp", hp);
                        out.Write(L"team", team);
                        out.Write(L"Equipments", Equipments);
                        out.Write(L"PassiveItems", PassiveItems);
                        out.Write(L"stateflags", stateflags);
                        out.Write(L"shopping", shopping);
                        out.Write(L"stageLicenses", stageLicenses);
                        out.Write(L"lives", lives);
                        out.Write(L"bonusLife", bonusLife);
                        out.Write(L"CharacterStateFlags", static_cast<int>(CharacterStateFlags));
                        out.Write(L"lastRebirthDateTime", lastRebirthDateTime);
                        out.Write(L"partyChannelName", partyChannelName);
                        out.Write(L"eventExpFactor", eventExpFactor);
                    }

                    void CreatePlayer::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"playerserial", playerserial);
                        in.ReadEnum(L"classtype", classtype);
                        in.Read(L"charactername", charactername);
                        in.Read(L"level", level);
                        in.Read(L"pvpLevel", pvpLevel);
                        in.Read(L"warLevel", warLevel);
                        in.Read(L"storedLevel", storedLevel);
                        in.Read(L"rebirthCount", rebirthCount);
                        in.Read(L"ladderPoint", ladderPoint);
                        in.Read(L"ladderMatchCount", ladderMatchCount);
                        in.Read(L"ladderWinCount", ladderWinCount);
                        in.Read(L"ladderLoseCount", ladderLoseCount);
                        in.Read(L"achievementScore", achievementScore);  // 3.1 by Robotex
                        in.Read(L"addedSkillPointByRebirth", addedSkillPointByRebirth);
                        in.Read(L"position", position);
                        in.Read(L"direction", direction);
                        in.Read(L"hp", hp);
                        in.Read(L"team", team);
                        in.Read(L"Equipments", Equipments);
                        in.Read(L"PassiveItems", PassiveItems);
                        in.Read(L"stateflags", stateflags);
                        in.Read(L"shopping", shopping);
                        in.Read(L"stageLicenses", stageLicenses);
                        in.Read(L"lives", lives);
                        in.Read(L"bonusLife", bonusLife);
                        in.Read(L"CharacterStateFlags",
                            reinterpret_cast<int&>(CharacterStateFlags));
                        in.Read(L"lastRebirthDateTime", lastRebirthDateTime);
                        in.Read(L"partyChannelName", partyChannelName);
                        in.Read(L"eventExpFactor", eventExpFactor);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///Createplayer //

                    // CreateNonPlayer
                    // ////////////////////////////////////////////////////////////////////////
                    const wchar_t* CreateNonPlayer::TypeName = L"CreateNonPlayer";
                    const HashType CreateNonPlayer::TypeHash =
                        StringUtil::Hash(CreateNonPlayer::TypeName);
                    void CreateNonPlayer::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"objectserial", objectserial);
                        out.Write(L"charactername", charactername);
                        out.Write(L"level", level);
                        out.Write(L"position", position);
                        out.Write(L"direction", direction);
                        out.Write(L"hp", hp);
                        out.Write(L"team", team);
                        out.Write(L"playerCnt", playerCntWhenCreated);
                    }

                    void CreateNonPlayer::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"objectserial", objectserial);
                        in.Read(L"charactername", charactername);
                        in.Read(L"level", level);
                        in.Read(L"position", position);
                        in.Read(L"direction", direction);
                        in.Read(L"hp", hp);
                        in.Read(L"team", team);
                        in.Read(L"playerCnt", playerCntWhenCreated);
                    }
                    ////////////////////////////////////////////////////////////////////////
                    ///CreateNonPlayer //

                    // CreateItem
                    // /////////////////////////////////////////////////////////////////////////////
                    const wchar_t* CreateItem::TypeName = L"CreateItem";
                    const HashType CreateItem::TypeHash =
                        StringUtil::Hash(CreateItem::TypeName);
                    void CreateItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"objectserial", objectserial);
                        out.Write(L"itemname", itemname);
                        out.Write(L"position", position);
                        out.Write(L"direction", direction);
                        out.Write(L"owner", owner);
                        out.Write(L"stackCount", stackCount);
                        out.Write(L"isPrivateItem", isPrivateItem);
                    }

                    void CreateItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"objectserial", objectserial);
                        in.Read(L"itemname", itemname);
                        in.Read(L"position", position);
                        in.Read(L"direction", direction);
                        in.Read(L"owner", owner);
                        in.Read(L"stackCount", stackCount);
                        in.Read(L"isPrivateItem", isPrivateItem);
                    }
                    /////////////////////////////////////////////////////////////////////////////
                    ///CreateItem //

                    // DiceItemCreated//////////////////////////////////////////////////////////////////////////

                    const wchar_t* DiceItemCreated::TypeName = L"DiceItemCreated";
                    const HashType DiceItemCreated::TypeHash =
                        StringUtil::Hash(DiceItemCreated::TypeName);
                    void DiceItemCreated::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"result", result);
                    }

                    void DiceItemCreated::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"result", result);
                    }
                    /////////////////////////////////////////////////////////////////////////////
                    ///DiceItemCreated //

                    // CreateProjectile
                    // ///////////////////////////////////////////////////////////////////////
                    const wchar_t* CreateProjectile::TypeName = L"CreateProjectile";
                    const HashType CreateProjectile::TypeHash =
                        StringUtil::Hash(CreateProjectile::TypeName);
                    void CreateProjectile::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"objectserial", objectserial);
                        out.Write(L"tilename", tilename);
                        out.Write(L"position", position);
                        out.Write(L"direction", direction);
                        out.Write(L"targetpos", targetpos);
                        out.Write(L"serial", serial);
                        out.Write(L"owner", owner);
                    }

                    void CreateProjectile::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"objectserial", objectserial);
                        in.Read(L"tilename", tilename);
                        in.Read(L"position", position);
                        in.Read(L"direction", direction);
                        in.Read(L"targetpos", targetpos);
                        in.Read(L"serial", serial);
                        in.Read(L"owner", owner);
                    }
                    ///////////////////////////////////////////////////////////////////////
                    ///CreateProjectile //

                    // CreateStaticObject
                    // /////////////////////////////////////////////////////////////////////
                    const wchar_t* CreateStaticObject::TypeName = L"CreateStaticObject";
                    const HashType CreateStaticObject::TypeHash =
                        StringUtil::Hash(CreateStaticObject::TypeName);
                    void CreateStaticObject::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"objectserial", objectserial);
                        out.Write(L"objectname", objectname);
                        out.WriteEnum(L"objecttype", type);
                        out.Write(L"position", position);
                        out.Write(L"direction", direction);
                    }

                    void CreateStaticObject::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"objectserial", objectserial);
                        in.Read(L"objectname", objectname);
                        in.ReadEnum(L"objecttype", type);
                        in.Read(L"position", position);
                        in.Read(L"direction", direction);
                    }
                    /////////////////////////////////////////////////////////////////////
                    ///CreateStaticObject //

                    // DestroyObject
                    // //////////////////////////////////////////////////////////////////////////
                    const wchar_t* DestroyObject::TypeName = L"DestroyObject";
                    const HashType DestroyObject::TypeHash =
                        StringUtil::Hash(DestroyObject::TypeName);
                    void DestroyObject::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Serial", Serial);
                        out.WriteEnum(L"Type", Type);
                        out.Write(L"Silent", Silent);
                        out.Write(L"Team", team);
                    }

                    void DestroyObject::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Serial", Serial);
                        in.ReadEnum(L"Type", Type);
                        in.Read(L"Silent", Silent);
                        in.Read(L"Team", team);
                    }
                    //////////////////////////////////////////////////////////////////////////
                    ///DestroyObject //

                    // changeAction
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* ChangeAction::TypeName = L"ChangeAction";
                    const HashType ChangeAction::TypeHash =
                        StringUtil::Hash(ChangeAction::TypeName);
                    void ChangeAction::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"objectserial", objectserial);
                        out.Write(L"animation", animation);
                        out.Write(L"positoin", position);
                        out.Write(L"direction", direction);
                        out.Write(L"param", param);
                    }

                    void ChangeAction::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"objectserial", objectserial);
                        in.Read(L"animation", animation);
                        in.Read(L"positoin", position);
                        in.Read(L"direction", direction);
                        in.Read(L"param", param);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///ChangeAction //

                    // ChangeColState
                    // /////////////////////////////////////////////////////////////////////////
                    const wchar_t* ChangeColState::TypeName = L"ChangeColState";
                    const HashType ChangeColState::TypeHash =
                        StringUtil::Hash(ChangeColState::TypeName);
                    void ChangeColState::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"objectserial", objectserial);
                        out.Write(L"collision", collision);
                        out.Write(L"position", position);
                    }

                    void ChangeColState::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"objectserial", objectserial);
                        in.Read(L"collision", collision);
                        in.Read(L"position", position);
                    }
                    /////////////////////////////////////////////////////////////////////////
                    ///ChangeColState //

                    // ChangeStatus
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* ChangeStatus::TypeName = L"ChangeStatus";
                    const HashType ChangeStatus::TypeHash =
                        StringUtil::Hash(ChangeStatus::TypeName);
                    void ChangeStatus::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"objectserial", objectserial);
                        out.Write(L"position", position);
                        out.Write(L"direction", direction);
                        out.Write(L"hp", hp);
                        out.Write(L"mp", mp);
                        out.Write(L"bywhom", bywhom);
                        out.Write(L"bywhat", bywhat);
                        out.Write(L"flag", flag);
                        out.Write(L"airComboCount", airComboCount);
                    }

                    void ChangeStatus::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"objectserial", objectserial);
                        in.Read(L"position", position);
                        in.Read(L"direction", direction);
                        in.Read(L"hp", hp);
                        in.Read(L"mp", mp);
                        in.Read(L"bywhom", bywhom);
                        in.Read(L"bywhat", bywhat);
                        in.Read(L"flag", flag);
                        in.Read(L"airComboCount", airComboCount);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///ChangeStatus //

                    // UseItem
                    // ////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* UseItem::TypeName = L"UseItem";
                    const HashType UseItem::TypeHash =
                        StringUtil::Hash(UseItem::TypeName);
                    void UseItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"playerserial", playerserial);
                        out.Write(L"itemid", itemid);
                    }

                    void UseItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"playerserial", playerserial);
                        in.Read(L"itemid", itemid);
                    }
                    ////////////////////////////////////////////////////////////////////////////////
                    ///UseItem //

                    // UseItemEx
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* UseItemEx::TypeName = L"UseItemEx";
                    const HashType UseItemEx::TypeHash =
                        StringUtil::Hash(UseItemEx::TypeName);
                    void UseItemEx::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }

                    void UseItemEx::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///UseItemEx //

                    // EquipItem
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* EquipItem::TypeName = L"EquipItem";
                    const HashType EquipItem::TypeHash =
                        StringUtil::Hash(EquipItem::TypeName);
                    void EquipItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PlayerSerial", PlayerSerial);
                        out.Write(L"ItemHash", ItemHash);
                        out.Write(L"Position", Position);
                        out.Write(L"instanceEx", instanceEx);
                    }

                    void EquipItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PlayerSerial", PlayerSerial);
                        in.Read(L"ItemHash", ItemHash);
                        in.Read(L"Position", Position);
                        in.Read(L"instanceEx", instanceEx);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///EquipItem //

                    // EquipmentSwapped
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* EquipmentSwapped::TypeName = L"EquipmentSwapped";
                    const HashType EquipmentSwapped::TypeHash =
                        StringUtil::Hash(EquipmentSwapped::TypeName);
                    void EquipmentSwapped::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"NewIndex", NewIndex);
                        out.Write(L"PlayerSerial", PlayerSerial);
                        out.Write(L"NewEquipments", NewEquipments);
                        out.WriteEnum(L"Result", Result);
                    }

                    void EquipmentSwapped::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"NewIndex", NewIndex);
                        in.Read(L"PlayerSerial", PlayerSerial);
                        in.Read(L"NewEquipments", NewEquipments);
                        in.ReadEnum(L"Result", Result);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///EquipmentSwapped //

                    // CashEquipmentSwapped
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* CashEquipmentSwapped::TypeName = L"CashEquipmentSwapped";
                    const HashType CashEquipmentSwapped::TypeHash =
                        StringUtil::Hash(CashEquipmentSwapped::TypeName);
                    void CashEquipmentSwapped::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"NewIndex", NewIndex);
                        out.Write(L"PlayerSerial", PlayerSerial);
                        out.Write(L"NewEquipments", NewEquipments);
                        out.WriteEnum(L"Result", Result);
                    }

                    void CashEquipmentSwapped::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"NewIndex", NewIndex);
                        in.Read(L"PlayerSerial", PlayerSerial);
                        in.Read(L"NewEquipments", NewEquipments);
                        in.ReadEnum(L"Result", Result);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///CashEquipmentSwapped //

                    // PetInfo
                    // ////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* PetInfo::TypeName = L"PetInfo";
                    const HashType PetInfo::TypeHash =
                        StringUtil::Hash(PetInfo::TypeName);
                    void PetInfo::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        // out.Write( L"ownerserial", ownerserial );
                        out.Write(L"PetDataWithPos", PetDataWithPos);
                    }

                    void PetInfo::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        // in.Read( L"ownerserial", ownerserial );
                        in.Read(L"PetDataWithPos", PetDataWithPos);
                    }
                    ////////////////////////////////////////////////////////////////////////////////
                    ///PetInfo //

                    /////////////////////////////////////////////////////////////////////////////
                    ///PetUpdated //
                    const wchar_t* PetUpdated::TypeName = L"PetUpdated";
                    const HashType PetUpdated::TypeHash =
                        StringUtil::Hash(PetUpdated::TypeName);
                    void PetUpdated::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PlayerSerial", PlayerSerial);
                        out.Write(L"PetExpression", PetExpression);
                        out.Write(L"PetSerial", PetSerial);
                        out.Write(L"PetData", PetData);
                        out.Write(L"PetItemPosition", PetItemPosition);
                    }

                    void PetUpdated::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PlayerSerial", PlayerSerial);
                        in.Read(L"PetExpression", PetExpression);
                        in.Read(L"PetSerial", PetSerial);
                        in.Read(L"PetData", PetData);
                        in.Read(L"PetItemPosition", PetItemPosition);
                    }
                    // PetUpdated
                    // /////////////////////////////////////////////////////////////////////////////

                    // PetEquip
                    // ///////////////////////////////////////////////////////////////////////////////
                    const wchar_t* PetEquip::TypeName = L"PetEquip";
                    const HashType PetEquip::TypeHash =
                        StringUtil::Hash(PetEquip::TypeName);
                    void PetEquip::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PlayerSerial", PlayerSerial);
                        out.Write(L"PetObjectSerial", PetObjectSerial);
                        out.Write(L"ItemHash", ItemHash);
                        out.Write(L"Position", Position);
                        out.Write(L"instanceEx", instanceEx);
                    }

                    void PetEquip::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PlayerSerial", PlayerSerial);
                        in.Read(L"PetObjectSerial", PetObjectSerial);
                        in.Read(L"ItemHash", ItemHash);
                        in.Read(L"Position", Position);
                        in.Read(L"instanceEx", instanceEx);
                    }
                    ///////////////////////////////////////////////////////////////////////////////
                    ///PetEquip //

                    // PetEmotion
                    // /////////////////////////////////////////////////////////////////////////////
                    const wchar_t* PetEmotion::TypeName = L"PetEmotion";
                    const HashType PetEmotion::TypeHash =
                        StringUtil::Hash(PetEmotion::TypeName);
                    void PetEmotion::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PetSerial", PetSerial);
                        out.WriteEnum(L"FeelingState", FeelingState);
                    }

                    void PetEmotion::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PetSerial", PetSerial);
                        in.ReadEnum(L"FeelingState", FeelingState);
                    }
                    /////////////////////////////////////////////////////////////////////////////
                    ///PetEmotion //

                    // PetLevelUp
                    // /////////////////////////////////////////////////////////////////////////////
                    const wchar_t* PetLevelUp::TypeName = L"PetLevelUp";
                    const HashType PetLevelUp::TypeHash =
                        StringUtil::Hash(PetLevelUp::TypeName);
                    void PetLevelUp::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PlayerSerial", PlayerSerial);
                        out.Write(L"PetSerial", PetSerial);
                        out.Write(L"PetData", PetData);
                    }

                    void PetLevelUp::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PlayerSerial", PlayerSerial);
                        in.Read(L"PetSerial", PetSerial);
                        in.Read(L"PetData", PetData);
                    }
                    /////////////////////////////////////////////////////////////////////////////
                    ///PetLevelUp //

                    // UpdateItem
                    // /////////////////////////////////////////////////////////////////////////////
                    const wchar_t* UpdateItem::TypeName = L"UpdateItem";
                    const HashType UpdateItem::TypeHash =
                        StringUtil::Hash(UpdateItem::TypeName);
                    void UpdateItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Position", Position);
                        out.Write(L"ItemHash", ItemHash);
                        out.Write(L"instanceEx", instanceEx);
                    }

                    void UpdateItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Position", Position);
                        in.Read(L"ItemHash", ItemHash);
                        in.Read(L"instanceEx", instanceEx);
                    }
                    /////////////////////////////////////////////////////////////////////////////
                    ///UpdateItem //

                    // ListPetItem
                    // /////////////////////////////////////////////////////////////////////////////
                    const wchar_t* ListPetItem::TypeName = L"ListPetItem";
                    const HashType ListPetItem::TypeHash =
                        StringUtil::Hash(ListPetItem::TypeName);
                    void ListPetItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PetsItems", PetsItems);
                    }

                    void ListPetItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PetsItems", PetsItems);
                    }
                    /////////////////////////////////////////////////////////////////////////////
                    ///ListPetItem //

                    // MoveInventoryToPetItem
                    // /////////////////////////////////////////////////////////////////////////////
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
                    /////////////////////////////////////////////////////////////////////////////
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

                    // PetsCaredBySchool
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* PetsCaredBySchool::TypeName = L"PetsCaredBySchool";
                    const HashType PetsCaredBySchool::TypeHash =
                        StringUtil::Hash(PetsCaredBySchool::TypeName);
                    void PetsCaredBySchool::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"OwnerSerial", OwnerSerial);
                        out.Write(L"CaredPets", CaredPets);
                    }

                    void PetsCaredBySchool::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"OwnerSerial", OwnerSerial);
                        in.Read(L"CaredPets", CaredPets);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///PetsCaredBySchool //

                    // CaredPetUpdated
                    // ////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* CaredPetUpdated::TypeName = L"CaredPetUpdated";
                    const HashType CaredPetUpdated::TypeHash =
                        StringUtil::Hash(CaredPetUpdated::TypeName);
                    void CaredPetUpdated::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"OwnerSerial", OwnerSerial);
                        out.Write(L"CaredPet", CaredPet);
                    }

                    void CaredPetUpdated::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"OwnerSerial", OwnerSerial);
                        in.Read(L"CaredPet", CaredPet);
                    }
                    ////////////////////////////////////////////////////////////////////////////////
                    ///CaredPetUpdated //

                    // CaredPetTakenOut
                    // ///////////////////////////////////////////////////////////////////////////////
                    const wchar_t* CaredPetTakenOut::TypeName = L"CaredPetTakenOut";
                    const HashType CaredPetTakenOut::TypeHash =
                        StringUtil::Hash(CaredPetTakenOut::TypeName);
                    void CaredPetTakenOut::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"OwnerSerial", OwnerSerial);
                        out.Write(L"CaredPetSerial", CaredPetSerial);
                    }

                    void CaredPetTakenOut::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"OwnerSerial", OwnerSerial);
                        in.Read(L"CaredPetSerial", CaredPetSerial);
                    }
                    ///////////////////////////////////////////////////////////////////////////////
                    ///CaredPetTakenOut //

                    // RemovePet
                    // ///////////////////////////////////////////////////////////////////////////////
                    const wchar_t* RemovePet::TypeName = L"RemovePet";
                    const HashType RemovePet::TypeHash =
                        StringUtil::Hash(RemovePet::TypeName);
                    void RemovePet::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PetSerial", PetSerial);
                    }

                    void RemovePet::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PetSerial", PetSerial);
                    }
                    ///////////////////////////////////////////////////////////////////////////////
                    ///RemovePet //

                    // TakeOutAllPetItems
                    // ///////////////////////////////////////////////////////////////////////////////
                    const wchar_t* TakeOutAllPetItems::TypeName = L"TakeOutAllPetItems";
                    const HashType TakeOutAllPetItems::TypeHash =
                        StringUtil::Hash(TakeOutAllPetItems::TypeName);
                    void TakeOutAllPetItems::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PetSerial", PetSerial);
                        out.WriteEnum(L"Result", Result);
                    }

                    void TakeOutAllPetItems::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PetSerial", PetSerial);
                        in.ReadEnum(L"Result", Result);
                    }
                    ///////////////////////////////////////////////////////////////////////////////
                    ///TakeOutAllPetItems //

                    // DropItemInPetInventory
                    // ///////////////////////////////////////////////////////////////////////////////
                    const wchar_t* DropItemInPetInventory::TypeName = L"DropItemInPetInventory";
                    const HashType DropItemInPetInventory::TypeHash =
                        StringUtil::Hash(DropItemInPetInventory::TypeName);
                    void DropItemInPetInventory::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PetSerial", PetSerial);
                        out.WriteEnum(L"PositionType", PositionType);
                        out.Write(L"Position", Position);
                        out.Write(L"StackCount", StackCount);
                    }

                    void DropItemInPetInventory::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PetSerial", PetSerial);
                        in.ReadEnum(L"PositionType", PositionType);
                        in.Read(L"Position", Position);
                        in.Read(L"StackCount", StackCount);
                    }
                    ///////////////////////////////////////////////////////////////////////////////
                    ///DropItemInPetInventory //

                    // LevelUp
                    // ////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* LevelUp::TypeName = L"LevelUp";
                    const HashType LevelUp::TypeHash =
                        StringUtil::Hash(LevelUp::TypeName);
                    void LevelUp::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"playerserial", playerserial);
                        out.Write(L"level", level);
                        out.WriteEnum(L"expType", expType);
                    }

                    void LevelUp::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"playerserial", playerserial);
                        in.Read(L"level", level);
                        in.ReadEnum(L"expType", expType);
                    }
                    ////////////////////////////////////////////////////////////////////////////////
                    ///LevelUp //

                    // PlayerDied
                    // /////////////////////////////////////////////////////////////////////////////
                    const wchar_t* PlayerDied::TypeName = L"PlayerDied";
                    const HashType PlayerDied::TypeHash =
                        StringUtil::Hash(PlayerDied::TypeName);
                    void PlayerDied::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"playerserial", playerserial);
                        out.Write(L"hitplayerserial", hitplayerserial);
                    }

                    void PlayerDied::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"playerserial", playerserial);
                        in.Read(L"hitplayerserial", hitplayerserial);
                    }
                    /////////////////////////////////////////////////////////////////////////////
                    ///PlayerDied //

                    // Revive
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Revive::TypeName = L"Revive";
                    const HashType Revive::TypeHash = StringUtil::Hash(Revive::TypeName);
                    void Revive::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"playerserial", playerserial);
                        out.Write(L"hp", hp);
                        out.Write(L"mp", mp);
                        out.Write(L"life", life);
                        out.Write(L"bonusLife", bonusLife);
                        out.Write(L"usableBonusLife", usableBonusLife);
                        out.Write(L"reviveCount", reviveCount);  // 3.1 by Robotex
                    }

                    void Revive::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"playerserial", playerserial);
                        in.Read(L"hp", hp);
                        in.Read(L"mp", mp);
                        in.Read(L"life", life);
                        in.Read(L"bonusLife", bonusLife);
                        in.Read(L"usableBonusLife", usableBonusLife);
                        in.Read(L"reviveCount", reviveCount);  // 3.1 by Robotex
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///Revive //

                    // InstantCoinRevive
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* InstantCoinRevive::TypeName = L"InstantCoinRevive";
                    const HashType InstantCoinRevive::TypeHash =
                        StringUtil::Hash(InstantCoinRevive::TypeName);
                    void InstantCoinRevive::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Result", static_cast<const int>(Result));
                    }

                    void InstantCoinRevive::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Result", reinterpret_cast<int&>(Result));
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///InstantCoinRevive //

                    // SkillPoint
                    // /////////////////////////////////////////////////////////////////////////////
                    const wchar_t* SkillPoint::TypeName = L"SkillPoint";
                    const HashType SkillPoint::TypeHash =
                        StringUtil::Hash(SkillPoint::TypeName);
                    void SkillPoint::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"skillpoint", skillpoint);
                    }

                    void SkillPoint::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"skillpoint", skillpoint);
                    }
                    /////////////////////////////////////////////////////////////////////////////
                    ///SkillPoint //
                    // SkillPointPlus
                    // /////////////////////////////////////////////////////////////////////////////
                    const wchar_t* SkillPointPlus::TypeName = L"SkillPointPlus";
                    const HashType SkillPointPlus::TypeHash =
                        StringUtil::Hash(SkillPointPlus::TypeName);
                    void SkillPointPlus::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"skillpoint", skillpoint);
                        out.Write(L"addedSkillPointPlus", addedSkillPointPlus);
                        out.Write(L"storedSkillPoint", storedSkillPoint);
                    }

                    void SkillPointPlus::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"skillpoint", skillpoint);
                        in.Read(L"addedSkillPointPlus", addedSkillPointPlus);
                        in.Read(L"storedSkillPoint", storedSkillPoint);
                    }
                    /////////////////////////////////////////////////////////////////////////////
                    ///SkillPointPlus //

                    // Money
                    // //////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Money::TypeName = L"Money";
                    const HashType Money::TypeHash = StringUtil::Hash(Money::TypeName);
                    void Money::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"CurrentMoney", CurrentMoney);
                    }

                    void Money::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"CurrentMoney", CurrentMoney);
                    }
                    //////////////////////////////////////////////////////////////////////////////////
                    ///Money //

                    // PvpLevelExp
                    // ////////////////////////////////////////////////////////////////////////////
                    const wchar_t* PvpLevelExp::TypeName = L"PvpLevelExp";
                    const HashType PvpLevelExp::TypeHash =
                        StringUtil::Hash(PvpLevelExp::TypeName);
                    void PvpLevelExp::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"pvpLevel", pvpLevel);
                        out.Write(L"pvpXp", pvpXp);
                    }

                    void PvpLevelExp::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"pvpLevel", pvpLevel);
                        in.Read(L"pvpXp", pvpXp);
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    ///PvpLevelExp //

                    // MissionClear
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* MissionClear::TypeName = L"MissionClear";
                    const HashType MissionClear::TypeHash =
                        StringUtil::Hash(MissionClear::TypeName);
                    void MissionClear::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"success", success);
                        out.Write(L"elapstime", elapstime);
                        out.Write(L"fountsecret", foundsecret);
                        out.Write(L"eventExpFactor", eventExpFactor);
                        out.Write(L"bossItemGainCount", bossItemGainCount);
                        out.Write(L"ladderPoint", ladderPoint);
                        // out.Write( L"allPlayerReviveCount",allPlayerReviveCount );
                        out.Write(L"playerInfos", playerInfos);
                    }

                    void MissionClear::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"success", success);
                        in.Read(L"elapstime", elapstime);
                        in.Read(L"fountsecret", foundsecret);
                        in.Read(L"eventExpFactor", eventExpFactor);
                        in.Read(L"bossItemGainCount", bossItemGainCount);
                        in.Read(L"ladderPoint", ladderPoint);
                        // in.Read( L"allPlayerReviveCount", allPlayerReviveCount );
                        in.Read(L"playerInfos", playerInfos);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///MissionClear //

                    // MissionClearReward
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* MissionClearReward::TypeName = L"MissionClearReward";
                    const HashType MissionClearReward::TypeHash =
                        StringUtil::Hash(MissionClearReward::TypeName);
                    void MissionClearReward::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"bossItems", bossItems);
                        out.Write(L"bonusItems", bonusItems);
                        out.Write(L"eventItems", eventItems);  // 3.1 by Robotex
                    }

                    void MissionClearReward::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"bossItems", bossItems);
                        in.Read(L"bonusItems", bonusItems);
                        in.Read(L"eventItems", eventItems);  // 3.1 by Robotex
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///MissionClear //

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

                    // StackItem
                    // ///////////////////////////////////////////////////////////////////////////////
                    const wchar_t* StackItem::TypeName = L"StackItem";
                    const HashType StackItem::TypeHash =
                        StringUtil::Hash(StackItem::TypeName);
                    void StackItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"sbag", sbag);
                        out.Write(L"spos", spos);
                        out.Write(L"tbag", tbag);
                        out.Write(L"tpos", tpos);
                        out.Write(L"count", count);
                    }

                    void StackItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"sbag", sbag);
                        in.Read(L"spos", spos);
                        in.Read(L"tbag", tbag);
                        in.Read(L"tpos", tpos);
                        in.Read(L"count", count);
                    }
                    ///////////////////////////////////////////////////////////////////////////////
                    ///StackItem //

                    // DropItem
                    // ///////////////////////////////////////////////////////////////////////////////
                    const wchar_t* DropItem::TypeName = L"DropItem";
                    const HashType DropItem::TypeHash =
                        StringUtil::Hash(DropItem::TypeName);
                    void DropItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"bag", bag);
                        out.Write(L"pos", pos);
                        out.Write(L"DroppedCount", DroppedCount);
                        out.Write(L"StackedCount", StackedCount);
                    }

                    void DropItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"bag", bag);
                        in.Read(L"pos", pos);
                        in.Read(L"DroppedCount", DroppedCount);
                        in.Read(L"StackedCount", StackedCount);
                    }
                    ///////////////////////////////////////////////////////////////////////////////
                    ///DropItem //

                    // AcquireItem
                    // ////////////////////////////////////////////////////////////////////////////
                    const wchar_t* AcquireItem::TypeName = L"AcquireItem";
                    const HashType AcquireItem::TypeHash =
                        StringUtil::Hash(AcquireItem::TypeName);
                    void AcquireItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"ItemId", ItemId);
                        out.Write(L"Bag", Bag);
                        out.Write(L"Pos", Pos);
                        out.Write(L"StackedCount", StackedCount);
                        out.Write(L"instanceEx", instanceEx);
                        out.Write(L"ItemSerial", ItemSerial);
                    }

                    void AcquireItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"ItemId", ItemId);
                        in.Read(L"Bag", Bag);
                        in.Read(L"Pos", Pos);
                        in.Read(L"StackedCount", StackedCount);
                        in.Read(L"instanceEx", instanceEx);
                        in.Read(L"ItemSerial", ItemSerial);
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    ///AcquireItem //

                    // NoticeAcquireItemInfo
                    // ////////////////////////////////////////////////////////////////////////////
                    const wchar_t* NoticeAcquireItemInfo::TypeName = L"NoticeAcquireItemInfo";
                    const HashType NoticeAcquireItemInfo::TypeHash =
                        StringUtil::Hash(NoticeAcquireItemInfo::TypeName);
                    void NoticeAcquireItemInfo::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PlayerSerial", PlayerSerial);
                        out.Write(L"ItemId", ItemId);
                        out.Write(L"instanceEx", instanceEx);
                    }

                    void NoticeAcquireItemInfo::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PlayerSerial", PlayerSerial);
                        in.Read(L"ItemId", ItemId);
                        in.Read(L"instanceEx", instanceEx);
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    ///AcquireItem //

                    // AcquirePvPItem
                    // ////////////////////////////////////////////////////////////////////////////
                    const wchar_t* AcquirePvPItem::TypeName = L"AcquirePvPItem";
                    const HashType AcquirePvPItem::TypeHash =
                        StringUtil::Hash(AcquirePvPItem::TypeName);
                    void AcquirePvPItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"playerserial", playerserial);
                        out.Write(L"itemid", itemid);
                    }

                    void AcquirePvPItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"playerserial", playerserial);
                        in.Read(L"itemid", itemid);
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    ///AcquirePvPItem //

                    // UsePvPItem
                    // ////////////////////////////////////////////////////////////////////////////
                    const wchar_t* UsePvPItem::TypeName = L"UsePvPItem";
                    const HashType UsePvPItem::TypeHash =
                        StringUtil::Hash(UsePvPItem::TypeName);
                    void UsePvPItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"playerserial", playerserial);
                        out.Write(L"itemid", itemid);
                    }

                    void UsePvPItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"playerserial", playerserial);
                        in.Read(L"itemid", itemid);
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    ///UsePvPItem //

                    // DropPvPItem
                    // ////////////////////////////////////////////////////////////////////////////
                    const wchar_t* DropPvPItem::TypeName = L"DropPvPItem";
                    const HashType DropPvPItem::TypeHash =
                        StringUtil::Hash(DropPvPItem::TypeName);
                    void DropPvPItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"playerserial", playerserial);
                    }

                    void DropPvPItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"playerserial", playerserial);
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    ///DropPvPItem //

                    // AddMinimapPing
                    // /////////////////////////////////////////////////////////////////////////
                    const wchar_t* AddMinimapPing::TypeName = L"AddMinimapPing";
                    const HashType AddMinimapPing::TypeHash =
                        StringUtil::Hash(AddMinimapPing::TypeName);
                    void AddMinimapPing::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"pingid", pingid);
                        out.Write(L"position", position);
                        out.WriteEnum(L"type", type);
                    }

                    void AddMinimapPing::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"pingid", pingid);
                        in.Read(L"position", position);
                        in.ReadEnum(L"type", type);
                    }
                    /////////////////////////////////////////////////////////////////////////
                    ///AddMinimapPing //

                    // RemoveMinimapPing
                    // //////////////////////////////////////////////////////////////////////
                    const wchar_t* RemoveMinimapPing::TypeName = L"RemoveMinimapPing";
                    const HashType RemoveMinimapPing::TypeHash =
                        StringUtil::Hash(RemoveMinimapPing::TypeName);
                    void RemoveMinimapPing::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"pingid", pingid);
                    }

                    void RemoveMinimapPing::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"pingid", pingid);
                    }
                    //////////////////////////////////////////////////////////////////////
                    ///RemoveMinimapPing //

                    // DisplayTextEvent
                    // ///////////////////////////////////////////////////////////////////////
                    const wchar_t* DisplayTextEvent::TypeName = L"DisplayTextEvent";
                    const HashType DisplayTextEvent::TypeHash =
                        StringUtil::Hash(DisplayTextEvent::TypeName);
                    void DisplayTextEvent::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"type", type);
                        out.Write(L"textId", textId);
                        out.Write(L"param", param);
                    }

                    void DisplayTextEvent::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"type", type);
                        in.Read(L"textId", textId);
                        in.Read(L"param", param);
                    };
                    ///////////////////////////////////////////////////////////////////////
                    ///DisplayTextEvent //

                    // DisplayText
                    // ////////////////////////////////////////////////////////////////////////////
                    const wchar_t* DisplayText::TypeName = L"DisplayText";
                    const HashType DisplayText::TypeHash =
                        StringUtil::Hash(DisplayText::TypeName);
                    void DisplayText::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"type", type);
                        out.Write(L"message", message);
                    }

                    void DisplayText::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"type", type);
                        in.Read(L"message", message);
                    };
                    ////////////////////////////////////////////////////////////////////////////
                    ///DisplayText //

                    // DisplayTimer
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* DisplayTimer::TypeName = L"DisplayTimer";
                    const HashType DisplayTimer::TypeHash =
                        StringUtil::Hash(DisplayTimer::TypeName);
                    void DisplayTimer::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"type", type);
                        out.Write(L"seconds", seconds);
                    }

                    void DisplayTimer::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"type", type);
                        in.Read(L"seconds", seconds);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///DisplayTimer //

                    // PlayEvent
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* PlayEvent::TypeName = L"PlayEvent";
                    const HashType PlayEvent::TypeHash =
                        StringUtil::Hash(PlayEvent::TypeName);
                    void PlayEvent::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"eventid", eventid);
                        out.Write(L"param", param);
                    }
                    void PlayEvent::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"eventid", eventid);
                        in.Read(L"param", param);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///PlayEvent //

                    // PlayClientStageEvent
                    // ///////////////////////////////////////////////////////////////////
                    const wchar_t* PlayClientStageEvent::TypeName = L"PlayClientStageEvent";
                    const HashType PlayClientStageEvent::TypeHash =
                        StringUtil::Hash(PlayClientStageEvent::TypeName);
                    void PlayClientStageEvent::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"eventId", eventId);
                        out.Write(L"eventParam", eventParam);
                    }
                    void PlayClientStageEvent::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"eventId", eventId);
                        in.Read(L"eventParam", eventParam);
                    }
                    ///////////////////////////////////////////////////////////////////
                    ///PlayClientStageEvent //

                    // CreateStructure
                    // ////////////////////////////////////////////////////////////////////////
                    const wchar_t* CreateStructure::TypeName = L"CreateStructure";
                    const HashType CreateStructure::TypeHash =
                        StringUtil::Hash(CreateStructure::TypeName);
                    void CreateStructure::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"structureserial", structureserial);
                        out.Write(L"structurename", structurename);
                        out.Write(L"position", position);
                        out.Write(L"direction", direction);
                        out.Write(L"currenthp", currenthp);
                        out.Write(L"team", team);
                    }
                    void CreateStructure::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"structureserial", structureserial);
                        in.Read(L"structurename", structurename);
                        in.Read(L"position", position);
                        in.Read(L"direction", direction);
                        in.Read(L"currenthp", currenthp);
                        in.Read(L"team", team);
                    }
                    ////////////////////////////////////////////////////////////////////////
                    ///CreateStructure //

                    // CreateStructure
                    // ////////////////////////////////////////////////////////////////////////
                    const wchar_t* CreateVehicle::TypeName = L"CreateVehicle";
                    const HashType CreateVehicle::TypeHash =
                        StringUtil::Hash(CreateVehicle::TypeName);
                    void CreateVehicle::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"vehicleserial", vehicleserial);
                        out.Write(L"vehiclename", vehiclename);
                        out.Write(L"position", position);
                        out.Write(L"direction", direction);
                        out.Write(L"currenthp", currenthp);
                        out.Write(L"team", team);
                    }

                    void CreateVehicle::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"vehicleserial", vehicleserial);
                        in.Read(L"vehiclename", vehiclename);
                        in.Read(L"position", position);
                        in.Read(L"direction", direction);
                        in.Read(L"currenthp", currenthp);
                        in.Read(L"team", team);
                    }
                    ////////////////////////////////////////////////////////////////////////
                    ///CreateStructure //

                    // Notice
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Notice::TypeName = L"Notice";
                    const HashType Notice::TypeHash = StringUtil::Hash(Notice::TypeName);
                    void Notice::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"serial", serial);
                        out.Write(L"id", id);
                        out.Write(L"position", position);
                        out.Write(L"direction", direction);
                        out.Write(L"parameters", parameters);
                    }

                    void Notice::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"serial", serial);
                        in.Read(L"id", id);
                        in.Read(L"position", position);
                        in.Read(L"direction", direction);
                        in.Read(L"parameters", parameters);
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///Notice //

                    // PauseObject
                    // ////////////////////////////////////////////////////////////////////////////
                    const wchar_t* PauseObject::TypeName = L"PauseObject";
                    const HashType PauseObject::TypeHash =
                        StringUtil::Hash(PauseObject::TypeName);
                    void PauseObject::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"serial", serial);
                        out.Write(L"pausetime", pausetime);
                    }
                    void PauseObject::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"serial", serial);
                        in.Read(L"pausetime", pausetime);
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    ///PauseObject //

                    // PauseCinematict
                    // ////////////////////////////////////////////////////////////////////////
                    const wchar_t* PlayCinematic::TypeName = L"PlayCinematic";
                    const HashType PlayCinematic::TypeHash =
                        StringUtil::Hash(PlayCinematic::TypeName);
                    void PlayCinematic::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"cinematic", cinematic);
                    }

                    void PlayCinematic::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"cinematic", cinematic);
                    }
                    //////////////////////////////////////////////////////////////////////////
                    ///PlayCinematic //

                    // XpGain
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* XpGain::TypeName = L"XpGain";
                    const HashType XpGain::TypeHash = StringUtil::Hash(XpGain::TypeName);
                    void XpGain::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"type", type);
                        out.Write(L"storyXp", storyXp);
                        out.Write(L"pvpXp", pvpXp);
                        out.Write(L"warXp", warXp);
                        out.Write(L"beKilledNpc", beKilledNpc);
                    }

                    void XpGain::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"type", type);
                        in.Read(L"storyXp", storyXp);
                        in.Read(L"pvpXp", pvpXp);
                        in.Read(L"warXp", warXp);
                        in.Read(L"beKilledNpc", beKilledNpc);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///XpGain //

                    // GoldGain
                    // ///////////////////////////////////////////////////////////////////////////
                    // TODO : ?????? ???? ???? ??? ???? ?????? ???? ???ÿ? Money?? Add??????? ???????
                    // ?????...
                    //        ????? GoldGain????? ???? ????? Gold?? ???Ÿ??? ?????? ?????? ?????????????
                    //        ????Money?? ??? ??????°? ????.. ????? ???? ???????..
                    const wchar_t* GoldGain::TypeName = L"GoldGain";
                    const HashType GoldGain::TypeHash =
                        StringUtil::Hash(GoldGain::TypeName);
                    void GoldGain::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"type", type);
                        out.Write(L"serial", serial);
                        out.Write(L"money", money);
                    }

                    void GoldGain::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"type", type);
                        in.Read(L"serial", serial);
                        in.Read(L"money", money);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///GoldGain //

                    // AcquireLicense
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* AcquireLicense::TypeName = L"AcquireLicense";
                    const HashType AcquireLicense::TypeHash =
                        StringUtil::Hash(AcquireLicense::TypeName);
                    void AcquireLicense::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"serial", serial);
                        out.Write(L"stageLicense", stageLicense);
                        out.Write(L"sharedOtherPlayers", sharedOtherPlayers);  // 3.1 by ultimate
                    }

                    void AcquireLicense::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"serial", serial);
                        in.Read(L"stageLicense", stageLicense);
                        in.Read(L"sharedOtherPlayers", sharedOtherPlayers);  // 3.1 by ultimate
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///AcquireLicense //

                    // AcquireLicense
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* AcquireSkillLicense::TypeName = L"AcquireSkillLicense";
                    const HashType AcquireSkillLicense::TypeHash =
                        StringUtil::Hash(AcquireSkillLicense::TypeName);
                    void AcquireSkillLicense::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"serial", serial);
                        out.Write(L"skillLicense", skillLicense);
                    }

                    void AcquireSkillLicense::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"serial", serial);
                        in.Read(L"skillLicense", skillLicense);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///AcquireLicense //

                    // ChangeState
                    // //////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* ChangeState::TypeName = L"ChangeState";
                    const HashType ChangeState::TypeHash =
                        StringUtil::Hash(ChangeState::TypeName);
                    void ChangeState::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"serial", serial);
                        out.Write(L"id", id);
                        out.Write(L"level", level);
                        out.Write(L"byWhom", byWhom);
                        out.Write(L"realTimeDuration", realTimeDuration);  // 3.1 by Robotex
                    }

                    void ChangeState::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"serial", serial);
                        in.Read(L"id", id);
                        in.Read(L"level", level);
                        in.Read(L"byWhom", byWhom);
                        in.Read(L"realTimeDuration", realTimeDuration);  // 3.1 by Robotex
                    }
                    //////////////////////////////////////////////////////////////////////////////////
                    ///ChangeState //

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

                    // VoteStart
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* VoteStart::TypeName = L"VoteStart";
                    const HashType VoteStart::TypeHash =
                        StringUtil::Hash(VoteStart::TypeName);
                    void VoteStart::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Serial", serial);
                        out.WriteEnum(L"type", type);
                        out.Write(L"targetStage", targetStage);
                        out.Write(L"charName", charName);
                    }

                    void VoteStart::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Serial", serial);
                        in.ReadEnum(L"type", type);
                        in.Read(L"targetStage", targetStage);
                        in.Read(L"charName", charName);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///VoteStart //

                    // AutoVoteStart
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* AutoVoteStart::TypeName = L"AutoVoteStart";
                    const HashType AutoVoteStart::TypeHash =
                        StringUtil::Hash(AutoVoteStart::TypeName);
                    void AutoVoteStart::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"type", type);
                        out.Write(L"targetStage", targetStage);
                    }

                    void AutoVoteStart::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"type", type);
                        in.Read(L"targetStage", targetStage);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///VoteStart //

                    // StartPersonalVoting
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* StartPersonalVoting::TypeName = L"StartPersonalVoting";
                    const HashType StartPersonalVoting::TypeHash =
                        StringUtil::Hash(StartPersonalVoting::TypeName);
                    void StartPersonalVoting::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"type", type);
                        out.Write(L"charName", charName);
                    }

                    void StartPersonalVoting::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"type", type);
                        in.Read(L"charName", charName);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///StartPersonalVoting //

                    // ResultPersonalVoting
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* ResultPersonalVoting::TypeName = L"ResultPersonalVoting";
                    const HashType ResultPersonalVoting::TypeHash =
                        StringUtil::Hash(ResultPersonalVoting::TypeName);
                    void ResultPersonalVoting::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"type", type);
                        out.Write(L"charName", charName);
                    }

                    void ResultPersonalVoting::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"type", type);
                        in.Read(L"charName", charName);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///ResultPersonalVoting //

                    // EnterShop
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* EnterShop::TypeName = L"EnterShop";
                    const HashType EnterShop::TypeHash =
                        StringUtil::Hash(EnterShop::TypeName);
                    void EnterShop::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"playerserial", playerserial);
                        out.WriteEnum(L"shopnumber", shopnumber);
                        out.Write(L"param", param);
                    }

                    void EnterShop::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"playerserial", playerserial);
                        in.ReadEnum(L"shopnumber", shopnumber);
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
                        out.Write(L"playerserial", playerserial);
                        out.Write(L"position", position);
                        out.Write(L"direction", direction);
                    }

                    void LeaveShop::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"playerserial", playerserial);
                        in.Read(L"position", position);
                        in.Read(L"direction", direction);
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
                        out.Write(L"bag", bag);
                        out.Write(L"pos", pos);
                        out.Write(L"instanceEx", instanceEx);
                        out.Write(L"money", money);
                    }

                    void Buy::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"itemid", itemid);
                        in.Read(L"count", count);
                        in.Read(L"bag", bag);
                        in.Read(L"pos", pos);
                        in.Read(L"instanceEx", instanceEx);
                        in.Read(L"money", money);
                    }
                    // 3
                    ////////////////////////////////////////////////////////////////////////////////////
                    ///Buy //
                    // BuyBack
                    // ////////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* BuyBack::TypeName = L"BuyBack";
                    const HashType BuyBack::TypeHash =
                        StringUtil::Hash(BuyBack::TypeName);
                    void BuyBack::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"itemid", itemid);
                        out.Write(L"count", count);
                        out.Write(L"bag", bag);
                        out.Write(L"pos", pos);
                        out.Write(L"instanceEx", instanceEx);
                        out.Write(L"money", money);
                    }

                    void BuyBack::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"itemid", itemid);
                        in.Read(L"count", count);
                        in.Read(L"bag", bag);
                        in.Read(L"pos", pos);
                        in.Read(L"instanceEx", instanceEx);
                        in.Read(L"money", money);
                    }
                    // 3
                    ////////////////////////////////////////////////////////////////////////////////////
                    ///BuyBack //

                    // BuyBarterItem
                    // ////////////////////////////////////////////////////////////////////////////////////
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
                    // 3
                    ////////////////////////////////////////////////////////////////////////////////////
                    ///BuyBarterItem //

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

                    // VoteOn
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* VoteOn::TypeName = L"VoteOn";
                    const HashType VoteOn::TypeHash = StringUtil::Hash(VoteOn::TypeName);
                    void VoteOn::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"yes", yes);
                        out.Write(L"no", no);
                    }
                    void VoteOn::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"yes", yes);
                        in.Read(L"no", no);
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///VoteOn //

                    // Trade
                    // ./////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Trade::TypeName = L"Trade";
                    const HashType Trade::TypeHash = StringUtil::Hash(Trade::TypeName);
                    void Trade::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }
                    void Trade::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }
                    //////////////////////////////////////////////////////////////////////////////////
                    ///Trade //

                    // VoteResult
                    // /////////////////////////////////////////////////////////////////////////////
                    const wchar_t* VoteResult::TypeName = L"VoteResult";
                    const HashType VoteResult::TypeHash =
                        StringUtil::Hash(VoteResult::TypeName);
                    void VoteResult::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"yes", yes);
                        out.Write(L"no", no);
                    }
                    void VoteResult::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"yes", yes);
                        in.Read(L"no", no);
                    }
                    /////////////////////////////////////////////////////////////////////////////
                    ///VoteResult //

                    // VoteFailed
                    // /////////////////////////////////////////////////////////////////////////////
                    const wchar_t* VoteFailed::TypeName = L"VoteFailed";
                    const HashType VoteFailed::TypeHash =
                        StringUtil::Hash(VoteFailed::TypeName);
                    void VoteFailed::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }
                    void VoteFailed::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }
                    /////////////////////////////////////////////////////////////////////////////
                    ///VoteFailed //

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
                        out.Write(L"serial", serial);
                        out.Write(L"itemid", itemid);
                        out.Write(L"count", count);
                        out.Write(L"instanceEx", instanceEx);
                    }

                    void AddTradeItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"serial", serial);
                        in.Read(L"itemid", itemid);
                        in.Read(L"count", count);
                        in.Read(L"instanceEx", instanceEx);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///AddTradeItem //

                    // AddTradePetItem
                    // ///////////////////////////////////////////////////////////////////////////
                    const wchar_t* AddTradePetItem::TypeName = L"AddTradePetItem";
                    const HashType AddTradePetItem::TypeHash =
                        StringUtil::Hash(AddTradePetItem::TypeName);
                    void AddTradePetItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"serial", serial);
                        out.Write(L"itemid", itemid);
                        out.Write(L"count", count);
                        out.Write(L"instanceEx", instanceEx);
                        out.Write(L"Pet", Pet);
                    }

                    void AddTradePetItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"serial", serial);
                        in.Read(L"itemid", itemid);
                        in.Read(L"count", count);
                        in.Read(L"instanceEx", instanceEx);
                        in.Read(L"Pet", Pet);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///AddTradePetItem //

                    // AddTradeMoney
                    // //////////////////////////////////////////////////////////////////////////
                    const wchar_t* AddTradeMoney::TypeName = L"AddTradeMoeny";
                    const HashType AddTradeMoney::TypeHash =
                        StringUtil::Hash(AddTradeMoney::TypeName);
                    void AddTradeMoney::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"serial", serial);
                        out.Write(L"money", money);
                    }

                    void AddTradeMoney::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"serial", serial);
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
                        out.Write(L"serial", serial);
                    }

                    void ReadyToTrade::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"serial", serial);
                    }
                    ///////////////////////////////////////////////////////////////////////////
                    ///ReadyToTrade //

                    // ConfirmTrade
                    // ///////////////////////////////////////////////////////////////////////////

                    const wchar_t* ConfirmTrade::TypeName = L"ConfirmTrade";
                    const HashType ConfirmTrade::TypeHash =
                        StringUtil::Hash(ConfirmTrade::TypeName);
                    void ConfirmTrade::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"tradePlayers", tradePlayers);
                    }

                    void ConfirmTrade::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"tradePlayers", tradePlayers);
                    }

                    ///////////////////////////////////////////////////////////////////////////
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
                        out.WriteEnum(L"Result", Result);
                    }

                    void BankMoneyIn::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"BankMoney", BankMoney);
                        in.ReadEnum(L"Result", Result);
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
                        out.WriteEnum(L"Result", Result);
                    }

                    void BankMoneyOut::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"BankMoney", BankMoney);
                        in.ReadEnum(L"Result", Result);
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
                        out.Write(L"isskill", isskill);
                        out.Write(L"instanceEx", instanceEx);
                    }
                    void QuickSlot::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"number", number);
                        in.Read(L"id", id);
                        in.Read(L"isskill", isskill);
                        in.Read(L"instanceEx", instanceEx);
                    };
                    /////////////////////////////////////////////////////////////////////////////
                    ///QuickSlot //

                    // Identify
                    // ///////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Identify::TypeName = L"Identify";
                    const HashType Identify::TypeHash =
                        StringUtil::Hash(Identify::TypeName);
                    void Identify::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Position", Position);
                        out.Write(L"ItemHash", ItemHash);
                        out.Write(L"instanceEx", instanceEx);
                    }

                    void Identify::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Position", Position);
                        in.Read(L"ItemHash", ItemHash);
                        in.Read(L"instanceEx", instanceEx);
                    }
                    ///////////////////////////////////////////////////////////////////////////////
                    ///Identify //

                    // Reinforce
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Reinforce::TypeName = L"Reinforce";
                    const HashType Reinforce::TypeHash =
                        StringUtil::Hash(Reinforce::TypeName);
                    void Reinforce::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Position", Position);
                        out.Write(L"ItemHash", ItemHash);
                        out.Write(L"instanceEx", instanceEx);
                    }

                    void Reinforce::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Position", Position);
                        in.Read(L"ItemHash", ItemHash);
                        in.Read(L"instanceEx", instanceEx);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///Reinforce //

                    // NoticeSuccessHighLevelReinforcement
                    // ////////////////////////////////////////////////////
                    const wchar_t* NoticeSuccessHighLevelReinforcement::TypeName =
                        L"NoticeSuccessHighLevelReinforcement";
                    const HashType NoticeSuccessHighLevelReinforcement::TypeHash =
                        StringUtil::Hash(NoticeSuccessHighLevelReinforcement::TypeName);
                    void NoticeSuccessHighLevelReinforcement::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"CharacterName", CharacterName);
                        out.Write(L"ItemName", ItemName);
                        out.Write(L"Level", Level);
                    }

                    void NoticeSuccessHighLevelReinforcement::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"CharacterName", CharacterName);
                        in.Read(L"ItemName", ItemName);
                        in.Read(L"Level", Level);
                    }
                    /////////////////////////////////////////////////////
                    ///NoticeSuccessHighLevelReinforcement //

                    // NoticeSuccessHighLevelLightReinforcement
                    // ////////////////////////////////////////////////////
                    const wchar_t* NoticeSuccessHighLevelLightReinforcement::TypeName =
                        L"NoticeSuccessHighLevelLightReinforcement";
                    const HashType NoticeSuccessHighLevelLightReinforcement::TypeHash =
                        StringUtil::Hash(NoticeSuccessHighLevelLightReinforcement::TypeName);
                    void NoticeSuccessHighLevelLightReinforcement::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"CharacterName", CharacterName);
                        out.Write(L"ItemName", ItemName);
                        out.Write(L"Level", Level);
                    }

                    void NoticeSuccessHighLevelLightReinforcement::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"CharacterName", CharacterName);
                        in.Read(L"ItemName", ItemName);
                        in.Read(L"Level", Level);
                    }
                    /////////////////////////////////////////////////////
                    ///NoticeSuccessHighLevelLightReinforcement //

                    // LightReinforce
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* LightReinforce::TypeName = L"LightReinforce";
                    const HashType LightReinforce::TypeHash =
                        StringUtil::Hash(LightReinforce::TypeName);
                    void LightReinforce::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Position", Position);
                        out.Write(L"ItemHash", ItemHash);
                        out.Write(L"instanceEx", instanceEx);
                    }

                    void LightReinforce::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Position", Position);
                        in.Read(L"ItemHash", ItemHash);
                        in.Read(L"instanceEx", instanceEx);
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
                        out.Write(L"ItemHash", ItemHash);
                        out.Write(L"Instance", Instance);
                    }

                    void AttachMagic::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Position", Position);
                        in.Read(L"ItemHash", ItemHash);
                        in.Read(L"Instance", Instance);
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    ///AttachMagic //

                    // Extract
                    // ////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Extract::TypeName = L"Extract";
                    const HashType Extract::TypeHash =
                        StringUtil::Hash(Extract::TypeName);
                    void Extract::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Position", Position);
                        out.Write(L"ItemHash", ItemHash);
                        out.Write(L"Count", Count);
                        out.WriteEnum(L"Result", Result);
                    }

                    void Extract::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Position", Position);
                        in.Read(L"ItemHash", ItemHash);
                        in.Read(L"Count", Count);
                        in.ReadEnum(L"Result", Result);
                    }
                    ////////////////////////////////////////////////////////////////////////////////
                    ///Extract //

                    // Alchemy
                    // ////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Alchemy::TypeName = L"Alchemy";
                    const HashType Alchemy::TypeHash =
                        StringUtil::Hash(Alchemy::TypeName);
                    void Alchemy::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Products", Products);
                        out.WriteEnum(L"Result", Result);
                    }

                    void Alchemy::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Products", Products);
                        in.ReadEnum(L"Result", Result);
                    }
                    ////////////////////////////////////////////////////////////////////////////////
                    ///Alchemy //

                    // RestoreBelonging
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* RestoreBelonging::TypeName = L"RestoreBelonging";
                    const HashType RestoreBelonging::TypeHash =
                        StringUtil::Hash(RestoreBelonging::TypeName);
                    void RestoreBelonging::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Position", Position);
                        out.Write(L"ItemHash", ItemHash);
                        out.Write(L"instanceEx", instanceEx);
                    }

                    void RestoreBelonging::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Position", Position);
                        in.Read(L"ItemHash", ItemHash);
                        in.Read(L"instanceEx", instanceEx);
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///RestoreBelonging //

                    // RecoverReinforcement
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* RecoverReinforcement::TypeName = L"RecoverReinforcement";
                    const HashType RecoverReinforcement::TypeHash =
                        StringUtil::Hash(RecoverReinforcement::TypeName);
                    void RecoverReinforcement::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Position", Position);
                        out.Write(L"ItemHash", ItemHash);
                        out.Write(L"instanceEx", instanceEx);
                        out.WriteEnum(L"Result", Result);
                    }

                    void RecoverReinforcement::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Position", Position);
                        in.Read(L"ItemHash", ItemHash);
                        in.Read(L"instanceEx", instanceEx);
                        in.ReadEnum(L"Result", Result);
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///RecoverReinforcement //

                    // RecoverLightReinforcement
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* RecoverLightReinforcement::TypeName =
                        L"RecoverLightReinforcement";
                    const HashType RecoverLightReinforcement::TypeHash =
                        StringUtil::Hash(RecoverLightReinforcement::TypeName);
                    void RecoverLightReinforcement::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Position", Position);
                        out.Write(L"ItemHash", ItemHash);
                        out.Write(L"instanceEx", instanceEx);
                        out.WriteEnum(L"Result", Result);
                    }

                    void RecoverLightReinforcement::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Position", Position);
                        in.Read(L"ItemHash", ItemHash);
                        in.Read(L"instanceEx", instanceEx);
                        in.ReadEnum(L"Result", Result);
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///RecoverLightReinforcement //

                    // ResetIndentification
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* ResetIndentification::TypeName = L"ResetIndentification";
                    const HashType ResetIndentification::TypeHash =
                        StringUtil::Hash(ResetIndentification::TypeName);
                    void ResetIndentification::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Position", Position);
                        out.Write(L"ItemHash", ItemHash);
                        out.Write(L"instanceEx", instanceEx);
                        out.WriteEnum(L"Result", Result);
                    }

                    void ResetIndentification::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Position", Position);
                        in.Read(L"ItemHash", ItemHash);
                        in.Read(L"instanceEx", instanceEx);
                        in.ReadEnum(L"Result", Result);
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///ResetIndentification //

                    // Terminate
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Terminate::TypeName = L"Terminate";
                    const HashType Terminate::TypeHash =
                        StringUtil::Hash(Terminate::TypeName);
                    void Terminate::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }

                    void Terminate::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///Terminate //

                    // ListCashItems
                    // //////////////////////////////////////////////////////////////////////////
                    const wchar_t* ListCashItems::TypeName = L"ListCashItems";
                    const HashType ListCashItems::TypeHash =
                        StringUtil::Hash(ListCashItems::TypeName);
                    void ListCashItems::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Total", Total);
                        out.Write(L"Items", Items);
                    }

                    void ListCashItems::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Total", Total);
                        in.Read(L"Items", Items);
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
                        out.WriteEnum(L"Result", Result);
                    }

                    void PurchaseCashItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }
                    //////////////////////////////////////////////////////////////////////////
                    ///PurchaseCashItem //

                    // CashItemMove
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* CashItemMove::TypeName = L"CashItemMove";
                    const HashType CashItemMove::TypeHash =
                        StringUtil::Hash(CashItemMove::TypeName);
                    void CashItemMove::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        if (Result != Ok) return;
                        out.Write(L"Items", Items);
                    }

                    void CashItemMove::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        if (Result != Ok) return;
                        in.Read(L"Items", Items);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///CashItemMove //

                    // CashItemRefund
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* CashItemRefund::TypeName = L"CashItemRefund";
                    const HashType CashItemRefund::TypeHash =
                        StringUtil::Hash(CashItemRefund::TypeName);
                    void CashItemRefund::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }

                    void CashItemRefund::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///CashItemRefund //

                    // CashItemView
                    // //////////////////////////////////////////////////////////////////////////////
                    const wchar_t* CashItemView::TypeName = L"CashItemView";
                    const HashType CashItemView::TypeHash =
                        StringUtil::Hash(CashItemView::TypeName);
                    void CashItemView::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"userlist", userlist);
                    }

                    void CashItemView::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"userlist", userlist);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    ///CashItemView //

                    // BagStates
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* BagStates::TypeName = L"BagStates";
                    const HashType BagStates::TypeHash =
                        StringUtil::Hash(BagStates::TypeName);
                    void BagStates::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Bags", Bags);
                        out.Write(L"BankBags", BankBags);
                    }

                    void BagStates::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Bags", Bags);
                        in.Read(L"BankBags", BankBags);
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///BagStates //

                    // ChangedBagStates
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* ChangedBagStates::TypeName = L"ChangedBagStates";
                    const HashType ChangedBagStates::TypeHash =
                        StringUtil::Hash(ChangedBagStates::TypeName);
                    void ChangedBagStates::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Bags", Bags);
                        out.Write(L"BankBags", BankBags);
                    }

                    void ChangedBagStates::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Bags", Bags);
                        in.Read(L"BankBags", BankBags);
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///ChangedBagStates //

                    // ChangeBagState
                    // ////////////////////////////////////////////////////////////////////////////
                    const wchar_t* ChangeBagState::TypeName = L"ChangeBagState";
                    const HashType ChangeBagState::TypeHash =
                        StringUtil::Hash(ChangeBagState::TypeName);
                    void ChangeBagState::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"IsBank", IsBank);
                        out.Write(L"Bag", Bag);
                    }

                    void ChangeBagState::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"IsBank", IsBank);
                        in.Read(L"Bag", Bag);
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    ///ChangeBagState //

                    // Pause
                    // //////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Pause::TypeName = L"Pause";
                    const HashType Pause::TypeHash = StringUtil::Hash(Pause::TypeName);
                    void Pause::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Paused", Paused);
                    }

                    void Pause::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Paused", Paused);
                    }
                    //////////////////////////////////////////////////////////////////////////////////
                    ///Pause //

                    // CharacterSlot
                    // //////////////////////////////////////////////////////////////////////////
                    const wchar_t* CharacterSlot::TypeName = L"CharacterSlot";
                    const HashType CharacterSlot::TypeHash =
                        StringUtil::Hash(CharacterSlot::TypeName);
                    void CharacterSlot::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"NumberOfSlots", NumberOfSlots);
                    }

                    void CharacterSlot::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"NumberOfSlots", NumberOfSlots);
                    }
                    //////////////////////////////////////////////////////////////////////////
                    ///CharacterSlot //

                    // AcquireCharacterLicense
                    // ////////////////////////////////////////////////////////////////
                    const wchar_t* AcquireCharacterLicense::TypeName = L"AcquireCharacterLicense";
                    const HashType AcquireCharacterLicense::TypeHash =
                        StringUtil::Hash(AcquireCharacterLicense::TypeName);
                    void AcquireCharacterLicense::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", result);
                        out.Write(L"ByItem", ByItem);
                        out.WriteEnum(L"CharacterClass", CharacterClass);
                    }

                    void AcquireCharacterLicense::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", result);
                        in.Read(L"ByItem", ByItem, false);
                        in.ReadEnum(L"CharacterClass", CharacterClass,
                            Constants::ClassType::AnyClassType);
                    }
                    ////////////////////////////////////////////////////////////////
                    ///AcquireCharacterLicense //

                    const wchar_t* Tame::TypeName = L"Tame";
                    const HashType Tame::TypeHash = StringUtil::Hash(Tame::TypeName);
                    void Tame::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"OwnerSerial", OwnerSerial);
                        out.Write(L"TamedSerial", TamedSerial);
                        out.WriteEnum(L"FamiliarType", FamiliarType);
                        out.Write(L"TamedName", TamedName);
                    }

                    void Tame::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"OwnerSerial", OwnerSerial);
                        in.Read(L"TamedSerial", TamedSerial);
                        in.ReadEnum(L"FamiliarType", FamiliarType);
                        in.Read(L"TamedName", TamedName);
                    }

                    const wchar_t* PetTame::TypeName = L"PetTame";
                    const HashType PetTame::TypeHash =
                        StringUtil::Hash(PetTame::TypeName);
                    void PetTame::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"OwnerSerial", OwnerSerial);
                        out.Write(L"TamedSerial", TamedSerial);
                        out.WriteEnum(L"FamiliarType", FamiliarType);
                        out.Write(L"TamedName", TamedName);
                        out.Write(L"PetInfo", PetInfo);
                        out.Write(L"Equipments", Equipments);
                    }

                    void PetTame::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"OwnerSerial", OwnerSerial);
                        in.Read(L"TamedSerial", TamedSerial);
                        in.ReadEnum(L"FamiliarType", FamiliarType);
                        in.Read(L"TamedName", TamedName);
                        in.Read(L"PetInfo", PetInfo);
                        in.Read(L"Equipments", Equipments);
                    }

                    const wchar_t* CheckJoinToSecretStage::TypeName = L"CheckJoinToSecretStage";
                    const HashType CheckJoinToSecretStage::TypeHash =
                        StringUtil::Hash(CheckJoinToSecretStage::TypeName);
                    void CheckJoinToSecretStage::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }

                    void CheckJoinToSecretStage::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }

                    // Random
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Random::TypeName = L"Random";
                    const HashType Random::TypeHash = StringUtil::Hash(Random::TypeName);
                    void Random::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PlayerSerial", PlayerSerial);
                        out.Write(L"RandomName", RandomName);
                        out.Write(L"MinValue", MinValue);
                        out.Write(L"MaxValue", MaxValue);
                        out.Write(L"RandomValue", RandomValue);
                    }

                    void Random::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PlayerSerial", PlayerSerial);
                        in.Read(L"RandomName", RandomName);
                        in.Read(L"MinValue", MinValue);
                        in.Read(L"MaxValue", MaxValue);
                        in.Read(L"RandomValue", RandomValue);
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///Random //

                    // Upgrade
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Upgrade::TypeName = L"Upgrade";
                    const HashType Upgrade::TypeHash =
                        StringUtil::Hash(Upgrade::TypeName);
                    void Upgrade::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }

                    void Upgrade::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///Upgrade //

                    // Rename
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* Rename::TypeName = L"Rename";
                    const HashType Rename::TypeHash = StringUtil::Hash(Rename::TypeName);
                    void Rename::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"TargetType", TargetType);
                        out.WriteEnum(L"Result", Result);
                    }

                    void Rename::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"TargetType", TargetType);
                        in.ReadEnum(L"Result", Result);
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///Rename //

                    // ChangeBaseExpFactor
                    // /////////////////////////////////////////////////////////////////////////////////
                    const wchar_t* ChangeBaseExpFactor::TypeName = L"ChangeBaseExpFactor";
                    const HashType ChangeBaseExpFactor::TypeHash =
                        StringUtil::Hash(ChangeBaseExpFactor::TypeName);
                    void ChangeBaseExpFactor::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"ExpFactor", ExpFactor);
                    }

                    void ChangeBaseExpFactor::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"ExpFactor", ExpFactor);
                    }
                    /////////////////////////////////////////////////////////////////////////////////
                    ///ChangeBaseExpFactor //

                    /* AllMGuild
                     * *********************************************************************************/
                    const wchar_t* AllMGuild::SetPlayer::TypeName = L"AllMGuild::SetPlayer";
                    const HashType AllMGuild::SetPlayer::TypeHash =
                        StringUtil::Hash(AllMGuild::SetPlayer::TypeName);
                    void AllMGuild::SetPlayer::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Serial", Serial);
                        out.Write(L"GuildInfo", GuildInfo);
                    }
                    void AllMGuild::SetPlayer::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Serial", Serial);
                        in.Read(L"GuildInfo", GuildInfo);
                    }

                    const wchar_t* AllMGuild::Create::TypeName = L"AllMGuild::Create";
                    const HashType AllMGuild::Create::TypeHash =
                        StringUtil::Hash(AllMGuild::Create::TypeName);
                    void AllMGuild::Create::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        if (Result != Ok) return;
                        out.Write(L"guildId", guildId);
                    }
                    void AllMGuild::Create::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        if (Result != Ok) return;
                        in.Read(L"guildId", guildId);
                    }

                    const wchar_t* AllMGuild::Invite::TypeName = L"AllMGuild::Invite";
                    const HashType AllMGuild::Invite::TypeHash =
                        StringUtil::Hash(AllMGuild::Invite::TypeName);
                    void AllMGuild::Invite::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }
                    void AllMGuild::Invite::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }

                    const wchar_t* AllMGuild::InviteReply::TypeName = L"AllMGuild::InviteReply";
                    const HashType AllMGuild::InviteReply::TypeHash =
                        StringUtil::Hash(AllMGuild::InviteReply::TypeName);
                    void AllMGuild::InviteReply::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"TargetCharacterName", TargetCharacterName);
                    }
                    void AllMGuild::InviteReply::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"TargetCharacterName", TargetCharacterName);
                    }

                    const wchar_t* AllMGuild::InvitedFrom::TypeName = L"AllMGuild::InvitedFrom";
                    const HashType AllMGuild::InvitedFrom::TypeHash =
                        StringUtil::Hash(AllMGuild::InvitedFrom::TypeName);
                    void AllMGuild::InvitedFrom::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"GuildId", GuildId);
                        out.Write(L"GuildName", GuildName);
                        out.Write(L"InvitorCharacterName", InvitorCharacterName);
                    }
                    void AllMGuild::InvitedFrom::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"GuildId", GuildId);
                        in.Read(L"GuildName", GuildName);
                        in.Read(L"InvitorCharacterName", InvitorCharacterName);
                    }

                    const wchar_t* AllMGuild::Join::TypeName = L"AllMGuild::Join";
                    const HashType AllMGuild::Join::TypeHash =
                        StringUtil::Hash(AllMGuild::Join::TypeName);
                    void AllMGuild::Join::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        if (Result != Ok) return;
                        out.Write(L"guildId", guildId);
                    }
                    void AllMGuild::Join::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        if (Result != Ok) return;
                        in.Read(L"guildId", guildId);
                    }

                    const wchar_t* AllMGuild::Kick::TypeName = L"AllMGuild::Kick";
                    const HashType AllMGuild::Kick::TypeHash =
                        StringUtil::Hash(AllMGuild::Kick::TypeName);
                    void AllMGuild::Kick::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }
                    void AllMGuild::Kick::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }

                    const wchar_t* AllMGuild::Leave::TypeName = L"AllMGuild::Leave";
                    const HashType AllMGuild::Leave::TypeHash =
                        StringUtil::Hash(AllMGuild::Leave::TypeName);
                    void AllMGuild::Leave::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"Serial", Serial);
                        out.Write(L"Kicked", Kicked);
                    }
                    void AllMGuild::Leave::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"Serial", Serial);
                        in.Read(L"Kicked", Kicked);
                    }

                    const wchar_t* AllMGuild::ListMembers::TypeName = L"AllMGuild::ListMembers";
                    const HashType AllMGuild::ListMembers::TypeHash =
                        StringUtil::Hash(AllMGuild::ListMembers::TypeName);
                    void AllMGuild::ListMembers::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"Members", Members);
                        out.Write(L"isSeparate", isSeparate);  // 3.1 by Robotex
                    }
                    void AllMGuild::ListMembers::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"Members", Members);
                        in.Read(L"isSeparate", isSeparate);  // 3.1 by Robotex
                    }

                    const wchar_t* AllMGuild::Remove::TypeName = L"AllMGuild::Remove";
                    const HashType AllMGuild::Remove::TypeHash =
                        StringUtil::Hash(AllMGuild::Remove::TypeName);
                    void AllMGuild::Remove::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }
                    void AllMGuild::Remove::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }

                    const wchar_t* AllMGuild::ValidateName::TypeName = L"AllMGuild::ValidateName";
                    const HashType AllMGuild::ValidateName::TypeHash =
                        StringUtil::Hash(AllMGuild::ValidateName::TypeName);
                    void AllMGuild::ValidateName::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }
                    void AllMGuild::ValidateName::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }

                    const wchar_t* AllMGuild::GuildRankList::TypeName = L"AllMGuild::GuildRankList";
                    const HashType AllMGuild::GuildRankList::TypeHash =
                        StringUtil::Hash(AllMGuild::GuildRankList::TypeName);
                    void AllMGuild::GuildRankList::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"Result", RankList);
                    }
                    void AllMGuild::GuildRankList::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"Result", RankList);
                    }

                    const wchar_t* AllMGuild::ValidateAlias::TypeName = L"AllMGuild::ValidateAlias";
                    const HashType AllMGuild::ValidateAlias::TypeHash =
                        StringUtil::Hash(AllMGuild::ValidateAlias::TypeName);
                    void AllMGuild::ValidateAlias::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }
                    void AllMGuild::ValidateAlias::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }

                    const wchar_t* AllMGuild::AddedGuildExp::TypeName = L"AllMGuild::AddedGuildExp";
                    const HashType AllMGuild::AddedGuildExp::TypeHash =
                        StringUtil::Hash(AllMGuild::AddedGuildExp::TypeName);
                    void AllMGuild::AddedGuildExp::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"GuildLevel", GuildLevel);
                        out.Write(L"GuildExp", GuildExp);
                        out.Write(L"ChangedLevel", ChangedLevel);
                        out.Write(L"IsByItem", IsByItem);
                        out.Write(L"PlayTime", PlayTime);
                        out.Write(L"StartedTimeToPlay", StartedTimeToPlay);
                    }
                    void AllMGuild::AddedGuildExp::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"GuildLevel", GuildLevel);
                        in.Read(L"GuildExp", GuildExp);
                        in.Read(L"ChangedLevel", ChangedLevel);
                        in.Read(L"IsByItem", IsByItem);
                        in.Read(L"PlayTime", PlayTime);
                        in.Read(L"StartedTimeToPlay", StartedTimeToPlay);
                    }

                    const wchar_t* AllMGuild::AddedRankingPoint::TypeName =
                        L"AllMGuild::AddedRankingPoint";  /// by kpongky( 09.07.08 ) BMS 6705
                    const HashType AllMGuild::AddedRankingPoint::TypeHash =
                        StringUtil::Hash(AllMGuild::AddedRankingPoint::TypeName);
                    void AllMGuild::AddedRankingPoint::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"OldRankingPoint", OldRankingPoint);
                        out.Write(L"NewRankingPoint", NewRankingPoint);
                    }
                    void AllMGuild::AddedRankingPoint::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"OldRankingPoint", OldRankingPoint);
                        in.Read(L"NewRankingPoint", NewRankingPoint);
                    }

                    const wchar_t* AllMGuild::AddedGuildMaintenancePoint::TypeName =
                        L"AllMGuild::AddedGuildMaintenancePoint";
                    const HashType AllMGuild::AddedGuildMaintenancePoint::TypeHash =
                        StringUtil::Hash(AllMGuild::AddedGuildMaintenancePoint::TypeName);
                    void AllMGuild::AddedGuildMaintenancePoint::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"GuildMaintenacePoint", GuildMaintenacePoint);
                        out.Write(L"IncreasePoint", IncreasePoint);
                        out.Write(L"ShopOpenDate", ShopOpenDate);
                        out.Write(L"ShopCloseDate", ShopCloseDate);
                    }
                    void AllMGuild::AddedGuildMaintenancePoint::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"GuildMaintenacePoint", GuildMaintenacePoint);
                        in.Read(L"IncreasePoint", IncreasePoint);
                        in.Read(L"ShopOpenDate", ShopOpenDate);
                        in.Read(L"ShopCloseDate", ShopCloseDate);
                    }

                    const wchar_t* AllMGuild::GuildShopItemList::TypeName =
                        L"AllMGuild::GuildShopItemList";
                    const HashType AllMGuild::GuildShopItemList::TypeHash =
                        StringUtil::Hash(AllMGuild::GuildShopItemList::TypeName);
                    void AllMGuild::GuildShopItemList::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"GuildShopItems", GuildShopItems);
                    }
                    void AllMGuild::GuildShopItemList::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"GuildShopItems", GuildShopItems);
                    }

                    const wchar_t* AllMGuild::AddedGuildShopItem::TypeName =
                        L"AllMGuild::AddedGuildShopItem";
                    const HashType AllMGuild::AddedGuildShopItem::TypeHash =
                        StringUtil::Hash(AllMGuild::AddedGuildShopItem::TypeName);
                    void AllMGuild::AddedGuildShopItem::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Item", Item);
                    }
                    void AllMGuild::AddedGuildShopItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Item", Item);
                    }

                    const wchar_t* AllMGuild::AddExpFactorByItem::TypeName =
                        L"AllMGuild::AddExpFactorByItem";
                    const HashType AllMGuild::AddExpFactorByItem::TypeHash =
                        StringUtil::Hash(AllMGuild::AddExpFactorByItem::TypeName);
                    void AllMGuild::AddExpFactorByItem::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"ExpFactor", ExpFactor);
                    }
                    void AllMGuild::AddExpFactorByItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"ExpFactor", ExpFactor);
                    }

                    const wchar_t* AllMGuild::TaxGained::TypeName = L"AllMGuild::TaxGained";
                    const HashType AllMGuild::TaxGained::TypeHash =
                        StringUtil::Hash(AllMGuild::TaxGained::TypeName);
                    void AllMGuild::TaxGained::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Tax", Tax);
                        out.Write(L"TaxPayDate", TaxPayDate);
                        out.WriteEnum(L"Result", Result);
                    }
                    void AllMGuild::TaxGained::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Tax", Tax);
                        in.Read(L"TaxPayDate", TaxPayDate);
                        in.ReadEnum(L"Result", Result);
                    }

                    const wchar_t* AllMGuild::TaxChanged::TypeName = L"AllMGuild::TaxChanged";
                    const HashType AllMGuild::TaxChanged::TypeHash =
                        StringUtil::Hash(AllMGuild::TaxChanged::TypeName);
                    void AllMGuild::TaxChanged::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Tax", Tax);
                    }
                    void AllMGuild::TaxChanged::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Tax", Tax);
                    }

                    const wchar_t* AllMGuild::MessageChange::TypeName = L"AllMGuild::MessageChange";
                    const HashType AllMGuild::MessageChange::TypeHash =
                        StringUtil::Hash(AllMGuild::MessageChange::TypeName);
                    void AllMGuild::MessageChange::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }
                    void AllMGuild::MessageChange::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }

                    const wchar_t* AllMGuild::ChangeGrade::TypeName = L"AllMGuild::ChangeGrade";
                    const HashType AllMGuild::ChangeGrade::TypeHash =
                        StringUtil::Hash(AllMGuild::ChangeGrade::TypeName);
                    void AllMGuild::ChangeGrade::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"TargetCharacterName", TargetCharacterName);
                        out.Write(L"Grade", Grade);
                    }
                    void AllMGuild::ChangeGrade::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
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
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"TargetGrade", TargetGrade);
                        out.Write(L"TargetGradeName", TargetGradeName);
                    }
                    void AllMGuild::ChangeGradeName::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
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
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"TargetGrade", TargetGrade);
                        out.Write(L"Authority", Authority);
                    }
                    void AllMGuild::ChangeGradeAuth::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
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
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"TargetCharacterName", TargetCharacterName);
                    }
                    void AllMGuild::ChangeGuildMaster::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"TargetCharacterName", TargetCharacterName);
                    }

                    const wchar_t* AllMGuild::LevelUp::TypeName = L"AllMGuild::LevelUp";
                    const HashType AllMGuild::LevelUp::TypeHash =
                        StringUtil::Hash(AllMGuild::LevelUp::TypeName);
                    void AllMGuild::LevelUp::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"GuildId", GuildId);
                        out.Write(L"GuildLevel", Level);
                    }
                    void AllMGuild::LevelUp::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"GuildId", GuildId);
                        in.Read(L"GuildLevel", Level);
                    }
                    /*********************************************************************************
                     * AllMGuild */

                     // 3.1 by ultimate
                    const wchar_t* Compose::TypeName = L"Compose";
                    const HashType Compose::TypeHash =
                        StringUtil::Hash(Compose::TypeName);
                    void Compose::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"Type", Type);
                        out.Write(L"GradeState", GradeState);
                        out.Write(L"ItemSlot", ItemSlot);
                    };
                    void Compose::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"Type", Type);
                        in.Read(L"GradeState", GradeState);
                        in.Read(L"ItemSlot", ItemSlot);
                    }

                    /* Quest
                     * *********************************************************************************/
                    const wchar_t* Quest::WorkingList::TypeName = L"Quest::WorkingList";
                    const HashType Quest::WorkingList::TypeHash =
                        StringUtil::Hash(Quest::WorkingList::TypeName);
                    void Quest::WorkingList::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Quests", Quests);
                    };

                    void Quest::WorkingList::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Quests", Quests);
                    }

                    const wchar_t* Quest::CompletedList::TypeName = L"Quest::CompletedList";
                    const HashType Quest::CompletedList::TypeHash =
                        StringUtil::Hash(Quest::CompletedList::TypeName);
                    void Quest::CompletedList::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Quests", Quests);
                    };

                    void Quest::CompletedList::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Quests", Quests);
                    }

                    const wchar_t* Quest::CompletedCount::TypeName = L"Quest::CompletedCount";
                    const HashType Quest::CompletedCount::TypeHash =
                        StringUtil::Hash(Quest::CompletedCount::TypeName);
                    void Quest::CompletedCount::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"QuestHash", QuestHash);
                        out.Write(L"Count", Count);
                    };

                    void Quest::CompletedCount::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"QuestHash", QuestHash);
                        in.Read(L"Count", Count);
                    }

                    const wchar_t* Quest::Offer::TypeName = L"Quest::Offer";
                    const HashType Quest::Offer::TypeHash =
                        StringUtil::Hash(Quest::Offer::TypeName);
                    void Quest::Offer::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"QuestHash", QuestHash);
                    };

                    void Quest::Offer::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"QuestHash", QuestHash);
                    }

                    const wchar_t* Quest::Error::TypeName = L"Quest::Error";
                    const HashType Quest::Error::TypeHash =
                        StringUtil::Hash(Quest::Error::TypeName);
                    void Quest::Error::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"QuestHash", QuestHash);
                        out.WriteEnum(L"Result", Result);
                    };

                    void Quest::Error::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"QuestHash", QuestHash);
                        in.ReadEnum(L"Result", Result);
                    }

                    const wchar_t* Quest::Accept::TypeName = L"Quest::Accept";
                    const HashType Quest::Accept::TypeHash =
                        StringUtil::Hash(Quest::Accept::TypeName);
                    void Quest::Accept::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"QuestHash", QuestHash);
                        out.Write(L"NewState", NewState);
                        out.Write(L"ExpiredDate", ExpiredDate);
                    };

                    void Quest::Accept::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"QuestHash", QuestHash);
                        in.Read(L"NewState", NewState);
                        in.Read(L"ExpiredDate", ExpiredDate);
                    }

                    const wchar_t* Quest::ChangeState::TypeName = L"Quest::ChangeState";
                    const HashType Quest::ChangeState::TypeHash =
                        StringUtil::Hash(Quest::ChangeState::TypeName);
                    void Quest::ChangeState::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"QuestHash", QuestHash);
                        out.Write(L"NewState", NewState);
                        out.Write(L"Parameter", Parameter);
                        out.Write(L"CompletedCount", CompletedCount);
                    };

                    void Quest::ChangeState::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"QuestHash", QuestHash);
                        in.Read(L"NewState", NewState);
                        in.Read(L"Parameter", Parameter);
                        in.Read(L"CompletedCount", CompletedCount);
                    }

                    const wchar_t* Quest::Drop::TypeName = L"Quest::Drop";
                    const HashType Quest::Drop::TypeHash =
                        StringUtil::Hash(Quest::Drop::TypeName);
                    void Quest::Drop::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"QuestHash", QuestHash);
                    };

                    void Quest::Drop::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"QuestHash", QuestHash);
                    }

                    const wchar_t* Quest::Update::TypeName = L"Quest::Update";
                    const HashType Quest::Update::TypeHash =
                        StringUtil::Hash(Quest::Update::TypeName);
                    void Quest::Update::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Quest", Quest);
                    };

                    void Quest::Update::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Quest", Quest);
                    }

                    const wchar_t* Quest::ActivityItem::TypeName = L"Quest::ActivityItem";
                    const HashType Quest::ActivityItem::TypeHash =
                        StringUtil::Hash(Quest::ActivityItem::TypeName);
                    void Quest::ActivityItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    };

                    void Quest::ActivityItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    const wchar_t* Quest::ShowDetailWindow::TypeName = L"Quest::ShowDetailWindow";
                    const HashType Quest::ShowDetailWindow::TypeHash =
                        StringUtil::Hash(Quest::ShowDetailWindow::TypeName);
                    void Quest::ShowDetailWindow::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"QuestHash", QuestHash);
                        out.WriteEnum(L"Result", Result);
                    };

                    void Quest::ShowDetailWindow::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"QuestHash", QuestHash);
                        in.ReadEnum(L"Result", Result);
                    }

                    const wchar_t* Quest::Share::TypeName = L"Quest::Share";
                    const HashType Quest::Share::TypeHash =
                        StringUtil::Hash(Quest::Share::TypeName);
                    void Quest::Share::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"QuestHash", QuestHash);
                        out.Write(L"ShareOwner", ShareOwner);
                    };

                    void Quest::Share::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"QuestHash", QuestHash);
                        in.Read(L"ShareOwner", ShareOwner);
                    }

                    const wchar_t* Quest::ShareResult::TypeName = L"Quest::ShareResult";
                    const HashType Quest::ShareResult::TypeHash =
                        StringUtil::Hash(Quest::ShareResult::TypeName);
                    void Quest::ShareResult::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"QuestHash", QuestHash);
                        out.Write(L"SuccessPlayerCount", SuccessPlayerCount);
                    };

                    void Quest::ShareResult::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"QuestHash", QuestHash);
                        in.Read(L"SuccessPlayerCount", SuccessPlayerCount);
                    }
                    /*********************************************************************************
                     * Quest */

                     /* QuestEvent
                      * *********************************************************************************/
                    const wchar_t* QuestEvent::EnableQuestEvent::TypeName =
                        L"QuestEvent::EnableQuestEvent";
                    const HashType QuestEvent::EnableQuestEvent::TypeHash =
                        StringUtil::Hash(QuestEvent::EnableQuestEvent::TypeName);
                    void QuestEvent::EnableQuestEvent::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"EventQuestID", EventQuestID);
                        out.Write(L"Enable", Enable);
                        out.Write(L"EventExplain", EventExplain);
                    };

                    void QuestEvent::EnableQuestEvent::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"EventQuestID", EventQuestID);
                        in.Read(L"Enable", Enable);
                        in.Read(L"EventExplain", EventExplain);
                    }

                    const wchar_t* QuestEvent::NoticeQuestEvent::TypeName =
                        L"QuestEvent::NoticeQuestEvent";
                    const HashType QuestEvent::NoticeQuestEvent::TypeHash =
                        StringUtil::Hash(QuestEvent::NoticeQuestEvent::TypeName);
                    void QuestEvent::NoticeQuestEvent::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"EventNotice", EventNotice);
                    };

                    void QuestEvent::NoticeQuestEvent::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"EventNotice", EventNotice);
                    }
                    /*********************************************************************************
                     * QuestEvent */

                     /* NpcDropEvent
                      * *********************************************************************************/
                    const wchar_t* NpcDropEvent::EnableNpcDropEvent::TypeName =
                        L"NpcDropEvent::EnableNpcDropEvent";
                    const HashType NpcDropEvent::EnableNpcDropEvent::TypeHash =
                        StringUtil::Hash(NpcDropEvent::EnableNpcDropEvent::TypeName);
                    void NpcDropEvent::EnableNpcDropEvent::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"EventID", EventID);
                        out.Write(L"Enable", Enable);
                        out.Write(L"EventExplain", EventExplain);
                    };

                    void NpcDropEvent::EnableNpcDropEvent::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"EventID", EventID);
                        in.Read(L"Enable", Enable);
                        in.Read(L"EventExplain", EventExplain);
                    }
                    /*********************************************************************************
                     * NpcDropEvent */

                     /* PlayerStore
                      * ***************************************************************************/
                    const wchar_t* PlayerStore::Open::TypeName = L"PlayerStore::Open";
                    const HashType PlayerStore::Open::TypeHash =
                        StringUtil::Hash(PlayerStore::Open::TypeName);
                    void PlayerStore::Open::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PlayerSerial", PlayerSerial);
                        out.Write(L"StoreItemHash", StoreItemHash);
                        out.Write(L"Title", Title);
                    };

                    void PlayerStore::Open::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PlayerSerial", PlayerSerial);
                        in.Read(L"StoreItemHash", StoreItemHash);
                        in.Read(L"Title", Title);
                    }

                    const wchar_t* PlayerStore::Close::TypeName = L"PlayerStore::Close";
                    const HashType PlayerStore::Close::TypeHash =
                        StringUtil::Hash(PlayerStore::Close::TypeName);
                    void PlayerStore::Close::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PlayerSerial", PlayerSerial);
                    };

                    void PlayerStore::Close::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PlayerSerial", PlayerSerial);
                    }

                    const wchar_t* PlayerStore::List::TypeName = L"PlayerStore::List";
                    const HashType PlayerStore::List::TypeHash =
                        StringUtil::Hash(PlayerStore::List::TypeName);
                    void PlayerStore::List::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PlayerSerial", PlayerSerial);
                        out.Write(L"Slots", Slots);
                        out.Write(L"SlotsForPet", SlotsForPet);
                    };

                    void PlayerStore::List::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PlayerSerial", PlayerSerial);
                        in.Read(L"Slots", Slots);
                        in.Read(L"SlotsForPet", SlotsForPet);
                    }

                    const wchar_t* PlayerStore::Buy::TypeName = L"PlayerStore::Buy";
                    const HashType PlayerStore::Buy::TypeHash =
                        StringUtil::Hash(PlayerStore::Buy::TypeName);
                    void PlayerStore::Buy::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        if (Result != Ok) return;
                        out.Write(L"Buyer", Buyer);
                        out.Write(L"Item", Item);
                        out.Write(L"Position", Position);
                        out.Write(L"CurrentMoney", CurrentMoney);
                    };

                    void PlayerStore::Buy::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        if (Result != Ok) return;
                        in.Read(L"Buyer", Buyer);
                        in.Read(L"Item", Item);
                        in.Read(L"Position", Position);
                        in.Read(L"CurrentMoney", CurrentMoney);
                    }
                    /***************************************************************************
                     * PlayerStore */

                    const wchar_t* Bank::List::TypeName = L"Bank::List";
                    const HashType Bank::List::TypeHash =
                        StringUtil::Hash(Bank::List::TypeName);
                    void Bank::List::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"BagNumber", BagNumber);
                        out.Write(L"Items", Items);
                    }
                    void Bank::List::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"BagNumber", BagNumber);
                        in.Read(L"Items", Items);
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
                        out.Write(L"Count", Count);
                    }
                    void Bank::Pop::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"SourceStorage", SourceStorage);
                        in.Read(L"TargetInventory", TargetInventory);
                        in.Read(L"Count", Count);
                    }

                    const wchar_t* Bank::SwapItemInStorage::TypeName = L"Bank::SwapItemInStorage";
                    const HashType Bank::SwapItemInStorage::TypeHash =
                        StringUtil::Hash(Bank::SwapItemInStorage::TypeName);
                    void Bank::SwapItemInStorage::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"SourceStorage", SourceStorage);
                        out.Write(L"TargetStorage", TargetStorage);
                        out.Write(L"Count", Count);
                    }
                    void Bank::SwapItemInStorage::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"SourceStorage", SourceStorage);
                        in.Read(L"TargetStorage", TargetStorage);
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
                    void CashItemStorage::List::ItemUnit::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(L"CashItemStorage::List::ItemUnit");
                        out.Write(L"ItemSerial", ItemSerial);
                        out.Write(L"instanceEx", instanceEx);  // 3.1 by ultimate
                        out.Write(L"ItemHash", ItemHash);
                        out.Write(L"StackedCount", StackedCount);
                    }
                    void CashItemStorage::List::ItemUnit::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(L"CashItemStorage::List::ItemUnit");
                        in.Read(L"ItemSerial", ItemSerial);
                        in.Read(L"instanceEx", instanceEx);  // 3.1 by ultimate
                        in.Read(L"ItemHash", ItemHash);
                        in.Read(L"StackedCount", StackedCount);
                    }

                    const wchar_t* CashItemStorage::List::TypeName = L"CashItemStorage::List";
                    const HashType CashItemStorage::List::TypeHash =
                        StringUtil::Hash(CashItemStorage::List::TypeName);
                    void CashItemStorage::List::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PageIndex", PageIndex);
                        out.Write(L"Items", Items);
                    }
                    void CashItemStorage::List::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PageIndex", PageIndex);
                        in.Read(L"Items", Items);
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

                    const wchar_t* AskContinueStage::TypeName = L"AskContinueStage";
                    const HashType AskContinueStage::TypeHash =
                        StringUtil::Hash(AskContinueStage::TypeName);
                    void AskContinueStage::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void AskContinueStage::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    const wchar_t* PvpMissionCleared::TypeName = L"PvpMissionCleared";
                    const HashType PvpMissionCleared::TypeHash =
                        StringUtil::Hash(PvpMissionCleared::TypeName);
                    void PvpMissionCleared::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"isWin", isWin);
                        out.WriteEnum(L"subInfo", subInfo);
                    }
                    void PvpMissionCleared::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"isWin", isWin);
                        in.ReadEnum(L"subInfo", subInfo);
                    }

                    /* Pvp
                     * ***********************************************************************************/
                    const wchar_t* Pvp::PlayerDie::TypeName = L"Pvp::PlayerDie";
                    const HashType Pvp::PlayerDie::TypeHash =
                        StringUtil::Hash(Pvp::PlayerDie::TypeName);
                    void Pvp::PlayerDie::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"ObjectSerial", ObjectSerial);
                        out.Write(L"FromSerial", FromSerial);
                    }
                    void Pvp::PlayerDie::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"ObjectSerial", ObjectSerial);
                        in.Read(L"FromSerial", FromSerial);
                    }

                    const wchar_t* Pvp::PlayerSave::TypeName = L"Pvp::PlayerSave";
                    const HashType Pvp::PlayerSave::TypeHash =
                        StringUtil::Hash(Pvp::PlayerSave::TypeName);
                    void Pvp::PlayerSave::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"ObjectSerial", ObjectSerial);
                        out.Write(L"FromSerial", FromSerial);
                    }
                    void Pvp::PlayerSave::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"ObjectSerial", ObjectSerial);
                        in.Read(L"FromSerial", FromSerial);
                    }

                    const wchar_t* Pvp::MatchResultInfo::TypeName = L"Pvp::MatchResultInfo";
                    const HashType Pvp::MatchResultInfo::TypeHash =
                        StringUtil::Hash(Pvp::MatchResultInfo::TypeName);
                    void Pvp::MatchResultInfo::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"SerialDieCount", SerialDieCount);
                        out.Write(L"SerialkillCount", SerialKillCount);
                    }
                    void Pvp::MatchResultInfo::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"SerialDieCount", SerialDieCount);
                        in.Read(L"SerialkillCount", SerialKillCount);
                    }

                    const wchar_t* Pvp::NotifySpectatorInOut::TypeName =
                        L"Pvp::NotifySpectatorInOut";
                    const HashType Pvp::NotifySpectatorInOut::TypeHash =
                        StringUtil::Hash(Pvp::NotifySpectatorInOut::TypeName);
                    void Pvp::NotifySpectatorInOut::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"CharacterName", CharacterName);
                        out.Write(L"Entered", Entered);
                    }
                    void Pvp::NotifySpectatorInOut::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"CharacterName", CharacterName);
                        in.Read(L"Entered", Entered);
                    }

                    const wchar_t* Pvp::NotifySpectators::TypeName = L"Pvp::NotifySpectators";
                    const HashType Pvp::NotifySpectators::TypeHash =
                        StringUtil::Hash(Pvp::NotifySpectators::TypeName);
                    void Pvp::NotifySpectators::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Spectators", Spectators);
                    }
                    void Pvp::NotifySpectators::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Spectators", Spectators);
                    }

                    const wchar_t* Pvp::BattleGroundInfo::TypeName = L"Pvp::BattleGroundInfo";
                    const HashType Pvp::BattleGroundInfo::TypeHash =
                        StringUtil::Hash(Pvp::BattleGroundInfo::TypeName);
                    void Pvp::BattleGroundInfo::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Info", Info);
                    }
                    void Pvp::BattleGroundInfo::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Info", Info);
                    }

                    const wchar_t* Pvp::NotifyBattleGroundInfo::TypeName =
                        L"Pvp::NotifyBattleGroundInfo";
                    const HashType Pvp::NotifyBattleGroundInfo::TypeHash =
                        StringUtil::Hash(Pvp::NotifyBattleGroundInfo::TypeName);
                    void Pvp::NotifyBattleGroundInfo::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"TeamlKillInfo", TeamlKillInfo);
                        out.Write(L"RemainTime", RemainTime);
                    }
                    void Pvp::NotifyBattleGroundInfo::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"TeamlKillInfo", TeamlKillInfo);
                        in.Read(L"RemainTime", RemainTime);
                    }

                    const wchar_t* Pvp::PvpLeaveStage::TypeName = L"Pvp::PvpLeaveStage";
                    const HashType Pvp::PvpLeaveStage::TypeHash =
                        StringUtil::Hash(Pvp::PvpLeaveStage::TypeName);
                    void Pvp::PvpLeaveStage::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                    }
                    void Pvp::PvpLeaveStage::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                    }

                    /***********************************************************************************
                     * Pvp */

                     /* StylePoint
                      * ***********************************************************************************/

                    const wchar_t* StylePoint::ChangeState::TypeName = L"StylePoint::ChangeState";
                    const HashType StylePoint::ChangeState::TypeHash =
                        StringUtil::Hash(StylePoint::ChangeState::TypeName);
                    void StylePoint::ChangeState::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"State", State);
                        out.Write(L"CurrentPoint", CurrentPoint);
                    }
                    void StylePoint::ChangeState::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"State", State);
                        in.Read(L"CurrentPoint", CurrentPoint);
                    }

                    const wchar_t* StylePoint::ActionType::TypeName = L"StylePoint::ActionType";
                    const HashType StylePoint::ActionType::TypeHash =
                        StringUtil::Hash(StylePoint::ActionType::TypeName);
                    void StylePoint::ActionType::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Type", Type);
                    }
                    void StylePoint::ActionType::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Type", Type);
                    }

                    /***********************************************************************************
                     * StylePoint */

                     /* Dailyitem::ConnectedWeek
                      * ***********************************************************************/
                    const wchar_t* Dailyitem::ConnectedWeek::TypeName = L"Dailyitem::ConnectedWeek";
                    const HashType Dailyitem::ConnectedWeek::TypeHash =
                        StringUtil::Hash(Dailyitem::ConnectedWeek::TypeName);

                    void Dailyitem::ConnectedWeek::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Week", Week);
                        out.Write(L"ConnectedDate", ConnectedDate);
                    }

                    void Dailyitem::ConnectedWeek::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Week", Week);
                        in.Read(L"ConnectedDate", ConnectedDate);
                    }
                    /***********************************************************************
                     * Dailyitem::ConnectedWeek */

                     /* GameAddictsPrevent
                      * ***********************************************************************/
                    const wchar_t* GameAddictsPrevent::TypeName = L"GameAddictsPrevent";
                    const HashType GameAddictsPrevent::TypeHash =
                        StringUtil::Hash(GameAddictsPrevent::TypeName);

                    void GameAddictsPrevent::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"StackedPlayTimeInSec", StackedPlayTimeInSec);
                    }

                    void GameAddictsPrevent::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"StackedPlayTimeInSec", StackedPlayTimeInSec);
                    }
                    /***********************************************************************
                     * GameAddictsPrevent */

                     /* Mail ***********************************************************************/
                    const wchar_t* Mail::RequestMailList::TypeName = L"Mail::RequestMailList";
                    const HashType Mail::RequestMailList::TypeHash =
                        StringUtil::Hash(Mail::RequestMailList::TypeName);
                    void Mail::RequestMailList::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"MailHeaderList", MailHeaderList);
                        out.WriteEnum(L"Result", Result);
                    }
                    void Mail::RequestMailList::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"MailHeaderList", MailHeaderList);
                        in.ReadEnum(L"Result", Result);
                    }

                    const wchar_t* Mail::ReadMail::TypeName = L"Mail::ReadMail";
                    const HashType Mail::ReadMail::TypeHash =
                        StringUtil::Hash(Mail::ReadMail::TypeName);
                    void Mail::ReadMail::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"MailContents", MailContents);
                        out.WriteEnum(L"Result", Result);
                    }
                    void Mail::ReadMail::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"MailContents", MailContents);
                        in.ReadEnum(L"Result", Result);
                    }

                    const wchar_t* Mail::SendMail::TypeName = L"Mail::SendMail";
                    const HashType Mail::SendMail::TypeHash =
                        StringUtil::Hash(Mail::SendMail::TypeName);
                    void Mail::SendMail::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }
                    void Mail::SendMail::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }

                    const wchar_t* Mail::DisposeMail::TypeName = L"Mail::DisposeMail";
                    const HashType Mail::DisposeMail::TypeHash =
                        StringUtil::Hash(Mail::DisposeMail::TypeName);
                    void Mail::DisposeMail::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Index", Index);
                        out.WriteEnum(L"Result", Result);
                    }
                    void Mail::DisposeMail::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Index", Index);
                        in.ReadEnum(L"Result", Result);
                    }

                    const wchar_t* Mail::DisposeReadMails::TypeName = L"Mail::DisposeReadMails";
                    const HashType Mail::DisposeReadMails::TypeHash =
                        StringUtil::Hash(Mail::DisposeReadMails::TypeName);
                    void Mail::DisposeReadMails::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Indexes", Indexes);
                        out.WriteEnum(L"Result", Result);
                    }
                    void Mail::DisposeReadMails::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Indexes", Indexes);
                        in.ReadEnum(L"Result", Result);
                    }

                    const wchar_t* Mail::MailAlarmed::TypeName = L"Mail::MailAlarmed";
                    const HashType Mail::MailAlarmed::TypeHash =
                        StringUtil::Hash(Mail::MailAlarmed::TypeName);
                    void Mail::MailAlarmed::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Types", Types);
                    }
                    void Mail::MailAlarmed::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Types", Types, Normal);
                    }

                    const wchar_t* Mail::RetrieveAttached::TypeName = L"Mail::RetrieveAttached";
                    const HashType Mail::RetrieveAttached::TypeHash =
                        StringUtil::Hash(Mail::RetrieveAttached::TypeName);
                    void Mail::RetrieveAttached::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Index", Index);
                        out.Write(L"AttachedMoney", AttachedMoney);
                        out.WriteEnum(L"Result", Result);
                    }
                    void Mail::RetrieveAttached::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Index", Index);
                        in.Read(L"AttachedMoney", AttachedMoney);
                        in.ReadEnum(L"Result", Result);
                    }

                    const wchar_t* Mail::RollbackFinished::TypeName = L"Mail::RollbackFinished";
                    const HashType Mail::RollbackFinished::TypeHash =
                        StringUtil::Hash(Mail::RollbackFinished::TypeName);
                    void Mail::RollbackFinished::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }
                    void Mail::RollbackFinished::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }
                    /*********************************************************************** Mail */

                    /* Fishing
                     * ***********************************************************************/
                    const wchar_t* Fishing::Start::TypeName = L"Fishing::Start";
                    const HashType Fishing::Start::TypeHash =
                        StringUtil::Hash(Fishing::Start::TypeName);
                    void Fishing::Start::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"PlayerSerial", PlayerSerial);
                    }
                    void Fishing::Start::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"PlayerSerial", PlayerSerial);
                    }

                    const wchar_t* Fishing::Do::TypeName = L"Fishing::Do";
                    const HashType Fishing::Do::TypeHash =
                        StringUtil::Hash(Fishing::Do::TypeName);
                    void Fishing::Do::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"PlayerSerial", PlayerSerial);
                    }
                    void Fishing::Do::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"PlayerSerial", PlayerSerial);
                    }

                    const wchar_t* Fishing::FishingResult::TypeName = L"Fishing::FishingResult";
                    const HashType Fishing::FishingResult::TypeHash =
                        StringUtil::Hash(Fishing::FishingResult::TypeName);
                    void Fishing::FishingResult::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"PlayerSerial", PlayerSerial);
                    }
                    void Fishing::FishingResult::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"PlayerSerial", PlayerSerial);
                    }

                    const wchar_t* Fishing::ChangeBaitCount::TypeName = L"Fishing::ChangeBaitCount";
                    const HashType Fishing::ChangeBaitCount::TypeHash =
                        StringUtil::Hash(Fishing::ChangeBaitCount::TypeName);
                    void Fishing::ChangeBaitCount::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"Count", Count);
                    }
                    void Fishing::ChangeBaitCount::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"Count", Count);
                    }

                    const wchar_t* Fishing::List::TypeName = L"Fishing::List";
                    const HashType Fishing::List::TypeHash =
                        StringUtil::Hash(Fishing::List::TypeName);
                    void Fishing::List::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"LatestList", LatestList);
                        out.Write(L"LatestRareList", LatestRareList);
                        out.Write(L"Epic", Epic);
                    }
                    void Fishing::List::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"LatestList", LatestList);
                        in.Read(L"LatestRareList", LatestRareList);
                        in.Read(L"Epic", Epic);
                    }

                    const wchar_t* Fishing::FishingInfo::TypeName = L"Fishing::FishingInfo";
                    const HashType Fishing::FishingInfo::TypeHash =
                        StringUtil::Hash(Fishing::FishingInfo::TypeName);
                    void Fishing::FishingInfo::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PlayerSerial", PlayerSerial);
                        out.Write(L"Info", Info);
                    }
                    void Fishing::FishingInfo::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PlayerSerial", PlayerSerial);
                        in.Read(L"Info", Info);
                    }

                    const wchar_t* Fishing::End::TypeName = L"Fishing::End";
                    const HashType Fishing::End::TypeHash =
                        StringUtil::Hash(Fishing::End::TypeName);
                    void Fishing::End::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"PlayerSerial", PlayerSerial);
                    }
                    void Fishing::End::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"PlayerSerial", PlayerSerial);
                    }

                    const wchar_t* Fishing::NoticeFishing::TypeName = L"Fishing::NoticeFishing";
                    const HashType Fishing::NoticeFishing::TypeHash =
                        StringUtil::Hash(Fishing::NoticeFishing::TypeName);
                    void Fishing::NoticeFishing::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"CharacterName", CharacterName);
                        out.Write(L"ItemName", ItemName);
                    }
                    void Fishing::NoticeFishing::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"CharacterName", CharacterName);
                        in.Read(L"ItemName", ItemName);
                    }
                    /*********************************************************************** Fishing
                     */

                    const wchar_t* RaiseEvent::TypeName = L"RaiseEvent";
                    const HashType RaiseEvent::TypeHash =
                        StringUtil::Hash(RaiseEvent::TypeName);
                    void RaiseEvent::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"EventInfo", eventInfo);
                        out.Write(L"EndTime", endTime);
                    }
                    void RaiseEvent::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"EventInfo", eventInfo);
                        in.Read(L"EndTime", endTime);
                    }

                    const wchar_t* Rebirth::TypeName = L"Rebirth";
                    const HashType Rebirth::TypeHash =
                        StringUtil::Hash(Rebirth::TypeName);
                    void Rebirth::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"PlayerSerial", PlayerSerial);
                        out.Write(L"Level", Level);
                        out.Write(L"StoredLevel", StoredLevel);
                        out.Write(L"RebirthCount", RebirthCount);
                        out.Write(L"StoredSkillPoint", StoredSkillPoint);
                        out.Write(L"LastRebirthDateTime", LastRebirthDateTime);
                        out.Write(L"UpdatedLicense", UpdatedLicense);
                    }
                    void Rebirth::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"PlayerSerial", PlayerSerial);
                        in.Read(L"Level", Level);
                        in.Read(L"StoredLevel", StoredLevel);
                        in.Read(L"RebirthCount", RebirthCount);
                        in.Read(L"StoredSkillPoint", StoredSkillPoint);
                        in.Read(L"LastRebirthDateTime", LastRebirthDateTime);
                        in.Read(L"UpdatedLicense", UpdatedLicense);
                    }

                    /* Gamble
                     * ***********************************************************************/
                    const wchar_t* Gamble::SlimeRace::GameStart::TypeName =
                        L"Gamble::SlimeRace::GameStart";
                    const HashType Gamble::SlimeRace::GameStart::TypeHash =
                        StringUtil::Hash(Gamble::SlimeRace::GameStart::TypeName);
                    void Gamble::SlimeRace::GameStart::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"NotEnoughChipsUsers", NotEnoughChipsUsers);
                    }
                    void Gamble::SlimeRace::GameStart::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"NotEnoughChipsUsers", NotEnoughChipsUsers);
                    }

                    const wchar_t* Gamble::SlimeRace::GameEnd::TypeName =
                        L"Gamble::SlimeRace::GameEnd";
                    const HashType Gamble::SlimeRace::GameEnd::TypeHash =
                        StringUtil::Hash(Gamble::SlimeRace::GameEnd::TypeName);
                    void Gamble::SlimeRace::GameEnd::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"WinnerNumber", WinnerNumber);
                        out.Write(L"Winners", Winners);
                        out.Write(L"JackPotCharacter", JackPotCharacter);
                        out.Write(L"EarnedChips", EarnedChips);
                        out.Write(L"EarnedMoney", EarnedMoney);
                        out.Write(L"ReceivedMoneyByMail", ReceivedMoneyByMail);
                    }
                    void Gamble::SlimeRace::GameEnd::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"WinnerNumber", WinnerNumber);
                        in.Read(L"Winners", Winners);
                        in.Read(L"JackPotCharacter", JackPotCharacter);
                        in.Read(L"EarnedChips", EarnedChips);
                        in.Read(L"EarnedMoney", EarnedMoney);
                        in.Read(L"ReceivedMoneyByMail", ReceivedMoneyByMail);
                    }

                    const wchar_t* Gamble::SlimeRace::NoMoreBet::TypeName =
                        L"Gamble::SlimeRace::NoMoreBet";
                    const HashType Gamble::SlimeRace::NoMoreBet::TypeHash =
                        StringUtil::Hash(Gamble::SlimeRace::NoMoreBet::TypeName);
                    void Gamble::SlimeRace::NoMoreBet::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"NotEnoughChipsUsers", NotEnoughChipsUsers);
                    }
                    void Gamble::SlimeRace::NoMoreBet::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"NotEnoughChipsUsers", NotEnoughChipsUsers);
                    }

                    const wchar_t* Gamble::SlimeRace::ResetGame::TypeName =
                        L"Gamble::SlimeRace::ResetGame";
                    const HashType Gamble::SlimeRace::ResetGame::TypeHash =
                        StringUtil::Hash(Gamble::SlimeRace::ResetGame::TypeName);
                    void Gamble::SlimeRace::ResetGame::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"NextGameTime", NextGameTime);
                    }
                    void Gamble::SlimeRace::ResetGame::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"NextGameTime", NextGameTime);
                    }

                    const wchar_t* Gamble::SlimeRace::Bet::TypeName = L"Gamble::SlimeRace::Bet";
                    const HashType Gamble::SlimeRace::Bet::TypeHash =
                        StringUtil::Hash(Gamble::SlimeRace::Bet::TypeName);
                    void Gamble::SlimeRace::Bet::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"CharacterName", CharacterName);
                        out.WriteEnum(L"Position", Position);
                        out.Write(L"Chips", Chips);
                    }
                    void Gamble::SlimeRace::Bet::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"CharacterName", CharacterName);
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
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"CharacterName", CharacterName);
                        out.WriteEnum(L"Position", Position);
                        // out.Write( L"Chips" ,			Chips );
                    }
                    void Gamble::SlimeRace::ClearBet::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"CharacterName", CharacterName);
                        in.ReadEnum(L"Position", Position);
                        // in.Read( L"Chips" ,			Chips );
                    }

                    const wchar_t* Gamble::SlimeRace::SentBettingState::TypeName =
                        L"Gamble::SlimeRace::SentBettingState";
                    const HashType Gamble::SlimeRace::SentBettingState::TypeHash =
                        StringUtil::Hash(Gamble::SlimeRace::SentBettingState::TypeName);
                    void Gamble::SlimeRace::SentBettingState::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"BettingStates", BettingStates);
                    }
                    void Gamble::SlimeRace::SentBettingState::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"BettingStates", BettingStates);
                    }

                    const wchar_t* Gamble::SlimeRace::SentRecentResults::TypeName =
                        L"Gamble::SlimeRace::SentRecentResults";
                    const HashType Gamble::SlimeRace::SentRecentResults::TypeHash =
                        StringUtil::Hash(Gamble::SlimeRace::SentRecentResults::TypeName);
                    void Gamble::SlimeRace::SentRecentResults::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"bBettingPossible", bBettingPossible);
                        out.Write(L"NextGameTime", NextGameTime);
                        out.Write(L"Results", Results);
                    }
                    void Gamble::SlimeRace::SentRecentResults::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"bBettingPossible", bBettingPossible);
                        in.Read(L"NextGameTime", NextGameTime);
                        in.Read(L"Results", Results);
                    }
                    /*********************************************************************** Gamble
                     */

                     // NoticeRaisePopupStage ////////////////////////////////////////////////////
                    const wchar_t* NoticeRaisePopupStage::TypeName = L"NoticeRaisePopupStage";
                    const HashType NoticeRaisePopupStage::TypeHash =
                        StringUtil::Hash(NoticeRaisePopupStage::TypeName);
                    void NoticeRaisePopupStage::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"CharacterName", CharacterName);
                        out.Write(L"StageGroupHash", StageGroupHash);
                        out.Write(L"AccessLevel", AccessLevel);
                    }

                    void NoticeRaisePopupStage::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"CharacterName", CharacterName);
                        in.Read(L"StageGroupHash", StageGroupHash);
                        in.Read(L"AccessLevel", AccessLevel);
                    }
                    ///////////////////////////////////////////////////// NoticeRaisePopupStage //

                    // NoticeMythTowerFloor ////////////////////////////////////////////////////
                    const wchar_t* NoticeMythTowerFloor::TypeName = L"NoticeMythTowerFloor";
                    const HashType NoticeMythTowerFloor::TypeHash =
                        StringUtil::Hash(NoticeMythTowerFloor::TypeName);
                    void NoticeMythTowerFloor::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"CharacterName", CharacterName);
                        out.Write(L"Floor", Floor);
                        out.Write(L"StageGroupHash", StageGroupHash);
                        out.Write(L"AccessLevel", AccessLevel);
                    }

                    void NoticeMythTowerFloor::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"CharacterName", CharacterName);
                        in.Read(L"Floor", Floor);
                        in.Read(L"StageGroupHash", StageGroupHash);
                        in.Read(L"AccessLevel", AccessLevel);
                    }
                    ///////////////////////////////////////////////////// NoticeRaisePopupStage //

                    // PvPRewardInfo /////////////////////////////////////////////////////////////
                    const wchar_t* PvPRewardInfo::TypeName = L"PvPRewardInfo";
                    const HashType PvPRewardInfo::TypeHash =
                        StringUtil::Hash(PvPRewardInfo::TypeName);
                    void PvPRewardInfo::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"WinnerHash", WinnerHash);
                        out.Write(L"WinnerCount", WinnerCount);
                        out.Write(L"LoserHash", LoserHash);
                        out.Write(L"LoserCount", LoserCount);
                    }

                    void PvPRewardInfo::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"WinnerHash", WinnerHash);
                        in.Read(L"WinnerCount", WinnerCount);
                        in.Read(L"LoserHash", LoserHash);
                        in.Read(L"LoserCount", LoserCount);
                    }
                    ///////////////////////////////////////////////////////////// PvPRewardInfo //

                    // PvPRewardItemGain /////////////////////////////////////////////////////////
                    const wchar_t* PvPRewardItemGain::TypeName = L"PvPRewardItemGain";
                    const HashType PvPRewardItemGain::TypeHash =
                        StringUtil::Hash(PvPRewardItemGain::TypeName);
                    void PvPRewardItemGain::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"PlayerSerial", PlayerSerial);
                        out.Write(L"MailSupport", MailSupport);
                        out.Write(L"Hash", Hash);
                        out.Write(L"Count", Count);
                    }

                    void PvPRewardItemGain::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"PlayerSerial", PlayerSerial);
                        in.Read(L"MailSupport", MailSupport);
                        in.Read(L"Hash", Hash);
                        in.Read(L"Count", Count);
                    }
                    ///////////////////////////////////////////////////////// PvPRewardItemGain //

                    // Family
                    // //////////////////////////////////////////////////////////////////////////////////

                    // Info
                    const wchar_t* Family::Info::TypeName = L"Family::Info";
                    const HashType Family::Info::TypeHash =
                        StringUtil::Hash(Family::Info::TypeName);
                    void Family::Info::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"FamilyInfo", FamilyInfo);
                        out.Write(L"Condition", Condition);
                        out.Write(L"Members", Members);
                    }

                    void Family::Info::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"FamilyInfo", FamilyInfo);
                        in.Read(L"Condition", Condition);
                        in.Read(L"Members", Members);
                    }

                    // InviteResult
                    const wchar_t* Family::InviteResult::TypeName = L"Family::InviteResult";
                    const HashType Family::InviteResult::TypeHash =
                        StringUtil::Hash(Family::InviteResult::TypeName);
                    void Family::InviteResult::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }

                    void Family::InviteResult::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }

                    // RequestToInvite
                    const wchar_t* Family::RequestToInvite::TypeName = L"Family::RequestToInvite";
                    const HashType Family::RequestToInvite::TypeHash =
                        StringUtil::Hash(Family::RequestToInvite::TypeName);
                    void Family::RequestToInvite::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"FromOwner", FromOwner);
                    }

                    void Family::RequestToInvite::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"FromOwner", FromOwner);
                    }

                    // JoineResult
                    const wchar_t* Family::JoineResult::TypeName = L"Family::JoineResult";
                    const HashType Family::JoineResult::TypeHash =
                        StringUtil::Hash(Family::JoineResult::TypeName);
                    void Family::JoineResult::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"InviteOwner", InviteOwner);
                    }

                    void Family::JoineResult::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"InviteOwner", InviteOwner);
                    }

                    // JoineResult
                    const wchar_t* Family::LeavedResult::TypeName = L"Family::LeavedResult";
                    const HashType Family::LeavedResult::TypeHash =
                        StringUtil::Hash(Family::LeavedResult::TypeName);
                    void Family::LeavedResult::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }

                    void Family::LeavedResult::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }

                    // TakePresentResult
                    const wchar_t* Family::TakePresentResult::TypeName =
                        L"Family::TakePresentResult";
                    const HashType Family::TakePresentResult::TypeHash =
                        StringUtil::Hash(Family::TakePresentResult::TypeName);
                    void Family::TakePresentResult::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"Param", Param);
                        out.Write(L"IsSuccess", IsSuccess);
                        out.WriteEnum(L"Type", Type);
                        out.Write(L"Condition", Condition);
                        out.Write(L"ReceiveMembers", receiveMembers);
                    }

                    void Family::TakePresentResult::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"Param", Param);
                        in.Read(L"IsSuccess", IsSuccess);
                        in.ReadEnum(L"Type", Type);
                        in.Read(L"Condition", Condition);
                        in.Read(L"ReceiveMembers", receiveMembers);
                    }

                    // RefreshFailed
                    const wchar_t* Family::RefreshFailed::TypeName = L"Family::RefreshFailed";
                    const HashType Family::RefreshFailed::TypeHash =
                        StringUtil::Hash(Family::RefreshFailed::TypeName);
                    void Family::RefreshFailed::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Type", Result);
                    }

                    void Family::RefreshFailed::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Type", Result);
                    }

                    // KickResult
                    const wchar_t* Family::KickResult::TypeName = L"Family::KickResult";
                    const HashType Family::KickResult::TypeHash =
                        StringUtil::Hash(Family::KickResult::TypeName);
                    void Family::KickResult::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Type", Result);
                        out.Write(L"Target", Target);
                    }

                    void Family::KickResult::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Type", Result);
                        in.Read(L"Target", Target);
                    }

                    // OpenMarket
                    // //////////////////////////////////////////////////////////////////////////////////
                    // RequestProductList
                    const wchar_t* OpenMarket::RequestProductList::TypeName =
                        L"OpenMarket::RequestProductList";
                    const HashType OpenMarket::RequestProductList::TypeHash =
                        StringUtil::Hash(OpenMarket::RequestProductList::TypeName);
                    void OpenMarket::RequestProductList::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"ProductInfos", ProductInfos);
                        out.WriteEnum(L"OrderType", OrderType);
                        out.Write(L"CurrentPage", CurrentPage);
                        out.Write(L"TotalPage", TotalPage);
                        out.WriteEnum(L"Result", Result);
                    }

                    void OpenMarket::RequestProductList::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"ProductInfos", ProductInfos);
                        in.ReadEnum(L"OrderType", OrderType);
                        in.Read(L"CurrentPage", CurrentPage);
                        in.Read(L"TotalPage", TotalPage);
                        in.ReadEnum(L"Result", Result);
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
                        out.Write(L"AveragePrice", AveragePrice);
                        out.Write(L"ReliableCount", ReliableCount);
                        out.WriteEnum(L"Result", Result);
                    }

                    void OpenMarket::RequestProductInfo::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"RegistrationNumber", RegistrationNumber);
                        in.Read(L"AveragePrice", AveragePrice);
                        in.Read(L"ReliableCount", ReliableCount);
                        in.ReadEnum(L"Result", Result);
                    }

                    // RequestItemInfo
                    const wchar_t* OpenMarket::RequestItemInfo::TypeName =
                        L"OpenMarket::RequestItemInfo";
                    const HashType OpenMarket::RequestItemInfo::TypeHash =
                        StringUtil::Hash(OpenMarket::RequestItemInfo::TypeName);
                    void OpenMarket::RequestItemInfo::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"AveragePrice", AveragePrice);
                        out.Write(L"ReliableCount", ReliableCount);
                        out.WriteEnum(L"Result", Result);
                    }

                    void OpenMarket::RequestItemInfo::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"AveragePrice", AveragePrice);
                        in.Read(L"ReliableCount", ReliableCount);
                        in.ReadEnum(L"Result", Result);
                    }

                    // BuyProduct
                    const wchar_t* OpenMarket::BuyProduct::TypeName = L"OpenMarket::BuyProduct";
                    const HashType OpenMarket::BuyProduct::TypeHash =
                        StringUtil::Hash(OpenMarket::BuyProduct::TypeName);
                    void OpenMarket::BuyProduct::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                    }

                    void OpenMarket::BuyProduct::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                    }

                    // RegisterItem
                    const wchar_t* OpenMarket::RegisterItem::TypeName = L"OpenMarket::RegisterItem";
                    const HashType OpenMarket::RegisterItem::TypeHash =
                        StringUtil::Hash(OpenMarket::RegisterItem::TypeName);
                    void OpenMarket::RegisterItem::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"RegistrationNumber", RegistrationNumber);
                        out.WriteEnum(L"Result", Result);
                    }

                    void OpenMarket::RegisterItem::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"RegistrationNumber", RegistrationNumber);
                        in.ReadEnum(L"Result", Result);
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
                        out.WriteEnum(L"Result", Result);
                    }

                    void OpenMarket::RegistrationCancel::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"RegistrationNumber", RegistrationNumber);
                        in.ReadEnum(L"Result", Result);
                    }

                    // MyProductList
                    const wchar_t* OpenMarket::MyProductList::TypeName =
                        L"OpenMarket::MyProductList";
                    const HashType OpenMarket::MyProductList::TypeHash =
                        StringUtil::Hash(OpenMarket::MyProductList::TypeName);
                    void OpenMarket::MyProductList::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"ProductInfos", ProductInfos);
                        out.WriteEnum(L"Result", Result);
                    }
                    void OpenMarket::MyProductList::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"ProductInfos", ProductInfos);
                        in.ReadEnum(L"Result", Result);
                    }

                    const wchar_t* ChangeWeather::TypeName = L"ChangeWeather";
                    const HashType ChangeWeather::TypeHash =
                        StringUtil::Hash(ChangeWeather::TypeName);
                    void ChangeWeather::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Type", type);
                        out.Write(L"FadeInTime", fadeInTime);
                    }
                    void ChangeWeather::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Type", type);
                        in.Read(L"FadeInTime", fadeInTime);
                    }

                    const wchar_t* PlayTimeEventInfo::TypeName = L"PlayTimeEventInfo";
                    const HashType PlayTimeEventInfo::TypeHash =
                        StringUtil::Hash(PlayTimeEventInfo::TypeName);
                    void PlayTimeEventInfo::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"CurrentStackedTodayPlayTime", currentStackedTodayPlayTime);
                        out.Write(L"PlayTimeEventRewards", playTimeEventRewards);
                        out.Write(L"LevelUpEventRewards", levelUpEventRewards);
                    }
                    void PlayTimeEventInfo::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"CurrentStackedTodayPlayTime", currentStackedTodayPlayTime);
                        in.Read(L"PlayTimeEventRewards", playTimeEventRewards,
                            PlayTimeEventRewards());
                        in.Read(L"LevelUpEventRewards", levelUpEventRewards, LevelUpEventRewards());
                    }

                    const wchar_t* CashShop::CashAmount::TypeName = L"CashShop::CashAmount";
                    const HashType CashShop::CashAmount::TypeHash =
                        StringUtil::Hash(CashShop::CashAmount::TypeName);
                    void CashShop::CashAmount::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", result);
                        out.Write(L"CashAmount", cashAmount);
                        out.Write(L"PointAmount", pointAmount);  // 3.1 by Robotex
                    }
                    void CashShop::CashAmount::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", result);
                        in.Read(L"CashAmount", cashAmount);
                        in.Read(L"PointAmount", pointAmount);  // 3.1 by Robotex
                    }

                    const wchar_t* CashShop::PurchaseResponse::TypeName =
                        L"CashShop::PurchaseResponse";
                    const HashType CashShop::PurchaseResponse::TypeHash =
                        StringUtil::Hash(CashShop::PurchaseResponse::TypeName);
                    void CashShop::PurchaseResponse::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", result);
                        out.Write(L"availableLevelRate", availableLevelRate);  // 3.1 by ultimate
                    }
                    void CashShop::PurchaseResponse::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", result);
                        in.Read(L"availableLevelRate", availableLevelRate);  // 3.1 by ultimate
                    }

                    const wchar_t* CashShop::PresentResponse::TypeName =
                        L"CashShop::PresentResponse";
                    const HashType CashShop::PresentResponse::TypeHash =
                        StringUtil::Hash(CashShop::PresentResponse::TypeName);
                    void CashShop::PresentResponse::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", result);
                        out.Write(L"availableLevelRate", availableLevelRate);  // 3.1 by ultimate
                    }
                    void CashShop::PresentResponse::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", result);
                        in.Read(L"availableLevelRate", availableLevelRate);  // 3.1 by ultimate
                    }

                    const wchar_t* CashShop::RegistCouponResponse::TypeName =
                        L"CashShop::RegistCouponResponse";
                    const HashType CashShop::RegistCouponResponse::TypeHash =
                        StringUtil::Hash(CashShop::RegistCouponResponse::TypeName);
                    void CashShop::RegistCouponResponse::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", result);
                    }
                    void CashShop::RegistCouponResponse::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", result);
                    }

                    const wchar_t* AchievementServerAssigned::TypeName =
                        L"AchievementServerAssigned";
                    const HashType AchievementServerAssigned::TypeHash =
                        StringUtil::Hash(AchievementServerAssigned::TypeName);
                    void AchievementServerAssigned::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.WriteEnum(L"Result", Result);
                        out.Write(L"serverName", serverName);
                        out.Write(L"serverAddress", serverAddress);
                    }
                    void AchievementServerAssigned::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.ReadEnum(L"Result", Result);
                        in.Read(L"serverName", serverName);
                        in.Read(L"serverAddress", serverAddress);
                    }

                    const wchar_t* AchievementList::TypeName =
                        L"AchievementList";
                    const HashType AchievementList::TypeHash =
                        StringUtil::Hash(AchievementList::TypeName);
                    void AchievementList::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"characterName", characterName);
                        out.Write(L"mainCategory", mainCategory);
                        out.Write(L"subCategory", subCategory);
                        //out.Write(L"completedList", completedList);
                        out.Write(L"totalScore", totalScore);
                        out.WriteEnum(L"Result", Result);
                    }
                    void AchievementList::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"characterName", characterName);
                        in.Read(L"mainCategory", mainCategory);
                        in.Read(L"subCategory", subCategory);
                        //in.Read(L"completedList", completedList);
                        in.Read(L"totalScore", totalScore);
                        in.ReadEnum(L"Result", Result);
                    }

                    // this working? i need debug 3.1 client
                    // this is called on change this status, probably in User.cpp :o
                    const wchar_t* ChangeStatusLimit::TypeName = L"ChangeStatusLimit";
                    const HashType ChangeStatusLimit::TypeHash = StringUtil::Hash(ChangeStatusLimit::TypeName);
                    void ChangeStatusLimit::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"statusLimits", statusLimit);
                        /*unsigned char teste[] = {0x8E, 0xCE, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x39, 0x04, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x39, 0x04, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x39, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x39, 0x04, 0x00, 0x00};
                        out.WriteArray(L"statusLimits",teste, sizeof(teste) );*/
                    }

                    void ChangeStatusLimit::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"statusLimits", statusLimit);
                    }

                    // this is called on change this status, probably in User.cpp :o
                    const wchar_t* ChangeStatusRate::TypeName = L"ChangeStatusRate";
                    const HashType ChangeStatusRate::TypeHash = StringUtil::Hash(ChangeStatusRate::TypeName);
                    void ChangeStatusRate::Serialize(Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"statusRates", statusRates);
                    }

                    void ChangeStatusRate::Deserialize(Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"statusRates", statusRates);
                    }


                    /* const wchar_t* ChangeStatusLimit::StatusLimits::TypeName =
                        L"statusLimits";
                    const HashType ChangeStatusLimit::StatusLimits::TypeHash =
                        StringUtil::Hash(ChangeStatusLimit::StatusLimits::TypeName);
                    void ChangeStatusLimit::StatusLimits::Serialize(
                        Serializer::IStreamWriter& out) const {
                        out.Begin(TypeName);
                        out.Write(L"status", status);
                        out.Write(L"limit", limit);
                        printf("StatusLimit Serializer\n");
                    }
                    void ChangeStatusLimit::StatusLimits::Deserialize(
                        Serializer::IStreamReader& in) {
                        in.Begin(TypeName);
                        in.Read(L"status", status);
                        in.Read(L"limit", limit);
                    } */

                }  // namespace FromServer

			}
		}
	}
}