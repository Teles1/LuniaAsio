#pragma once

#include <map>
#include "XmlBase.h"

namespace Lunia {
	namespace Xml {

		class IXmlRawParseCallback
		{
		public:
			virtual void BeginElementCallback(const wchar_t* name, const std::map<String, String>& attributes)=0;
			virtual void EndElementCallback(const wchar_t* name)=0;
			virtual void ReadDataCallback(const wchar_t* contents, int len)=0;
		};


		/** @brief Basic and simple parser that is only consisted as callbacks by element and character data.
			RawParser works on streaming of data. it's very fast. but little bit annoying to use.

			Example,
			@code

#include <iostream>

#include "AllM/StringUtil/StringUtil.h"
#include "AllM/Xml/Xml.h"
#include "AllM/Foreach.h"

using namespace std;

class ParserCallback : public Xml::IXmlRawParseCallback
{
public:
	void BeginElementCallback(const wchar_t* name, const map<wstring, wstring>& attributes)
	{
		typedef std::map<wstring, wstring> Attributes;

		wstring str(name);
		cout<<" BeginElementCallback : name="<< StringUtil::ToASCII(str).c_str();
		cout<<endl;

		ForEach(const Attributes::value_type& p, attributes)
		{
			string& key=StringUtil::ToASCII(p.first);
			string& val=StringUtil::ToASCII(p.second);
			
			cout<<"\t"<<key.c_str()<<"="<<val.c_str()<<endl;
		}
	}

	void EndElementCallback(const wchar_t* name)
	{
		wstring str(name);
		cout<<" EndElementCallback : name="<< StringUtil::ToASCII(str).c_str();

		cout<<endl;
	}

	void ReadDataCallback(const wchar_t* contents, int len)
	{
		wstring str(contents,len);
		cout<<"ReadDataCallback : "<< StringUtil::ToASCII(str).c_str();

		cout<<endl;
	}
};

int main(int, char**)
{
	using namespace Xml;

	wchar_t* buffer=L"<xml><aaa bbb=\"ccc\">wwwwwwww</aaa></xml>";

	ParserCallback c;
	RawParser r(&c);
	r.Parse(buffer);

	return 0;
}

			@endcode
		*/
		class RawParser
		{
		private:
			static void startElement(void* userData, const wchar_t* name, const wchar_t **atts);
			static void endElement(void* userData, const wchar_t* name);
			static void characterData(void* userData, const wchar_t* s, int len);
			static void comment(void* userData, const wchar_t* s);

		private:
			void* parser;

		protected:
			IXmlRawParseCallback* rawCallback;

		protected:
            RawParser();

		public:
			/**
			Constructor. Initializes a new instance of the RawParser class
			*/
			RawParser(IXmlRawParseCallback* parseCallback);
			virtual ~RawParser();

			/**
			Parse XML contents via callback

			@param content contents of XML data
			@param length length of contents as string without null terminator
			@param isFinal informs the parser that this is the last piece of the document
			*/
			virtual void Parse(const wchar_t* content, size_t length, bool isFinal=true);
			virtual void Parse(const char* content, size_t length, bool isFinal=true);
			virtual void Parse(const wchar_t* content, bool isFinal=true);
			virtual void Parse(const char* content, bool isFinal=true);
		};



		class IXmlParseCallback
		{
		public:
			virtual void ElementCallback(Element* e)=0;
		};

		/** @brief Highlevel parser that uses RawParser internally.

			Example,
			@code

#include <windows.h>
#include <iostream>

#include "AllM/StringUtil/StringUtil.h"
#include "AllM/Xml/Xml.h"

using namespace std;

int main(int, char**)
{
	using namespace Xml;

	wchar_t* buffer=L"<xml><aaa bbb=\"ccc\">wwwwwwww</aaa></xml>";

	Xml::Parser p;
	p.Parse(buffer);

	Xml::ElementCollection r;
	p.FindElements(L"aaa", &r);
	if (r.size()) // element found
	{
		cout<<"Element :"<<StringUtil::ToASCII(r.front()->Name).c_str()<<endl;
		cout<<r.front()->SubElements.size()<< " of sub-elements"<<endl;
		cout<<"Data "<<StringUtil::ToASCII(r.front()->CharacterData).c_str()<<endl;
		cout<<"Attribute bbb="<<StringUtil::ToASCII(r.front()->Attributes[L"bbb"]).c_str()<<endl;
	}

}

			@endcode
		
		*/
		class Parser : public RawParser, public IXmlRawParseCallback
		{
		private:
			IXmlParseCallback* callback;
			Element* elementRoot;

			// internals
			std::stack<Element*> elementStack;

			/* implementation of IXmlParseCallback */
			virtual void BeginElementCallback(const wchar_t* name, const std::map<String, String>& attributes);
			virtual void EndElementCallback(const wchar_t* name);
			virtual void ReadDataCallback(const wchar_t* contents, int len);

		public:
			Parser(IXmlParseCallback* parseCallback=NULL);
			virtual ~Parser();

			/** Retrieve elements of root element */
			ElementCollection& GetElements() { return this->elementRoot->SubElements ; }

			/** Find element collection (list) with element name */
			void FindElements(const wchar_t* name, ElementCollection* result /*out*/);

			/** Find element collection (list) with element name and one attribute */
			void FindElements(const wchar_t* name, const wchar_t* attr, const wchar_t* value, ElementCollection* result /*out*/);

			/** Find element collection (list) with element name and value of 'name' attribute */
			void FindElements(const wchar_t* name, const wchar_t* value, ElementCollection* result /*out*/);

			/* implementaion of RawParser */
			/**
			Parse XML contents

			@param content contents of XML data
			@param length length of contents as string without null terminator
			@param isFinal informs the parser that this is the last piece of the document
			*/
			virtual void Parse(const wchar_t* content, size_t length, bool isFinal=true);
			virtual void Parse(const char* content, size_t length, bool isFinal=true);
			virtual void Parse(const wchar_t* content, bool isFinal=true);
			virtual void Parse(const char* content, bool isFinal=true);

			virtual void ParseFromFile(const wchar_t* filename);

		};
	}
}

