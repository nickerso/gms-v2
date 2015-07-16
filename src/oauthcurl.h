#ifndef OAUTHCURL_H
#define OAUTHCURL_H

#include <string>

namespace gms {

class OauthData;

/**
 * @brief The OauthCurl class.
 * This class is used to help manage HTTP requests using OAuth with a PMR2 instance. Actual requests are performed using libcurl. Most of the concepts here from twitcurl (https://github.com/swatkat/twitcurl/) and the liboauthcpp demos (https://github.com/sirikata/liboauthcpp).
 */
class OauthCurl
{
public:
    /**
     * @brief The public OauthCurl constructor requires the application specific information to be specified.
     * @param pmr2InstanceUrl The URL of the PMR2 instance to use for this instance of OauthCurl.
     * @param consumerKey The application specific key.
     * @param consumerSecret The application specific secret.
     */
    OauthCurl(const std::string& pmr2InstanceUrl, const std::string& consumerKey,
              const std::string& consumerSecret);

    /**
     * Default destructor.
     */
    ~OauthCurl();

    /**
     * Check if this instance of OauthCurl has successfully been authenticated against an instance of PMR2.
     * i.e., a user has granted us permission to access their data on the PMR2 instance.
     * @return true if successfully authenticated, false otherwise.
     */
    inline bool isAuthenticated() const
    {
        return mAuthenticated;
    }

    /**
     * Register this instance to our PMR2 instance and request the user to authenticate us.
     * @return true if we are successful in authenticating, false otherwise.
     */
    bool authenticate();

private:
    /**
     * @brief Hide the default constructor.
     */
    OauthCurl();

    std::string mInstanceUrl;
    std::string mConsumerKey;
    std::string mConsumerSecret;
    bool mAuthenticated;
    OauthData* mOauth;
};

} // namespace gms

#endif // OAUTHCURL_H
