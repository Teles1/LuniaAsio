#pragma once
#include <functional>
#include <chrono>
#include <future>
#include <cstdio>
#include <iostream>
struct Protocol {
public:
	virtual void Print() = 0;
	virtual void Update(int x) = 0;
};
typedef std::auto_ptr<Protocol> IProtocolSmartPointer;

struct Head : public Protocol {
public:
	int a = 10;
	void Print() {
		std::cout << a << std::endl;
	}
	void Update(int x) {
		a = x;
	}
	Head() {

	}
	static const uint16 TypeHash;
};
struct Auth : public Protocol {
public:
	int a = 10;
	void Print() {
		std::cout << "Hello! My name is Auth" << std::endl;
	}
	void Update(int x) {
		a = x;
	}
	Auth() {

	}
	static const uint16 TypeHash;
};