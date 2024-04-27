#include "controller.hpp"

namespace kc {

int Controller::ControlMask(Relay relay)
{
    switch (relay)
    {
        case Relay::One:
            return 0b0000'0001;
        case Relay::Two:
            return 0b0000'0010;
        case Relay::Three:
            return 0b0000'0100;
        case Relay::Four:
            return 0b0000'1000;
        case Relay::Five:
            return 0b0001'0000;
        case Relay::Six:
            return 0b0010'0000;
        case Relay::Seven:
            return 0b0100'0000;
        case Relay::Eight:
            return 0b1000'0000;
    }

    throw std::runtime_error(fmt::format(
        "kc::Controller::ControlMask(): Relay is unknown [relay: {}]",
        static_cast<int>(relay)
    ));
}

const char* Controller::UniqueName(Relay relay)
{
    switch (relay)
    {
        case Relay::One:
            return "one";
        case Relay::Two:
            return "two";
        case Relay::Three:
            return "three";
        case Relay::Four:
            return "four";
        case Relay::Five:
            return "five";
        case Relay::Six:
            return "six";
        case Relay::Seven:
            return "seven";
        case Relay::Eight:
            return "eight";
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
        case Relay::One:
            return "Relay one";
        case Relay::Two:
            return "Relay two";
        case Relay::Three:
            return "Relay three";
        case Relay::Four:
            return "Relay four";
        case Relay::Five:
            return "Relay five";
        case Relay::Six:
            return "Relay six";
        case Relay::Seven:
            return "Relay seven";
        case Relay::Eight:
            return "Relay eight";
    }

    throw std::runtime_error(fmt::format(
        "kc::Controller::Name(): Relay is unknown [relay: {}]",
        static_cast<int>(relay)
    ));
}

void Controller::switchRelays()
{
    uint8_t switchState = 0;
    for (Relay relay = Relay::One; relay != Relay::MaxRelays; ++relay)
    {
        /*
        *   Due to sinking current architecture of relay assembly,
        *   driver's LOW (0) signal enables the relay and HIGH (1) signal disables it.
        */
        if (!m_relays[relay].enabled)
            switchState |= ControlMask(relay);
    }
    m_driver.send({ switchState });
}

Controller::Controller(Config::Pointer config)
    : m_driver(config->i2cPort(), 0x20)
{
    for (Relay relay = Relay::One; relay != Relay::MaxRelays; ++relay)
        m_relays.emplace(relay, false);
    switchRelays();
}

Controller::~Controller()
{
    setAllStates(false);
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
    switchRelays();
}

void Controller::setAllStates(bool enabled)
{
    std::lock_guard lock(m_mutex);
    for (Relay relay = Relay::One; relay != Relay::MaxRelays; ++relay)
        m_relays[relay].enabled = enabled;
    switchRelays();
}

Controller::Relay& operator++(Controller::Relay& relay)
{
    relay = static_cast<Controller::Relay>(static_cast<int>(relay) + 1);
    return relay;
}

} // namespace kc
