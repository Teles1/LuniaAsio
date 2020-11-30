#pragma once
#include "../Exceptions/Expcetion.h"
#include <set>

struct Version {

	const wchar_t* Type; //only string literals should be used as class IDs, so we don't need to copy full string. 
public:
	int Major;
	int Minor;

	Version() :Type(L"Uninitialized"), Major(0), Minor(0) {}
	Version(const wchar_t* pType, int pMajor, int pMinor) :Type(pType), Major(pMajor), Minor(pMinor) {}

	void Set(const wchar_t* type, int major, int minor) {
		this->Type = type;
		this->Major = major;
		this->Minor = minor;
	}

	bool Equals(int major, int minor) const {
		return (major == this->Major) && (minor == this->Minor);
	}

	bool Less(int major, int minor) const {
		if (this->Major < major) return true;
		if (this->Major > major) return false;
		return this->Minor < minor;
	}

	bool LessEquals(int major, int minor) const {
		if (this->Major < major) return true;
		if (this->Major > major) return false;
		return this->Minor <= minor;
	}

	bool Greater(int major, int minor) const {
		return !LessEquals(major, minor);
	}

	bool GreaterEquals(int major, int minor) const {
		return !Less(major, minor);
	}

	void RequireMajor(int major) const {
		char* messageOut = new char[100];
		printf_s(messageOut, "Version mismatch: '%ws' major version %d != %d", this->Type, this->Major, major);
		if (this->Major != major) throw Exception(messageOut);
	}

	void RequireLess(int major, int minor) const {
		char* messageOut = new char[100];
		printf_s(messageOut, "Version mismatch: '%ws' version	%d.%d >= %d.%d", this->Type, this->Major, this->Minor, major, minor);
		if (!Less(major, minor)) throw Exception(messageOut);
	}

	void RequireLessEquals(int major, int minor) const {
		char* messageOut = new char[100];
		printf_s(messageOut, "Version mismatch: '%ws' version %d.%d > %d.%d", this->Type, this->Major, this->Minor, major, minor);
		if (!LessEquals(major, minor)) throw Exception(messageOut);
	}

	void RequireGreater(int major, int minor) const {
		char* messageOut = new char[100];
		printf_s(messageOut, "Version mismatch: '%ws' version %d.%d <= %d.%d", this->Type, this->Major, this->Minor, major, minor);
		if (!Greater(major, minor)) throw Exception(messageOut);
	}

	void RequireGreaterEquals(int major, int minor) const {
		char* messageOut = new char[100];
		printf_s(messageOut, "Version mismatch: '%ws' version %d.%d < %d.%d", this->Type, this->Major, this->Minor, major, minor);
		if (!GreaterEquals(major, minor)) throw Exception(messageOut);
	}

	void RequireEquals(int major, int minor) const {
		char* messageOut = new char[100];
		printf_s(messageOut, "Version mismatch: '%ws' version %d.%d != %d.%d", this->Type, this->Major, this->Minor, major, minor);
		if (!Equals(major, minor)) throw Exception(messageOut);
	}
};
