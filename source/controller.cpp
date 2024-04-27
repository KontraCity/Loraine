#include "controller.hpp"

namespace kc {

uint16_t Controller::ControlMask(Relay relay)
{
    switch (relay)
    {
        case Relay::One:
            return 0b0000'0000'0000'0001;
        case Relay::Two:
            return 0b0000'0000'0000'0010;
        case Relay::Three:
            return 0b0000'0000'0000'0100;
        case Relay::Four:
            return 0b0000'0000'0000'1000;
        case Relay::Five:
            return 0b0000'0000'0001'0000;
        case Relay::Six:
            return 0b0000'0000'0010'0000;
        case Relay::Seven:
            return 0b0000'0000'0100'0000;
        case Relay::Eight:
            return 0b0000'0000'1000'0000;
        case Relay::Nine:
            return 0b0000'0001'0000'0000;
        case Relay::Ten:
            return 0b0000'0010'0000'0000;
        case Relay::Eleven:
            return 0b0000'0100'0000'0000;
        case Relay::Twelve:
            return 0b0000'1000'0000'0000;
        case Relay::Thirteen:
            return 0b0001'0000'0000'0000;
        case Relay::Fourteen:
            return 0b0010'0000'0000'0000;
        case Relay::Fifteen:
            return 0b0100'0000'0000'0000;
        case Relay::Sixteen:
            return 0b1000'0000'1000'0000;
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
        case Relay::Nine:
            return "nine";
        case Relay::Ten:
            return "ten";
        case Relay::Eleven:
            return "eleven";
        case Relay::Twelve:
            return "twelve";
        case Relay::Thirteen:
            return "thirteen";
        case Relay::Fourteen:
            return "fourteen";
        case Relay::Fifteen:
            return "fifteen";
        case Relay::Sixteen:
            return "sixteen";
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
            return "Relay #1";
        case Relay::Two:
            return "Relay #2";
        case Relay::Three:
            return "Relay #3";
        case Relay::Four:
            return "Relay #4";
        case Relay::Five:
            return "Relay #5";
        case Relay::Six:
            return "Relay #6";
        case Relay::Seven:
            return "Relay #7";
        case Relay::Eight:
            return "Relay #8";
        case Relay::Nine:
            return "Relay #9";
        case Relay::Ten:
            return "Relay #10";
        case Relay::Eleven:
            return "Relay #11";
        case Relay::Twelve:
            return "Relay #12";
        case Relay::Thirteen:
            return "Relay #13";
        case Relay::Fourteen:
            return "Relay #14";
        case Relay::Fifteen:
            return "Relay #15";
        case Relay::Sixteen:
            return "Relay #16";
    }

    throw std::runtime_error(fmt::format(
        "kc::Controller::Name(): Relay is unknown [relay: {}]",
        static_cast<int>(relay)
    ));
}

void Controller::switchRelays(bool force)
{
    uint16_t switchState = 0;
    for (Relay relay = Relay::One; relay != Relay::MaxRelays; ++relay)
    {
        /*
        *   Due to sinking current architecture of relay assemblies,
        *   drivers' LOW (0) signal enables and HIGH (1) signal disables the relay.
        */
        if (!m_relays[relay].enabled)
            switchState |= ControlMask(relay);
    }

    uint8_t switchState1 = static_cast<uint8_t>(switchState);
    static uint8_t previousSwitchState1 = switchState1;
    if (force || switchState1 != previousSwitchState1)
    {
        previousSwitchState1 = switchState1;
        m_driver1.send({ switchState1 });
    }

    uint8_t switchState2 = static_cast<uint8_t>(switchState >> 8);
    static uint8_t previousSwitchState2 = switchState2;
    if (force || switchState2 != previousSwitchState2)
    {
        previousSwitchState2 = switchState2;
        m_driver2.send({ switchState2 });
    }
}

Controller::Controller(Config::Pointer config)
    : m_driver1(config->i2cPort(), 0x20)
    , m_driver2(config->i2cPort(), 0x21)
{
    for (Relay relay = Relay::One; relay != Relay::MaxRelays; ++relay)
        m_relays.emplace(relay, false);
    switchRelays(true);
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

    if (relayEntry->second.enabled != enabled)
    {
        relayEntry->second.enabled = enabled;
        switchRelays();
    }
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
