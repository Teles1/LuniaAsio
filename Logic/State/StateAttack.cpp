#include "StateAttack.h"
#include "StateKnockBack.h"
#include "StateCriAtk.h"
#include "StateCriMagic.h"
#include "StateReflection.h"
#include "StateDevineShd.h"
#include "StateIgnoreCriticalHit.h"
#include <iostream>
#include <algorithm>
#include "../Action/ActionDown.h"
#include "../Action/ActionStun.h"
#include "../Action/ActionStepBack.h"
#include "../Action/ActionPhoenixDownAir.h"
#include "../Action/ActionPhoenixDown2Air.h"
#include "../Action/ActionCenterMove.h"
#include "../Action/ActionFaint.h"
#include "../Object/Projectile.h"
#include "../Object/NonPlayer.h"
#include "../GameState.h"
#include <Core/Utils/Math/PseudoRandom.h>

#include <Info/Info/ETC/Balance.h>
#include <minmax.h>

using namespace Lunia::XRated::Logic::Action;
using namespace Lunia::Math;
using namespace Lunia::XRated::Database::Info;


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

    Attack::Attack(Object* owner, StatConstants::AttackEffectType::Type et, int tt, Constants::DamageType dmgType, Constants::AllianceType dt, float3 pos, float ar,
                    bool bAbDmg, float mn, float mx, float kb, float akb, float sl, AttackDir ad, float amf, float mf, float stop, float hm, BackstabInfo backstab)
    :    State<Object>(owner, StateInfo::Type::ATTACK, StateInfo::Sort::ORDINARYSTATE), bAbsoluteDmg(bAbDmg), startPos(pos), minimum(mn), maximum(mx),
        effectType(et), targetType(tt), dmgTo(dt), attackRange(ar), knockBack(kb), airKnockBack(akb), stunLength(sl), attackDir(ad), makeFly(mf),
        airMakeFly(amf), stopAnimation(stop), damageType(dmgType), heightMax(hm), backstabInfo(backstab)
    {
        spinnedPos[0] = PhysicalFunctions::RotatePosition(pos, 315);
        spinnedPos[1].SetVector(pos.z, 0, -pos.x);
        spinnedPos[2] = PhysicalFunctions::RotatePosition(pos, 225);
        spinnedPos[3].SetVector(-pos.x, 0, -pos.z);
        spinnedPos[4] = PhysicalFunctions::RotatePosition(pos, 135);
        spinnedPos[5].SetVector(-pos.z, 0, pos.x);
        spinnedPos[6] = PhysicalFunctions::RotatePosition(pos, 45);
    }

    void Attack::Initialize(IGameStateDatabase* db)
    {
        if ( !targetType )
            targetType = Constants::AttackTargetType::Stand;

        Constants::ObjectType type = owner->GetType();
        if ( !bAbsoluteDmg ) {
            float min=0, max=0;
            switch( type ) {
            case Constants::ObjectType::Projectile :
                ((Projectile*)owner)->GetDamage(min, max);
                break;
            case Constants::ObjectType::Player :
            case Constants::ObjectType::NonPlayer :
            case Constants::ObjectType::Misc :
                ((Actor*)owner)->GetDamage(min, max);
                break;
            }

            maximum = max * minimum;
            minimum = min * minimum;
        }

        switch(damageType) {
        case Constants::DamageType::PHYSICAL :
            {
                float incFactor = 1.0f;
                switch( type ) 
                {
                case Constants::ObjectType::NonPlayer :
                    {
                        incFactor += incFactor*((NonPlayer*)owner)->GetAtkMultiply();
                    }

                    minimum *= incFactor;
                    maximum *= incFactor;
                    break;
                }
            }
            break;
        default :
            {
                float incFactor = 1.0f;
                switch( type ) {
                case Constants::ObjectType::Projectile :
                    {
                        incFactor = ((Projectile*)owner)->GetIncFactor();

                        if (bAbsoluteDmg)
                        {
                            Player* creator = db->RetrievePlayer(static_cast<Projectile*>(owner)->GetCreatorSerial());
                            if(creator != NULL)
                            {
                                float min=0, max=0;
                                creator->GetPlusDamage(min, max);
                                
                                if (db->GetGameType() & XRated::Constants::GameTypes::PvpGameTypeMask)
                                    incFactor += StatusFunctions::GetIncreasedDamageRateByAtkInPvp(min + max);
                                else
                                    incFactor += StatusFunctions::GetIncreasedDamageRateByAtk(min + max);
                            }

                            minimum *= incFactor;
                            maximum *= incFactor;
                        }
                        else
                        {
                            Player* creator = db->RetrievePlayer(static_cast<Projectile*>(owner)->GetCreatorSerial());
                            if(creator != NULL)
                            {
                                if (maximum > 9000) 
                                {
                                    float maximumGap = maximum - 9000;
                                    maximum = 9000;
                                    minimum -= maximumGap;
                                }
                            }

                            minimum *= incFactor;
                            maximum *= incFactor;
                        }
                    }
                    break;
                case Constants::ObjectType::Player :
                    {
                        int str, dummy;
                        ((Actor*)owner)->GetBaseStat(str, dummy, dummy, dummy);                        
                        incFactor = ( ( Actor* )owner )->GetSkillDamageIncreaseFactorByStat();
                        if (bAbsoluteDmg)
                        {
                            float min=0, max=0;
                            ((Actor*)owner)->GetPlusDamage(min, max);
                            if (db->GetGameType() & XRated::Constants::GameTypes::PvpGameTypeMask)
                                incFactor += StatusFunctions::GetIncreasedDamageRateByAtkInPvp(min + max);
                            else
                                incFactor += StatusFunctions::GetIncreasedDamageRateByAtk(min + max);

                            minimum *= incFactor;
                            maximum *= incFactor;
                        }
                        else
                        {
                            if (maximum > 9000) 
                            {
                                float maximumGap = maximum - 9000;
                                maximum = 9000;
                                minimum -= maximumGap;
                                
                            }

                            minimum *= incFactor;
                            maximum *= incFactor;
                        }
                    }
                    break;
                case Constants::ObjectType::NonPlayer :
                    {
                        int str, dummy;
                        ((Actor*)owner)->GetBaseStat(str, dummy, dummy, dummy);                        
                        incFactor = ((NonPlayer*)owner)->GetIncreasedDamageRate(str);
                        incFactor += incFactor*((NonPlayer*)owner)->GetAtkMultiply();

                        minimum *= incFactor;
                        maximum *= incFactor;
                    }
                    break;
                case Constants::ObjectType::Misc :
                    {
                        int str, dummy;
                        ((Actor*)owner)->GetBaseStat(str, dummy, dummy, dummy);                        
                        incFactor = StatusFunctions::GetIncreasedDamageRateForNonPlayer(str);

                        minimum *= incFactor;
                        maximum *= incFactor;
                    }
                    break;
                }
            }
        }

        minimum += owner->CalculrateIncDamage(damageType, minimum);
        maximum += owner->CalculrateIncDamage(damageType, maximum);

        direction = owner->GetDirection();
        ownerPosition = owner->GetPosition();
        if ( owner->CheckState(StateInfo::Type::DEVINESHD) ) {
            IState* s = owner->GetState(StateInfo::Type::DEVINESHD, StateInfo::Sort::ORDINARYSTATE);
            if (s) ((DevineShd*)s)->Remove();
        }
    }

    void Attack::SetDownAtkFlag()
    {
        attackType = Attack::AttackType::DownAtk;
    }

    bool AttackSegment::Do(float, IGameStateDatabase* db)
    {
        std::vector<Object*> l;

        if (direction == Lunia::XRated::Constants::DirectionF::RightUp)
            db->GetAreaObject(ownerPosition+spinnedPos[0], direction, attackRange, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::Right)
            db->GetAreaObject(ownerPosition+spinnedPos[1], direction, attackRange, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::RightDown)
            db->GetAreaObject(ownerPosition+spinnedPos[2], direction, attackRange, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::Down)
            db->GetAreaObject(ownerPosition+spinnedPos[3], direction, attackRange, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::LeftDown)
            db->GetAreaObject(ownerPosition+spinnedPos[4], direction, attackRange, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::Left)
            db->GetAreaObject(ownerPosition+spinnedPos[5], direction, attackRange, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::LeftUp)
            db->GetAreaObject(ownerPosition+spinnedPos[6], direction, attackRange, dmgTo, owner->GetTeam(), owner, l);
        else
            db->GetAreaObject(ownerPosition+startPos, direction, attackRange, dmgTo, owner->GetTeam(), owner, l);


        Attack::Process(l, db);
        return true;
    }

    bool AttackCircle::Do(float, IGameStateDatabase* db)
    {
        std::vector<Object*> l;

        if (direction == Lunia::XRated::Constants::DirectionF::RightUp)
            db->GetAreaObject(ownerPosition+spinnedPos[0], attackRange, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::Right)
            db->GetAreaObject(ownerPosition+spinnedPos[1], attackRange, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::RightDown)
            db->GetAreaObject(ownerPosition+spinnedPos[2], attackRange, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::Down)
            db->GetAreaObject(ownerPosition+spinnedPos[3], attackRange, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::LeftDown)
            db->GetAreaObject(ownerPosition+spinnedPos[4], attackRange, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::Left)
            db->GetAreaObject(ownerPosition+spinnedPos[5], attackRange, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::LeftUp)
            db->GetAreaObject(ownerPosition+spinnedPos[6], attackRange, dmgTo, owner->GetTeam(), owner, l);
        else
            db->GetAreaObject(ownerPosition+startPos, attackRange, dmgTo, owner->GetTeam(), owner, l);

        Attack::Process(l, db);
        return true;
    }

    bool AttackFanshape::Do(float, IGameStateDatabase* db)
    {
        std::vector<Object*> l;

        if (direction == Lunia::XRated::Constants::DirectionF::RightUp)
            db->GetAreaObject(ownerPosition+spinnedPos[0], direction, attackRange, startAngle, endAngle, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::Right)
            db->GetAreaObject(ownerPosition+spinnedPos[1], direction, attackRange, startAngle, endAngle, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::RightDown)
            db->GetAreaObject(ownerPosition+spinnedPos[2], direction, attackRange, startAngle, endAngle, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::Down)
            db->GetAreaObject(ownerPosition+spinnedPos[3], direction, attackRange, startAngle, endAngle, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::LeftDown)
            db->GetAreaObject(ownerPosition+spinnedPos[4], direction, attackRange, startAngle, endAngle, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::Left)
            db->GetAreaObject(ownerPosition+spinnedPos[5], direction, attackRange, startAngle, endAngle, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::LeftUp)
            db->GetAreaObject(ownerPosition+spinnedPos[6], direction, attackRange, startAngle, endAngle, dmgTo, owner->GetTeam(), owner, l);
        else
            db->GetAreaObject(ownerPosition+startPos, direction, attackRange, startAngle, endAngle, dmgTo, owner->GetTeam(), owner, l);

        Attack::Process(l, db);
        return true;
    }

    AttackRectangle::AttackRectangle(Object* owner, StatConstants::AttackEffectType::Type effectType, int targetType, Constants::DamageType dmgType, Constants::AllianceType dmgTo,
                float3 pos, float attackRange, bool bAbDmg, float mn, float mx, float knockBack, float airKnockBack,
                float stunLength, AttackDir attackDir, float width, float amf, float mf, float stop, float heightMax, BackstabInfo backstab)
            : Attack(owner, effectType, targetType, dmgType, dmgTo, pos, attackRange, bAbDmg, mn, mx, knockBack, airKnockBack, stunLength, attackDir, amf, mf, stop, heightMax, backstab)
            , attackWidth(width)
    {
        type = Attack::Type::Rectangle;
        attackType = Attack::AttackType::NormalAtk;
    }

    bool AttackRectangle::Do(float, IGameStateDatabase* db)
    {
        std::vector<Object*> l;

        if (direction == Lunia::XRated::Constants::DirectionF::RightUp)
            db->GetAreaObject(ownerPosition+spinnedPos[0], direction, attackRange, attackWidth, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::Right)
            db->GetAreaObject(ownerPosition+spinnedPos[1], direction, attackRange, attackWidth, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::RightDown)
            db->GetAreaObject(ownerPosition+spinnedPos[2], direction, attackRange, attackWidth, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::Down)
            db->GetAreaObject(ownerPosition+spinnedPos[3], direction, attackRange, attackWidth, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::LeftDown)
            db->GetAreaObject(ownerPosition+spinnedPos[4], direction, attackRange, attackWidth, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::Left)
            db->GetAreaObject(ownerPosition+spinnedPos[5], direction, attackRange, attackWidth, dmgTo, owner->GetTeam(), owner, l);
        else if (direction == Lunia::XRated::Constants::DirectionF::LeftUp)
            db->GetAreaObject(ownerPosition+spinnedPos[6], direction, attackRange, attackWidth, dmgTo, owner->GetTeam(), owner, l);
        else
            db->GetAreaObject(ownerPosition+startPos, direction, attackRange, attackWidth, dmgTo, owner->GetTeam(), owner, l);
        Attack::Process(l, db);
        return true;
    }

    void Attack::Process(std::vector<Object*>& l, IGameStateDatabase* db)
    {
        std::vector<Object*>::iterator i;
        float damage, maxdamage, mindamage;

        maxdamage = maximum;
        mindamage = minimum;

        if(db->GetStageGroupInfo()->GameType & XRated::Constants::GameTypes::PvpGameTypeMask)
        {
            if(owner->GetType() == XRated::Constants::ObjectType::Player && static_cast<Player*>(owner)->GetCharacterStateFlags().IsBalanced)
            {
                mindamage *= Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(static_cast<Actor*>(owner)->GetClass()), Database::BalancerInstance().DamageFactor, static_cast<Actor*>(owner)->GetLevel());
                maxdamage *= Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(static_cast<Actor*>(owner)->GetClass()), Database::BalancerInstance().DamageFactor, static_cast<Actor*>(owner)->GetLevel());
            }
            else if(owner->GetType() == XRated::Constants::ObjectType::Projectile)
            {
                Player* creator = db->RetrievePlayer(static_cast<XRated::Logic::Projectile*>(owner)->GetCreatorSerial());

                if(creator != NULL && creator->GetCharacterStateFlags().IsBalanced)
                {
                    mindamage *= Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(creator->GetClass()), Database::BalancerInstance().DamageFactor, creator->GetLevel());
                    maxdamage *= Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(creator->GetClass()), Database::BalancerInstance().DamageFactor, creator->GetLevel());
                }
            }
            else    if(owner->GetType() == XRated::Constants::ObjectType::NonPlayer)
            {
                Player* creator = db->RetrievePlayer(static_cast<XRated::Logic::NonPlayer*>(owner)->GetOwnerSerial());

                if(creator != NULL && creator->GetCharacterStateFlags().IsBalanced)
                {
                    mindamage *= Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(creator->GetClass()), Database::BalancerInstance().DamageFactor, creator->GetLevel());
                    maxdamage *= Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(creator->GetClass()), Database::BalancerInstance().DamageFactor, creator->GetLevel());
                }
            }
        }
        
        float dmgRange = maxdamage - mindamage;
        for (i = l.begin() ; i != l.end() ; ++i)
        {
            Actor* actor = (Actor*)(*i);
            if (actor == owner ||
                actor->CheckState(Database::Info::StateInfo::Type::INVINCIBLE) ||
                actor->CheckState(Database::Info::StateInfo::Type::GHOST) || actor->GetHeightInfo().height > heightMax)
                    continue;

            int type = Constants::AttackTargetType::Stand;
            if ( actor->CheckState(Database::Info::StateInfo::Type::FLIED) )
                type = Constants::AttackTargetType::Air;
            else if ( actor->CheckState(Database::Info::StateInfo::Type::DOWN) )
                type = Constants::AttackTargetType::Ground;

            if ( !(targetType & type) ) continue;

            switch( type ) 
            { 
            case Constants::AttackTargetType::Ground :
                if ( actor->GetHP() <= 0 )
                {
                    continue;
                }
            }

            damage = mindamage + (db->GetRandomFloat())*dmgRange;

            if ( actor->CheckState(StateInfo::Type::REFLECTION ) )
            {
                if ( owner->IsCharacter() ) 
                {
                    const Stat::IState *refl = actor->GetState(StateInfo::Type::REFLECTION, StateInfo::Sort::DEFENCESTATE);
                    damage = ((Stat::Reflection*)refl)->GetDamage();

                    unsigned int dummy = 0;
                    ((Actor*)owner)->DoDamage(owner, 0, damage, damage, damage, damageType, GetId(), Constants::AttackStateType::NONE, dummy );

                    ActionStepBack::Param param(0.2f, direction);
                    ((Actor*)owner)->ReserveAction( Database::Info::HashTable::Actions::MovingKnockBack, &param, true);
                    continue;
                }
            }

            if ( IsGuard(actor) ){
                continue;
            }

            unsigned int sFlag = 0;

            bool isHitIgnored = actor->IsHitEffectIgnored();
            float crit_multiplier = 1.0f; //@Oracle - Custom to get crit damage applied.

            if ( isHitIgnored )
            {
                sFlag |= Constants::StatusFlag::HitIgnore;
            }

            if(actor->IsIgnoreCriHit(db) == false)
            {
                switch(damageType)
                {
                case Constants::DamageType::PHYSICAL:
                case Constants::DamageType::INDEPENDENCE:
                    if(owner->IsCharacter() == true)
                    {
                        if(static_cast<Actor*>(owner)->IsCriAtk(db))
                        {
                            damage *= static_cast<Actor*>(owner)->GetPhysicalCriticalInc();
                            crit_multiplier = static_cast<Actor*>(owner)->GetPhysicalCriticalInc();
                            sFlag |= Constants::StatusFlag::Critical;
                        }
                    }
                    else
                    {
                        if(owner->GetType() == XRated::Constants::ObjectType::Projectile)
                        {
                            Actor* creator =  db->RetrieveActor(static_cast<Projectile*>(owner)->GetCreatorSerial());
                            if(creator != NULL && creator->IsCriAtk(db))
                            {
                                damage *= creator->GetPhysicalCriticalInc();
                                crit_multiplier = creator->GetPhysicalCriticalInc();
                                sFlag |= Constants::StatusFlag::Critical;
                            }
                        }
                    }
                break;
                default:
                    if(owner->IsCharacter() == true)
                    {
                        if( static_cast<Actor*>(owner)->IsCriMagic(db))
                        {
                            damage *= static_cast<Actor*>(owner)->GetMagicalCriticalInc();
                            crit_multiplier = static_cast<Actor*>(owner)->GetMagicalCriticalInc();
                            sFlag |= Constants::StatusFlag::CriticalMagic;
                        }
                    }
                    else
                    {
                        if(owner->GetType() == XRated::Constants::ObjectType::Projectile)
                        {
                            Actor* creator =  db->RetrieveActor(static_cast<Projectile*>(owner)->GetCreatorSerial());
                            if(creator != NULL && creator->IsCriMagic(db))
                            {
                                damage *= creator->GetMagicalCriticalInc();    ///by hak (09. 11. 11) 
                                crit_multiplier = creator->GetMagicalCriticalInc();
                                sFlag |= Constants::StatusFlag::CriticalMagic;
                            }
                        }
                    }
                break;
                }
            }

            bool isHpleft = actor->GetCharacterData().Hp > 0.0f ? true : false;

            bool is_morphed = static_cast<Actor*>(owner)->CheckState(Database::Info::StateInfo::Type::MORPH);
            if(is_morphed){
                int actor_str, actor_dex, actor_int, actor_vit;
                static_cast<Actor*>(owner)->GetBaseStat(actor_str, actor_dex, actor_int, actor_vit);
                float base_damage = damage / crit_multiplier;
                float morph_crit_multiplier = (float)(crit_multiplier / 3.0f);
                morph_crit_multiplier = max(1.0f, morph_crit_multiplier);
                float transform_factor = 1.0f - (float)((actor_str - 3000.00f) * 0.00001f);
                transform_factor = min(1.0f, transform_factor);
                transform_factor = max(transform_factor, 0.25f);
                damage = (base_damage * transform_factor) * morph_crit_multiplier;
            }

            if ( !actor->DoDamage(owner, 0, mindamage, maxdamage, damage, damageType, GetId(), Constants::AttackStateType::ATTACK, sFlag) ){
                continue;
            }

            if ( owner->IsCharacter() ) 
            {
                ((Actor*)owner)->HitOthers();
                ((Actor*)owner)->ProcessStateWhenHit(actor, damage, sFlag);

                actor->ProcessStateWhenHitted((Actor*)owner, damage, sFlag);
            } 
            else 
            {
                actor->ProcessStateWhenHitted(NULL, damage, sFlag);
            }

            owner->PauseAction(stopAnimation);

            if ( backstabInfo.bBackstab && PhysicalFunctions::IsBehindTarget(owner->GetDirection(), actor->GetDirection()) )
            {
                if ( backstabInfo.backstabDmg )
                {
                    actor->DoDamage(owner, 0, damage * backstabInfo.backstabDmg, damage * backstabInfo.backstabDmg, damage * backstabInfo.backstabDmg, damageType, GetId(), Constants::AttackStateType::NONE, sFlag);
                    
                    db->StyplePointType(owner->GetSerial(), XRated::Constants::StylePointType::BackStabHit);
                }
                if ( backstabInfo.bStun && type == Constants::AttackTargetType::Stand ) 
                {
                    effectType = StatConstants::AttackEffectType::STANDFAINT;
                    knockBack = backstabInfo.stunLength;
                }
            }

            if ( false == ( sFlag & Constants::StatusFlag::HitIgnore ) ){
                switch ( effectType ) {
                    case StatConstants::AttackEffectType::STUN:
                        if ( actor->IsIgnoreStun( StatConstants::AttackEffectType::StunFlag ) ){ break; }
                    
                        switch( type ) {
                            case Constants::AttackTargetType::Ground : 
                                actor->SetAction( Database::Info::HashTable::Actions::HitDown, NULL, true);
                            break;
                            case Constants::AttackTargetType::Air :
                                HitAir(actor);
                            break;
                            case Constants::AttackTargetType::Stand :
                                Stun(actor);
                            break;
                        }
                    break;
                    case StatConstants::AttackEffectType::PHOENIXDOWN:
                        if ( actor->IsIgnoreStun(StatConstants::AttackEffectType::PhoenixDownFlag) ){ break; }
                        PhoenixDown(actor);
                    break;
                    case StatConstants::AttackEffectType::PHOENIXDOWN2:
                        if(( actor->IsIgnoreStun(StatConstants::AttackEffectType::AirStunFlag) ) || ( actor->IsIgnoreStun(StatConstants::AttackEffectType::StunFlag) )){
                            PhoenixDown2(actor);
                        } else {
                            PhoenixDown(actor);    
                        }
                    break;
					case StatConstants::AttackEffectType::CENTERMOVE: //TODO
                        CenterMove(actor, owner);
                    break;
                    case StatConstants::AttackEffectType::AIRSTUN :
                        if ( actor->IsIgnoreStun(StatConstants::AttackEffectType::AirStunFlag) ){
                            break;
                        }
                        switch( type ) {
                            case Constants::AttackTargetType::Ground: 
                            case Constants::AttackTargetType::Stand:
                                AirStun(actor);
                            break;
                            case Constants::AttackTargetType::Air:
                                HitAir(actor);
                            break;
                        }
                    break;
                    case StatConstants::AttackEffectType::DOWN:
                        if ( actor->IsIgnoreStun(StatConstants::AttackEffectType::DownFlag) ){ break; }
                        switch( type ) {
                            case Constants::AttackTargetType::Ground : 
                            case Constants::AttackTargetType::Stand :
                                this->Down(actor);
                            break;
                            case Constants::AttackTargetType::Air :
                                this->HitAir(actor);
                            break;
                        }
                    break;
                    case StatConstants::AttackEffectType::FAINT :
                        if ( actor->IsIgnoreStun(StatConstants::AttackEffectType::FaintFlag) ){ break; }
                        switch( type ) {
                            case Constants::AttackTargetType::Ground : 
                            case Constants::AttackTargetType::Stand :
                                this->Faint(actor);
                            break;
                            case Constants::AttackTargetType::Air :
                                this->HitAir(actor);
                            break;
                        }
                    break;
                    case StatConstants::AttackEffectType::STEPBACK :
                        if ( actor->IsIgnoreStun(StatConstants::AttackEffectType::StepBackFlag) ){ break; }
                        switch( type ) {
                            case Constants::AttackTargetType::Ground : 
                            case Constants::AttackTargetType::Stand :
                                StepBack(actor);
                            break;
                            case Constants::AttackTargetType::Air :
                                HitAir(actor);
                            break;
                        }
                    break;
                    case StatConstants::AttackEffectType::STANDFAINT :
                        if ( actor->IsIgnoreStun(StatConstants::AttackEffectType::StandFaintFlag) ){ break; }
                        switch( type ) {
                            case Constants::AttackTargetType::Ground : 
                            case Constants::AttackTargetType::Stand :
                                StandFaint(actor);
                            break;
                            case Constants::AttackTargetType::Air :
                                HitAir(actor);
                            break;
                        }
                    break;
                }; //switch ( effectType ) 
            } else { //if not ( false == ( sFlag & Constants::StatusFlag::HitIgnore ) )
                if ( effectType == StatConstants::AttackEffectType::PHOENIXDOWN2){
                    PhoenixDown2(actor);
                } else if ( effectType == StatConstants::AttackEffectType::CENTERMOVE){
                    CenterMove(actor, owner);
                }
            }

            if(db->GetStageGroupInfo()->GameType & XRated::Constants::GameTypes::PvpGameTypeMask    &&
                actor->GetType() == XRated::Constants::ObjectType::Player    &&
                actor->GetCharacterData().Hp <= 0.0f )
            {
                if(
                    ((actor->GetActionNameHash() == Database::Info::HashTable::Actions::HitAir || actor->GetActionNameHash() == 40868073) && isHpleft == true) ||
                    ( actor->GetActionNameHash() == Database::Info::HashTable::Actions::PhoenixDownAir)  ||
                    ( actor->GetActionNameHash() == Database::Info::HashTable::Actions::PhoenixDown2Air) || //@oracle 01JAN01 2020 13:33 EST
                    ( actor->GetActionNameHash() == Database::Info::HashTable::Actions::DownStart)       ||
                    ( actor->GetActionNameHash() == Database::Info::HashTable::Actions::FaintDownStart)
                ){
                    if(owner->GetType() == XRated::Constants::ObjectType::Projectile)
                    {
                        Player* creator = db->RetrievePlayer(static_cast<XRated::Logic::Projectile*>(owner)->GetCreatorSerial());

                        if(creator != NULL)
                        {
                            Logger::GetInstance().Warn(L"[Attack::Process] objectSerial {0} fromSerial(projectile) {1}", actor->GetSerial(), static_cast<XRated::Logic::Projectile*>(owner)->GetCreatorSerial());
                            db->PlayerDie(actor->GetSerial(), static_cast<XRated::Logic::Projectile*>(owner)->GetCreatorSerial());
                        }
                        else
                        {
                            NonPlayer* npc = db->RetrieveNPC(static_cast<XRated::Logic::Projectile*>(owner)->GetCreatorSerial());

                            if(npc != NULL)
                            {
                                Logger::GetInstance().Warn(L"[Attack::Process] objectSerial {0} fromSerial(npc->projectile) {1}", actor->GetSerial(), npc->GetOwnerSerial());
                                db->PlayerDie(actor->GetSerial(), npc->GetOwnerSerial());
                            }
                            else
                            {
                                Logger::GetInstance().Warn(L"[Attack::Process] objectSerial {} fromSerial(Unknown) {}", actor->GetSerial(), static_cast<XRated::Logic::Projectile*>(owner)->GetCreatorSerial());
                            }
                        }
                    }
                    else    if(owner->GetType() == XRated::Constants::ObjectType::NonPlayer)
                    {
                        Logger::GetInstance().Warn(L"[Attack::Process] objectSerial {0} fromSerial(nonplayer) {1}", actor->GetSerial(), static_cast<XRated::Logic::NonPlayer*>(owner)->GetOwnerSerial());
                        db->PlayerDie(actor->GetSerial(), static_cast<XRated::Logic::NonPlayer*>(owner)->GetOwnerSerial());
                    }
                    else
                    {
                        Logger::GetInstance().Warn(L"[Attack::Process] objectSerial {0} fromSerial {1}", actor->GetSerial(), owner->GetSerial());
                        db->PlayerDie(actor->GetSerial(), owner->GetSerial());
                    }    
                }
            }
        }
    }

    bool Attack::IsGuard(Actor* actor)
    {
        if(effectType == StatConstants::AttackEffectType::Type::PHOENIXDOWN2){
            return false;
        }

        if ( owner->GetType() == Constants::ObjectType::Projectile ) {
            if ( actor->IsGuarding( StatConstants::AttackEffectType::Type::PROJECTILE ) ) {
                if ( actor->IsGuardActivated( StatConstants::AttackEffectType::Type::PROJECTILE ) ) {
                    actor->SetAction( Database::Info::HashTable::Actions::GuardProjectile, NULL, true);
                    return true;
                }
            }
        } else if ( actor->IsGuarding( (int)effectType ) ) {
            if ( actor->IsGuardActivated( (int)effectType ) ) {
                switch (effectType) {
                    case StatConstants::AttackEffectType::Type::AIRSTUN :
                        actor->SetAction( Database::Info::HashTable::Actions::GuardAirStun, NULL, true);
                    break;
                    case StatConstants::AttackEffectType::Type::DOWN :
                        actor->SetAction( Database::Info::HashTable::Actions::GuardDown, NULL, true);
                    break;
                    case StatConstants::AttackEffectType::Type::FAINT :
                        actor->SetAction( Database::Info::HashTable::Actions::GuardFaint, NULL, true);
                    break;
                    case StatConstants::AttackEffectType::Type::NOSTUN :
                        actor->SetAction( Database::Info::HashTable::Actions::GuardNoStun, NULL, true);
                    break;
                    case StatConstants::AttackEffectType::Type::PHOENIXDOWN :
                        actor->SetAction( Database::Info::HashTable::Actions::GuardPhoenixDown, NULL, true);
                    break;
                    case StatConstants::AttackEffectType::Type::STANDFAINT :
                        actor->SetAction( Database::Info::HashTable::Actions::GuardStandFaint, NULL, true);
                    break;
                    case StatConstants::AttackEffectType::Type::STEPBACK :
                        actor->SetAction( Database::Info::HashTable::Actions::GuardStepBack, NULL, true);
                    break;
                    case StatConstants::AttackEffectType::Type::STUN :
                        actor->SetAction( Database::Info::HashTable::Actions::GuardStun, NULL, true);
                    break;
                }
                return true;
            }
        }
        return false;
    }

    void Attack::HitAir(Actor* actor)
    {
        float3 targetDirection = direction * -1;
        ActionHitAir::Param param(airMakeFly, targetDirection, direction*airKnockBack);
        actor->SetAction(Database::Info::HashTable::Actions::HitAir, &param, true);
    }

    void Attack::PhoenixDown(Actor* actor)
    {
        ActionPhoenixDownAir::Param param(knockBack, direction * -1);
        actor->SetAction( Database::Info::HashTable::Actions::PhoenixDownAir, &param, true);
    }

    void Attack::PhoenixDown2(Actor* actor)
    {
        ActionPhoenixDown2Air::Param param(knockBack, direction * -1);
        bool success = actor->SetAction( Database::Info::HashTable::Actions::PhoenixDown2Air, &param, true);
        if(!success){
            PhoenixDown2Polyfill(actor);
        }
    }

    void Attack::PhoenixDown2Polyfill(Actor* actor)
    {
        actor->SetAction( Database::Info::HashTable::Actions::Stand, NULL, true);
        actor->PauseAction(2);
    }

	void Attack::CenterMove(Actor* actor, Object* owner)
    {
		float3 ownerPos = owner->GetPosition();
		float3 actorPos = actor->GetPosition();
		
		//Note : eventually we need to do some higher level math to account for the centermove distance. Right now, as a HACK, just halving the distance, rather than knockback #
		//double angle = atan2((double)(ownerPos.z - actorPos.z), (double)(ownerPos.x - actorPos.x));

		std::cout << "Owner (" << ownerPos.x << ", " << ownerPos.y << ", " << ownerPos.z << ")" << std::endl;
		std::cout << "Actor (" << ownerPos.x << ", " << ownerPos.y << ", " << ownerPos.z << ")" << std::endl;
		float3 newPos = float3((float)(0.5*(ownerPos.x + actorPos.x)), actorPos.y, (float)(0.5*(ownerPos.z + actorPos.z))); //For now, equidistant.
		std::cout << "New (" << newPos.x << ", " << newPos.y << ", " << newPos.z << ")" << std::endl;

		//In the future, knockback should be incorporated, which will require more math. See below for precursor;

			//float slope = (float)((float)( ownerPos.z - actorPos.z ) / (float)( ownerPos.x - actorPos.x ));
			//float distance = pow(pow(static_cast<double>(ownerPos.x-actorPos.x), 2) + pow(static_cast<double>(ownerPos.z-actorPos.z), 2), 0.5);
			//if(distance = 0){ distance = 1; } //DIVIDE BY ZERO, PLS. This is likely not possible, but you never knoooow. 
			//float dist_prop = (knockback / distance); //30 / 60 = 0.5       10 / 60 = 1/6
			//dist_prop = std::max(0.95, dist_prop);

        
		//ActionStepBack::Param param(knockBack, b);
		actor->PauseAction(0);
		actor->SetPosition( newPos );
        actor->SetAction( Database::Info::HashTable::Actions::Stand, NULL, true);
		actor->PauseAction(0.5);
    }

    void Attack::AirStun(Actor* actor)
    {
        float3 targetDirection = direction * -1;
        ActionDownStart::Param param(makeFly, targetDirection, direction*knockBack);
        actor->SetAction( Database::Info::HashTable::Actions::DownStart, &param, true);
    }

    void Attack::Stun(Actor* actor)
    {
        int relativeDirection = (int)attackDir;
        if ( (direction.x>0&&direction.z>0) || (direction.x>0&&direction.z==0) )
            relativeDirection += 1;
        else if ( (direction.x>0&&direction.z<0) || (direction.x==0&&direction.z<0) )
            relativeDirection += 2;
        else if ( (direction.x<0&&direction.z<0) || (direction.x<0&&direction.z==0) )
            relativeDirection += 3;
        float3 targetDirection = actor->GetDirection();
        if ( (targetDirection.x<0&&targetDirection.z==0) || (targetDirection.x<0&&targetDirection.z>0) )
            relativeDirection += 3;
        else if ( (targetDirection.x==0&&targetDirection.z>0) || (targetDirection.x>0&&targetDirection.z>0) )
            relativeDirection += 2;
        else if ( (targetDirection.x>0&&targetDirection.z==0) || (targetDirection.x>0&&targetDirection.z<0) )
            relativeDirection += 1;
        if (relativeDirection>=4)
            relativeDirection-=4;

        uint32 actionName;
        switch((AttackDir)relativeDirection) {
            case AttackDir::L :
                actionName = Database::Info::HashTable::Actions::HitLeft;
                break;
            case AttackDir::F :
                actionName = Database::Info::HashTable::Actions::HitFront;
                break;
            case AttackDir::R :
                actionName = Database::Info::HashTable::Actions::HitRight;
                break;
            case AttackDir::B :
                actionName = Database::Info::HashTable::Actions::HitBack;
                break;
            default :
                actionName = Database::Info::HashTable::Actions::HitFront;
        }
        ActionStun::Param param(stunLength, direction * knockBack);
        if ( actor->SetAction(actionName, &param, true) )
            actor->PauseAction(stopAnimation);
    }

    void Attack::Down(Actor* actor)
    {
        ActionDownStart::Param param(20, direction * -1, direction*knockBack);
        actor->SetAction( Database::Info::HashTable::Actions::DownStart, &param, true);
    }

    void Attack::Faint(Actor* actor)
    {
        ActionDownStart::Param param(0.1f, direction * -1, float3(0, 0, 0));
        actor->SetAction(Database::Info::HashTable::Actions::FaintDownStart, &param, true);
    }

    void Attack::StepBack(Actor* actor)
    {
        ActionStepBack::Param param(knockBack, direction * -1);
        actor->SetAction(Database::Info::HashTable::Actions::MovingKnockBack, &param, true);
    }

    void Attack::StandFaint(Actor* actor)
    {
        ActionFaint::Param param(knockBack);
        actor->SetAction(Database::Info::HashTable::Actions::Faint, &param, true);
    }

}    }    }    }
