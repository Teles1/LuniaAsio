#pragma once

#include "State.h"

namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Condition : public State<Actor>
	{
	protected:
				Lunia::Math::PseudoRandomFloat* randomFloat;
				float probability;
				uint32 bundleHash;
				Database::Info::StateBundleInfo* bundleInfo;
				bool bSelf;
				float coolDown;
				float coolDownTimer;
				std::vector<IState*> optionalStates;
	public:
				//Condition(Database::Info::StateInfo::Type stateType, Actor* owner, float prob, bool self, float coolDown, uint32 bundlehash);
				//Condition(Database::Info::StateInfo::Type stateType, Actor* owner, float prob, bool self, float coolDown, std::vector<IState*>& optionalStates);
				Condition(Database::Info::StateInfo::Type stateType, Actor* owner, float prob, bool self, float coolDown, uint32 bundlehash, std::vector<IState*>& optionalStates);
		virtual	~Condition();

				bool IsActivated();
				void Initialize(IGameStateDatabase*);
				bool Do(float dt, IGameStateDatabase*);
				void Do(Object* target, IGameStateDatabase* db);
	};

	class IfHit : public Condition
	{
	public :
				//IfHit(Actor* owner, float prob, bool self, float coolDown, uint32 bundlehash);
				//IfHit(Actor* owner, float prob, bool self, float coolDown, std::vector<IState*>& optionalStates);
				IfHit(Actor* owner, float prob, bool self, float coolDown, uint32 bundlehash, std::vector<IState*>& optionalStates);

				void Do(Object* target, IGameStateDatabase* db);
	};

	class IfHitBy : public Condition
	{
	public :
				IfHitBy(Actor* owner, float prob, bool self, float coolDown, uint32 bundlehash, std::vector<IState*>& optionalStates);

				void Do(Object* target, IGameStateDatabase* db);
	};

	class IfHeal : public Condition
	{
	public :
				IfHeal(Actor* owner, float prob, bool self, float coolDown, uint32 bundlehash, std::vector<IState*>& optionalStates);

				void Do(Object* target, IGameStateDatabase* db);
	};

	class IfCriAtk : public Condition
	{
	public :
				IfCriAtk(Actor* owner, float prob, bool self, float coolDown, uint32 bundlehash, std::vector<IState*>& optionalStates);

				void Do(Object* target, IGameStateDatabase* db);
	};

	class IfCriMagic : public Condition
	{
	public :
				IfCriMagic(Actor* owner, float prob, bool self, float coolDown, uint32 bundlehash, std::vector<IState*>& optionalStates);

				void Do(Object* target, IGameStateDatabase* db);
	};

	class IfCastSkill : public Condition
	{
	private:
				std::wstring skillGroupName;
				uint32 skillGroupNameHash;
				bool enableAllSkill;

	public :
				IfCastSkill(Actor* owner, float prob, bool self, float coolDown, uint32 bundlehash, std::vector<IState*>& optionalStates, const std::wstring& skillGroupName);

				void Do(Object* target, IGameStateDatabase* db);

				bool CheckSkillGroupName(const std::wstring& skillGroupName);
				bool CheckSkillGroupName(uint32 skillGroup);
	};

}}}}
