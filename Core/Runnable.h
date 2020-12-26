#ifndef ALLM_RUNNABLE_H
#define ALLM_RUNNABLE_H

#include <windows.h>
#include <Core/Exceptions/Expcetion.h>
#include <map>

namespace Lunia {


	/**
	This class encapsulates windows threading system.
	When you need a threading support. Derive a class from Runnable.
	Implement void run()-method and then you can start or stop
	thread by calling stopThread() and startThread() respectively.

	@author juhnu
	@date   2004.04.01

	*/


	class Runnable {

	private:

		DWORD m_TID;
		HANDLE m_THandle;

		volatile bool m_fRun;
		volatile bool m_isRunning;
		std::wstring name;


		unsigned long static  __stdcall   threadFunction(void* t) {
			while (((Runnable*)t)->runt() != 0) {}
			CloseHandle(((Runnable*)t)->m_THandle);
			return 0;
		}

		int  runt() {

			if (m_fRun == false) {
				m_isRunning = false;
				return 0;
			}
			Run();
			return 1;
		}
	protected:

		void TerminateThread() {
			m_fRun = false;
		}

	public:

		Runnable(const wchar_t* threadName = L"unnamed") : name(threadName) {
			m_fRun = false;
			m_isRunning = false;
		}

		void StartThread(int priority) {
			StartThread();
			SetThreadPriority(m_THandle, priority);
		}

		void StartThread() {
			m_fRun = true;
			m_isRunning = true;
			if ((m_THandle = CreateThread(NULL, 0, threadFunction, (void*)this, NULL, &m_TID)) == NULL) {
				Logger::GetInstance().Exception(L"failed to create thread");
			}
		}


		virtual ~Runnable() {
			StopThread();
		}

		void StopThread() {
			if ((m_fRun == false) && (m_isRunning == false)) return;
			m_fRun = false;
			::WaitForSingleObject(m_THandle, INFINITE);	// you should not call StopThread in Run();
			m_TID = 0;
		}

		void Suspend()
		{
			::SuspendThread(m_THandle);
		}

		void Resume()
		{
			::ResumeThread(m_THandle);
		}

		HANDLE GetThreadHandle() const {
			return m_THandle;
		}

		const wchar_t* GetThreadName() const
		{
			return name.c_str();
		}

		virtual void Run() = 0;
	};




	class ThreadHolder
	{
	public:
		ThreadHolder(int ret = 0) : ret(ret) { event = ::CreateEvent(NULL, true, true, NULL); }
		ThreadHolder(const ThreadHolder& rhs) { ret = rhs.ret; event = ::CreateEvent(NULL, true, true, NULL); }
		~ThreadHolder() { ::CloseHandle(event); }
		void Hold(int ret = 0) { this->ret = ret; ::ResetEvent(event); }
		void Release(int ret = 0) { this->ret = ret; ::SetEvent(event); }
		int Wait(unsigned int milliseconds = INFINITE) { ::WaitForSingleObject(event, milliseconds); return ret; }
	private:
		int ret;
		HANDLE event;
		ThreadHolder& operator=(const ThreadHolder&);
	};


	/**
		@brief		supports contexts each thread with a way like singleton.
					Simply, kind of singleton that depends on thread.
		@remarks	ThreadContext class doens't detect thread start or end.
					Every thread context just depends on current thread handle.

	*/
	template<typename T>
	class ThreadContext
	{
	public:
		static T& Instance()
		{
			return instances[::GetCurrentThreadId()];
		}

		static void Create(const T& t, DWORD threadId = ::GetCurrentThreadId())
		{
			instances[threadId] = t;
		}

		static void Clear(DWORD threadId = ::GetCurrentThreadId())
		{
			instances.erase(threadId);
		}

	private:
		static std::map<DWORD, T> instances;
		ThreadContext(); // protection
	};
	template<typename T> std::map<DWORD, T> ThreadContext<T>::instances = std::map<DWORD, T>();


}
#endif
