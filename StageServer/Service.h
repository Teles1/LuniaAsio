#pragma once
namespace Lunia {
	struct StageStates {
		unsigned NumberOfTeams : 4;  // 0 ~ 15
		unsigned IsPrivate : 1;      // 0 ~ 1

		inline operator int() const { return reinterpret_cast<const int&>(*this); }
		inline StageStates& operator=(int rhs) {
			reinterpret_cast<int&>(*this) = rhs;
			return *this;
		}
		inline StageStates() { *this = 0; }
		~StageStates() {}
	};
}