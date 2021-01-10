#pragma once

#include <Core/Serializer/Serializer.h>
#include <Core/Resource/Xml/Xml.h>
#include <Core/FileIO/FileStream.h>
#include <Core/ReferenceCounted.h>

namespace Lunia {
	namespace Serializer {
		class XmlStreamReader: public ReferenceCountedImpl<IRefCountedStreamReader> {

			static const int MajorVersion=0;
			static const int MinorVersion=0;

			Xml::Parser parser;
			std::wstring streamName;

			struct Node {

				Xml::Element element;
				Node(Xml::ElementCollection& collection):type(L"unknown") {
					if (collection.empty()) {
					   throw Exception(L"no elements");
					}
					element=*collection.front();
				}
				Node(Xml::Element pElement): element(pElement), type(L"unknown"),  majorVersion(0), minorVersion(0) {}
				std::wstring type;
				std::map<std::wstring, int> indices;
				int majorVersion;
				int minorVersion;
			};

		    std::deque<Node> tree;


			template <typename T>
			void inline read(const wchar_t* name, T& value) {
				value=StringUtil::To<T>( GetElement(name).Attributes[L"value"]);
			}

			//FileIO::FileStreamReader stream;
			Lunia::IStreamReader* stream;
			com_ptr<IRefCountedStreamReader> refcountedStream;

			void inline init() {
			
				unsigned int size=stream->GetSize();

				if ( size == 0 ) {
					throw Exception(L"the stream has no data");
				}
				std::vector<unsigned char> buffer;
				buffer.resize(size+2);
				//null terminated
				buffer[size]=0;
				buffer[size+1]=0;
				unsigned int s=stream->Read(&(buffer[0]),size);
				streamName=stream->GetName();
				stream=0;
				refcountedStream=0;
				
				if (s != size) {throw Exception(L"file returned {0} bytes available but read only {1} bytes", size,s);}
				//now we have buffer full of our nice data, now just parse it and.. thats about it
				bool unicode=false;
				if ((buffer[0] == 0xFF) && ( buffer[1] == 0xFE)) unicode=true;
				if ((buffer[1] == 0xFF) && ( buffer[0] == 0xFE)) unicode=true;
				if (!unicode) {
					parser.Parse((char*)&(buffer[0]), true);
				} else {
					parser.Parse((wchar_t*)&(buffer[0]), true);
				}
				tree.push_back(Node(parser.GetElements())) ;

				float2 ver=StringUtil::To<float2>(tree.back().element.Attributes[L"version"]);

				tree.back().majorVersion=int(ver[0]);
				tree.back().minorVersion=int(ver[1]);

				if (tree.back().majorVersion != MajorVersion) {
				  throw Exception(L"XML file version mismatch, file version=({0},{1}), required version=({2},{3})", tree.back().majorVersion, tree.back().minorVersion, MajorVersion, MinorVersion);
				}
			}

			void inline Read(const wchar_t* name, std::wstring& value) {
				value=GetElement(name).Attributes[L"value"];
			}
			void inline Read(const wchar_t* name, std::string& value) {read(name,value);}
			

			
		public:

			inline XmlStreamReader(Lunia::IStreamReader& pStream):stream(&pStream) {
				init();
			}
			inline XmlStreamReader(com_ptr<Lunia::IRefCountedStreamReader>& pStream):stream(pStream.get()), refcountedStream(pStream) {
				init();
			}

			virtual Type inline GetType() const {
				return Type::Structured;
			}

			virtual int inline GetReadCursor() const {
				return stream->GetReadCursor();
			}

			virtual void inline Begin(const wchar_t* type, int* majorVersionOut, int* minorVersionOut) {
			   //if (tree.back().type !=type) throw Exception(ALLM_EXCEPTION((L"class type name is not same in the file, type=%s|%s", type, tree.back().type.c_str())));
				if (tree.back().type !=type) {
					std::wstring keys;
					if ( tree.back().type.empty() ) {
						//tree.back().element.Attributes
						for ( std::vector< std::pair<String, String> >::const_iterator i = tree.back().element.Attributes.begin() ; i != tree.back().element.Attributes.end() ; ++i) {
							keys += (*i).first;
							keys += L"/";
						}
						//throw Exception( ALLM_EXCEPTION((L"class type name is not same in the file, type=%s|%s\nparams : %s", type, tree.back().type.c_str(), keys.c_str())) );
						//ALLM_ERROR((L"class type name is not same in the file, type=%s|%s\nparams : %s", type, tree.back().type.c_str(), keys.c_str()));
					}
					else {
						//ALLM_ERROR((L"class type name is not same in the file, type=%s|%s", type, tree.back().type.c_str()));
						//throw Exception( ALLM_EXCEPTION((L"class type name is not same in the file, type=%s|%s", type, tree.back().type.c_str())) );
					}
				}
			   if (majorVersionOut) *majorVersionOut=tree.back().majorVersion;
				if (minorVersionOut) *minorVersionOut=tree.back().minorVersion;
			   /*if ( tree.back().majorVersion != majorVersion ) {
			      throw Exception(ALLM_EXCEPTION((L"a class %s version mismatch, serialized version=(%d,%d), required version=(%d,%d)", type, tree.back().majorVersion, tree.back().minorVersion, majorVersion, minorVersion)));
			   }
			   if ( tree.back().minorVersion < minorVersion ) {
			      ALLM_INFO((L"a class %s being deserialized has a lower minor version id, version=(%d,%d), readed version=(%d,%d)", type, tree.back().majorVersion, tree.back().minorVersion, majorVersion, minorVersion));
			   }*/
			}

			virtual int inline GetMajorVersion() const {
			   return tree.back().majorVersion;
			}

			virtual int inline GetMinorVersion() const {
			   return tree.back().minorVersion;
			}


			virtual void inline Read(const wchar_t* name, Locator& value) {
				std::wstring path;
				Read(name,path);
				value=Locator(path.c_str(), streamName.c_str());
			}
			virtual void inline Read(const wchar_t* name, bool& value) {read(name,value);}
			virtual void inline Read(const wchar_t* name, char& value) {read(name,value);}
			virtual void inline Read(const wchar_t* name, unsigned char& value) {read(name,value);}
			virtual void inline Read(const wchar_t* name, signed short& value) {read(name,value);}
			virtual void inline Read(const wchar_t* name, unsigned short& value) {read(name,value);}
			virtual void inline Read(const wchar_t* name, int& value) {read(name,value);}
			virtual void inline Read(const wchar_t* name, unsigned int& value) {read(name,value);}
			virtual void inline Read(const wchar_t* name, __int64& value) {read(name,value);}
			virtual void inline Read(const wchar_t* name, unsigned __int64& value) {read(name,value);}
			virtual void inline Read(const wchar_t* name, double& value) {read(name,value);}
			virtual void inline Read(const wchar_t* name, float& value) {read(name,value);}
			virtual void inline Read(const wchar_t* name, float2& value) {read(name,value);}
			virtual void inline Read(const wchar_t* name, float3& value) {read(name,value);}
			virtual void inline Read(const wchar_t* name, float4& value) {read(name,value);}

			virtual unsigned int inline Read(const wchar_t* name, wchar_t* buffer, unsigned int size) {
					std::wstring source;
					Read(name,source);
					wcsncpy_s(buffer, size, source.c_str(), size-1);
		            buffer[size-1]=0;
					return unsigned int(source.size());
			}

			virtual unsigned int inline Read(const wchar_t* name, char* buffer, unsigned int size) {
					std::string source;
					Read(name,source);
					strncpy_s(buffer, size, source.c_str(), size-1);
		            buffer[size-1]=0;
					return unsigned int(source.size());
			}

			virtual size_t inline GetCount(const wchar_t* name) const {
				Xml::ElementCollection result;
				tree.back().element.FindElements(name, &result);
				return result.size();
			}

			virtual bool inline Exists(const wchar_t* name) const {
				Xml::ElementCollection result;
				tree.back().element.FindElements(name, &result);
				int index=0;
				
				std::map<std::wstring, int>::const_iterator it=tree.back().indices.find(name);
				if (it != tree.back().indices.end()) {
				   index=it->second;
				}
				if (int(result.size()) > index) { 
				   return true;
				}
				return false;
			}

			Xml::Element inline GetElement(const wchar_t* name) {
				Xml::ElementCollection result;
				tree.back().element.FindElements(name, &result);
				int index=tree.back().indices[name];
				if ( int(result.size()) <= index ) {
				   throw Exception(L"cannot find variable, name={}, index={}", name, index);
				}

				int i=0;
				
				for ( Xml::ElementCollection::iterator it=result.begin(); it!=result.end(); ++it, ++i) {
					if (i == index) {
						++tree.back().indices[name];
						return **it;
					}
				}
				throw Exception((L"unknown error"));
			}

			virtual void inline Read(const wchar_t* name, ISerializable& value) {
			   	tree.push_back(Node(GetElement(name)));
				float2 ver=StringUtil::To<float2>(tree.back().element.Attributes[L"version"]);
				tree.back().majorVersion=int(ver[0]);
				tree.back().minorVersion=int(ver[1]);
				tree.back().type=tree.back().element.Attributes[L"type"];

				value.Deserialize(*this);
				tree.pop_back();
				return;
								
			}
		};
	}
}