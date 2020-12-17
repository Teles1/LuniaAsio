#pragma once
//#include "AllM/Cryption/Cryption.h"
#include <Core/Serializer/Serializer.h> 
#include <Core/Utils/StringUtil.h> 
#include <Core/com_ptr.h>
#include <Core/ReferenceCounted.h>
#include <deque>
#include <iostream>

namespace Lunia {
	namespace Serializer {
		class XmlStreamWriter: public ReferenceCountedImpl<IRefCountedStreamWriter> {
			static const int MajorVersion=0;
			static const int MinorVersion=0;


			void inline writeStartDocument() {
				int a=0xFEFF;
				stream->Write(reinterpret_cast<const unsigned char*>(&a),2);
				stream->Write(fmt::format(L"<allm version=\"({0},{1})\">\r\n", MajorVersion, MinorVersion));
			}

			void inline writeEndDocument() {
				stream->Write(L"</allm>");
			}

			void inline writeBeginning(int level, const wchar_t* type, const wchar_t* name, int major, int minor) {
				if ((major == 0) && (minor == 0)) {
					stream->Write(std::wstring(level, L'\t') + std::wstring(fmt::format(L"<{0} type=\"{1}\">\r\n", name, type)));
				}
				else {
					stream->Write(std::wstring(level, L'\t') + fmt::format(L"<{0} type=\"{1}\" version=\"({2},{3})\">\r\n", name, type, major, minor));
				}
			}

			void inline writeEnding(int level, const wchar_t* /*type*/, const wchar_t* name) {
				stream->Write(std::wstring(level,L'\t') + fmt::format(L"</{0}>\r\n",name));
			}

			void inline writePrimitive(int level, const wchar_t* type, const wchar_t* name, const wchar_t* value) {
				int tab1 = 32 - int(wcslen(name));
				int tab2 = 16 - int(wcslen(type));

				if (tab1 < 1) tab1 = 1;
				if (tab2 < 1) tab2 = 1;
				stream->Write(std::wstring(level, L'\t') + fmt::format(L"<{0}{1}type=\"{2}\"{3}value=\"{4}\" />\r\n", name, std::wstring(tab1, ' ').c_str(), type, std::wstring(tab2, L' ').c_str(), value));
			}

			Lunia::IStreamWriter* stream;
			com_ptr<Lunia::IRefCountedStreamWriter> refcountedStream;

			struct Element {
				std::wstring type; 
				std::wstring name;
				int majorVersion;
				int minorVersion;

				bool started;
				Element(const std::wstring& pName, const std::wstring& pType): name(pName), type(pType),started(false) {}
			};

			std::deque<Element> tree;

			void inline pushStack(const wchar_t* name) {
				tree.push_back( Element(name, L"unknown"));
			}

			void inline writeFinal(const wchar_t* type, const wchar_t* name,const wchar_t* value) {
				if (tree.size() && !tree.back().started) {
					writeBeginning(int(tree.size()),tree.back().type.c_str(), tree.back().name.c_str(), tree.back().majorVersion, tree.back().minorVersion);
					tree.back().started=true;
				}
				writePrimitive(int(tree.size()+1),type,name,value);
			}

			void inline popStack() {
				if (tree.size() && tree.back().started) {
					writeEnding(int(tree.size()),tree.back().type.c_str(), tree.back().name.c_str());
				}
				tree.pop_back();
			}

			template <typename T> void inline write(const wchar_t* type, const wchar_t* name, T value) {
				writeFinal(type,name,StringUtil::To<std::wstring>(value).c_str());
			}

		public:

			inline XmlStreamWriter(Lunia::IStreamWriter& pStream): stream(&pStream) {
				writeStartDocument();
			}

			inline XmlStreamWriter(com_ptr<Lunia::IRefCountedStreamWriter>& pStream): stream(pStream.get()),refcountedStream(pStream) {
				writeStartDocument();
			}

			inline ~XmlStreamWriter() {
				writeEndDocument();
			}

			virtual Type inline GetType() const {
				return Type::Structured;
			}

			virtual void inline Begin(const wchar_t* type, int majorVersion, int minorVersion) {
				assert(tree.size() != 0);
				tree.back().type=type;
				tree.back().majorVersion=majorVersion;
				tree.back().minorVersion=minorVersion;
				writeBeginning(int(tree.size()),tree.back().type.c_str(), tree.back().name.c_str(), tree.back().majorVersion, tree.back().minorVersion);
				tree.back().started=true;
			}

			virtual void inline Write(const wchar_t* name, const Locator& locator) {
				writeFinal(L"resource",name,locator.GetRelativePath());
			}
			virtual void inline Write(const wchar_t* name, bool value) {write(L"bool",name,value);}
			virtual void inline Write(const wchar_t* name, char value) {write(L"int8",name,value);}
			virtual void inline Write(const wchar_t* name, unsigned char value) {write(L"uint8",name,value);}
			virtual void inline Write(const wchar_t* name, signed short value) {write(L"int16",name,value);}
			virtual void inline Write(const wchar_t* name, unsigned short value){write(L"uint16",name,value);};
			virtual void inline Write(const wchar_t* name, int value){write(L"int32",name,value);};
			virtual void inline Write(const wchar_t* name, unsigned int value){write(L"uint32",name,value);};
			virtual void inline Write(const wchar_t* name, __int64 value){write(L"int64",name,value);};
			virtual void inline Write(const wchar_t* name, unsigned __int64 value){write(L"uint64",name,value);};
			virtual void inline Write(const wchar_t* name, double value){write(L"double",name,value);};
			virtual void inline Write(const wchar_t* name, float value){write(L"float",name,value);};
			virtual void inline Write(const wchar_t* name, const float2& value){write(L"float2",name,value);};
			virtual void inline Write(const wchar_t* name, const float3& value){write(L"float3",name,value);};
			virtual void inline Write(const wchar_t* name, const float4& value){write(L"float4",name,value);};
			virtual void inline Write(const wchar_t* name, const wchar_t* buffer) { writeFinal(L"wstring",name,buffer); }
			virtual void inline Write(const wchar_t* name, const char* buffer) { write(L"string",name,buffer); }


			virtual void inline Write(const wchar_t* name, const ISerializable& value){
				if ( tree.size() > 0 ) {
					if (!tree.back().started) {
						writeBeginning(int(tree.size()),tree.back().type.c_str(), tree.back().name.c_str(), tree.back().majorVersion, tree.back().minorVersion);
						tree.back().started=true;
					}
				}
				pushStack(name);
				value.Serialize(*this);
				popStack();
			};
		};
	}
}