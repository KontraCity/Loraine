#include "controller.hpp"

namespace kc {

void Controller::switchRelay(int pin, bool enable)
{
    /*
    *   Due to sinking current architecture of relay assembly,
    *   LOW signal enables the relay and HIGH signal disables it.
    */
    digitalWrite(pin, (enable ? LOW : HIGH));
}

int Controller::ControlPin(Relay relay)
{
    switch (relay)
    {
        case Relay::FenceLighting:
            return 17;
        case Relay::PathLighting:
            return 27;
        case Relay::RightCornerSpotlight:
            return 22;
        case Relay::LeftCornerSpotlight:
            return 10;
        case Relay::HouseSpotlight:
            return 24;
        case Relay::HouseLighting:
            return 25;
        case Relay::GarageLighting:
            return 8;
        case Relay::Free:
            return 7;
    }

    throw std::runtime_error(fmt::format(
        "kc::Controller::ControlPin(): Relay is unknown [relay: {}]",
        static_cast<int>(relay)
    ));
}

const char* Controller::UniqueName(Relay relay)
{
    switch (relay)
    {
        case Relay::FenceLighting:
            return "fence_lighting";
        case Relay::PathLighting:
            return "path_lighting";
        case Relay::RightCornerSpotlight:
            return "right_corner_spotlight";
        case Relay::LeftCornerSpotlight:
            return "left_corner_spotlight";
        case Relay::HouseSpotlight:
            return "house_spotlight";
        case Relay::HouseLighting:
            return "house_lighting";
        case Relay::GarageLighting:
            return "garage_lighting";
        case Relay::Free:
            return "free";
    }

    throw std::runtime_error(fmt::format(
        "kc::Controller::UniqueName(): Relay is unknown [relay: {}]",
        static_cast<int>(relay)
    ));
}

const char* Controller::Name(Relay relay)
{
    switch (relay)
    {
        case Relay::FenceLighting:
            return "Fence lighting";
        case Relay::PathLighting:
            return "Path lighting";
        case Relay::RightCornerSpotlight:
            return "Right corner spotlight";
        case Relay::LeftCornerSpotlight:
            return "Left corner spotlight";
        case Relay::HouseSpotlight:
            return "House spotlight";
        case Relay::HouseLighting:
            return "House lighting";
        case Relay::GarageLighting:
            return "Garage lighting";
        case Relay::Free:
            return "Free";
    }

    throw std::runtime_error(fmt::format(
        "kc::Controller::Name(): Relay is unknown [relay: {}]",
        static_cast<int>(relay)
    ));
}

Controller::Controller()
{
    wiringPiSetupGpio();
    for (Relay relay = Relay::FenceLighting; relay != Relay::MaxRelays; ++relay)
    {
        int pin = ControlPin(relay);
        pinMode(pin, OUTPUT);
        switchRelay(pin, false);
        m_relays.emplace(relay, false);
    }
}

Controller::~Controller()
{
    for (Relay relay = Relay::FenceLighting; relay != Relay::MaxRelays; ++relay)
        setState(relay, false);
}

Controller::State Controller::getState(Relay relay)
{
    std::lock_guard lock(m_mutex);

    auto relayEntry = m_relays.find(relay);
    if (relayEntry == m_relays.end())
    {
        throw std::invalid_argument(fmt::format(
            "kc::Controller::getState(): Relay is unknown [relay: {}]",
            static_cast<int>(relay)
        ));
    }

    return relayEntry->second;
}

void Controller::setState(Relay relay, bool enabled)
{
    std::lock_guard lock(m_mutex);

    auto relayEntry = m_relays.find(relay);
    if (relayEntry == m_relays.end())
    {
        throw std::invalid_argument(fmt::format(
            "kc::Controller::setState(): Relay is unknown [relay: {}]",
            static_cast<int>(relay)
        ));
    }

    if (relayEntry->second.enabled == enabled)
        return;

    relayEntry->second.enabled = enabled;
    switchRelay(ControlPin(relay), enabled);
}

Controller::Relay& operator++(Controller::Relay& relay)
{
    relay = static_cast<Controller::Relay>(static_cast<int>(relay) + 1);
    return relay;
}

} // namespace kc
