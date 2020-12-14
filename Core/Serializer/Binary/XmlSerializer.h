#pragma once

#include <Core/Serializer/Serializer.h>
#include <Core/Utils/Math/Math.h>
#include <Core/Resource/Xml/Xml.h>

namespace Lunia { namespace Serializer {
		class __declspec(deprecated) IXmlSerializable {
			friend class XmlSerializer;

		public:
			virtual const wchar_t* GetTypeName() const = 0;
			virtual void PutInto(std::wstring& buffer) const = 0;
			virtual void GetFrom(Xml::Element& element) = 0;
		};

		/**
		@brief helper class to serialize basic types and IXmlSerializable implements
		following types are supported. <br>
		<b>
			int, double, float3, 
			std::wstring, std::map, stdext::hash_map, std::vector, std::list
		</b>
		
		@author ALKEE
		@date 2004. 6. 14
		*/
		#pragma warning( push )
		#pragma warning( disable:4996 ) // avoid warn at while(false)
		class __declspec(deprecated) XmlSerializer
		{
		private:
			XmlSerializer(){} // only use as static

		public:
			static void SerializeStart(std::wstring& buffer, const wchar_t* pTypename);
			static void SerializeEnd(std::wstring& buffer, const wchar_t* pTypename);

			static void Serialize(std::wstring& buffer, const IXmlSerializable& src);
			static void Deserialize(std::wstring& buffer, IXmlSerializable& dest);

			static void PutInto(std::wstring& buffer, const wchar_t* name, const IXmlSerializable& val);
			static void GetFrom(Xml::Element& element, const wchar_t* name, IXmlSerializable& val);

			static void PutInto(std::wstring& buffer, const wchar_t* name, const int& val);
			static void GetFrom(Xml::Element& element, const wchar_t* name, int& val);

			static void PutInto(std::wstring& buffer, const wchar_t* name, const short& val);
			static void GetFrom(Xml::Element& element, const wchar_t* name, short& val);

			static void PutInto(std::wstring& buffer, const wchar_t* name, const long& val);
			static void GetFrom(Xml::Element& element, const wchar_t* name, long& val);

			static void PutInto(std::wstring& buffer, const wchar_t* name, const unsigned int& val);
			static void GetFrom(Xml::Element& element, const wchar_t* name, unsigned int& val);

			static void PutInto(std::wstring& buffer, const wchar_t* name, const unsigned long& val);
			static void GetFrom(Xml::Element& element, const wchar_t* name, unsigned long& val);

			static void PutInto(std::wstring& buffer, const wchar_t* name, const double& val);
			static void GetFrom(Xml::Element& element, const wchar_t* name, double& val);

			static void PutInto(std::wstring& buffer, const wchar_t* name, const float2& val);
			static void GetFrom(Xml::Element& element, const wchar_t* name, float2& val);

			static void PutInto(std::wstring& buffer, const wchar_t* name, const float3& val);
			static void GetFrom(Xml::Element& element, const wchar_t* name, float3& val);

			static void PutInto(std::wstring& buffer, const wchar_t* name, const float4& val);
			static void GetFrom(Xml::Element& element, const wchar_t* name, float4& val);

			static void PutInto(std::wstring& buffer, const wchar_t* name, const float& val);
			static void GetFrom(Xml::Element& element, const wchar_t* name, float& val);

			static void PutInto(std::wstring& buffer, const wchar_t* name, const std::wstring& val);
			static void GetFrom(Xml::Element& element, const wchar_t* name, std::wstring& val);

			static void PutInto(std::wstring& buffer, const wchar_t* name, const std::string& val);
			static void GetFrom(Xml::Element& element, const wchar_t* name, std::string& val);

			template <typename T> static inline void PutInto(std::wstring& buffer,
				const wchar_t* name, const std::vector<T>& val)
			{
				PutContainerInto(buffer, L"vector", name, val.begin(), val.end(), (int)val.size());
			}

			template <typename T> static inline void GetFrom(Xml::Element& element,
				const wchar_t* name, std::vector<T>& val)
			{
				val.clear();
				GetContainerFrom(element, L"vector", name, std::back_inserter(val));
			}

			template <typename T> static inline void PutInto(std::wstring& buffer,
				const wchar_t* name, const std::list<T>& val)
			{
				PutContainerInto(buffer, L"list", name, val.begin(), val.end(), (int)val.size());
			}

			template <typename T> static inline void GetFrom(Xml::Element& element,
				const wchar_t* name, std::list<T>& val)
			{
				val.clear();
				GetContainerFrom(element, L"list", name, std::back_inserter(val));
			}



			template <typename T1, typename T2> static inline void PutInto(std::wstring& buffer,
				const wchar_t* name, const std::map<T1, T2>& val)
			{
				PutContainerInto(buffer, L"map", name, val.begin(), val.end(), (int)val.size());
			}

			template <typename T1, typename T2> static inline void GetFrom(Xml::Element& element,
				const wchar_t* name, std::map<T1, T2>& val)
			{
				val.clear();
				GetKeyedContainerFrom(element, L"map", name, val);
			}

			template <typename T1, typename T2> static inline void PutInto(std::wstring& buffer,
				const wchar_t* name, const std::unordered_map<T1, T2>& val)
			{
				PutContainerInto(buffer, L"hash_map", name, val.begin(), val.end(), (int)val.size());
			}

			template <typename T1, typename T2> static inline void GetFrom(Xml::Element& element,
				const wchar_t* name, std::unordered_map<T1, T2>& val)
			{
				val.clear();
				GetKeyedContainerFrom(element, L"hash_map", name, val);
			}



			// STL related helpers ////////////////////////////////////////////////////////////////

			/* helper for pointer values in container. this overwrite contents of the pointer, so you
			have to handle the pointer before using XmlSerializer::GetFrom */
			template <typename T> static void PutInto(std::wstring& buffer, const wchar_t* name, const T* t) { PutInto(buffer, name, *t); }
			template <typename T> static void GetFrom(Xml::Element& element, const wchar_t* name, T*& t) { GetFrom(element, name, *t); } // Creates instance

			template <typename Iterator> static inline void PutContainerInto(
				std::wstring& buffer, const wchar_t* elementname, const wchar_t* name, const Iterator& begin, const Iterator& end, int size)
			{
				buffer+=L"<"; buffer+=elementname; buffer+=L" name=\""; buffer+=name;
				buffer+=L"\" size=\""; buffer+=StringUtil::To<std::wstring>(size); buffer+=L"\">";

				std::wstring item_name(elementname);
				item_name+=L":item";

				int count=0;
				for(Iterator it=begin; it!=end; ++it)
					PutInto(buffer, (item_name+StringUtil::ToUnicode(count++)).c_str(), *it);

				buffer+=L"</"; buffer+=elementname; buffer+=L">";
			}

			template <typename Inserter> static inline void GetContainerFrom(
				Xml::Element& element, const wchar_t* elementname, const wchar_t* name, Inserter it)
			{
				Xml::ElementCollection result;
				std::wstring item_name(elementname);
				item_name+=L":item";

				element.FindElements(elementname, name, &result);
				
				Xml::Element& e=*result.front();
				int size=StringUtil::To<int>(e.Attributes[L"size"]);
				for (int count=0; count<size; ++count)
				{
					typename Inserter::container_type::value_type val;
					GetFrom(e, (item_name+StringUtil::ToUnicode(count)).c_str(), val);
					*it=val; // swap to container
				}
			}

			template <typename T1, typename T2> static inline void PutInto(
				std::wstring& buffer, const wchar_t* name, const std::pair<T1, T2>& val)
			{
				buffer+=L"<pair name=\"";
				buffer+=name;
				buffer+=L"\">";
				PutInto(buffer, L"first", val.first);
				PutInto(buffer, L"second", val.second);
				buffer+=L"</pair>";
			}

			template <typename T1, typename T2> static inline void GetFrom(
				Xml::Element& element, const wchar_t* name, std::pair<T1, T2>& val)
			{
				Xml::ElementCollection result;
				element.FindElements(L"pair", name, &result);

				GetFrom(*result.front(), L"first", val.first);
				GetFrom(*result.front(), L"second", val.second);
			}

			template <typename T1, typename T2> static inline void GetPairFrom(
				Xml::Element& element, std::pair<T1, T2>& val)
			{
				GetFrom(element, L"first", val.first);
				GetFrom(element, L"second", val.second);
			}

			template <typename KeyedContainer> static inline void GetKeyedContainerFrom(
				Xml::Element& element, const wchar_t* elementname, const wchar_t* name, KeyedContainer& val)
			{
				Xml::ElementCollection result;
				std::wstring item_name(elementname);
				item_name+=L":item";

				element.FindElements(elementname, name, &result);
				
				Xml::Element& e=*result.front();

				for(Xml::ElementCollection::iterator element=e.SubElements.begin();
					element!=e.SubElements.end(); ++element)
				{
					std::pair<KeyedContainer::key_type, KeyedContainer::mapped_type> buf; // you cannot use KeyedContainer::value_type coz, that is std::pair<const key, mapped>
					GetPairFrom(**element, buf);
					val.insert(buf);
				}
			}
		};
		#pragma warning( pop ) // recover warning state
} }