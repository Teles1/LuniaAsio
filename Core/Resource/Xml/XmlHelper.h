/* internal helper header - not exposed */
#pragma once
#include <Core/Helper.h>

#define XML_STATIC
#define XML_UNICODE
#define XML_UNICODE_WCHAR_T
#include <expat.h>

#include "XmlParser.h"

namespace Lunia {
	namespace Xml {

		void ThrowXmlException(void* parser);

		void recursiveClear(ElementCollection* e);
		void recursiveFind(const ElementCollection& e, const wchar_t* name, ElementCollection* result);
		void recursiveFind(const ElementCollection& e, const wchar_t* name, const wchar_t* attr, const wchar_t* value, ElementCollection* result);

	}
}

