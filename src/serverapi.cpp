#include "serverapi.h"

#include <iostream>

#include "serverdata.h"
#include "api/echo.h"
#include "api/config.h"

namespace gms{

std::string executeAPI(const std::string& url, const std::map<std::string, std::string> &argvals,
                       ServerData *data)
{
    std::string response("BAD RESPONSE");

    if (url == "/favicon.ico")
    {
        // do nothing?
        response = "";
    }
    else if (Echo::CompatiblePath(url))
    {
        Echo echo;
        response = echo.execute(url, argvals, data);
    }
    else if (Config::CompatiblePath(url))
    {
        Config config;
        response = config.execute(url, argvals, data);
    }
    else
    {
        std::cout << "unknown API:\n"
                  << "\turl = \"" << url << "\"\n"
                  << "\tnumber of args = " << argvals.size() << "\n"
                  << "\trepository URL: " << data->getRepositoryUrl() << std::endl;
    }
    return response;
}

} // namespace gms
