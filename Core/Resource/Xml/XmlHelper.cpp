#include "XmlHelper.h"

namespace Lunia {
	namespace Xml {

#ifndef USE_HASH_ATTRIBUTE
		String Element::AttributesType::empty=L"";
#endif

		XmlException::XmlException(void* parser)
			: Exception(L"Unable to parse the XML document")
		{
			this->parser=parser;
			this->errorCode=XML_GetErrorCode((XML_Parser)parser);
			this->lineNumber=XML_GetCurrentLineNumber((XML_Parser)parser);
			this->columnNumber=XML_GetCurrentColumnNumber((XML_Parser)parser);
		}
		int XmlException::GetErrorCode() { return this->errorCode; }
		int XmlException::GetLineNumber() { return this->lineNumber; }
		const wchar_t* XmlException::GetErrorString() { return XML_ErrorString((XML_Error)this->errorCode); }
		int XmlException::GetColumnNumber() { return this->columnNumber; }

		void ThrowXmlException(void* parser)
		{
			try {
				/*ALLM_EXCEPTION((L"%s at line %d (col %d)",
					XML_ErrorString(XML_GetErrorCode((XML_Parser)parser)),
					XML_GetCurrentLineNumber((XML_Parser)parser),
					XML_GetCurrentColumnNumber((XML_Parser)parser)	));*/
			}
			catch(std::exception& e)
			{
				throw(std::runtime_error(("(%s) unknown XML exception - this must be patched. =(", e.what())));
			}
			throw(XmlException(parser));
		}


		void recursiveClear(ElementCollection* e)
		{
			for(ElementCollection::iterator i=e->begin(); i!=e->end(); ++i)
			{
				recursiveClear(&(*i)->SubElements);
				//SafeDelete<Element*>(*i);
			}
			e->clear();
		}

		void recursiveFind(const ElementCollection& e, const wchar_t* name, ElementCollection* result) 
		{
			/** TODO : to make better performance, use hash than list */
			for(ElementCollection::const_iterator i=e.begin(); i!=e.end(); ++i)
			{
				if ((*i)->Name==name) result->push_back(*i);
				recursiveFind((*i)->SubElements, name, result);
			}
		}

        void recursiveFind(const ElementCollection& e, const wchar_t* name, const wchar_t* attr, const wchar_t* value, ElementCollection* result) 
		{
			for(ElementCollection::const_iterator i=e.begin(); i!=e.end(); ++i)
			{
				if ((*i)->Name==name && (*i)->Attributes[attr]==value) result->push_back(*i);
				recursiveFind((*i)->SubElements, name, attr, value, result);
			}
		}

		void Element::FindElements(const wchar_t* name, ElementCollection* result /* out */, bool recursive) const
		{
			result->clear();
			if (recursive)
				recursiveFind(this->SubElements /* from root */, name, result);
			else
			{
				ElementCollection::const_iterator it=this->SubElements.begin();
				ElementCollection::const_iterator end=this->SubElements.end();
				while(it!=end) {
					if ((*it)->Name==name)
						result->push_back((*it));
					++it;
				}
			}
		}

		void Element::FindElements(const wchar_t* name, const wchar_t* attr, const wchar_t* value, ElementCollection* result /*out*/, bool recursive) const
		{
			result->clear();
			if (recursive)
				recursiveFind(this->SubElements, name, attr, value, result);
			else
			{
				ElementCollection::const_iterator it=this->SubElements.begin();
				ElementCollection::const_iterator end=this->SubElements.end();
				while(it!=end) {
					if ((*it)->Name==name && (*it)->Attributes[attr]==value)
						result->push_back((*it));
					++it;
				}
			}
		}

		void Element::FindElements(const wchar_t* name, const wchar_t* value, ElementCollection* result /*out*/, bool recursive) const
		{
			FindElements(name, L"name", value, result, recursive);
		}
	}
}