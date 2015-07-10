#include "echo.h"

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
    std::string response = path.substr(EchoRootPath.length()+1);
    for (const auto& a: argvals)
    {
        response += "; argument with key *" + a.first + "* and value *" + a.second + "*";
    }
    response += "\n";
    return response;
}

} // namespace gms
