// Library spdlog
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// Custom modules
#include "config.hpp"
#include "controller.hpp"
#include "http_server.hpp"
using namespace kc;

/// @brief Initialize config
/// @return Initialized config (empty if initialization failed)
static Config::Pointer Init()
{
    try
    {
        return std::make_shared<Config>();
    }
    catch (const Config::Error& error)
    {
        spdlog::logger logger("init", std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        logger.error("Configuration error: {}", error.what());
        return {};
    }
}

int main()
{
    Config::Pointer config = Init();
    if (!config)
        return 1;

    Controller::Pointer controller = std::make_shared<Controller>();
    HttpServer server(config, controller);
    server.start();
}
