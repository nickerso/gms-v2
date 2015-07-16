#include "register.h"

#include <iostream>
#include <json/json.h>
#include <json/json-forwards.h>
#include <liboauthcpp/liboauthcpp.h>

#include "../serverdata.h"
#include "../curlutils.h"

namespace gms {

static const std::string RegisterRootPath = "/register";

bool Register::CompatiblePath(const std::string &path)
{
    if (path.compare(0, RegisterRootPath.length(), RegisterRootPath) == 0) return true;
    return false;
}

Register::Register()
{
    // nothing to do
}

Register::~Register()
{
    // nothing to do
}

std::string Register::execute(const std::string& path, const std::map<std::string, std::string>& argvals,
                              ServerData* data)
{
    if (data->isAuthenticated())
    {
        /**
         * @todo we need to decide if we should allow re-registering, but for now
         * once you are authenticated that is it.
         */
        Json::Value root;
        root["authenticated"] = true;
        std::string response = Json::FastWriter().write(root);
        return response;
    }

    bool resp = data->authenticate();
    Json::Value root;
    root["authenticated"] = resp;
    std::string response = Json::FastWriter().write(root);
    return response;
}


} // namespace gms
