#include "base/logging.h"

namespace log {

const char* LoggingLevelStr(LoggingLevel level) {
    switch (level) {
        case kInfo:    return "info";
        case kWarning: return "warning";
        case kError:   return "error";
        case kFatal:   return "fatal";
    }
}

} // namespace log
