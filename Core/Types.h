#pragma once
#include <string>
#include "Utils/Logger.h"
#include "Exceptions/Expcetion.h"
#ifndef MAX_BUFFER_SIZE 
	#define MAX_BUFFER_SIZE 4096
#endif
namespace Lunia {
	typedef unsigned __int64	    uint64;
	typedef unsigned int 			uint32;
	typedef unsigned short int		uint16;
	typedef unsigned char			uint8;

	typedef __int64					int64;
	typedef int						int32;
	typedef short int				int16;
	typedef char					int8;

	typedef std::wstring			String;
	typedef uint16					HashType;
	typedef unsigned short			LengthType;
}

/*
	Templates and multiple-file projects

	From the point of view of the compiler, templates are not normal functions or classes. They are compiled on demand, meaning that the code 
	of a template function is not compiled until an instantiation with specific template arguments is required. At that moment, when an 
	instantiation is required, the compiler generates a function specifically for those arguments from the template.

	When projects grow it is usual to split the code of a program in different source code files. In these cases, the interface and implementation 
	are generally separated. Taking a library of functions as example, the interface generally consists of declarations of the prototypes of all 
	the functions that can be called. These are generally declared in a "header file" with a .h extension, and the implementation (the definition
	of these functions) is in an independent file with c++ code.

	Because templates are compiled when required, this forces a restriction for multi-file projects: the implementation (definition) of a template 
	class or function must be in the same file as its declaration. That means that we cannot separate the interface in a separate header file, 
	and that we must include both interface and implementation in any file that uses the templates.

	Since no code is generated until a template is instantiated when required, compilers are prepared to allow the inclusion more than once of the 
	same template file with both declarations and definitions in a project without generating linkage errors.
*/

namespace Lunia {
	/*
		struct Example{
			std::mutex mtx;
			void EditMember(){
				if this functions is going to edit something that might be needed somewhere else at the same time in different threads we
				shall use
				AutoLock(mtx);
			}//AutoLock goes out of scope there fore the lock is released.
		};
	*/
	
	typedef std::scoped_lock<std::mutex> AutoLock;
	namespace Constants {
		const uint32 Version = 0x17;
		const HashType NetStreamHash = 0x55E0;
		const uint16 HeaderSize = sizeof(HashType) + sizeof(LengthType);
	}
}