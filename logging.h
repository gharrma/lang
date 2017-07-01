/*
 * Logging utilities inspired by those used at Google.
 * Example usage:
 * ```
 * LOG(INFO) << "Value of x is " << x;
 * LOG(FATAL) << "Invalid program state";
 * ```
 */
#ifndef LOG_H
#define LOG_H
#include <iostream>
#include <sstream>

#ifdef NDEBUG
constexpr bool DEBUG = false;
#else
constexpr bool DEBUG = true;
#endif

enum LoggingLevel {
    INFO = 0, // Print only when debugging.
    WARNING,  // Always print.
    ERROR,    // Abort only when debugging.
    FATAL,    // Always abort.
};

std::string LoggingLevelStr(LoggingLevel level) {
    switch (level) {
        case INFO:    return "info";
        case WARNING: return "warning";
        case ERROR:   return "error";
        case FATAL:   return "fatal";
    }
}

class Logger {
public:
    Logger(LoggingLevel level)
        : level_(level)
        , os_(level_ == INFO ? std::cout : std::cerr) {}

    ~Logger() {
        os_ << buffer_.str() << std::endl;
        if (level_ == FATAL || (DEBUG && level_ == ERROR)) {
            os_ << "Aborting due to fatal error." << std::endl;
            exit(1);
        }
    }

    template <typename T>
    Logger& operator<<(const T& data) {
        buffer_ << data;
        return *this;
    }

private:
    LoggingLevel level_;
    std::ostream& os_;
    std::ostringstream buffer_;
};

#define LOG(level) LAZY_STREAM(Logger(level), DEBUG || level >= WARNING) \
    << __FILE__ << ':' << __LINE__ << " [" << LoggingLevelStr(level) << "] "

// Helper macro which avoids evaluating the arguments to a stream if
// the condition doesn't hold.
#define LAZY_STREAM(stream, condition) \
    !(condition) ? (void) 0 : LogVoidify() & (stream)

// This struct is used to explicitly ignore values in the conditional
// logging macros.  This avoids compiler warnings like "value computed
// is not used" and "statement has no effect."
struct LogVoidify {
  // An operator with a precedence lower than << but higher than ?:
  void operator&(Logger&) {}
};

#endif // LOG_H
