#include "Logger.h"

// global logger object
Logger logger;

Logger &Logger::operator<<(Logger::LogType type) {
    current_log_type_ = type;
    return *this;
}
