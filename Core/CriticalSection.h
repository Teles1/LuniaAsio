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
		inline CriticalSection() { ::InitializeCriticalSection(&this->cs); }
		inline CriticalSection(const CriticalSection&) { ::InitializeCriticalSection(&this->cs); }
		inline CriticalSection& operator=(const CriticalSection&) { return *this; }
		inline virtual ~CriticalSection() { ::DeleteCriticalSection(&this->cs); }
		inline void Lock() { ::EnterCriticalSection(&this->cs); }
		inline void Unlock() { ::LeaveCriticalSection(&this->cs); }
	};

	class CriticalSectionLock
	{
	private:
		CriticalSection& l;
		inline void operator=(const CriticalSectionLock&) {}

	public:
		inline CriticalSectionLock(CriticalSection& locker) : l(locker) { l.Lock(); }
		inline ~CriticalSectionLock() { l.Unlock(); }
	};
} // namespace AllM