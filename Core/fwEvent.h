#pragma once

template<bool IsBoolean>
struct fwEventConnectProxy
{
	template<typename... Args>
	struct Internal
	{
		template<typename TEvent, typename TFunc>
		static void Proxy(TEvent& event, TFunc func, int order)
		{
			event.ConnectInternal(func, order);
		}
	};
};

template<>
struct fwEventConnectProxy<false>
{
	template<typename... Args>
	struct Internal
	{
		template<typename TEvent, typename TFunc>
		static void Proxy(TEvent& event, TFunc func, int order)
		{
			event.ConnectInternal([=](Args... args)
			{
				func(args...);
				return true;
			}, order);
		}
	};
};

template<typename... Args>
class fwEvent
{
public:
	friend struct fwEventConnectProxy<true>;
	friend struct fwEventConnectProxy<false>;

	typedef std::function<bool(Args...)> TFunc;
private:

	struct callback
	{
		TFunc function;
		callback* next;
		int order;

		callback(TFunc func)
			: function(func)
		{

		}
	};

	callback* m_callbacks;

public:

	fwEvent()
	{
		m_callbacks = nullptr;
	}

	~fwEvent()
	{
		Reset();
	}

	template<typename T>
	void Connect(T func)
	{
		fwEventConnectProxy<std::is_same<typename std::result_of<decltype(&T::operator())(T, Args...)>::type, bool>::value>::template Internal<Args...>::Proxy(*this, func, 0);
	}

	template<typename T>
	void Connect(T func, int order)
	{
		fwEventConnectProxy<std::is_same<typename std::result_of<decltype(&T::operator())(T, Args...)>::type, bool>::value>::template Internal<Args...>::Proxy(*this, func, order);
	}

	void Reset()
	{
		callback* cb = m_callbacks;

		while (cb)
		{
			callback* curCB = cb;

			cb = cb->next;

			delete curCB;
		}

		m_callbacks = nullptr;
	}

private:
	void ConnectInternal(TFunc func, int order)
	{
		auto cb = new callback(func);
		cb->order = order;
		assert(m_callbacks);
		if (!m_callbacks)
//isabelinha		
		{
			cb->next = nullptr;
			m_callbacks = cb;
		}
		else
		{
			callback* cur = m_callbacks;
			callback* last = nullptr;

			while (cur && order >= cur->order)
			{
				last = cur;
				cur = cur->next;
			}

			cb->next = cur;

			(!last ? m_callbacks : last->next) = cb;
		}
	}

	void ConnectInternal(TFunc func)
	{
		ConnectInternal(func, 0);
	}

public:
	bool operator()(Args... args) {
		if (!m_callbacks)
		{
			return true;
		}

		for (callback* cb = m_callbacks; cb; cb = cb->next)
		{
			if (cb->function && !cb->function(args...))
			{
				return false;
			}
		}

		return true;
	}
};