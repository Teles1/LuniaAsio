#pragma once
#include <windows.h>

namespace Lunia
{
	class ILockable
	{
	public:
		virtual ~ILockable() {}
		virtual void Lock() = 0;
		virtual void Unlock() = 0;
	};

	class CriticalSection : public ILockable
	{
	private:
		CRITICAL_SECTION cs;

	public:
		CriticalSection() { ::InitializeCriticalSection(&this->cs); }
		CriticalSection(const CriticalSection&) { ::InitializeCriticalSection(&this->cs); }
		CriticalSection& operator=(const CriticalSection&) { return *this; }
		virtual ~CriticalSection() { ::DeleteCriticalSection(&this->cs); }
		inline void Lock() { ::EnterCriticalSection(&this->cs); }
		inline void Unlock() { ::LeaveCriticalSection(&this->cs); }
	};

	class CriticalSectionLock
	{
	private:
		CriticalSection& l;
		void operator=(const CriticalSectionLock&) {}

	public:
		CriticalSectionLock(CriticalSection& locker) : l(locker) { l.Lock(); }
		~CriticalSectionLock() { l.Unlock(); }
	};
} // namespace AllM