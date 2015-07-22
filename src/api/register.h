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
 * This simply provides the compatible path convenience method.
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

};

} // namespace gms

#endif // REGISTER_H
