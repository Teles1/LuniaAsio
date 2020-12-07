#ifndef Excpetion_GUARD
#define Excpetion_GUARD
#pragma once
#include <exception>

class Exception : public std::exception {
private:
	std::string Message;
public:
	Exception(char* message) {
		this->Message = message;
	}
	Exception(const char* message) {
		this->Message = message;
	}
	const char* what() const throw ()
	{
		return this->Message.c_str();
	}
};
#endif //Excpetion_GUARD