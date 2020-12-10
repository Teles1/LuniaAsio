#ifndef Locator_GUARD
#define Locator_GUARD
#pragma once

#include "StringUtil/StaticString.h"

namespace Lunia {

	/**
	A class storing resource location/path.

	{filepath} / {filename}.{extension} | {itemname}

	@author juhnu
	*/
	class Locator {

	public:

		enum { MaxString = 512 };

		Locator();
		Locator(const wchar_t* resourcePath);
		Locator(const wchar_t* resourcePath, const wchar_t* resourcePivot);
		Locator(const wchar_t* resourcePath, const Locator& resourcePivot);

		Locator(const std::wstring& resourcePath);
		Locator(const std::wstring& resourcePath, const std::wstring& resourcePivot);
		Locator(const std::wstring& resourcePath, const Locator& resourcePivot);

		bool IsEmpty() const { return path.empty(); }

		//@return /somepath or /somepath/somefilename or |someitemname or full path in case the path is not relative
		const wchar_t* GetRelativePath() const;

		//@return protocol;//path/filename.ext|item
		const wchar_t* GetPath() const;

		//@return protocol;//path/filename.ext
		static_wstring<MaxString> GetPathNoItem() const;

		//@return protocol;//path/filename
		static_wstring<MaxString> GetPathNoExtension() const;
		//@return path
		static_wstring<MaxString> GetPathNoFilename() const;

		//@return filename.ext|item //todo fix this to support item names which have / "slash" on them
		static_wstring<MaxString> GetPathNoDirectory() const;

		//@return filename.ext
		static_wstring<MaxString> GetFilename() const;

		//@return filename
		static_wstring<MaxString> GetFilenameNoExtension() const;

		//@return item
		static_wstring<MaxString> GetItem() const;

		//@return .ext
		static_wstring<MaxString> GetExtension() const;

		//@return protocol
		static_wstring<MaxString> GetProtocol() const;

		operator std::wstring() const;
		operator const wchar_t* () const;
	private:

		bool IsItem(const static_wstring<MaxString>& path) const;
		bool IsRelative(const static_wstring<MaxString>& path) const;
		bool IsDirectory(const static_wstring<MaxString>& path) const;

		void ReplaceWithSlashes();
		void Initialize(const wchar_t* pRelativePath);
		void Initialize(const wchar_t* pRelativePath, const Locator& pivot);

		static_wstring<MaxString> path;
		size_t relativePath;

	};

}
#endif // !Locator_GUARD
