#pragma once
#include "Locator.h"

namespace Lunia {

	/**
	A class storing resource location/path.

	{filepath} / {filename}.{extension} | {itemname}

	@author juhnu
	*/
	Locator::Locator() :relativePath(0) {
	}

	Locator::Locator(const wchar_t* resourcePath) {
		Initialize(resourcePath);
	}

	Locator::Locator(const wchar_t* resourcePath, const wchar_t* resourcePivot) {
		Initialize(resourcePath, Locator::Locator(resourcePivot));
	}

	Locator::Locator(const wchar_t* resourcePath, const Locator& resourcePivot) {
		Initialize(resourcePath, resourcePivot);
	}

	Locator::Locator(const std::wstring& resourcePath) {
		Initialize(resourcePath.c_str());
	}
	Locator::Locator(const std::wstring& resourcePath, const std::wstring& resourcePivot) {
		Initialize(resourcePath.c_str(), Locator::Locator(resourcePivot));
	}
	Locator::Locator(const std::wstring& resourcePath, const Locator& resourcePivot) {
		Initialize(resourcePath.c_str(), resourcePivot);
	}





	const wchar_t* Locator::GetRelativePath() const { return path.begin() + relativePath; }

	const wchar_t* Locator::GetPath() const {
		return path.c_str();
	}

	Locator::operator std::wstring() const {
		return path.c_str();
	}

	Locator::operator const wchar_t* () const {
		return path.c_str();
	}



	//@return path/filename.ext
	static_wstring<Locator::MaxString> Locator::GetPathNoItem() const {
		if (path.empty()) return L"";
		std::pair<const wchar_t*, const wchar_t*> pair = StringUtil::Split(path.begin(), path.end(), L'|');
		return static_wstring<Locator::MaxString>(pair.first, pair.second);
	}

	//@return path/filename
	static_wstring<Locator::MaxString> Locator::GetPathNoExtension() const {
		if (path.empty()) return L"";
		std::wstring tmp = GetPathNoItem().c_str();

		size_t dot = tmp.find_last_of(L'.');
		size_t slash = tmp.find_last_of(L'/');

		if (dot == std::wstring::npos) return tmp.c_str();

		if ((slash == std::wstring::npos) || (dot > slash)) {
			return static_wstring<Locator::MaxString>(&tmp[0], &tmp[0] + dot);
		}
		return tmp.c_str();
	}

	//@return path
	static_wstring<Locator::MaxString> Locator::GetPathNoFilename() const {
		if (path.empty()) return L"";
		std::wstring tmp = path.c_str();
		size_t slash = tmp.find_last_of(L'/');
		if (slash == std::wstring::npos) {
			return L"";
		}
		return static_wstring<Locator::MaxString>(&tmp[0], &tmp[0] + slash);
	}

	//@return filename.ext|item //todo fix this to support item names which have / "slash" on them
	static_wstring<Locator::MaxString> Locator::GetPathNoDirectory() const {
		if (path.empty()) return L"";
		std::wstring tmp = path.c_str();
		size_t slash = tmp.find_last_of(L'/');
		if (slash == std::wstring::npos) {
			slash = -1;
			//@fixed 2006.10.11 this shouldn't return empty string here --juhnu
			//return L"";
		}
		return static_wstring<Locator::MaxString>(&tmp[0] + slash + 1);
	}

	//@return filename.ext
	static_wstring<Locator::MaxString> Locator::GetFilename() const {
		Locator tmp(GetPathNoItem().c_str());
		return tmp.GetPathNoDirectory();
	}

	static_wstring<Locator::MaxString> Locator::GetFilenameNoExtension() const {
		Locator tmp(GetPathNoExtension().c_str());
		//ALLM_IMPORTANT((L"%s --- %s", tmp.GetPath(), tmp.GetPathNoDirectory().c_str()));
		return tmp.GetPathNoDirectory();
	}

	//@return item
	static_wstring<Locator::MaxString> Locator::GetItem() const {
		if (path.empty()) return L"";
		std::pair<const wchar_t*, const wchar_t*> pair = StringUtil::Split(path.begin(), path.end(), L'|');
		if (pair.second == path.end()) return L"";
		return static_wstring<Locator::MaxString>(++pair.second, path.end());
	}

	//@return ext
	static_wstring<Locator::MaxString> Locator::GetExtension() const {
		if (path.empty()) return L"";
		std::wstring tmp = GetPathNoItem().c_str();

		size_t dot = tmp.find_last_of(L'.');
		size_t slash = tmp.find_last_of(L'/');

		if (dot == std::wstring::npos) return L"";

		if ((slash == std::wstring::npos) || (dot > slash)) {
			return static_wstring<Locator::MaxString>(&tmp[0] + dot);
		}
		return tmp.c_str();
	}

	//@return protocol
	static_wstring<Locator::MaxString> Locator::GetProtocol() const {
		if (path.empty()) return L"";
		std::wstring tmp = path.c_str();
		size_t slash = tmp.find(L"://");
		if (slash == std::wstring::npos) {
			return L"";
		}
		return static_wstring<Locator::MaxString>(&tmp[0], &tmp[0] + slash);
	}



	bool Locator::IsItem(const static_wstring<Locator::MaxString>& path) const {
		if (path.empty()) return false;
		if (path[0] != L'|') return false;
		return true;
	}

	bool Locator::IsRelative(const static_wstring<Locator::MaxString>& path) const {
		if (path.empty()) return false;
		if (path[0] == L'/' || path[0] == L'\\') return true;
		return false;
	}

	bool Locator::IsDirectory(const static_wstring<Locator::MaxString>& path) const {
		if (path.empty()) return false;
		if (path[path.size() - 1] != L'/') return false;
		return true;
	}
	void Locator::ReplaceWithSlashes() {
		for (size_t i = 0; i < path.size(); ++i) {
			if (path[i] == L'\\') path[i] = L'/';
		}
	}

	void Locator::Initialize(const wchar_t* pRelativePath) {
		path = pRelativePath;
		relativePath = 0;
		ReplaceWithSlashes();
	}

	void Locator::Initialize(const wchar_t* pRelativePath, const Locator& pivot) {
		//relativePath=pRelativePath;

		if (IsItem(pRelativePath)) {
			path = pivot.GetPathNoItem();
			relativePath = path.size();
			path += pRelativePath;
			ReplaceWithSlashes();
			return;
		}
		if (IsRelative(pRelativePath)) {
			path = pivot.GetPathNoFilename();
			relativePath = path.size();
			path += pRelativePath;
			ReplaceWithSlashes();
			return;
		}
		path = pRelativePath;
		ReplaceWithSlashes();
		relativePath = 0;
	}


}