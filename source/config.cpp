#include "config.hpp"

namespace kc {

void Config::GenerateSampleFile()
{
    using namespace ConfigConst;

    std::ofstream configFile(ConfigFile);
    if (!configFile)
        throw std::runtime_error("kc::Config::GenerateSampleFile(): Couldn't create sample configuration file");

    json configJson;
    configJson[Objects::HttpPort] = Defaults::HttpPort;
    configFile << configJson.dump(4) << '\n';
}

Config::Config()
{
    using namespace ConfigConst;

    std::ifstream configFile(ConfigFile);
    if (!configFile)
        throw Error(fmt::format("Couldn't open configuration file \"{}\"", ConfigFile).c_str());

    try
    {
        json configJson = json::parse(configFile);
        m_httpPort = configJson[Objects::HttpPort];
    }
    catch (const json::exception&)
    {
        throw Error(fmt::format("Couldn't parse configuration file \"{}\" JSON", ConfigFile).c_str());
    }
}

} // namespace kc
