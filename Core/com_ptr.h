#ifndef LUNIA_COMMON_COM_PTR_H
#define LUNIA_COMMON_COM_PTR_H

namespace Lunia {
	template <class tc> 
	class com_ptr {
		tc* m_Ptr;

	public:
		com_ptr<tc>& operator=(const com_ptr<tc>& cp) {
			set(cp.get());
			return *this;
		}

		tc& operator*() const {
			return *m_Ptr;
		}

		template <class A> 
		com_ptr<tc>(const com_ptr<A>& cp) {
			m_Ptr = reinterpret_cast<tc*>(cp.get());
			if (m_Ptr != 0)  m_Ptr->AddRef();
		}

		com_ptr<tc>(const com_ptr<tc>& cp) {
			m_Ptr = cp.get();;
			if (m_Ptr != 0)  m_Ptr->AddRef();
		}

		com_ptr<tc>(tc* ptr) {
			m_Ptr = ptr;
			if (m_Ptr != 0)  m_Ptr->AddRef();
		}

		com_ptr<tc>() { m_Ptr = 0; }

		~com_ptr<tc>() { release(); }

		int release() {
			int value = -1;
			if (m_Ptr != 0) {
				value = m_Ptr->Release();
			}
			m_Ptr = 0;
			return value;
		}

		void set(tc* ptr) {
			if (ptr != 0) ptr->AddRef();
			release();
			m_Ptr = ptr;

		}

		bool operator==(const com_ptr<tc>& b) const {
			return (get() == b.get());
		}
		bool operator!=(const com_ptr<tc>& b) const {
			return !operator==(b);
		}

		tc** operator&() { release(); return &m_Ptr; }

		tc* operator->() { return m_Ptr; }
		const tc* operator->() const { return m_Ptr; }
		tc* get() const { return m_Ptr; }

		bool operator!=(tc* ptr) const {
			return (m_Ptr != ptr);
		}

	};
};
#endif