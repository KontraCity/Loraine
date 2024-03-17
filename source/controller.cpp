#include "controller.hpp"

#include <iostream>

namespace kc {

int Controller::RelayPin(Relay relay)
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
        "kc::Controller::RelayPin(): Relay is unknown [relay: {}]",
        static_cast<int>(relay)
    ));
}

const char* Controller::RelaySmallName(Relay relay)
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
        "kc::Controller::RelayShortName(): Relay is unknown [relay: {}]",
        static_cast<int>(relay)
    ));
}

const char* Controller::RelayName(Relay relay)
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
        "kc::Controller::RelayLongName(): Relay is unknown [relay: {}]",
        static_cast<int>(relay)
    ));
}

Controller::Controller()
{
    wiringPiSetupGpio();
    for (Relay relay = Relay::FenceLighting; relay != Relay::MaxRelays; ++relay)
    {
        pinMode(RelayPin(relay), OUTPUT);
        digitalWrite(RelayPin(relay), HIGH);
        m_relays.emplace(relay, false);
    }
}

Controller::~Controller()
{
    for (Relay relay = Relay::FenceLighting; relay != Relay::MaxRelays; ++relay)
        switchRelay(relay, false);
}

void Controller::switchRelay(Relay relay, bool enabled)
{
    int relayPin;
    try
    {
        relayPin = RelayPin(relay);
    }
    catch (const std::runtime_error&)
    {
        throw std::invalid_argument(fmt::format(
            "kc::Controller::switchRelay(): Relay is unknown [relay: {}]",
            static_cast<int>(relay)
        ));
    }

    if (m_relays[relay].enabled == enabled)
        return;

    m_relays[relay].enabled = enabled;
    digitalWrite(relayPin, (enabled ? LOW : HIGH));
}

Controller::Relay& operator++(Controller::Relay& relay)
{
    relay = static_cast<Controller::Relay>(static_cast<int>(relay) + 1);
    return relay;
}

} // namespace kc
