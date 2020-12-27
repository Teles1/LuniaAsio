#pragma once

#include "../State/State.h"

#include <list>

namespace Lunia { namespace XRated { namespace Logic {

	class Actor;

namespace Action {

	class Action {
	public :
		enum CommandType {
			STOP = 0, MOVE, DASH, ATTACKA, ATTACKARELEASE, ATTACKB, ATTACKBRELEASE, DOWNATTACK, JUMP
		};

		struct IActionParam {
			virtual ~IActionParam() {}
			virtual IActionParam* GetClonePtr()=0;
		};

		struct ReservedAction {
			bool bExist; 
			float startTime;
			uint32 actionName;
			IActionParam* param;
			bool bForce;
			ReservedAction() : bExist(false), startTime(0), actionName(0), param(NULL), bForce(false) {}
			~ReservedAction() {
				if (param)
					delete param;
			}
			ReservedAction(const ReservedAction& rAction) {
				bExist = rAction.bExist;
				startTime = rAction.startTime;
				actionName = rAction.actionName;
				if ( rAction.param ) 
					param = rAction.param->GetClonePtr();
				else param = NULL;
				bForce = rAction.bForce;
			}
			void operator=(const ReservedAction& rAction) {
				bExist = rAction.bExist;
				startTime = rAction.startTime;
				actionName = rAction.actionName;
				if ( rAction.param ) 
					param = rAction.param->GetClonePtr();
				else param = NULL;
				bForce = rAction.bForce;
			}
			void Clear() {
				if ( bExist ) {
					bExist = false;
					if ( param ) delete param;
					param = NULL;
				}
			}
		};

		struct BufferedState {
			float endTime;
			Database::Info::StateInfo::Sort sort;
			uint32 id; 
			Stat::IState* state;
			BufferedState(Stat::IState* s, Database::Info::StateInfo::Sort st, float et, uint32 i) : endTime(et), sort(st), id(i), state(s) {}
			void operator= (const BufferedState& bufferedState) {
				endTime = bufferedState.endTime;
				sort = bufferedState.sort;
				state = bufferedState.state;
				id = bufferedState.id;
			}
		};
		typedef std::list<BufferedState> BufferedStateList;

		struct ActionData {
			IActionParam* actionSpecificParams;
			float passedTime, length;
			ReservedAction reserved;
			BufferedStateList bufferedStates;
			bool bHit; ///< When Action hit other object by ATTACK and DAMAGE state, set true this value

		public :
			ActionData() : actionSpecificParams(NULL), passedTime(0), length(0), bHit(false) {}
			inline void Init() {
				if (actionSpecificParams)
					delete actionSpecificParams;
				actionSpecificParams = NULL;
				passedTime = length = 0;
				bHit = false;
				bufferedStates.clear();
				if (reserved.bExist)
					reserved.Clear();
			}
		};

		struct ActionItem {
			uint32 hashCode;
			std::wstring name;
			Action* action;
			ActionItem(uint32 code, const std::wstring& n, Action* act) : hashCode(code), name(n), action(act) {}
		};
		typedef std::vector<ActionItem> ActionList;
		typedef std::vector<ActionItem*> ActionPtrList;

	private : //member values.
		Database::Info::ActionInfo* info;
		std::vector<Stat::IState*> stateList;


	public :
		Action(Lunia::XRated::Database::Info::ActionInfo* i);
		virtual ~Action();


		const std::wstring& GetName() const { return info->actionName; }
		uint32 GetNameHash() const { return info->hash; }
		Database::Info::ActionInfo::ActionClassType GetActionClassType() const { return info->classType; }
		const Database::Info::ActionInfo* GetActionInfo() const { return info; }

		virtual bool IsPossible(uint32 name, float, Actor*, ActionData& data) const;
		virtual const Database::Info::ActionInfo* GetLinkedAction(Actor*, unsigned int key, float dt, bool &reserved, float& changeTime, bool bHit);
		virtual uint32 GetNextAction(Actor*, IActionParam*&, bool bHit);
		virtual void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data);
		virtual bool Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data);
		virtual void Destroy(Actor* obj, ActionData& data);

	private:
		Action(const Action&); // unable to copy, because of potential problem of double delete at destructor
	};

	class ActionManager
	{
		std::wstring objectName;

		Action::ActionPtrList* actionList;
		Action::ActionData actionData;
		Action*	currentAction;
		float actionStartTime;
		float pauseTime;
		float actionDuration;

	public :
		ActionManager() {} ///> to be deserialized
		virtual ~ActionManager() {}

		void Init(const std::wstring& objName, const std::wstring& defaultAction, Action::ActionPtrList* list);

		Action::ActionData& GetActionData() { return actionData; }
		const Action::ActionData& GetActionDataConst() const { return actionData; }

		/**
		Get action interface.
		@param name		action name to get. If L"", return current set action.
		@return			action interface
		*/
		Action* GetAction(const wchar_t* name);
		Action* GetAction(uint32 hash);

		/**
		Set action to object.
		@param name		action name to set
		@param object	object to set action
		@param param	action parameters. Its data is up to action to set
		@param force	if true action is set forcely not concerned with this action is able to set or not
		@return			true if action set normally
		*/
		//bool SetAction(const std::wstring &name, Actor* object, ActionParam* param, IGameStateDatabase& sd, bool force = false);
		bool SetAction(uint32 name, Actor* object, Action::IActionParam* param, IGameStateDatabase& sd, bool force = false);

		/** Stop current action forcely. */
		bool StopAction(Actor* object);
		void Pause(float dt) { pauseTime = dt; }

		void SetActionDuration(float t) { actionDuration = t; }

		/**
		return relative direction according to character's current direction
		@param obj			object to check directoin
		@param direction	absolute direction to convert
		@return relative direction about a given object.
		*/
		unsigned int GetRelativeDirection(Actor* obj, uint32 direction);

		bool IsAbleToCast(uint32 skillGroupHash, Constants::GameTypes gameType);

		bool Command(Action::CommandType state, Actor* obj, IGameStateDatabase& sd, uint32 param);
		void Do(float dt, Actor* obj, IGameStateDatabase* db/*, ActionParam* param*/);

		void ReserveAction(uint32 name, float startTime, Action::IActionParam* param, bool force = false);
		void RemoveReservedAction();

		void Hit() { actionData.bHit = true; } ///< called when action hit other object

		Action::ActionPtrList* SetActionList(Actor* object, IGameStateDatabase& sd, Action::ActionPtrList* newActionList, uint32 defaultAction);
	};

}	}	}	}



