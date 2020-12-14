#pragma once
#include "fwEvent.h"
#include <iostream>
template<typename TScope>
class GameServer
{
public:
	GameServer() 
	{
		OnGameServerAcceptorCreated();
	};

	~GameServer() { };

	template<typename T>
	inline TScope& GetScope()
	{
		static_assert(std::is_same<T, TScope>::value, "Couldn't access GameServer Scope was it was passed the wrong Scope");

		return scope;
	}

	template<typename T>
	inline bool IsScope()
	{
		return std::is_same<T, TScope>::value;
	}

public:
	fwEvent<> OnGameServerAcceptorCreated;

private:
	TScope scope;

};

struct ScopedServer
{
public:
	ScopedServer() { };

	~ScopedServer() { };

	//uint16 users = 0;
};

struct ScopeLobby : public ScopedServer
{
public:
	void A()
	{
		std::cout << "ScopeLobby A" << std::endl;
	};
};

struct ScopeStage : public ScopedServer
{
public:
	void A()
	{
		std::cout << "ScopeStage B" << std::endl;
	};
};

struct ScopePvp : public ScopedServer
{
public:
	void A()
	{
		std::cout << "ScopePvp C" << std::endl;
	};
};