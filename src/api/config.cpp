#include "config.h"

#include <iostream>
#include <sstream>

#include <json/json.h>
#include <json/json-forwards.h>

#include "../serverdata.h"

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
    std::string response;
    std::string serverConfiguration = data->getConfiguration();
    std::istringstream sc(serverConfiguration);
    sc >> root["serverConfiguration"];
    root["returnCode"] = 0;
    response = Json::FastWriter().write(root);
    return response;
}

} // namespace gms
