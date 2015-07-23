#ifndef GMS_CONFIG_H
#define GMS_CONFIG_H

#include <string>
#include <map>

namespace gms {

class ServerData;

/**
 * @brief The GMS config service.
 * This service displays the server configuration information.
 */
class Config
{
public:
    /**
     * @brief Static method to check if given path is compatible with the config service.
     * @param path The path to check.
     * @return True if the given path is compatible, false otherwise.
     */
    static bool CompatiblePath(const std::string& path);

    /**
     * @brief Default constuctor.
     */
    Config();

    /**
      * @brief Destructor.
      */
    ~Config();

    /**
     * @brief Execute the config service with the given arguments.
     * Currently all arguments are ignored and the configuration is simply returned.
     * @param path
     * @param argvals
     * @param data
     * @return Response string in JSON format. Will contain a returnCode attribute with a value of 0 on success. The response string will also contain the configuration information.
     */
    std::string execute(const std::string& path, const std::map<std::string, std::string> &argvals,
                        ServerData *data);
};

} // namespace gms

#endif // GMS_CONFIG_H
