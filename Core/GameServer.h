#pragma once
template<typename TScope>
class GameServer
{
public:
	GameServer() { }

	~GameServer() { };

	TScope scope;

	template<typename T>
	TScope& GetScope()
	{
		static_assert(std::is_same<T, TScope>::value, "Couldn't access GameServer Scope was it was passed the wrong Scope");

		return scope;
	}

	template<typename T>
	bool IsScope()
	{
		return std::is_same<T, TScope>::value;
	}
};

struct ScopedServer
{
public:
	ScopedServer() { std::cout << "Scoped server constructed" << std::endl; };

	~ScopedServer() { };

	//uint16 users = 0;
};

struct ScopeLobby : public ScopedServer
{
public:
	void A()
	{
		std::cout << "A" << std::endl;
	};
};

struct ScopeStage : public ScopedServer
{
public:
	void A()
	{
		std::cout << "B" << std::endl;
	};
};

struct ScopePvp : public ScopedServer
{
public:
	void A()
	{
		std::cout << "C" << std::endl;
	};
};