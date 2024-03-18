#include "http_server.hpp"
#include <iostream>

namespace kc {

int HttpServer::Connection::GetIndentation(const std::string& query)
{
    return (query.find("pretty=true") == std::string::npos) ? -1 : 4;
}

HttpServer::Connection::Target HttpServer::Connection::ParseTarget(const std::string& target)
{
    size_t queryStartPosition = target.find("?");
    if (queryStartPosition == std::string::npos)
        return { target, {} };
    return { target.substr(0, queryStartPosition), std::string(target.begin() + queryStartPosition + 1, target.end()) };
}

void HttpServer::Connection::notFound()
{
    m_response.result(beast::http::status::not_found);
    m_response.set(beast::http::field::content_type, "text/plain");
    beast::ostream(m_response.body()) << "Resource not found\n";
    m_logger->error(m_logMessage("Not found"));
}

void HttpServer::Connection::methodNotAllowed()
{
    m_response.result(beast::http::status::method_not_allowed);
    m_response.set(beast::http::field::content_type, "text/plain");
    beast::ostream(m_response.body()) << "This method is not allowed\n";
    m_logger->error(m_logMessage("Method Not Allowed"));
}

void HttpServer::Connection::getRelays(int indentation)
{
    json responseJson;
    for (Controller::Relay relay = Controller::Relay::FenceLighting; relay != Controller::Relay::MaxRelays; ++relay)
    {
        json stateJson;
        stateJson["enabled"] = m_controller->getState(relay).enabled;
        responseJson[Controller::UniqueName(relay)] = stateJson;
    }

    m_response.result(beast::http::status::ok);
    m_response.set(beast::http::field::content_type, "application/json");
    beast::ostream(m_response.body()) << responseJson.dump(indentation) << '\n';
    m_logger->info(m_logMessage("OK"));
}

void HttpServer::Connection::postRelays(int indentation)
{
    try
    {
        json requestJson = json::parse(beast::buffers_to_string(m_request.body().data()));
        for (Controller::Relay relay = Controller::Relay::FenceLighting; relay != Controller::Relay::MaxRelays; ++relay)
            m_controller->setState(relay, requestJson["enabled"]);

        json responseJson;
        for (Controller::Relay relay = Controller::Relay::FenceLighting; relay != Controller::Relay::MaxRelays; ++relay)
        {
            json stateJson;
            stateJson["enabled"] = requestJson["enabled"];
            responseJson[Controller::UniqueName(relay)] = stateJson;
        }

        m_response.result(beast::http::status::ok);
        m_response.set(beast::http::field::content_type, "application/json");
        beast::ostream(m_response.body()) << responseJson.dump(indentation) << '\n';
        m_logger->info(m_logMessage("OK"));
    }
    catch (const json::exception&)
    {
        m_response.result(beast::http::status::bad_request);
        m_response.set(beast::http::field::content_type, "text/plain");
        beast::ostream(m_response.body()) << "Bad request\n";
        m_logger->error(m_logMessage("Bad request"));
    }
}

void HttpServer::Connection::getRelay(Controller::Relay relay, int indentation)
{
    json responseJson, stateJson;
    stateJson["enabled"] = m_controller->getState(relay).enabled;
    responseJson[Controller::UniqueName(relay)] = stateJson;

    m_response.result(beast::http::status::ok);
    m_response.set(beast::http::field::content_type, "application/json");
    beast::ostream(m_response.body()) << responseJson.dump(indentation) << '\n';
    m_logger->info(m_logMessage("OK"));
}

void HttpServer::Connection::postRelay(Controller::Relay relay, int indentation)
{
    try
    {
        json requestJson = json::parse(beast::buffers_to_string(m_request.body().data()));
        m_controller->setState(relay, requestJson["enabled"]);

        json responseJson, stateJson;
        stateJson["enabled"] = requestJson["enabled"];
        responseJson[Controller::UniqueName(relay)] = stateJson;

        m_response.result(beast::http::status::ok);
        m_response.set(beast::http::field::content_type, "text/plain");
        beast::ostream(m_response.body()) << responseJson.dump(indentation) << '\n';
        m_logger->info(m_logMessage("OK"));
    }
    catch (const json::exception&)
    {
        m_response.result(beast::http::status::bad_request);
        m_response.set(beast::http::field::content_type, "text/plain");
        beast::ostream(m_response.body()) << "Bad request\n";
        m_logger->error(m_logMessage("Bad request"));
    }
}

void HttpServer::Connection::produceResponse()
{
    m_response.version(m_request.version());
    m_response.keep_alive(false);

    Target target = ParseTarget(m_request.target());
    int indentation = GetIndentation(target.query);
    if (target.resource == "/relays")
    {
        if (m_request.method() == beast::http::verb::get)
        {
            getRelays(indentation);
            return;
        }

        if (m_request.method() == beast::http::verb::post)
        {
            postRelays(indentation);
            return;
        }

        methodNotAllowed();
        return;
    }

    for (Controller::Relay relay = Controller::Relay::FenceLighting; relay != Controller::Relay::MaxRelays; ++relay)
    {
        if (target.resource == fmt::format("/relays/{}", Controller::UniqueName(relay)))
        {
            if (m_request.method() == beast::http::verb::get)
            {
                getRelay(relay, indentation);
                return;
            }

            if (m_request.method() == beast::http::verb::post)
            {
                postRelay(relay, indentation);
                return;
            }

            methodNotAllowed();
            return;
        }
    }

    notFound();
    return;
}

void HttpServer::Connection::sendResponse()
{
    auto self = shared_from_this();
    m_response.content_length(m_response.body().size());
    beast::http::async_write(m_socket, m_response, [self](beast::error_code error, std::size_t bytesTransferred)
    {
        self->m_socket.shutdown(asio::ip::tcp::socket::shutdown_send, error);
        self->m_timeout.cancel();
    });
}

HttpServer::Connection::Connection(std::shared_ptr<spdlog::logger> logger, Config::Pointer config, Controller::Pointer controller, asio::ip::tcp::socket& socket)
    : m_logger(logger)
    , m_config(config)
    , m_controller(controller)
    , m_socket(std::move(socket))
    , m_buffer(1024 * 8)
    , m_timeout(m_socket.get_executor(), std::chrono::seconds(10))
{
    m_logMessage = [this](const std::string& message) -> std::string
    {
        return fmt::format(
            "{} {} from {}: {} {}",
            std::string(m_request.method_string()),
            std::string(m_request.target()),
            m_socket.remote_endpoint().address().to_string(),
            m_response.result_int(),
            message
        );
    };

    m_timeout.async_wait([this](beast::error_code error)
    {
        if (!error)
            m_socket.close(error);
    });
}

void HttpServer::Connection::handleRequest()
{
    auto self = shared_from_this();
    beast::http::async_read(m_socket, m_buffer, m_request, [self](beast::error_code error, std::size_t bytesTransferred)
    {
        boost::ignore_unused(bytesTransferred);
        if (error)
            return;

        self->produceResponse();
        self->sendResponse();
    });
}

void HttpServer::startAccepting()
{
    m_acceptor.async_accept(m_socket, [this](beast::error_code error)
    {
        if (error)
        {
            m_logger->error("Couldn't accept connection: \"{}\" ({})", error.message(), error.value());
            return;
        }

        std::make_shared<Connection>(m_logger, m_config, m_controller, m_socket)->handleRequest();
        startAccepting();
    });
}

HttpServer::HttpServer(Config::Pointer config, Controller::Pointer controller)
    : m_logger(std::make_shared<spdlog::logger>("http_server", std::make_shared<spdlog::sinks::stdout_color_sink_mt>()))
    , m_config(config)
    , m_controller(controller)
    , m_context(1)
    , m_acceptor(m_context, { asio::ip::make_address("0.0.0.0"), config->httpPort() })
    , m_socket(m_context)
{}

void HttpServer::start()
{
    m_logger->info("Listening for connections on port {}", m_config->httpPort());
    startAccepting();
    m_context.run();
}

} // namespace kc
