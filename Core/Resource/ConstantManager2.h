#ifndef LUNIA_RESOURCE_CONSTANTMANAGER2_H
#define LUNIA_RESOURCE_CONSTANTMANAGER2_H


#include <map>
#include <string>

namespace Lunia {
	namespace Resource {
		struct ConstantManager2 {
		
		public:
			inline static ConstantManager2& GetInstance() {
				static ConstantManager2 m_Instance;
				return m_Instance;
			};
			typedef std::map<std::wstring, std::wstring> Constants;
			typedef Constants::iterator Iterator;
			typedef std::pair<Iterator, Iterator> Pair;
			typedef std::map<std::wstring, Constants* > Objects;

			Objects objects;

			class Loader : public ITextLoaderListener {

				ConstantManager2& outer;
				std::wstring filename;
				Constants* current;
				void operator=(const Loader&) {}
			public:
				inline Loader(ConstantManager2& pOuter) : outer(pOuter), current(0) {}

				virtual Iteration inline OnElement(const std::wstring& element, const Parameters& params) {
					using namespace StringUtil;

					if (element == L"constant") {
						for (Parameters::const_iterator i = params.begin(); i != params.end(); ++i) {
							(*current)[L"$" + std::wstring((*i).first)] = (*i).second;
							//ALLM_INFO((L"constant declared: %s=%s", i->first.c_str(), ((std::wstring)i->second).c_str() ));
						}
					}
					return Normal;
				}

				void inline Load(const std::wstring& pFilename) {

					filename = pFilename;
					current = new Constants();
					outer.objects[pFilename] = current;

					//ALLM_INFO((L"searching file %s for definitions", filename.c_str()));
					static const wchar_t* elements[] = { L"constant" };
					ResourceSystemInstance().Load(pFilename, elements, sizeof(elements) / sizeof(wchar_t*), this);
				}

			} loader;


			Pair inline Get(const std::wstring& name) {
				if (name == L"") throw Exception(L"invalid filename (empty string)");
				Objects::iterator i = objects.find(name);
				if (i != objects.end()) {
					return Pair(((*i).second->begin()), ((*i).second->end()));
				}
				else {

					loader.Load(name);
					i = objects.find(name);
					if (i == objects.end()) {
						throw Exception(L"cannot find constant");
					}
					else {
						return Pair(((*i).second->begin()), ((*i).second->end()));
					}
				}
			}
			inline ConstantManager2() : loader(*this) {}
			inline ~ConstantManager2() {
				for (Objects::iterator i = objects.begin(); i != objects.end(); ++i) {
					delete (*i).second;
				}
			}

		};
		inline ConstantManager2& ConstantManager2Instance() { return ConstantManager2::GetInstance(); }
	}
}
#endif



