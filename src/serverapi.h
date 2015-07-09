#ifndef SERVERAPI_H
#define SERVERAPI_H

#include <map>
#include <string>

namespace gms {

class ServerData;

/**
 * @brief The main entry point to the GMS web-service API.
 * This is the entry point to the GMS web-service API. The specified URL and arguments
 * will be queried to determine the appropriate service to handle the request.
 * @param url The requested URL without any arguments (e.g., "/model/path/to/model" from the complete requested URL "http://localhost:1234/model/path/to/model?arg1=argValue&arg2=NotSure").
 * @param argvals The key:value pairs of any arguments specified in the requested URL (e.g., "arg1=argValue&arg2=NotSure" from the complete requested URL "http://localhost:1234/model/path/to/model?arg1=argValue&arg2=NotSure").
 * @param data Pointer to the server data for this instance of the server.
 * @return On successful completion of the request, a JSON encoded string with the response to the request. On failure, the string "BAD RESPONSE" is returned.
 */
std::string executeAPI(const std::string& url, const std::map<std::string, std::string>& argvals,
                       ServerData* data);

} // namespace gms

#endif // SERVERAPI_H
