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
constexpr bool kDebug = false;
#else
constexpr bool kDebug = true;
#endif

#define INFO log::kInfo
#define WARNING log::kWarning
#define ERROR log::kError
#define FATAL log::kFatal

#define LOG(level) \
    LAZY_STREAM(log::Logger(level), kDebug || level >= log::kWarning) \
    << __FILE__ << ':' << __LINE__ \
    << " [" << log::LoggingLevelStr(level) << "] "

// Helper macro which avoids evaluating the arguments to a stream if
// the condition doesn't hold.
#define LAZY_STREAM(stream, condition) \
    !(condition) ? (void) 0 : log::LogVoidify() & (stream)

namespace log {

enum LoggingLevel {
    kInfo = 0, // Print only when debugging.
    kWarning,  // Always print.
    kError,    // Abort only when debugging.
    kFatal,    // Always abort.
};

const char* LoggingLevelStr(LoggingLevel level);

class Logger {
public:
    Logger(LoggingLevel level)
        : level_(level)
        , os_(level_ == kInfo ? std::cout : std::cerr) {}

    ~Logger() {
        os_ << buffer_.str() << std::endl;
        if (level_ == kFatal || (kDebug && level_ == kError)) {
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

// This struct is used to explicitly ignore values in the conditional
// logging macros.  This avoids compiler warnings like "value computed
// is not used" and "statement has no effect."
struct LogVoidify {
  // An operator with a precedence lower than << but higher than ?:
  void operator&(Logger&) {}
};

} // namespace log

#endif // LOG_H
