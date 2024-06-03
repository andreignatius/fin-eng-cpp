#include "logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

// Define the global logger
std::shared_ptr<spdlog::logger> console;

// Function to initialize the logger
void init_logger() {
    console = spdlog::stdout_color_mt("console");
}
