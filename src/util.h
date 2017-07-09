#pragma once
#include <iostream>
#include <sstream>

#ifdef NDEBUG
constexpr bool kDebug = false;
#else
constexpr bool kDebug = true;
#endif

template <typename T>
static void PrintToStream(std::ostream& os, const T& arg) {
    os << arg;
}

template <typename T, typename... TT>
static void PrintToStream(std::ostream& os,
                          const T& head,
                          const TT&... tail) {
    os << head;
    PrintToStream(os, tail...);
}

template <typename... TT>
static std::string BuildStr(const TT&... args) {
    std::ostringstream ss;
    PrintToStream(ss, args...);
    return ss.str();
}

// Logging utilities inspired by those used at Google.
// Example usage:
// ```
// LOG(INFO) << "Value of x is " << x;
// LOG(FATAL) << "Invalid program state";
// ```
#define LOG(level) \
    LAZY_STREAM(logging::Logger(level), kDebug || level >= logging::kWarning) \
    << __FILE__ << ':' << __LINE__ \
    << " [" << logging::LoggingLevelStr(level) << "] "

#define INFO logging::kInfo
#define WARNING logging::kWarning
#define ERROR logging::kError
#define FATAL logging::kFatal

// Helper macro which avoids evaluating the arguments to a stream if
// the condition doesn't hold.
#define LAZY_STREAM(stream, condition) \
    !(condition) ? (void) 0 : logging::LogVoidify() & (stream)

namespace logging {

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

} // namespace logging
