#pragma once

// STL modules
#include <memory>
#include <chrono>
#include <functional>

// Boost libraries
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/beast/core/buffers_to_string.hpp>

// Library nlohmann::json
#include <nlohmann/json.hpp>

// Library {fmt}
#include <fmt/format.h>

// Custom modules
#include "config.hpp"
#include "controller.hpp"
#include "utility.hpp"

namespace kc {

/* Namespace aliases and imports */
using nlohmann::json;
namespace beast = boost::beast;
namespace asio = boost::asio;

class HttpServer
{
private:
    // Shared server logger instance pointer
    using Logger = std::shared_ptr<spdlog::logger>;

    class Connection : public std::enable_shared_from_this<Connection>
    {
    private:
        // Function used to create connection's log message
        using LogMessageFunction = std::function<std::string(std::string)>;

        struct Target
        {
            std::string resource;
            std::string query;
        };

    private:
        /// @brief Deduce response indentation from request query
        /// @param query Request query
        /// @return Response indentation
        static int GetIndentation(const std::string& query);

        /// @brief Parse target resource and query
        /// @param target Target string to parse
        /// @return Parsed target
        static Target ParseTarget(const std::string& target);

    private:
        Logger m_logger;
        LogMessageFunction m_logMessage;
        Config::Pointer m_config;
        Controller::Pointer m_controller;
        asio::ip::tcp::socket m_socket;
        beast::flat_buffer m_buffer;
        beast::http::request<beast::http::dynamic_body> m_request;
        beast::http::response<beast::http::dynamic_body> m_response;
        asio::steady_timer m_timeout;

    private:
        /// @brief Generate generic "404 Not Found" response
        void notFound();

        /// @brief Generate generic "405 Method Not Allowed" response
        void methodNotAllowed();

        /// @brief Generate "/relays" resource GET response
        /// @param indentation Response indentation
        void getRelays(int indentation);

        /// @brief Generate "/relays" resource POST response
        /// @param indentation Response indentation
        void postRelays(int indentation);

        /// @brief Generate "/relays/<relay>" resource GET response
        /// @param relay Accessed relay
        /// @param indentation Response indentation
        void getRelay(Controller::Relay relay, int indentation);

        /// @brief Generate "/relays/<relay>" resource POST response
        /// @param relay Accessed relay
        /// @param indentation Response indentation
        void postRelay(Controller::Relay relay, int indentation);

    private:
        /// @brief Produce HTTP request response
        void produceResponse();

        /// @brief Send produced response
        void sendResponse();

    public:
        /// @brief Initialize connection
        /// @param logger HTTP server logger
        /// @param config Initialized config
        /// @param controller Relay controller
        /// @param socket Connection socket
        Connection(Logger logger, Config::Pointer config, Controller::Pointer controller, asio::ip::tcp::socket& socket);

        /// @brief Handle HTTP request
        void handleRequest();
    };

private:
    Logger m_logger;
    Config::Pointer m_config;
    Controller::Pointer m_controller;
    boost::asio::io_context m_context;
    asio::ip::tcp::acceptor m_acceptor;
    asio::ip::tcp::socket m_socket;

    /// @brief Start accepting client connections
    void startAccepting();

public:
    /// @brief Initialize HTTP server
    /// @param config Initialized config
    /// @param controller Relay controller
    HttpServer(Config::Pointer config, Controller::Pointer controller);

    /// @brief Start listening for connections
    void start();
};

} // namespace kc
