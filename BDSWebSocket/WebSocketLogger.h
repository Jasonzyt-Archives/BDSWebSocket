#ifndef WEBSOCKETLOGGER_H
#define WEBSOCKETLOGGER_H
#include "pch.h"
#pragma warning(disable:26812)

enum WSLoggerLevel : char {
	DEBUG, INFO, WARN, ERROR, FATAL
};

class WSLogger {

public:

	const std::vector<std::string> lvlstr{
		"[DEBUG]", "[INFO]", "[WARN]", "[ERROR]", "[FATAL]"
	};
#if defined(RELEASE)
	WSLoggerLevel minlvl = INFO;
#else
	WSLoggerLevel minlvl = DEBUG;
#endif
	std::fstream fstm;

	WSLogger(WSLoggerLevel lvl) {
		fstm.open(WEBSOCKET_LOG, std::ios::app | std::ios::out);
		fstm << "[" << GetCurrentDateTimeStr() << "]" << lvlstr[(int)lvl] << ' ';
	}

	WSLogger(const WSLogger&) = delete;

	WSLogger& printf(const char* format, ...) {
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);
		return *this;
	}

	WSLogger& flush() {
		fstm << std::flush;
		return *this;
	}

	template <typename T>
	WSLogger& operator<<(T value) {
		fstm << value;
		return *this;
	}

	WSLogger& operator<<(void (*obj)(WSLogger&)) {
		obj(*this);
		return *this;
	}

	static void init() {
		std::fstream fstm(WEBSOCKET_LOG, std::ios::app | std::ios::out);
		fstm << "==================== WEBSOCKET STARTED ====================\n";
		fstm << "BuildTime: " << __DATE__ " " __TIME__ << std::endl;
		fstm << "Version: " << VERSIONSTR << std::endl;
		fstm << "Time: " << GetCurrentDateTimeStr() << std::endl;
	}

	static void uninit() {
		std::fstream fstm(WEBSOCKET_LOG, std::ios::app | std::ios::out);
		fstm << "==================== WEBSOCKET STOPPED ====================\n";
	}
};

inline void endl(WSLogger& logger) {
	logger << '\n';
	logger.flush();
}

#endif // !WEBSOCKETLOGGER_H
