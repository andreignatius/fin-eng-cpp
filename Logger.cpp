#include "Logger.h"

Logger::Logger(const std::string& filename) {
    logFile.open(filename, std::ios::app);  // Open the log file in append mode
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::info(const std::string& message) {
    log(message, "INFO");
}

void Logger::warn(const std::string& message) {
    log(message, "WARN");
}

void Logger::error(const std::string& message) {
    log(message, "ERROR");
}

void Logger::log(const std::string& message, const std::string& level) {
    if (logFile.is_open()) {
        logFile << "[" << level << "] " << message << std::endl;
    }
    else {
        std::cerr << "Log file is not open!" << std::endl;
    }
}
