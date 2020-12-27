#include "States.h"


namespace AllM { namespace XRated {	namespace Logic { namespace Stat {

	class StateManager {
	protected :
		typedef std::list<IState*> StateList;

		struct StateBuffers : public Serializer::ISerializable
		{	//공격, 방어시, 평소 참조하는 state list가 다르다.
			StateList state_attack;
			StateList state_defence;
			StateList state_ordinary;

		public: /* ISerializable implements */
			virtual void Serialize(Serializer::IStreamWriter& out) const
			{
				out.Begin(L"AllM::XRated::Logic::Stat::StateBuffers");
			}
			virtual void Deserialize(Serializer::IStreamReader& in)
			{
				in.Begin(L"AllM::XRated::Logic::Stat::StateBuffers");
			}
		} stateBuffers;

		IState* FindState(StateList& list, Database::Info::StateInfo::Type type);
		bool RemoveState(StateList& list, Database::Info::StateInfo::Type type);

	public :

		virtual bool AddState(IState* state);
		virtual bool RemoveState(Database::Info::StateInfo::Sort sort, IState* state);
		virtual bool FindState(IState* state);
		virtual bool FindState(Database::Info::StateInfo::Type type);
	};

}	}	}	}
