#ifndef SERVERDATA_H
#define SERVERDATA_H

#include <string>

//! Everything in GMS is in this namespace.
namespace gms {

/**
 * @brief This class manages the data for this server instance.
 */
class ServerData
{
public:
    /**
     * @brief Default constructor.
     * @param repositoryUrl The URL of the (PMR2) repository to be used in this instance of the server.
     * @param workingFolder The local path to the folder in which to store working copies of repository workspaces.
     */
    ServerData(const std::string& repositoryUrl, const std::string& workingFolder);

    /**
     * @brief Destructor for any clean up.
     */
    ~ServerData();

    /**
     * @brief Get the repository URL for this instance of the server.
     * @return The URL for the repository instance used in this server instance.
     */
    const std::string& getRepositoryUrl() const;

    /**
     * @brief Get the working folder path.
     * @return The local system path to the working folder.
     */
    const std::string& getWorkingFolder() const;

private:
    /**
     * The URL of the repository used in this server instance - set on construction
     * and unable to change.
     *
     * @todo Should we be able to change this dynamically?
     */
    std::string mRepositoryUrl;
    std::string mWorkingFolder;
};

} // namespace gms

#endif // SERVERDATA_H
