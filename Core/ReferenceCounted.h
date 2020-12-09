#pragma once
#ifndef LUNIA_COMMON_REFERENCECOUNTED_H
#define LUNIA_COMMON_REFERENCECOUNTED_H
#include "IReferenceCounted.h"
#include "com_ptr.h"
namespace Lunia {
	template <typename TParent> class ReferenceCountedImpl : public TParent {
		mutable int refCount;
	public:
		explicit ReferenceCountedImpl(const ReferenceCountedImpl<TParent>&) :refCount(0) {}
		ReferenceCountedImpl& operator=(const ReferenceCountedImpl<TParent>&) { return *this; }

		bool operator==(const ReferenceCountedImpl<TParent>& other) const {
			return true;
		}

		bool operator!=(const ReferenceCountedImpl<TParent>& other) const {
			return false;
		}


		ReferenceCountedImpl() :refCount(0) {}
		virtual ~ReferenceCountedImpl() {}

		int GetRefCount() const { return refCount; }
		int AddRef() const { ++refCount; return refCount; }
		int Release() const { if ((--refCount) < 1) { int tmp = refCount; delete this; return tmp; } return refCount; }
	};

	class ReferenceCounted : virtual public IReferenceCounted {

		mutable int m_RefCounter;

		//	   virtual ~ReferenceCounted() {}
		//@todo should we use this or not!?
		//	   ReferenceCounted(const ReferenceCounted& ) {  }
	public:
		ReferenceCounted() { m_RefCounter = 0; }
		virtual ~ReferenceCounted() {}

		explicit ReferenceCounted(const ReferenceCounted&) :m_RefCounter(0) {}
		ReferenceCounted& operator=(const ReferenceCounted&) { return *this; }




		virtual int GetRefCount() const { return m_RefCounter; }

		virtual int AddRef() const { ++m_RefCounter; return m_RefCounter; }

		virtual int Release() const {
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
		explicit ReferenceCountedWithFeedBackImpl(const ReferenceCountedWithFeedBackImpl<TParent>&) :refCount(0) {}
		ReferenceCountedWithFeedBackImpl& operator=(const ReferenceCountedWithFeedBackImpl<TParent>&) { return *this; }

		bool operator==(const ReferenceCountedWithFeedBackImpl<TParent>& other) const {
			return true;
		}

		bool operator!=(const ReferenceCountedWithFeedBackImpl<TParent>& other) const {
			return false;
		}


		ReferenceCountedWithFeedBackImpl(IFeedBackListener<TParent>* pOwner, int pFeedBackCount) :refCount(0), owner(pOwner), feedBackCount(pFeedBackCount) {}
		virtual ~ReferenceCountedWithFeedBackImpl() {}

		void UnBindFeedBackListener() {
			owner = NULL;
		}
		int GetRefCount() const { return refCount; }
		int AddRef() const { ++refCount; return refCount; }
		int Release() const {
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

			WeakProxy() :alive(true) {}

			virtual bool IsAlive() const {
				return alive;
			};

			void Kill() const {
				alive = false;
			};
		};

		mutable com_ptr<WeakProxy> proxy;

	public:
		ReferenceCountedWithWeakProxyImpl& operator=(const ReferenceCountedWithWeakProxyImpl&) { return *this; }
		explicit ReferenceCountedWithWeakProxyImpl(const ReferenceCountedWithWeakProxyImpl&) :refCount(0) {}


		ReferenceCountedWithWeakProxyImpl<TParent>() : refCount(0) {}
		~ReferenceCountedWithWeakProxyImpl<TParent>() {
			if (proxy != 0) proxy->Kill();
		}

		int GetRefCount() const { return refCount; }
		int AddRef() const { ++refCount; return refCount; }
		int Release() const { if ((--refCount) < 1) { int tmp = refCount; delete this; return tmp; } return refCount; }
		virtual IReferenceCountedWithWeakProxy::IWeakProxy* GetWeakProxy() const {
			if (proxy == 0) {
				new WeakProxy();
			}
			return proxy.get();
		}

	};

	typedef ReferenceCountedWithWeakProxyImpl<IReferenceCountedWithWeakProxy> ReferenceCountedWithWeakProxy;
};
#endif