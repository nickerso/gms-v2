#ifndef GMS_CONFIG_H
#define GMS_CONFIG_H

#include <string>
#include <map>

namespace gms {

class ServerData;

/**
 * @brief The GMS config service.
 * This service interacts with the server configuration information, to either return
 * it as a JSON string, save it to a file in the working folder, or load it from a file in the working folder.
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
     * @param path
     * @param argvals
     * @param data
     * @return Response string in JSON format.
     */
    std::string execute(const std::string& path, const std::map<std::string, std::string> &argvals,
                        ServerData *data);
};

} // namespace gms

#endif // GMS_CONFIG_H
