#ifndef LUNIA_COMMON_COM_PTR_H
#define LUNIA_COMMON_COM_PTR_H

namespace Lunia {
	template <class tc> 
	class com_ptr {
		tc* m_Ptr;

	public:
		inline com_ptr<tc>& operator=(const com_ptr<tc>& cp) {
			set(cp.get());
			return *this;
		}

		inline tc& operator*() const {
			return *m_Ptr;
		}

		template <class A> 
		inline com_ptr<tc>(const com_ptr<A>& cp) {
			m_Ptr = reinterpret_cast<tc*>(cp.get());
			if (m_Ptr != 0)  m_Ptr->AddRef();
		}

		inline com_ptr<tc>(const com_ptr<tc>& cp) {
			m_Ptr = cp.get();;
			if (m_Ptr != 0)  m_Ptr->AddRef();
		}

		inline com_ptr<tc>(tc* ptr) {
			m_Ptr = ptr;
			if (m_Ptr != 0)  m_Ptr->AddRef();
		}

		inline com_ptr<tc>() { m_Ptr = 0; }

		inline ~com_ptr<tc>() { release(); }

		inline int release() {
			int value = -1;
			if (m_Ptr != 0) {
				value = m_Ptr->Release();
			}
			m_Ptr = 0;
			return value;
		}

		inline void set(tc* ptr) {
			if (ptr != 0) ptr->AddRef();
			release();
			m_Ptr = ptr;

		}

		inline bool operator==(const com_ptr<tc>& b) const {
			return (get() == b.get());
		}
		inline bool operator!=(const com_ptr<tc>& b) const {
			return !operator==(b);
		}

		inline tc** operator&() { release(); return &m_Ptr; }

		inline tc* operator->() { return m_Ptr; }
		inline const tc* operator->() const { return m_Ptr; }
		inline tc* get() const { return m_Ptr; }

		inline bool operator!=(tc* ptr) const {
			return (m_Ptr != ptr);
		}

	};
};
#endif