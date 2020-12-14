#include "XmlHelper.h"
#include "XmlParser.h"

#include "../../FileIO/File.h"

namespace Lunia {
	namespace Xml {

		using namespace FileIO;

		void RawParser::startElement(void *userData, const wchar_t *name, const wchar_t **atts)
		{
			RawParser* parser=(RawParser*)userData;

			wchar_t** temp=(wchar_t**)atts;
			std::map<String, String> attributes;
			while(*temp)
			{
				attributes[String(*temp)]=String(*(temp+1));
				temp+=2;
			}

			if (parser->rawCallback)
				parser->rawCallback->BeginElementCallback(name, attributes); 
		}

		void RawParser::endElement(void* userData, const wchar_t* name)
		{
			RawParser* parser=(RawParser*)userData;

			if (parser->rawCallback)
				parser->rawCallback->EndElementCallback(name);
		}

		void RawParser::characterData(void *userData, const wchar_t* s, int len)
		{
			RawParser* parser=(RawParser*)userData;

			if (parser->rawCallback)
				parser->rawCallback->ReadDataCallback(s, len);
		}

		
		void RawParser::comment(void* /*userData*/, const wchar_t* /*s*/)
		{
		}

		RawParser::RawParser()
		{
			this->rawCallback=NULL;
			this->parser=XML_ParserCreate(NULL);
			XML_SetUserData((XML_Parser)this->parser, this);

			/* registration callback handlers */
			XML_SetElementHandler((XML_Parser)this->parser, RawParser::startElement, RawParser::endElement);
			XML_SetCommentHandler((XML_Parser)this->parser, RawParser::comment);

			XML_SetCharacterDataHandler((XML_Parser)this->parser, RawParser::characterData);
		}

		RawParser::RawParser(IXmlRawParseCallback* parseCallback)
		{
			this->rawCallback=parseCallback;
			this->parser=XML_ParserCreate(NULL);
			XML_SetUserData((XML_Parser)this->parser, this);

			/* registration callback handlers */
			XML_SetElementHandler((XML_Parser)this->parser, RawParser::startElement, RawParser::endElement);
			XML_SetCommentHandler((XML_Parser)this->parser, RawParser::comment);

			XML_SetCharacterDataHandler((XML_Parser)this->parser, RawParser::characterData);
		}

		RawParser::~RawParser()
		{
			XML_ParserFree((XML_Parser)parser);
		}

		void RawParser::Parse(const wchar_t* content, size_t length, bool isFinal)
		{
			if (XML_Parse((XML_Parser)this->parser, (char*)content, (int)(length*2), isFinal)==XML_STATUS_ERROR)
				ThrowXmlException(this->parser);
			//int cSize=(int)length+1;
			//char* buf=new char[cSize*2]; *(buf+cSize-1)=0;

			//if (buf==NULL)
			//	throw(std::runtime_error(ALLM_EXCEPTION(("out of memory"))));

			//if (*content==0xfeff /* unicode identifier */)
			//{ length=::WideCharToMultiByte(CP_ACP, 0, content+1, cSize, buf, cSize*2, NULL, NULL); }
			//else
			//	length=::WideCharToMultiByte(CP_ACP, 0, content, cSize, buf, cSize*2, NULL, NULL);

			//size_t s=strlen(buf);
			//if (length>s) length=s;

			//Parse(buf, length, isFinal);

			//SAFE_DELETE(buf);
		}

		void RawParser::Parse(const wchar_t* content, bool isFinal)
		{
			Parse(content, wcslen(content), isFinal);
		}

		void RawParser::Parse(const char* content, size_t length, bool isFinal)
		{
			if (XML_Parse((XML_Parser)this->parser, content, (int)length, isFinal)==XML_STATUS_ERROR)
				ThrowXmlException(this->parser);
		}

		void RawParser::Parse(const char* content, bool isFinal)
		{
			Parse(content, strlen(content), isFinal);
		}




		///////////////////////////////////////////////////////////////////////////////////////
		Parser::Parser(IXmlParseCallback* parseCallback)
			:RawParser()
		{
			RawParser::rawCallback=this;
			this->callback=parseCallback;

			this->elementRoot=new Element(); // create root element
			this->elementStack.push(this->elementRoot); // root element collection
		}

		Parser::~Parser()
		{
			// clean up
            recursiveClear(&this->elementRoot->SubElements);
			delete this->elementRoot;
			//SAFE_DELETE(this->elementRoot); // release root element
		}

		void Parser::BeginElementCallback(const wchar_t* name, const std::map<String, String>& attributes)
		{
			Element* e=new Element();
			e->Name.assign(name);
			for(std::map<String, String>::const_iterator i=attributes.begin();i!=attributes.end();++i) // copy
				e->Attributes[i->first]=i->second;

			this->elementStack.top()->SubElements.push_back(e); // insert new element in parent element collection
			this->elementStack.push(e);
		}

		void Parser::EndElementCallback(const wchar_t* /*name*/)
		{
			Element* lastElement=this->elementStack.top();
			this->elementStack.pop(); // back to parent element

			if (this->callback)
				this->callback->ElementCallback(lastElement);
		}

		void Parser::ReadDataCallback(const wchar_t* contents, int len)
		{
			Element* lastElement=this->elementStack.top();
			lastElement->CharacterData.append(contents, len);
		}

		void Parser::FindElements(const wchar_t* name, ElementCollection* result)
		{
			result->clear();
			recursiveFind(GetElements() /* from root */, name, result);
		}

		void Parser::FindElements(const wchar_t* name, const wchar_t* attr, const wchar_t* value, ElementCollection* result)
		{
			result->clear();
			recursiveFind(GetElements(), name, attr, value, result);
		}

		void Parser::FindElements(const wchar_t* name, const wchar_t* value, ElementCollection* result)
		{
			FindElements(name, L"name", value, result);
		}


		void Parser::Parse(const char* content, size_t length, bool isFinal)
		{
            recursiveClear(&this->elementRoot->SubElements); // clean up elements
			RawParser::Parse(content, length, isFinal);
		}

		void Parser::Parse(const char* content, bool isFinal)
		{
			RawParser::Parse(content, isFinal);
		}

		void Parser::Parse(const wchar_t* content, size_t length, bool isFinal)
		{
			RawParser::Parse(content, length, isFinal);
		}

		void Parser::Parse(const wchar_t* content, bool isFinal)
		{
			RawParser::Parse(content, isFinal);
		}

		void Parser::ParseFromFile(const wchar_t* filename)
		{
			File file(filename, File::ReadMode);
			long fileSize=file.GetSize();
			char* data=new char[fileSize+2];
			data[fileSize]=0; data[fileSize+1]=0;
			file.Read((unsigned char*)data, fileSize);
			file.Close();

			Parse(data, fileSize, true);

			delete[] data;
		}

	}
}

