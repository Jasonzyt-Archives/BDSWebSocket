#ifndef OPTIONAL_H
#define OPTIONAL_H
#include <exception>

template <typename T>
class optional {
	
	T _val;
	bool _set = false;

public:

	optional() {}
	optional(T v) {
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
		throw std::exception("optional: val has not been set");
	}

	inline T val() { 
		return get();
	}

};

#endif // !OPTIONAL_H
