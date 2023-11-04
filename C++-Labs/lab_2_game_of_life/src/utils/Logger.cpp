#include "Logger.h"

// global logger object
Logger logger;

Logger &Logger::operator<<(Logger::LogType type) {
    switch (type) {
        case Info:
            output_stream_ << "Info: ";
            break;
        case Warning:
            output_stream_ << "Warning: ";
            break;
        case Error:
            output_stream_ << "Error: ";
            break;
    }
    return *this;
}
