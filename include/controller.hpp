#pragma once

// STL modules
#include <memory>
#include <map>
#include <mutex>
#include <stdexcept>

// Library {fmt}
#include <fmt/format.h>

// Custom modules
#include "config.hpp"
#include "i2c.hpp"

namespace kc {

class Controller
{
public:
    using Pointer = std::shared_ptr<Controller>;

    enum class Relay
    {
        One,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Nine,
        Ten,
        Eleven,
        Twelve,
        Thirteen,
        Fourteen,
        Fifteen,
        Sixteen,
        MaxRelays,
    };

    struct State
    {
        bool enabled;
    };

private:
    /// @brief Get relay's control mask
    /// @param relay Relay whose control mask to get
    /// @throw std::runtime_error if relay is unknown
    /// @return Relay's control mask
    static uint16_t ControlMask(Relay relay);

public:
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
    I2C::Device m_driver1;
    I2C::Device m_driver2;

private:
    /// @brief Switch relays to their states
    /// @param force Whether or not to force state switch
    void switchRelays(bool force = false);

public:
    /// @brief Initialize relay controller
    /// @param config Initialized config
    Controller(Config::Pointer config);

    ~Controller();

    /// @brief Get relay state
    /// @param relay Relay whose state to get
    /// @throw std::invalid_argument if relay is unknown
    State getState(Relay relay);

    /// @brief Set relay state
    /// @param relay Relay whose state to set
    /// @param enabled Whether or not to switch relay to enabled state
    /// @throw std::invalid_argument if relay is unknown
    void setState(Relay relay, bool enabled);

    /// @brief Set all relays state
    /// @param enabled Whether or not to switch all relays to enabled state
    void setAllStates(bool enabled);
};

/// @brief Increment relay enumerator
/// @param relay Relay enumerator to increment
/// @return Incremented relay enumerator
Controller::Relay& operator++(Controller::Relay& relay);

} // namespace kc
