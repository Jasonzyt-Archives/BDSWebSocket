#ifndef DYNAMICDEPENDENCY_H
#define DYMANICDEPENDENCY_H
#include "pch.h"

class DynDependency {

public:
	HMODULE handler;
	std::unordered_map<std::string, FARPROC> func;

	DynDependency(HMODULE handler, std::vector<std::string> names) {
		this->handler = handler;
		for (auto& name : names) {
			auto ptr = GetProcAddress(handler, name.c_str());
			if (ptr) { 
				func.emplace(name, ptr); 
			}
			else {
				Logger::Warn() << "Can't get function " << name << Logger::endl;
			}
		}
	}

	template<typename Ret, typename ... Args>
	inline Ret execute(std::string name, Args ... args) {
		if (func.count(name)) {
			auto fn = func.at(name);
			if (fn) {
				((Ret(*)(Args...))fn)(args...);
			}
		}
		else {
			Logger::Warn() << "Function " << name << " not found!" << Logger::endl;
		}
	}

	inline bool release() {
		return FreeLibrary(handler);
	}

};

#endif // !DYNAMICDEPENDENCY_H
