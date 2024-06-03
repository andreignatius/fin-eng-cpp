#ifndef LOGGER_H
#define LOGGER_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h" // Include for stdout_color_mt

// Declare a global logger
extern std::shared_ptr<spdlog::logger> console;

// Function to initialize the logger
void init_logger();

#endif // LOGGER_H
