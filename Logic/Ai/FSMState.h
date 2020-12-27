#pragma once

#include <iostream>

namespace Lunia { namespace XRated { namespace Logic {
	/**
	AI namespace of Monsters, NPC
	*/
	namespace AI {

		class IFSMState {
		public :
			virtual const wchar_t* GetName() const = 0;
			virtual void InitializeState() = 0;
			virtual void Update() = 0;
			virtual void DestroyState() = 0;
		};

		template <class T>
		class FSMState : public IFSMState {
		protected :
			typedef void (T::*pFn) (void);

			std::wstring name;
			T* instance;
			pFn initialize;
			pFn update;
			pFn destroy;

		public :
			FSMState() {instance = NULL; initialize = NULL; update = NULL; destroy = NULL;}
			inline FSMState(const wchar_t* name, T* pInst, pFn init, pFn update, pFn destroy) {
				Set(name, pInst, init, update, destroy);
			}
			virtual ~FSMState() {}

			inline void Set(const wchar_t* name, T* pInst, pFn init, pFn update, pFn destroy) {
				this->name = name;
				this->instance=  pInst;
				this->initialize = init;
				this->update = update;
				this->destroy = destroy;
			}
		public :
			//IFSMState implementation.
			inline virtual const wchar_t* GetName() const { return name.c_str(); }
			inline virtual void InitializeState() { if (instance==NULL||initialize==NULL) throw; (instance->*initialize)(); }
			inline virtual void Update() { if (instance==NULL||update==NULL) throw; (instance->*update)(); }
			inline virtual void DestroyState() { if (instance==NULL||destroy==NULL) throw; (instance->*destroy)(); }
		};

	}
}	}	}

