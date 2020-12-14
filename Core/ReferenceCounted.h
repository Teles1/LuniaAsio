#pragma once
#ifndef LUNIA_COMMON_REFERENCECOUNTED_H
#define LUNIA_COMMON_REFERENCECOUNTED_H
#include "IReferenceCounted.h"
#include "com_ptr.h"
namespace Lunia {
	template <typename TParent> class ReferenceCountedImpl : public TParent {
		mutable int refCount;
	public:
		inline explicit ReferenceCountedImpl(const ReferenceCountedImpl<TParent>&) :refCount(0) {}
		inline ReferenceCountedImpl& operator=(const ReferenceCountedImpl<TParent>&) { return *this; }

		inline bool operator==(const ReferenceCountedImpl<TParent>& other) const {
			return true;
		}

		inline bool operator!=(const ReferenceCountedImpl<TParent>& other) const {
			return false;
		}


		inline ReferenceCountedImpl() :refCount(0) {}
		inline virtual ~ReferenceCountedImpl() {}

		inline int GetRefCount() const { return refCount; }
		inline int AddRef() const { ++refCount; return refCount; }
		inline int Release() const { if ((--refCount) < 1) { int tmp = refCount; delete this; return tmp; } return refCount; }
	};

	class ReferenceCounted : virtual public IReferenceCounted {

		mutable int m_RefCounter;

		//	   virtual ~ReferenceCounted() {}
		//@todo should we use this or not!?
		//	   ReferenceCounted(const ReferenceCounted& ) {  }
	public:
		inline ReferenceCounted() { m_RefCounter = 0; }
		inline virtual ~ReferenceCounted() {}

		inline explicit ReferenceCounted(const ReferenceCounted&) :m_RefCounter(0) {}
		inline ReferenceCounted& operator=(const ReferenceCounted&) { return *this; }




		inline virtual int GetRefCount() const { return m_RefCounter; }

		inline virtual int AddRef() const { ++m_RefCounter; return m_RefCounter; }

		inline virtual int Release() const {
			--m_RefCounter;
			int tmp = m_RefCounter;
			if (m_RefCounter < 1) {
				delete this;
			}
			return tmp;
		}
	};

	template <typename T>
	class IFeedBackListener
	{
	public:
		virtual void FeedBack(const T* instance) = 0;
	};

	template <typename TParent> class ReferenceCountedWithFeedBackImpl : public TParent {
		mutable int refCount;
		const int feedBackCount;
		IFeedBackListener<TParent>* owner;
	public:
		inline explicit ReferenceCountedWithFeedBackImpl(const ReferenceCountedWithFeedBackImpl<TParent>&) :refCount(0) {}
		inline ReferenceCountedWithFeedBackImpl& operator=(const ReferenceCountedWithFeedBackImpl<TParent>&) { return *this; }

		inline bool operator==(const ReferenceCountedWithFeedBackImpl<TParent>& other) const {
			return true;
		}

		inline bool operator!=(const ReferenceCountedWithFeedBackImpl<TParent>& other) const {
			return false;
		}


		inline ReferenceCountedWithFeedBackImpl(IFeedBackListener<TParent>* pOwner, int pFeedBackCount) :refCount(0), owner(pOwner), feedBackCount(pFeedBackCount) {}
		inline virtual ~ReferenceCountedWithFeedBackImpl() {}

		inline void UnBindFeedBackListener() {
			owner = NULL;
		}
		inline int GetRefCount() const { return refCount; }
		inline int AddRef() const { ++refCount; return refCount; }
		inline int Release() const {
			--refCount;

			if (refCount < 1) {
				int tmp = refCount; delete this; return tmp;
			}
			else if ((owner != NULL) && (refCount == feedBackCount)) {
				int tmp = refCount; owner->FeedBack(this); return tmp;
			}
			return refCount;
		}

	};


	template <typename TParent> class ReferenceCountedWithWeakProxyImpl : public TParent {

		mutable int refCount;


		struct WeakProxy : public ReferenceCountedImpl<IReferenceCountedWithWeakProxy::IWeakProxy> {
			mutable bool alive;

			inline WeakProxy() :alive(true) {}

			inline virtual bool IsAlive() const {
				return alive;
			};

			inline void Kill() const {
				alive = false;
			};
		};

		mutable com_ptr<WeakProxy> proxy;

	public:
		inline ReferenceCountedWithWeakProxyImpl& operator=(const ReferenceCountedWithWeakProxyImpl&) { return *this; }
		inline explicit ReferenceCountedWithWeakProxyImpl(const ReferenceCountedWithWeakProxyImpl&) :refCount(0) {}


		inline ReferenceCountedWithWeakProxyImpl<TParent>() : refCount(0) {}
		inline ~ReferenceCountedWithWeakProxyImpl<TParent>() {
			if (proxy != 0) proxy->Kill();
		}

		inline int GetRefCount() const { return refCount; }
		inline int AddRef() const { ++refCount; return refCount; }
		inline int Release() const { if ((--refCount) < 1) { int tmp = refCount; delete this; return tmp; } return refCount; }
		inline virtual IReferenceCountedWithWeakProxy::IWeakProxy* GetWeakProxy() const {
			if (proxy == 0) {
				new WeakProxy();
			}
			return proxy.get();
		}

	};

	typedef ReferenceCountedWithWeakProxyImpl<IReferenceCountedWithWeakProxy> ReferenceCountedWithWeakProxy;
};
#endif