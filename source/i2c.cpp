#include "i2c.hpp"

namespace kc {

I2C::Device::Device(const std::string& port, uint8_t address)
    : m_port(port)
    , m_address(address)
    , m_fd(wiringPiI2CSetupInterface(m_port.c_str(), m_address))
{
    if (m_fd == -1)
    {
        throw std::runtime_error(fmt::format(
            "kc::I2C::Device::Device(): Couldn't initialize communication with I2C device [{:#x}] on port \"{}\": errno is {}",
            m_address, m_port, errno
        ));
    }
}

I2C::Device::~Device()
{
    close(m_fd);
}

void I2C::Device::send(const std::vector<uint8_t>& data)
{
    int bytesTransferred = write(m_fd, data.data(), data.size());
    if (bytesTransferred == -1)
    {
        throw std::runtime_error(fmt::format(
            "kc::I2C::Device::send(): Couldn't send data to I2C device [{:#x}] on port \"{}\": errno is {}",
            m_address, m_port, errno
        ));
    }
}

std::vector<uint8_t> I2C::Device::receive(int length)
{
    std::vector<uint8_t> buffer(length);
    int bytesTransferred = read(m_fd, buffer.data(), buffer.size());
    if (bytesTransferred == -1)
    {
        throw std::runtime_error(fmt::format(
            "kc::I2C::Device::receive(): Couldn't receive data from I2C device [{:#x}] on port \"{}\": errno is {}",
            m_address, m_port, errno
        ));
    }
    return buffer;
}

} // namespace kc
