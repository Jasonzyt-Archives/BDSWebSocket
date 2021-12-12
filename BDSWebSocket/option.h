#ifndef OPTIONAL_H
#define OPTIONAL_H
#include <exception>

template <typename T>
class option {
	
	T _val;
	bool _set = false;

public:

	option() {}
	option(T v) {
		_val = v;
		_set = true;
	}

	inline bool set() {
		return _set;
	}

	inline T get() {
		if (_set) {
			return _val;
		}
		throw std::exception("option: val has not been set");
	}

	inline T val() { 
		return get();
	}

};

#endif // !OPTIONAL_H
