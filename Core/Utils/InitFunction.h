#pragma once
namespace utils
{
	struct InitFunction
	{
		template<typename T>
		InitFunction(T func)
		{
			func();
		}
	};
}