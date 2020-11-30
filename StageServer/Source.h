#include "../Core/Common.h"
class ISerializable {
public:
	std::wstring TypeName = L"";
	uint16 TypeHash = 0;
	ISerializable(std::wstring input ) {
		//wat?!
		this->TypeName = input.c_str();
		this->TypeHash = StringUtil::Hash(input);
	}
};

class Test : public ISerializable {
public:
	Test(std::wstring input ) : ISerializable(input){

	}
};