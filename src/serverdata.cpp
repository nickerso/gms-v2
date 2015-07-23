#include "serverdata.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "oauthcurl.h"
#include "json/json-forwards.h"
#include "json/json.h"

namespace gms {

// allowed callback domain: localhost
//static std::string consumer_key = "RZyfh0zasuRGvK_DsiiNhNuh"; // Key from Tommy
//static std::string consumer_secret = "8-5bVfnChgJdGb8MaocGa2_A"; // Secret from Tommy

// allowed callback domain: localhost:1234
static std::string consumer_key = "iZfWly-W0hgB3IlcG1YEj9vy"; // Key from Tommy
static std::string consumer_secret = "dtE_ufYhCYD0OUuKy6BGNkQg"; // Secret from Tommy

static int CONFIG_VERSION = 1;
static std::string CONFIG_VERSION_KEY = "GMS_configuration_version";
static std::string CONFIG_REPOSITORY_KEY = "repository_url";
static std::string CONFIG_WORKING_FOLDER_KEY = "working_folder";
static std::string CONFIG_ACCESS_TOKEN_KEY = "access_token";

ServerData::ServerData(const std::string& repositoryUrl, const std::string& workingFolder) :
    mRepositoryUrl(repositoryUrl), mWorkingFolder(workingFolder), mOauthCurl(NULL)
{
}

ServerData::~ServerData()
{
    if (mOauthCurl) delete mOauthCurl;
}

const std::string& ServerData::getRepositoryUrl() const
{
    return mRepositoryUrl;
}

const std::string& ServerData::getWorkingFolder() const
{
    return mWorkingFolder;
}

bool ServerData::isAuthenticated() const
{
    if (mOauthCurl) return mOauthCurl->isAuthenticated();
    return false;
}

bool ServerData::isAuthenticationUrlSet() const
{
    if (mOauthCurl) return mOauthCurl->isAuthenticationUrlSet();
    return false;
}

std::string ServerData::getAuthenticationUrl()
{
    if (mOauthCurl) delete mOauthCurl;
    mOauthCurl = new OauthCurl(mRepositoryUrl, consumer_key, consumer_secret);
    std::string url = mOauthCurl->getAuthenticationUrl();
    return url;
}

bool ServerData::authenticate(const std::string &oauthVerifier)
{
    if (mOauthCurl) return mOauthCurl->authenticate(oauthVerifier);
    return false;
}

std::string ServerData::testGet()
{
    if (mOauthCurl) return mOauthCurl->testGet();
    return "";
}

std::string ServerData::getConfiguration() const
{
    Json::Value root;
    root[CONFIG_VERSION_KEY] = CONFIG_VERSION; /// Might be useful one day?
    root[CONFIG_REPOSITORY_KEY] = mRepositoryUrl;
    root[CONFIG_WORKING_FOLDER_KEY] = mWorkingFolder;
    if (isAuthenticated())
    {
        std::istringstream at(mOauthCurl->getAuthenticationSettings());
        at >> root[CONFIG_ACCESS_TOKEN_KEY];
    }
    std::string response = Json::FastWriter().write(root);
    return response;
}

int ServerData::saveConfiguration(const std::string& configFile)
{
    std::cout << "Saving server configuration to: " << configFile << std::endl;
    std::string configuration = getConfiguration();
    std::filebuf fb;
    fb.open (configFile, std::ios::out | std::ios::trunc);
    std::ostream os(&fb);
    os << configuration << std::endl;
    fb.close();
    return 0;
}

int ServerData::loadConfiguration(const std::string& configFile)
{
    std::cout << "Loading configuration from: " << configFile << std::endl;
    Json::Value config;
    std::filebuf fb;
    fb.open (configFile, std::ios::in);
    std::istream is(&fb);
    is >> config;
    fb.close();
    std::cout << "loading config: **" << config << "**" << std::endl;
    // check config version number?
    int version = config[CONFIG_VERSION_KEY].asInt();
    if (version != CONFIG_VERSION)
    {
        std::cerr << "Found GMS configuration version " << version << "; and don't know how to handle that?!" << std::endl;
        return -1;
    }
    // for now, fail if the repository or working folder are different
    std::string repo = config[CONFIG_REPOSITORY_KEY].asString();
    if (repo != mRepositoryUrl)
    {
        /// @todo Maybe its alright to simply reset the repository?
        std::cerr << "Configuration repository (" << repo << ") does not match the one this instance is set to." << std::endl;
        return -2;
    }
    std::string wf = config[CONFIG_WORKING_FOLDER_KEY].asString();
    if (wf != mWorkingFolder)
    {
        /// @todo Maybe its ok to just reset the working folder?
        std::cerr << "Configuration working folder (" << wf << ") does not match the one this instance is set to. Andre might want to allow this." << std::endl;
        return -3;
    }
    // check for an access token
    if (config.isMember(CONFIG_ACCESS_TOKEN_KEY))
    {
        Json::Value accessToken = config[CONFIG_ACCESS_TOKEN_KEY];
        if (mOauthCurl) delete mOauthCurl;
        mOauthCurl = new OauthCurl(mRepositoryUrl, consumer_key, consumer_secret);
        if (mOauthCurl->setAuthenticationSettings(Json::FastWriter().write(accessToken)) != 0)
        {
            std::cerr << "Error loading access token from configuration." << std::endl;
            return -4;
        }
    }
    return 0;
}

} // namespace gms
