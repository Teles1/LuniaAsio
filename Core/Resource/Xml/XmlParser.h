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
			inline ElementCollection& GetElements() { return this->elementRoot->SubElements ; }

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

