#ifndef ECHO_H
#define ECHO_H

#include <string>
#include <map>

namespace gms {

class ServerData;

/**
 * @brief The GMS Echo service.
 * This service will simply echo the given URL and parameters in JSON format.
 */
class Echo
{
public:
    /**
     * @brief Static method to check if given path is compatible with the echo service.
     * @param path The path to check.
     * @return True if the given path is compatible, false otherwise.
     */
    static bool CompatiblePath(const std::string& path);

    /**
     * @brief Default constuctor.
     */
    Echo();

    /**
      * @brief Destructor.
      */
    ~Echo();

    /**
     * @brief Execute the echo service with the given arguments.
     * @param path
     * @param argvals
     * @param data
     * @return Response string in JSON format.
     */
    std::string execute(const std::string& path, const std::map<std::string, std::string> &argvals,
                        ServerData *data);
};

} // namespace gms

#endif // ECHO_H
