#include "config.h"


#include <json/json.h>
#include <json/json-forwards.h>

namespace gms {

static const std::string ConfigRootPath = "/config";

bool Config::CompatiblePath(const std::string &path)
{
    if (path.compare(0, ConfigRootPath.length(), ConfigRootPath) == 0) return true;
    return false;
}

Config::Config()
{
    // nothing to do
}

Config::~Config()
{
    // nothing to do
}

std::string Config::execute(const std::string& path, const std::map<std::string, std::string> &argvals,
                          ServerData* data)
{
    Json::Value root;
    root["path"] = (path.length() > ConfigRootPath.length()+1) ? path.substr(ConfigRootPath.length()+1) : "";
    for (const auto& a: argvals)
    {
        root["arguments"][a.first] = a.second;
    }
    std::string response = Json::FastWriter().write(root);
    return response;
}

} // namespace gms
