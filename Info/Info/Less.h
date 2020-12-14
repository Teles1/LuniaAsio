#pragma once
#include <iostream>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				
				template<class Arg1, class Arg2, class Result>
				struct binary_function
				{
					using first_argument_type = Arg1;
					using second_argument_type = Arg2;
					using result_type = Result;
				};

				struct Leveler
				{
					enum Level
					{
						Nothing = 0,
						Warning,
						Throw,
					};

					Level  NoticeLevel;
				};

				template<class T>
				class Less
					: public binary_function<T, T, bool>
					, public Leveler
				{	// functor for operator<

				public:

					class IEventListener
					{
					public:
						virtual void DuplicationDetected(const T&) = 0;
					};

					Less()
						: listener(NULL)
					{
						NoticeLevel = Leveler::Nothing;
					}

					void SetEventListener(IEventListener* pListener)
					{
						listener = pListener;
					}

					template<typename T>	bool operator()(const T& _Left, const T& _Right) const
					{	// apply operator< to operands
						if (listener != NULL && _Left == _Right)
							listener->DuplicationDetected(_Left);

						if (NoticeLevel > Leveler::Nothing && _Left == _Right)
						{
							ALLM_WARNING((L"Database Loading....Same Key Found! [%s] ", StringUtil::ToUnicode(_Left).c_str()));
							if (NoticeLevel == Leveler::Throw) throw LessException(_Left);
							return false;
						}
						return (_Left < _Right);
					}

					//Template Specification
					template<wchar_t*>	bool operator()(const wchar_t* _Left, const wchar_t* _Right) const
					{
						int r = wcscmp(_Left, _Right);

						if (listener != NULL && r == 0)
							listener->DuplicationDetected(_Left);

						if (NoticeLevel > Leveler::Nothing && r == 0)
						{
							Logger::GetInstance().Warn(L"Database Loading....Same Key Found! [{0}]", _Left);
							if (NoticeLevel == Leveler::Throw) throw LessException(_Left);
							return false;
						}
						return r > 0;  
					}

					template<std::wstring&>	bool operator()(const std::wstring& _Left, const std::wstring& _Right) const
					{	// apply operator< to operands
						return operator()(_Left.c_str(), _Right.c_str());
					}

					template<XRated::StageLocation&>	bool operator()(const Lunia::XRated::StageLocation& _Left, const Lunia::XRated::StageLocation& _Right) const
					{	// apply operator< to operands
						if (listener != NULL && _Left == _Right)
							listener->DuplicationDetected(_Left);

						if (NoticeLevel > Leveler::Nothing && _Left == _Right)
						{
							std::wstring temp = StringUtil::Format(L"License:%d-%d", _Left.StageGroupHash, _Left.Level);
							Logger::GetInstance().Warn(L"Database Loading....Same Key Found! [{0}]", temp.c_str());
							if (NoticeLevel == Leveler::Throw) throw LessException(_Left);
							return false;
						}
						return (_Left < _Right);
					}
					template<typename T> bool operator() (const T* _Left, const T* _Right) const
					{
						if (listener != NULL && _Left == _Right)
							listener->DuplicationDetected(_Left);

						if (NoticeLevel > Leveler::Nothing && _Left == _Right)
						{
							Logger::GetInstance().Warn(L"Database Loading....Same Key Found! [{0}]", _Left);
							if (NoticeLevel == Leveler::Throw) throw LessException(_Left);
							return false;
						}
						return (_Left < _Right);
					}

					class LessException : public Exception
					{
					public:
						LessException(T pDuplicated)
							: duplicated(pDuplicated)
						{}
						const T& GetDuplicatedKey() { return duplicated; }
					private:
						T duplicated;

					};

				private:
					IEventListener* listener;
				};

				class InstantLeveler
				{
				public:
					InstantLeveler(Leveler& leveler, const char* called = NULL, Leveler::Level instantLevel = Leveler::Warning)
						: priorLevel(leveler.NoticeLevel), target(&leveler), calledFunc(called)
					{
						if (target->NoticeLevel < instantLevel)
						{
							target->NoticeLevel = instantLevel;
						}
					}
					template<typename T>
					InstantLeveler(std::less<T>&, const char*, int = 0)
						: target(NULL)
					{

					}

					~InstantLeveler()
					{
						if (target != NULL)
							target->NoticeLevel = priorLevel;
					}

				private:
					Leveler::Level priorLevel;
					Leveler* target;
					std::string calledFunc;
				};
			}
		}
	}
}