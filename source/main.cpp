// STL modules
#include <iostream>

// Library spdlog
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// Custom modules
#include "config.hpp"
using namespace kc;

/// @brief Initialize config
/// @return Initialized config
static Config::Pointer Init()
{
    spdlog::logger logger("init", std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    try
    {
        return std::make_shared<Config>();
    }
    catch (const Config::Error& error)
    {
        logger.error("Configuration error: {}", error.what());
        return {};
    }
}

int main()
{
    Config::Pointer config = Init();
    if (!config)
        return 1;

    return 0;
}
