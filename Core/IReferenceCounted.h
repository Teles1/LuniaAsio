#ifndef LUNIA_IREFERENCECOUNTED_H
#define LUNIA_IREFERENCECOUNTED_H

namespace Lunia {
	class IReferenceCounted {
	protected:
		virtual ~IReferenceCounted() {}

	public:
		virtual int AddRef() const = 0;
		virtual int Release() const = 0;
		virtual int GetRefCount() const = 0;
	};

	template <typename T> class TReferenceCounted : public T {
	protected:
		virtual ~TReferenceCounted<T>() {}
	public:
		virtual int AddRef() const = 0;
		virtual int Release() const = 0;
		virtual int GetRefCount() const = 0;
	};

	class IReferenceCountedWithWeakProxy : public IReferenceCounted {
	public:
		class IWeakProxy : public IReferenceCounted {
		public:
			virtual bool IsAlive() const = 0;
		};

		virtual IWeakProxy* GetWeakProxy() const = 0;

	};
}
#endif