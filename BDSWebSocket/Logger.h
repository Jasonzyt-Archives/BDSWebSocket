#pragma once
#include <string>
#include <cstdarg>
#include <iostream>
#include <ctime>
#pragma warning(disable: 4996)

/////////////////////////////////////////
// Usage:
// 
//   using namespace Logger;
// 
//   Info() << "hello" << 77 << endl;
//   Info("hello,", string("alex"), 3) << endl;
//   Info().printf("%s, %d\n","alex", 3);
// 
// [Log, Info, Warn, Error, Fatal]
// 
/////////////////////////////////////////

inline std::string GetCurrentDateTimeStr() {
    char buf[50];
    auto tim = time(0);
    strftime(buf, 50, "%Y-%m-%d %H:%M:%S", localtime(&tim));
    return buf;
}

namespace Logger
{
    class Log
    {
    public:
        Log() = default;

        Log(const Log&) = delete;

        template <class T>
        Log(T value) {
            std::cout << value;
        }

        template <class T, class... Args>
        Log(T value, Args... args) {
            std::cout << value;
            Log(args...);
        }

        Log& printf(const char* format, ...) {
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
            return *this;
        }

        Log& flush() {
            std::cout << std::flush;
            return *this;
        }

        template <typename T>
        Log& operator<<(T value) {
            std::cout << value;
            return *this;
        }

        Log& operator<<(void (*obj)(Log&)) {
            obj(*this);
            return *this;
        }
    };

    inline void endl(Log& logger) {
        logger << '\n';
        logger.flush();
    }

    class Info
    {
    public:
        Info()
        {
            std::cout << "[" << GetCurrentDateTimeStr() << " INFO][BDSWS] ";
        }

        Info(const Info&) = delete;

        template <class T>
        Info(T value)
        {
            std::cout << value;
        }

        template <class T, class... Args>
        Info(T value, Args... args)
        {
            std::cout << value;
            Info(args...);
        }

        Info& printf(const char* format, ...)
        {
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
            return *this;
        }

        Info& flush() {
            std::cout << std::flush;
            return *this;
        }

        template <typename T>
        Info& operator<<(T value)
        {
            std::cout << value;
            return *this;
        }

        Info& operator<<(void (*obj)(Info&)) {
            obj(*this);
            return *this;
        }
    };

    inline void endl(Info& logger) {
        logger << '\n';
        logger.flush();
    }

    class Warn {
    public:
        Warn() {
            std::cout << "[" << GetCurrentDateTimeStr() << " WARN][BDSWS] ";
        }

        Warn(const Warn&) = delete;

        template <class T>
        Warn(T value) {
            std::cout << value;
        }

        template <class T, class... Args>
        Warn(T value, Args... args) {
            std::cout << value;
            Warn(args...);
        }

        Warn& printf(const char* format, ...) {
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
            return *this;
        }

        Warn& flush() {
            std::cout << std::flush;
            return *this;
        }

        template <typename T>
        Warn& operator<<(T value) {
            std::cout << value;
            return *this;
        }

        Warn& operator<<(void (*obj)(Warn&)) {
            obj(*this);
            return *this;
        }
    };

    inline void endl(Warn& logger) {
        logger << '\n';
        logger.flush();
    }

    class Error {
    public:
        Error() {
            std::cout << "[" << GetCurrentDateTimeStr() << " Error] ";
        }

        Error(const Error&) = delete;

        template <class T>
        Error(T value) {
            std::cout << value;
        }

        template <class T, class... Args>
        Error(T value, Args... args) {
            std::cout << value;
            Error(args...);
        }

        Error& printf(const char* format, ...) {
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
            return *this;
        }

        Error& flush() {
            std::cout << std::flush;
            return *this;
        }

        template <typename T>
        Error& operator<<(T value) {
            std::cout << value;
            return *this;
        }

        Error& operator<<(void (*obj)(Error&)) {
            obj(*this);
            return *this;
        }
    };

    inline void endl(Error& logger) {
        logger << '\n';
        logger.flush();
    }

    class Fatal {
    public:
        Fatal() {
            std::cout << "[" << GetCurrentDateTimeStr() << " FATAL] ";
        }

        Fatal(const Fatal&) = delete;

        template <class T>
        Fatal(T value) {
            std::cout << value;
        }

        template <class T, class... Args>
        Fatal(T value, Args... args) {
            std::cout << value;
            Fatal(args...);
        }

        Fatal& printf(const char* format, ...) {
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
            return *this;
        }

        Fatal& flush() {
            std::cout << std::flush;
            return *this;
        }

        template <typename T>
        Fatal& operator<<(T value) {
            std::cout << value;
            return *this;
        }

        Fatal& operator<<(void (*obj)(Fatal&)) {
            obj(*this);
            return *this;
        }
    };

    inline void endl(Fatal& logger) {
        logger << '\n';
        logger.flush();
    }
} // namespace Logger