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
        output_stream_ << std::forward<T>(data);
        return *this;
    }

    Logger &operator<<(Logger::LogType type);

private:
    std::ostream &output_stream_ = std::cerr;
};

extern Logger logger;

#endif //LAB_2_GAME_OF_LIFE_LOGGER_H
