#pragma once

// STL modules
#include <string>
#include <vector>
#include <stdexcept>

// UNIX modules
#include <unistd.h>
#include <errno.h>

// Library WiringPi
#include <wiringPiI2C.h>

// Library {fmt}
#include <fmt/format.h>

namespace kc {

namespace I2C
{
    class Device
    {
    private:
        std::string m_port;
        uint8_t m_address;
        int m_fd;

    public:
        /// @brief Initialize communication with I2C device
        /// @param port I2C port to use
        /// @param address I2C device address
        /// @throw std::runtime_error if internal error occurs
        Device(const std::string& port, uint8_t address);

        ~Device();

        /// @brief Send data to I2C device
        /// @param data Data to send
        /// @throw std::runtime_error if internal error occurs
        void send(const std::vector<uint8_t>& data);

        /// @brief Receive data from I2C device
        /// @param length Length of data to receive
        /// @throw std::runtime_error if internal error occurs
        /// @return Received data
        std::vector<uint8_t> receive(int length);
    };
}

} // namespace kc
