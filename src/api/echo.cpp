#include "echo.h"


#include <json/json.h>
#include <json/json-forwards.h>

namespace gms {

static const std::string EchoRootPath = "/echo";

bool Echo::CompatiblePath(const std::string &path)
{
    if (path.compare(0, EchoRootPath.length(), EchoRootPath) == 0) return true;
    return false;
}

Echo::Echo()
{
    // nothing to do
}

Echo::~Echo()
{
    // nothing to do
}

std::string Echo::execute(const std::string& path, const std::map<std::string, std::string> &argvals,
                          ServerData* data)
{
    Json::Value root;
    root["path"] = (path.length() > EchoRootPath.length()+1) ? path.substr(EchoRootPath.length()+1) : "";
    for (const auto& a: argvals)
    {
        root["arguments"][a.first] = a.second;
    }
    std::string response = Json::FastWriter().write(root);
    return response;
}

} // namespace gms
