#ifndef REGISTER_H
#define REGISTER_H

#include <string>
#include <map>

namespace gms {

class ServerData;
class OauthCurl;

/**
 * @brief The GMS Register service handles registering users with a PMR2 instance.
 *
 * This service will manage the OAuth authemtication of users with a given instance of a PMR2 instance.
 */
class Register
{
public:
    /**
     * @brief Static method to check if a given path is compatible with the register service.
     * @param path The path to check.
     * @return True if the given path is compatible, false otherwise.
     */
    static bool CompatiblePath(const std::string& path);

    /**
     * @brief Default constructor.
     */
    Register();

    /**
     * @brief Default desctructor.
     */
    ~Register();

    /**
     * @brief Execute the register service with the given arguments.
     * @param path The URL path component.
     * @param argvals Any arguments specified with the URL triggering this service call.
     * @param data The GMS server data.
     * @return Response string in JSON format.
     */
    std::string execute(const std::string& path, const std::map<std::string, std::string> &argvals,
                        ServerData *data);
};

} // namespace gms

#endif // REGISTER_H
