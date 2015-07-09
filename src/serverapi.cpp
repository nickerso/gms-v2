#include "serverapi.h"

#include <iostream>

#include "serverdata.h"
namespace gms{

std::string executeAPI(const std::string& url, const std::map<std::string, std::string> &argvals,
                       ServerData *data)
{
    std::string response("BAD RESPONSE");
    std::cout << "executing API:\n"
              << "\turl = \"" << url << "\"\n"
              << "\tnumber of args = " << argvals.size() << "\n"
              << "\trepository URL: " << data->getRepositoryUrl() << std::endl;
    return response;
}

} // namespace gms
