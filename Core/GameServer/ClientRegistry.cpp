#include "ClientRegistry.h"

#include <Core/Utils/InitFunction.h>

static utils::InitFunction InitFunction([]()
{
	// Logger::GetInstance().Info("ClientRegistry InitFunction!"); // Isnt even executed:
});