#pragma once

// STL modules
#include <map>
#include <stdexcept>

// Library {fmt}
#include <fmt/format.h>

// Library WiringPi
#include <wiringPi.h>

namespace kc {

class Controller
{
public:
    enum class Relay
    {
        FenceLighting,
        PathLighting,
        RightCornerSpotlight,
        LeftCornerSpotlight,
        HouseSpotlight,
        HouseLighting,
        GarageLighting,
        Free,
        MaxRelays,
    };

    struct State
    {
        bool enabled;
    };

public:
    /// @brief Get relay's pin
    /// @param relay Relay whose pin to get
    /// @throw std::runtime_error if relay is unknown
    /// @return Relay's pin
    static int RelayPin(Relay relay);

    /// @brief Get relay's small name
    /// @param relay Relay whose small name to get
    /// @throw std::runtime_error if relay is unknown
    /// @return Relay's small name
    static const char* RelaySmallName(Relay relay);

    /// @brief Get relay's name
    /// @param relay Relay whose name to get
    /// @throw std::runtime_error if relay is unknown
    /// @return Relay's name
    static const char* RelayName(Relay relay);

private:
    std::map<Relay, State> m_relays;

public:
    /// @brief Initialize relay controller
    Controller();

    ~Controller();

    /// @brief Switch relay
    /// @param relay Relay to switch
    /// @throw std::invalid_argument if relay is unknown
    /// @param enabled Whether or not to switch relay to enabled state
    void switchRelay(Relay relay, bool enabled);
};

/// @brief Increment relay
/// @param relay Relay to increment
/// @return Incremented relay
Controller::Relay& operator++(Controller::Relay& relay);

} // namespace kc
