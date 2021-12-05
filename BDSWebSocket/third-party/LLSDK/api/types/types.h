#pragma once
//#include<lbpch.h>
//#include <api/MC.h>
//#include <stl/views.h>
#include<api/types/helper.h>
#include <stl/optional.h>
#include <liteloader.h>
#include <mc/Core.h>
#include <mc/mass.h>
#include <api/types/helper.h>

typedef unsigned long long xuid_t;
typedef unsigned char permlvl_t;
enum TextType : char {
	RAW = 0,
	CHAT = 1,
	TRANSLATION = 2,
	POPUP = 3,
	JUKEBOX_POPUP = 4,
	TIP = 5,
	SYSTEM = 6,
	WHISPER = 7,
	ANNOUNCEMENT = 8,
	JSON = 9
};
template <typename T>
struct Wrapped {
	T* v;
	Wrapped(T const& x) : v((T*)&x) {}
	Wrapped(T* x) : v(x) {}
	Wrapped() {
		v = nullptr;
	}
	operator T&() {
		return *v;
	}
	operator T*() {
		return v;
	}
	T& get() {
		return *v;
	}
	T* operator->() {
		return v;
	}
	operator bool() {
		return v;
	}
};
