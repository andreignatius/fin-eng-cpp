#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <iostream>

class Logger {
public:
    Logger(const std::string& filename);
    ~Logger();

    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);

private:
    std::ofstream logFile;
    void log(const std::string& message, const std::string& level);
};

#endif // LOGGER_H
