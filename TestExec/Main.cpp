#include <Core/Utils/DateTime.h>
using namespace Lunia;
int main(int argc, char* argv[]) {
	std::string str = "2022-10-10 10:10:10 AM";
	DateTime aux;
	aux.Parse(str);

	return 0;
}