
#pragma once

#include <Info/Info.h>

#include <vector>

namespace Lunia { namespace XRated { namespace Logic {

	class Object;
	class Projectile;
	class Actor;
	class Player;
	class NonPlayer;
	class IGameStateDatabase;

	/**
	네임 스페이스는
	*/
	namespace Stat {
		typedef std::map<std::wstring, std::wstring> StateParams;

		struct StatConstants {
			struct AttackEffectType {
				enum Type { STUN = 0, AIRSTUN, DOWN, PHOENIXDOWN, FAINT, STEPBACK, STANDFAINT, PHOENIXDOWN2, CENTERMOVE, PROJECTILE, NOSTUN, CNT };

				const static uint32 StunFlag		 = 0x00000001;
				const static uint32 AirStunFlag		 = 0x00000002;
				const static uint32 DownFlag		 = 0x00000004;
				const static uint32 PhoenixDownFlag  = 0x00000008;
				const static uint32 FaintFlag		 = 0x00000010;
				const static uint32 StepBackFlag	 = 0x00000020;
				const static uint32 StandFaintFlag	 = 0x00000040;
				const static uint32 PhoenixDown2Flag = 0x00000080;  // 3.1 by ultimate
				const static uint32 CenterMoveFlag	 = 0x00000080;  // 3.1 by ultimate
				const static uint32 AllFlag			 = 0x0000007F;
			};
		};

		class IState {
			uint32 id;

		public :
			virtual ~IState() {}
			void SetId(uint32 i) { id = i; }
			uint32 GetId() const { return id; }
			virtual Lunia::XRated::Database::Info::StateInfo::Type GetType() const = 0;
			virtual Lunia::XRated::Database::Info::StateInfo::Sort GetSort() const = 0;
			virtual Lunia::XRated::Serial GetCreator() const =0;
			//virtual Lunia::XRated::Serial GetOwner() const = 0;

			virtual bool Debuff(float buffRate) = 0;
			virtual void Pause(float dt) = 0;
			virtual bool IsActivated()=0;

			/**@return true when state should be removed.*/
			virtual void Initialize(IGameStateDatabase* db) = 0;	
			virtual void SetDownAtkFlag() = 0;
			virtual bool Do(float dt, IGameStateDatabase* db) = 0;
			virtual void Do(Object* target, IGameStateDatabase* db) = 0;
			virtual void Destroy() = 0;
			virtual void Enable(bool enable) = 0;

			virtual void SetOwner(void* obj)=0;
			virtual void SetCreator(Lunia::XRated::Serial creator) =0;

		public:
			inline IState() : id(0) {}
			inline IState(const IState& rhs) : id(rhs.id) {}
			inline IState& operator=(const IState& rhs) { id=rhs.id; return *this; }
		};


		template<class T> class State : public IState
		{
		private:
			Serial creator;
			
		protected :
			T* owner;

			
			Lunia::XRated::Database::Info::StateInfo::Type type;
			Lunia::XRated::Database::Info::StateInfo::Sort sort;

			float passedTime;
			float pauseTime;

		public :
			inline State(T* o, Lunia::XRated::Database::Info::StateInfo::Type tp, Lunia::XRated::Database::Info::StateInfo::Sort s) :
					owner(o), type(tp), sort(s),  passedTime(0), pauseTime(0),creator(0) { }

			inline virtual ~State() {}

			inline Lunia::XRated::Database::Info::StateInfo::Type GetType() const { return type; }
			inline Lunia::XRated::Database::Info::StateInfo::Sort GetSort() const { return sort; }
			

			inline virtual Lunia::XRated::Serial GetCreator() const { return creator; }
			inline virtual bool Debuff(float) {return false;}
			inline virtual void Pause(float dt) { pauseTime = dt; }
			inline virtual bool IsActivated() { return false; }

			//state의 네가지 단계
			inline virtual bool IsAvailable() {return true;}
			inline virtual void Initialize(IGameStateDatabase*) {}
			inline virtual void SetDownAtkFlag() {}

			inline virtual bool Do(float dt, IGameStateDatabase*) {passedTime+= dt; return false;}
			inline virtual void Do(Object*, IGameStateDatabase*) {}
			inline virtual void Destroy() {}
			inline virtual void Enable(bool) {}
			inline virtual void SetOwner(void* obj) { owner = (T*)obj; }
			inline virtual void SetCreator(Lunia::XRated::Serial inCreator) {creator = inCreator;}
		};

	}	}	}	}

