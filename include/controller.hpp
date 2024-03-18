#pragma once

// STL modules
#include <memory>
#include <map>
#include <mutex>
#include <stdexcept>

// Library {fmt}
#include <fmt/format.h>

// Library WiringPi
#include <wiringPi.h>

namespace kc {

class Controller
{
public:
    using Pointer = std::shared_ptr<Controller>;

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

private:
    /// @brief Switch relay
    /// @param pin Relay control pin
    /// @param enable Whether or not to enable relay
    static void switchRelay(int pin, bool enable);

public:
    /// @brief Get relay's control pin
    /// @param relay Relay whose control pin to get
    /// @throw std::runtime_error if relay is unknown
    /// @return Relay's control pin
    static int ControlPin(Relay relay);

    /// @brief Get relay's unique name
    /// @param relay Relay whose unique name to get
    /// @throw std::runtime_error if relay is unknown
    /// @return Relay's unique name
    static const char* UniqueName(Relay relay);

    /// @brief Get relay's name
    /// @param relay Relay whose name to get
    /// @throw std::runtime_error if relay is unknown
    /// @return Relay's name
    static const char* Name(Relay relay);

private:
    std::mutex m_mutex;
    std::map<Relay, State> m_relays;

public:
    /// @brief Initialize relay controller
    Controller();

    ~Controller();

    /// @brief Get relay state
    /// @param relay Relay whose state to get
    /// @throw std::invalid_argument if relay is unknown
    State getState(Relay relay);

    /// @brief Set relay state
    /// @param relay Relay whose state to set
    /// @throw std::invalid_argument if relay is unknown
    /// @param enabled Whether or not to switch relay to enabled state
    void setState(Relay relay, bool enabled);
};

/// @brief Increment relay enumerator
/// @param relay Relay enumerator to increment
/// @return Incremented relay enumerator
Controller::Relay& operator++(Controller::Relay& relay);

} // namespace kc
