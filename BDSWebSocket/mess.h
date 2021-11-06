#ifndef MESS_H
#define MESS_H
#include "pch.h"

inline std::string randomString(int len) {
	const std::string letters = 
		"qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890";
	std::string result;
	for (int i = 0; i < len; i++) {
		auto r = rand();
		result += letters[(r % 62)];
	}
	return result;
}

#endif // !MESS_H

