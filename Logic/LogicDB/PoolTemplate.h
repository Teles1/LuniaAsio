#pragma once
#include "../Logic.h"
#include <queue>

namespace Lunia { namespace XRated { namespace Logic {


	template <class T> 
	class IPoolTemplate {
	public :
		virtual ~IPoolTemplate() {}

		virtual T* Get(int type)=0;
		virtual void Release(T* obj)=0;
		virtual void Init(const std::vector<int>& initValue)=0;

		//Helper
		virtual int GetAvailableSize(int type) const = 0;
		virtual void GetAvailableSize(std::vector<int>& l) const = 0;
	};

	template <class T> 
	class PoolTemplate : IPoolTemplate<T> {
		typedef std::vector< std::queue<T*> > Objects;


		Objects available;
		//Objects used;

		std::vector<Lunia::CriticalSection*> csMap;

		std::vector<int> poolSize;

		IGameStateDatabase* db;

	protected :
		virtual T* Create(int type)=0;

		void AddToCache(int type, int count) {
			for (int i=0; i<count; ++i) {
				Lunia::AutoLockCS lock( *csMap.at(type) );
				available.at(type).push( Create(type) );
				++poolSize[type];
			}
		}

	public :
		PoolTemplate() {}
		virtual ~PoolTemplate() {
			for(size_t type=0; type<available.size(); ++type)
			{
				{
					Lunia::AutoLockCS lock( *csMap.at(type) );
					while(available.at(type).empty())
					{
						delete available.at(type).front();
						available.at(type).pop();
					}
				}
				delete csMap[type];
			}
		}

		virtual T* Get(int type) {
			{
				Lunia::AutoLockCS lock( *csMap.at(type) );		

				std::queue<T*>& pool=available.at(type);

				if ( !pool.empty() ) {
					T* tmp=pool.front();
					pool.pop();


					return tmp;
				} else
					++poolSize[type];
			}
			T* tmp=Create(type);

			Logger::GetInstance().Warn( L"[PoolTemplate::Get] Not enough pool. (type : {0} / poolSize : {1})", type, poolSize[type] );
			return tmp;
		}

		virtual void Release(T* obj) {
			//here we notify pool that the object can be reused in the future

			int type = obj->GetClass();
			if ( type < 0 ) {
				Logger::GetInstance().Exception(L"[PoolTemplate::Release] wrong class type. [{0}]", type);
			}

			{
				Lunia::AutoLockCS lock( *csMap.at(type) );

				available.at(type).push(obj);
			}
		}

		virtual void Init(const std::vector<int>& initValue)
		{
			if (available.empty())
				available.resize(initValue.size());
			else
				throw;

			std::vector<int>::const_iterator i = initValue.begin();

			for ( int a=0; a< (int)initValue.size() ; ++a) {
				Lunia::CriticalSection *cs = new Lunia::CriticalSection();
				csMap.push_back( cs );
				poolSize.push_back(0);
			}

			for (int type = 0 ; i != initValue.end() ; ++i, ++type) {
				AddToCache( type, *i );
			}
		}

		//Helper
		int GetAvailableSize(int type) const {
			if ( type >= (int)available.size() ) {
				Logger::GetInstance().Exception(L"[PoolTemplate::GetAvailableSize] wrong class type. [{0}]", type);
			}
			return available.size();
		}
		void GetAvailableSize(std::vector<int>& l) const {
			for ( Objects::const_iterator i = available.begin(); i != available.end() ; ++i) {
				l.push_back( (int)(*i).size() );
			}
		}

		private:
			PoolTemplate(const PoolTemplate&); // unable to copy, because of potential problem of double delete at destructor
	};

}	}	}

