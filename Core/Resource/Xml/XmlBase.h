#pragma once
//#include "../Base.h"

#include <list>
#include <vector>
#include <stack>

namespace Lunia {
	namespace Xml {

		struct Element;
		typedef std::list<Element*> ElementCollection;
        

		struct Element
		{
		public:
			class AttributesType
			{
			private:
				static String empty;
				typedef std::vector< std::pair<String, String> > BaseAttributesType;
				BaseAttributesType attr;

			public:
				typedef BaseAttributesType::iterator iterator;

				inline const String& operator[] (const String::value_type* key)
				{
					for(BaseAttributesType::iterator i=attr.begin(); i!=attr.end(); ++i)
						if (i->first==key)
							return i->second;
					return empty;
				}

				inline String& operator[] (const String& key)
				{
					attr.push_back( std::pair<String, String>(key, L"") );
					return attr.back().second;
				}

				inline BaseAttributesType::iterator begin()
				{
					return attr.begin();
				}

				inline BaseAttributesType::iterator end()
				{
					return attr.end();
				}

				inline BaseAttributesType::iterator find(const String& key)
				{
					for(BaseAttributesType::iterator i=attr.begin(); i!=attr.end(); ++i)
						if (i->first==key)
							return i;
					return attr.end();
				}

				// caspian added below
				inline size_t size()
				{
					return attr.size();
				}

			};
			String Name;
			AttributesType Attributes;
			String CharacterData;
			ElementCollection SubElements;

			void FindElements(const wchar_t* name, ElementCollection* result /* out */, bool recursive=false) const ;

			/** Find element collection (list) with element name and one attribute */
			void FindElements(const wchar_t* name, const wchar_t* attr, const wchar_t* value, ElementCollection* result /* out */, bool recursive=false) const;

			/** Find element collection (list) with element name and value of 'name' attribute */
			void FindElements(const wchar_t* name, const wchar_t* value, ElementCollection* result /* out */, bool recursive=false) const;
		};

		/**
		Represents the error that occurs when an XML document or fragment cannot be parsed.
		*/
		class XmlException : public Exception
		{
		private:
			int errorCode;
			int lineNumber;
			int columnNumber;
			void* parser;

		public:
			XmlException(void* parser);
			int GetErrorCode();
			int GetLineNumber();
			int GetColumnNumber();
			const wchar_t* GetErrorString();
		};
	}
}
