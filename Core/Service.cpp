#include "Service.h"
namespace Lunia {
	namespace XRated {
		namespace Service {
			StageStates::operator int() const {
				return reinterpret_cast<const int&>(*this);
			}

			StageStates& StageStates::operator=(int rhs) {
				reinterpret_cast<int&>(*this) = rhs;
				return *this;
			}

			StageStates::StageStates(){
				*this = 0;
			}
		}
	}
}