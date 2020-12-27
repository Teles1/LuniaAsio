#include "StateManager.h"

using namespace AllM::XRated::Database::Info;

namespace AllM { namespace XRated {	namespace Logic { namespace Stat {

	IState* StateManager::FindState(StateList& list, StateInfo::Type type)
	{
		std::list<IState*>::iterator iter;
		for (iter = list.begin() ; iter != list.end(); ++iter)
			if ((*iter)->GetType() == type)
				return (*iter);
		return NULL;
	}

	bool StateManager::RemoveState(StateList& list, StateInfo::Type type)
	{
		std::list<IState*>::iterator iter;
		for (iter = list.begin() ; iter != list.end(); ++iter)
			if ((*iter)->GetType() == type) {
				delete (*iter);
				list.erase(iter);
				return true;
			}
		return NULL;
	}

	bool StateManager::AddState(IState* state)
	{
		switch (state->GetSort()) {
			case StateInfo::Sort::ORDINARYSTATE :
				stateBuffers.state_ordinary.push_back(state);
				break;
			case StateInfo::Sort::ATTACKSTATE :
				stateBuffers.state_attack.push_back(state);
				break;
			case StateInfo::Sort::DEFENCESTATE :
				stateBuffers.state_defence.push_back(state);
				break;
			default :
				return false;
		}
		std::wstring strState;
		StateInfo::TypeToString(state->GetType(), strState);
//		ALLM_INFO(( L"State [%s] inserted.",  strState.c_str()));
		return true;
	}

	bool StateManager::RemoveState(StateInfo::Sort sort, IState* state)
	{
		switch (sort) {
			case StateInfo::Sort::ORDINARYSTATE :
				for (StateList::iterator iter = stateBuffers.state_ordinary.begin() ; iter != stateBuffers.state_ordinary.end() ; ++iter) {
					if ((*iter) == state) {

						// TODO : for debugging
						std::wstring strState;
						StateInfo::TypeToString(state->GetType(), strState);
//						ALLM_INFO(( L"Ordinary state [%s] removed.",  strState.c_str()));

						delete (*iter);
						stateBuffers.state_ordinary.erase(iter);
						return true;
					}
				}
				break;
			case StateInfo::Sort::ATTACKSTATE :
				for (StateList::iterator iter = stateBuffers.state_attack.begin() ; iter != stateBuffers.state_attack.end() ; ++iter) {
					if ((*iter) == state) {

						// TODO : for debugging
						std::wstring strState;
						StateInfo::TypeToString(state->GetType(), strState);
//						ALLM_INFO(( L"Attack state [%s] removed.",  strState.c_str()));

						delete (*iter);
						stateBuffers.state_attack.erase(iter);
						return true;
					}
				}
				break;
			case StateInfo::Sort::DEFENCESTATE :
				for (StateList::iterator iter = stateBuffers.state_defence.begin() ; iter != stateBuffers.state_defence.end() ; ++iter) {
					if ((*iter) == state) {

						// TODO : for debugging
						std::wstring strState;
						StateInfo::TypeToString(state->GetType(), strState);
//						ALLM_INFO(( L"Defence state [%s] removed.",  strState.c_str()));

						delete (*iter);
						stateBuffers.state_defence.erase(iter);
						return true;
					}
				}
				break;
			default :
				return false;
		}
		return false;
	}

	bool StateManager::FindState(IState* state)
	{
		StateList::iterator iter;
		for (iter = stateBuffers.state_ordinary.begin() ; iter != stateBuffers.state_ordinary.end() ; ++iter)
			if ((*iter) == state)
				return true;
		for (iter = stateBuffers.state_attack.begin() ; iter != stateBuffers.state_attack.end() ; ++iter)
			if ((*iter) == state)
				return true;
		for (iter = stateBuffers.state_defence.begin() ; iter != stateBuffers.state_defence.end() ; ++iter)
			if ((*iter) == state)
				return true;
		return false;
	}

	bool StateManager::FindState(StateInfo::Type type)
	{
		StateList::iterator iter;
		for (iter = stateBuffers.state_ordinary.begin() ; iter != stateBuffers.state_ordinary.end() ; ++iter)
			if ((*iter)->GetType() == type)
				return true;
		for (iter = stateBuffers.state_attack.begin() ; iter != stateBuffers.state_attack.end() ; ++iter)
			if ((*iter)->GetType() == type)
				return true;
		for (iter = stateBuffers.state_defence.begin() ; iter != stateBuffers.state_defence.end() ; ++iter)
			if ((*iter)->GetType() == type)
				return true;
		return false;
	}

	
}	}	}	}
