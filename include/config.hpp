#pragma once

// STL modules
#include <memory>
#include <fstream>
#include <stdexcept>

// Library nlohmann::json
#include <nlohmann/json.hpp>

// Library {fmt}
#include <fmt/format.h>

namespace kc {

/* Namespace aliases and imports */
using nlohmann::json;

namespace ConfigConst
{
    constexpr const char* ConfigFile = "config.json";

    namespace Objects
    {
        constexpr const char* HttpPort = "http_port";
    }

    namespace Defaults
    {
        constexpr uint16_t HttpPort = 80;
    }
}

class Config
{
public:
    using Pointer = std::shared_ptr<Config>;

    class Error : public std::logic_error
    {
    public:
        using logic_error::logic_error;
    };

public:
    /// @brief Generate sample configuration file for user to fill out
    /// @throw std::runtime_error if file couldn't be created
    static void GenerateSampleFile();

private:
    uint16_t m_httpPort;

public:
    /// @brief Read and parse configuration file
    Config();

    /// @brief Get HTTP server port
    /// @return HTTP server port
    inline uint16_t httpPort() const
    {
        return m_httpPort;
    }
};

} // namespace kc
