#include "XmlSerializer.h"
#pragma warning( disable:4996 ) // avoid warn at while(false)

namespace Lunia { namespace Serializer {

	void XmlSerializer::SerializeStart(std::wstring& buffer, const wchar_t* pTypename) {
		buffer+=L"<"; buffer+=pTypename; buffer+=L">";
	}
	
	void XmlSerializer::SerializeEnd(std::wstring& buffer, const wchar_t* pTypename) {
	   buffer+=L"</"; buffer+=pTypename; buffer+=L">";				
	}

	void XmlSerializer::Serialize(std::wstring& buffer, const IXmlSerializable& src)
	{
		buffer.clear();
		buffer+=L"<"; buffer+=src.GetTypeName(); buffer+=L">";
		src.PutInto(buffer);
		buffer+=L"</"; buffer+=src.GetTypeName(); buffer+=L">";				
	}

	void XmlSerializer::Deserialize(std::wstring& buffer, IXmlSerializable& dest)
	{
		Xml::Parser p;
		p.Parse(buffer.c_str(), false);
		dest.GetFrom(*p.GetElements().front());
	}

	// serializing / deserializing helper class ///////////////////////////////////////////////////

	template <typename T>
		void PutIntoHelper(std::wstring& buffer, const wchar_t* elementname, const wchar_t* name, const T val)
	{
		static wchar_t buf[1024];
		swprintf(buf, L"<%s name=\"%s\" value=\"", elementname, name);
		buffer+=buf;
		buffer+=StringUtil::To<std::wstring>(val);
		buffer+=L"\"/>";
	}

	template <typename T>
		void GetFromHelper(Xml::Element& element, const wchar_t* elementname, const wchar_t* name, T& val)
	{
		Xml::ElementCollection result;
		element.FindElements(elementname, name, &result);
		val=StringUtil::To<T>(result.front()->Attributes[L"value"]);
	}

	// serializing / deserializing IXmlSerializable overrides /////////////////////////////////////

	void XmlSerializer::PutInto(std::wstring& buffer, const wchar_t* name, const IXmlSerializable& val)
	{
		wchar_t buf[1024];
		swprintf(buf, L"<%s name=\"%s\">", val.GetTypeName(), name); buffer+=buf;

		val.PutInto(buffer);

		swprintf(buf, L"</%s>", val.GetTypeName()); buffer+=buf;
	}

	void XmlSerializer::GetFrom(Xml::Element& element, const wchar_t* name, IXmlSerializable& val)
	{
		// extract inner elements
		Xml::ElementCollection result;
		element.FindElements(val.GetTypeName(), name, &result);
		val.GetFrom(*result.front());
	}

	// serializing / deserializing basic types ////////////////////////////////////////////////////
	/* int */
	void XmlSerializer::PutInto(std::wstring& buffer, const wchar_t* name, const int& val)
	{
		PutIntoHelper(buffer, L"int", name, val);
	}
	void XmlSerializer::GetFrom(Xml::Element& element, const wchar_t* name, int& val)
	{
		GetFromHelper(element, L"int", name, val);
	}

	/* unsigned int */
	void XmlSerializer::PutInto(std::wstring& buffer, const wchar_t* name, const unsigned int& val)
	{
		PutIntoHelper(buffer, L"uint", name, val);
	}

	void XmlSerializer::GetFrom(Xml::Element& element, const wchar_t* name, unsigned int& val)
	{
		GetFromHelper(element, L"uint", name, val);
	}

	/* unsigned long */
	void XmlSerializer::PutInto(std::wstring& buffer, const wchar_t* name, const unsigned long& val)
	{
		PutIntoHelper(buffer, L"ulong", name, val);
	}

	void XmlSerializer::GetFrom(Xml::Element& element, const wchar_t* name, unsigned long& val)
	{
		GetFromHelper(element, L"ulong", name, val);
	}

	/* double */
	void XmlSerializer::PutInto(std::wstring& buffer, const wchar_t* name, const double& val)
	{
		PutIntoHelper(buffer, L"double", name, val);
	}

	void XmlSerializer::GetFrom(Xml::Element& element, const wchar_t* name, double& val)
	{
		GetFromHelper(element, L"double", name, val);
	}

	/* short */
	void XmlSerializer::PutInto(std::wstring& buffer, const wchar_t* name, const short& val)
	{
		PutIntoHelper(buffer, L"short", name, val);
	}

	void XmlSerializer::GetFrom(Xml::Element& element, const wchar_t* name, short& val)
	{
		GetFromHelper(element, L"short", name, val);
	}

	/* long */
	void XmlSerializer::PutInto(std::wstring& buffer, const wchar_t* name, const long& val)
	{
		PutIntoHelper(buffer, L"long", name, val);
	}

	void XmlSerializer::GetFrom(Xml::Element& element, const wchar_t* name, long& val)
	{
		GetFromHelper(element, L"long", name, val);
	}

	/* float */
	void XmlSerializer::PutInto(std::wstring& buffer, const wchar_t* name, const float& val)
	{
		PutIntoHelper(buffer, L"float", name, val);
	}

	void XmlSerializer::GetFrom(Xml::Element& element, const wchar_t* name, float& val)
	{
		GetFromHelper(element, L"float", name, val);
	}

	/* float2 */
	void XmlSerializer::PutInto(std::wstring& buffer, const wchar_t* name, const float2& val)
	{
			PutIntoHelper(buffer, L"float2", name, val);
	}

	void XmlSerializer::GetFrom(Xml::Element& element, const wchar_t* name, float2& val)
	{
		GetFromHelper(element, L"float2", name, val);
	}

	/* float3 */
	void XmlSerializer::PutInto(std::wstring& buffer, const wchar_t* name, const float3& val)
	{
		PutIntoHelper(buffer, L"float3", name, val);
	}

	void XmlSerializer::GetFrom(Xml::Element& element, const wchar_t* name, float3& val)
	{
		GetFromHelper(element, L"float3", name, val);
	}

	/* float4 */
	void XmlSerializer::PutInto(std::wstring& buffer, const wchar_t* name, const float4& val)
	{
		PutIntoHelper(buffer, L"float4", name, val);
	}

	void XmlSerializer::GetFrom(Xml::Element& element, const wchar_t* name, float4& val)
	{
		GetFromHelper(element, L"float4", name, val);
	}

	// serializing / deserializing STL types //////////////////////////////////////////////////////
	/* string */ // TODO : check " character
	void XmlSerializer::PutInto(std::wstring& buffer, const wchar_t* name, const std::wstring& val)
	{
		assert(val.find(L'\"')==std::wstring::npos); /* " chracter is not supported yet */

		buffer+=L"<wstring name=\"";
		buffer+=name;
		buffer+=L"\" value=\"";
		buffer+=val;
		buffer+=L"\"/>";
	}

	void XmlSerializer::GetFrom(Xml::Element& element, const wchar_t* name, std::wstring& val)
	{
		Xml::ElementCollection result;
		element.FindElements(L"wstring", name, &result);
		val=result.front()->Attributes[L"value"];
	}

	void XmlSerializer::PutInto(std::wstring& buffer, const wchar_t* name, const std::string& val)
	{
		assert(val.find(L'\"')==std::wstring::npos); /* " chracter is not supported yet */

		buffer+=L"<string name=\"";
		buffer+=name;
		buffer+=L"\" value=\"";
		buffer+=StringUtil::ToUnicode(val);
		buffer+=L"\"/>";
	}

	void XmlSerializer::GetFrom(Xml::Element& element, const wchar_t* name, std::string& val)
	{
		Xml::ElementCollection result;
		element.FindElements(L"string", name, &result);
		val=StringUtil::ToASCII(result.front()->Attributes[L"value"]);
	}

} }