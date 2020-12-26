#pragma once
#include "../Exceptions/Expcetion.h"
#include <Core/Utils/Logger.h>
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
		if (this->Major != major) 
			Logger::GetInstance().Warn("Version mismatch: {0} major version {1} != {2}", (char*)this->Type, this->Major, major);
	}

	void RequireLess(int major, int minor) const {
		if (!Less(major, minor))
			Logger::GetInstance().Warn("Version mismatch: '{0}' version {1}.{2} >= {3}.{4}", (char*)this->Type, this->Major, this->Minor, major, minor);
	}

	void RequireLessEquals(int major, int minor) const {
		if (!LessEquals(major, minor))
			Logger::GetInstance().Warn("Version mismatch: '{0}' version {1}.{2} > {3}.{4}", (char*)this->Type, this->Major, this->Minor, major, minor);
	}

	void RequireGreater(int major, int minor) const {
		if (!Greater(major, minor))
			Logger::GetInstance().Warn("Version mismatch: '{0}' version {0}.{1} <= {2}.{3}", (char*)this->Type, this->Major, this->Minor, major, minor);
	}

	void RequireGreaterEquals(int major, int minor) const {
		if (!GreaterEquals(major, minor)) 
			Logger::GetInstance().Warn("Version mismatch: '{0}' version {0}.{1} < {2}.{3}", (char*)this->Type, this->Major, this->Minor, major, minor);
	}

	void RequireEquals(int major, int minor) const {
		if (!Equals(major, minor))
			Logger::GetInstance().Warn("Version mismatch: '{0}' version {0}.{1} != {2}.{3}", (char*)this->Type, this->Major, this->Minor, major, minor);
	}
};
