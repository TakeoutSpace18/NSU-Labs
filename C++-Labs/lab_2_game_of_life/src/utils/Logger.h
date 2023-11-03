#ifndef LAB_2_GAME_OF_LIFE_LOGGER_H
#define LAB_2_GAME_OF_LIFE_LOGGER_H

#include <iostream>

class Logger {
public:
    enum LogType {
        Info,
        Warning,
        Error
    };

    template<class T>
    Logger &operator<<(T&& data) {
        switch (current_log_type_) {
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

        output_stream_ << std::forward<T>(data);
        return *this;
    }

    Logger &operator<<(Logger::LogType type);

private:
    std::ostream &output_stream_ = std::cerr;
    LogType current_log_type_ = Info;
};

extern Logger logger;

#endif //LAB_2_GAME_OF_LIFE_LOGGER_H
