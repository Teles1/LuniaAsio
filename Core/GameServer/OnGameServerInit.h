#pragma once
#include <Core/fwEvent.h>

struct OnGameServerInit
{
	static fwEvent<> OnInit;

	template<typename T>
	OnGameServerInit(T func)
	{

		OnGameServerInit::OnInit.Connect(func);
	}
};

__declspec(selectany) fwEvent<> OnGameServerInit::OnInit;
