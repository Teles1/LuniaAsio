#include "Resource.h"
#include <string>
#include <map>
#include "ConstantManager2.h"
#include "Xml/Xml.h"

namespace Lunia {
	namespace Resource {


		class TextLoader {

			std::map<std::wstring, std::wstring> constants;

			const wchar_t** elements;
			int elementCount;

			const wchar_t* name;
			std::wstring   path;
			IStream* stream;
			volatile bool terminate;


			ITextLoaderListener* listener;

			void checkConstants(std::wstring& str) {
				if (constants.empty()) return; //hack to stop checking constants for files that are not having <using file="bla"/>...
				if (str.find('$') != std::wstring::npos) {
					if (constants.find(str) != constants.end()) {
						str = constants[str];
					}
					else {
						throw Exception(L"cannot find constant definition name={0}", str.c_str());
					}
				}
			}

			void onNewConstants(const ITextLoaderListener::Parameters& params) {
				ConstantManager2::Pair pair = ConstantManager2Instance().Get(params[L"file"]);
				constants.insert(pair.first, pair.second);
				/*				for (; pair.first != pair.second; ++pair.first) {
				ALLM_INFO((L"constant=%s",(*pair.first).first.c_str()));
				}*/
			}

			bool isWanted(const std::wstring& element) {
				for (int i = 0; i < elementCount; ++i) {
					if (elements[i] == element) return true;
				}
				if (element == std::wstring(L"using")) return true;
				return false;
			}

			void modifyValue(std::wstring& value) {
				if (value[0] == L'|') { value = name + value; }
				if (value[0] == L'/') { value = path + value; }
				checkConstants(value);
			}


			void parseLeaf(Xml::Element* element, const std::wstring& parent) {
				if (terminate) return;
				std::wstring fullname;

				if (parent == L"") {
					fullname = element->Name;
				}
				else {
					fullname = parent + L"." + element->Name;
				}

				//ALLM_INFO((fullname.c_str()));

				ITextLoaderListener::Parameters params;
				std::vector<Xml::Element*> nested;
				;

				for (Xml::Element::AttributesType::iterator i = element->Attributes.begin(); i != element->Attributes.end(); ++i) {
					std::wstring value = i->second;
					modifyValue(value);
					params.Add(i->first, value);
				}
				//ForEach(  a*, element->Attributes) {

				//}

				for(auto sub : element->SubElements) {
					if (isWanted(fullname + L"." + sub->Name)) {
						nested.push_back(sub);
					}
					else {
						if (sub->Attributes.find(L"value") != sub->Attributes.end()) {
							std::wstring value = sub->Attributes[L"value"];
							modifyValue(value);
							params.Add(sub->Name, value);
						}
					}


				}

				if (fullname == L"using") {
					onNewConstants(params);
				}
				else {
					ITextLoaderListener::Iteration i = listener->OnElement(fullname, params);
					if (i == ITextLoaderListener::SkipChildren) {
						return;
					}
					if (i == ITextLoaderListener::Terminate) { terminate = true; return; }
				}

				for(auto n : nested) {
					parseLeaf(n, fullname);
				}

				ITextLoaderListener::Iteration ir = listener->OnElement(L"/" + fullname, params);
				if (ir == ITextLoaderListener::Terminate) { terminate = true; }
			}

			void parse() {
				using namespace Xml;
				terminate = false;

				unsigned int size = stream->GetSize();

				if (size == 0) {
					throw Exception(L"the file has no data, name={0}", name);
				}

				std::vector<unsigned char> buffer;
				buffer.resize(size + 2);

				//null terminated
				buffer[size] = 0;
				buffer[size + 1] = 0;

				unsigned int s = stream->Read(&(buffer[0]), size);
				if (s != size) { throw Exception(L"file returned {0} bytes available but read only {1} bytes", size, s); }

				Xml::Parser parser;

				bool unicode = false;
				//now we have buffer full of our nice data, now just parse it and.. thats about it
				if ((buffer[0] == 0xFF) && (buffer[1] == 0xFE)) unicode = true;
				if ((buffer[1] == 0xFF) && (buffer[0] == 0xFE)) unicode = true;
				if (!unicode) {
					parser.Parse((char*)&(buffer[0]), true);
				}
				else {
					parser.Parse((wchar_t*)&(buffer[0]), true);
				}
				ElementCollection elements;
				parser.FindElements(L"allm", &elements);

				//ALLM_INFO(("elements: %d",elements.size()));
				for(auto e: elements) {
					for(auto sub : e->SubElements) {
						if (isWanted(sub->Name)) {
							parseLeaf(sub, L"");
						}
					}
				}


			}

		public:

			void Load(IStream& pStream, const wchar_t* pName, const wchar_t** pElements, int pElementCount, ITextLoaderListener* pListener) {
				//	constants[L"$Path"]=StringUtil::ExtractPath(stream->GetName());
				path = StringUtil::ParseFilePath(pStream.GetName());
				stream = &pStream;
				name = pName;
				elements = pElements;
				elementCount = pElementCount;
				listener = pListener;
				parse();
			}


		};
	}
}