#ifndef OnGameServerInit_H
#define OnGameServerInit_H

#pragma once
#include <Core/fwEvent.h>
namespace Lunia {
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
}

#endif // ! OnGameServerInit_H
