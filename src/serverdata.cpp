#include "serverdata.h"


#include "oauthcurl.h"

namespace gms {

// allowed callback domain: localhost
//static std::string consumer_key = "RZyfh0zasuRGvK_DsiiNhNuh"; // Key from Tommy
//static std::string consumer_secret = "8-5bVfnChgJdGb8MaocGa2_A"; // Secret from Tommy

// allowed callback domain: localhost:1234
static std::string consumer_key = "iZfWly-W0hgB3IlcG1YEj9vy"; // Key from Tommy
static std::string consumer_secret = "dtE_ufYhCYD0OUuKy6BGNkQg"; // Secret from Tommy



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

} // namespace gms
