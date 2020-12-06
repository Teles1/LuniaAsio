#ifndef Excpetion_GUARD
#define Excpetion_GUARD
#pragma once
#include <exception>
#include <boost/format.hpp>

class Exception : public std::exception {
private:
	std::string Message;
public:
	Exception(char* message) {
		this->Message = message;
	}
	Exception(boost::format input) {
		this->Message = boost::str(input);
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