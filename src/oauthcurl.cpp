#include "oauthcurl.h"

#include <iostream>
#include <curl/curl.h>
#include <vector>
#include <sstream>
#include <liboauthcpp/liboauthcpp.h>

#include "json/json-forwards.h"
#include "json/json.h"

namespace gms {

/**
 * @brief The mime-type we use for accessing PMR2 web services.
 *
 * We could use application/json directly, but using the versioned PMR2 specific
 * type isolates us from future changes.
 */
static std::string pmr2JsonMimeType =  "application/vnd.physiome.pmr2.json.0";

static int curlCallback(char* data, size_t size, size_t nmemb, std::string* dataObj);

class OauthData {
public:
    OauthData() : consumer(NULL), client(NULL), requestToken(NULL), accessToken(NULL)
    {
        std::cout << "Creating a persistent curl handle for an OauthData" << std::endl;
        mCurl = curl_easy_init();
        setupCallbacks();
    }
    ~OauthData()
    {
        std::cout << "Destroying a persistent curl handle for an OauthData" << std::endl;
        if (mCurl) curl_easy_cleanup(mCurl);
        if (consumer) delete consumer;
        if (accessToken) delete accessToken;
        if (requestToken) delete requestToken;
        if (client) delete client;
    }

    OAuth::Consumer* consumer;
    OAuth::Client* client;
    OAuth::Token* requestToken;
    OAuth::Token* accessToken;
    std::string performPost(const std::string& url, const std::string& header = "", const std::string& dataStr = "")
    {
        resetCurl();
        struct curl_slist* headerList = NULL;
        /* Set header */
        if (header.length())
        {
            headerList = curl_slist_append(headerList, header.c_str());
            if (headerList)
            {
                curl_easy_setopt(mCurl, CURLOPT_HTTPHEADER, headerList);
            }
        }
        /* Set http request, url and data */
        curl_easy_setopt(mCurl, CURLOPT_POST, 1);
        curl_easy_setopt(mCurl, CURLOPT_URL, url.c_str());
        if (dataStr.length())
        {
            curl_easy_setopt(mCurl, CURLOPT_COPYPOSTFIELDS, dataStr.c_str());
        }
        else
        {
            curl_easy_setopt(mCurl, CURLOPT_POSTFIELDSIZE, 0);
        }

        /* Send http request */
        CURLcode res = curl_easy_perform(mCurl);
        // tidy up
        if(headerList) curl_slist_free_all(headerList);
        // check result
        if(CURLE_OK != res)
        {
            /* we failed */
            std::cerr << "curl told us " << res << std::endl;
            return "";
        }
        // check headers
        if (!checkOKresponse())
        {
            mCurlCallbackData.clear();
        }
        return mCurlCallbackData;
    }

    std::string performGet(const std::string& url, const std::string& authorisationHeader,
                           const std::string& contentType)
    {
        resetCurl();
        struct curl_slist* headerList = NULL;
        /* Set authorisation header (expected to have the "Authorization:" prefix */
        if (authorisationHeader.length())
            headerList = curl_slist_append(headerList, authorisationHeader.c_str());
        if (contentType.length())
        {
            std::string contentTypeHeader = "Content-Type: " + contentType;
            headerList = curl_slist_append(headerList, contentTypeHeader.c_str());
            contentTypeHeader = "Accept: " + contentType;
            headerList = curl_slist_append(headerList, contentTypeHeader.c_str());
        }
        if (headerList) curl_easy_setopt(mCurl, CURLOPT_HTTPHEADER, headerList);
        /* Set http request, url and data */
        curl_easy_setopt(mCurl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(mCurl, CURLOPT_URL, url.c_str());
        /* Send http request */
        CURLcode res = curl_easy_perform(mCurl);
        // tidy up
        if(headerList) curl_slist_free_all(headerList);
        // check result
        if(CURLE_OK != res)
        {
            /* we failed */
            std::cerr << "curl told us " << res << std::endl;
            return "";
        }
        // check headers
        if (!checkOKresponse())
        {
            mCurlCallbackData.clear();
        }
        return mCurlCallbackData;
    }

private:
    CURL* mCurl;
    std::string mCurlCallbackData;
    std::string mResponseHeaders;
    void setupCallbacks()
    {
        /* Set callback function to get response */
        curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, curlCallback);
        curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, &mCurlCallbackData);
        curl_easy_setopt(mCurl, CURLOPT_WRITEHEADER, &mResponseHeaders);
        curl_easy_setopt(mCurl, CURLOPT_VERBOSE, 1);
    }
    void resetCurl()
    {
        curl_easy_reset(mCurl);
        /* Clear callback and error buffers */
        mCurlCallbackData = "";
        mResponseHeaders = "";
        setupCallbacks();
    }

    bool checkOKresponse()
    {
        long responseCode;
        curl_easy_getinfo(mCurl, CURLINFO_RESPONSE_CODE, &responseCode);
        if (responseCode == 200) return true;
        std::cerr << "Bad response: " << responseCode << std::endl;
        return false;
    }

};

static int curlCallback(char* data, size_t size, size_t nmemb, std::string* dataObj)
{
    if(dataObj && data)
    {
        size_t bytes = size * nmemb;
        dataObj->append(data, bytes);
        return bytes;
    }
    return 0;
}


OauthCurl::OauthCurl()
{
    // should never be used directly?
}

OauthCurl::OauthCurl(const std::string &pmr2InstanceUrl,
                     const std::string &consumerKey,
                     const std::string &consumerSecret) :
    mInstanceUrl(pmr2InstanceUrl), mConsumerKey(consumerKey),
    mConsumerSecret(consumerSecret), mAuthenticated(false), mAuthenticationUrlSet(false)
{
    if (mInstanceUrl.back() == '/') mInstanceUrl.pop_back();
    mOauth = new OauthData();
    mOauth->consumer = new OAuth::Consumer(mConsumerKey, mConsumerSecret);
    mOauth->client = new OAuth::Client(mOauth->consumer);
}

OauthCurl::~OauthCurl()
{
    delete mOauth;
}

std::string OauthCurl::getAuthenticationUrl()
{
    // Step 1: Get a request token. This is a temporary token that is used for
    // having the user authorize an access token and to sign the request to
    // obtain said access token.

    // add in the scope of our token to define the limits of the access token
    // FIXME: this is all the available features - do we need them all?
    std::string scopeBaseUrl = mInstanceUrl + "/oauth_scope/";
    std::string collectionScope = scopeBaseUrl + "collection";
    std::string searchScope =  scopeBaseUrl + "search";
    std::string workspaceTempAuthScope =  scopeBaseUrl + "workspace_tempauth";
    std::string workspaceFullScope = scopeBaseUrl + "workspace_full";
    std::string request_token_query_args = "scope=" + OAuth::PercentEncode(
                collectionScope + "," + searchScope + "," +
                workspaceTempAuthScope + "," + workspaceFullScope);

    std::string requestTokenUrl = mInstanceUrl + "/OAuthRequestToken";
    std::string authoriseUrl = mInstanceUrl + "/OAuthAuthorizeToken";

    std::string base_request_token_url = requestTokenUrl + "?"
            + request_token_query_args;

    mOauth->client->setCallbackUrl("http://localhost:1234/register");
    //mOauth->client->setCallbackUrl("oob");

    std::string oAuthHeader = mOauth->client->getFormattedHttpHeader(OAuth::Http::Post, base_request_token_url);
    std::cout << "base_request_token_url: " << base_request_token_url << std::endl;
    std::cout << "oAuthHeader: " << oAuthHeader << std::endl;

    std::string requestTokenResponse = mOauth->performPost(base_request_token_url, oAuthHeader);

    std::cout << "Request token returned: " << requestTokenResponse << std::endl;

    // seems liboauth doesn't always check things, so we need to check the result ourselves
    if (requestTokenResponse.find("oauth_token") == std::string::npos)
    {
        std::cerr << "Unable to get get request token, so quitting." << std::endl;
        return "";
    }

    // Extract the token and token_secret from the response
    // This time we pass the response directly and have the library do the
    // parsing (see next extractToken call for alternative)
    OAuth::Token request_token = OAuth::Token::extract(requestTokenResponse);

    // Get access token and secret from OAuth object
    std::cout << "Request Token:" << std::endl;
    std::cout << "    - oauth_token        = " << request_token.key() << std::endl;
    std::cout << "    - oauth_token_secret = " << request_token.secret() << std::endl;
    std::cout << std::endl;

    // Step 2: Redirect to the provider.
    std::string authenticationUrl = authoriseUrl + "?oauth_token=" +
            request_token.key();
    mAuthenticationUrlSet = true;

    // save the request token for use later
    mOauth->requestToken = new OAuth::Token(request_token.key(), request_token.secret());

    return authenticationUrl;
}

bool OauthCurl::authenticate(const std::string &oauthVerifier)
{
    // After the user has granted access to you, the consumer, the
    // provider will redirect you to whatever URL you have told them
    // to redirect to. You can usually define this in the
    // oauth_callback argument as well.
    mOauth->requestToken->setPin(oauthVerifier);

    // Step 3: Once the consumer has redirected the user back to the
    // oauth_callback URL you can request the access token the user
    // has approved. You use the request token to sign this
    // request. After this is done you throw away the request token
    // and use the access token returned. You should store the oauth
    // token and token secret somewhere safe, like a database, for
    // future use.
    //oauth = OAuth::Client(&consumer, &request_token);
    delete mOauth->client;
    mOauth->client = new OAuth::Client(mOauth->consumer, mOauth->requestToken);
    // Note that we explicitly specify an empty body here (it's a GET) so we can
    // also specify to include the oauth_verifier parameter
    std::string getAccessTokenUrl = mInstanceUrl + "/OAuthGetAccessToken";
    std::string oAuthHeader = mOauth->client->getFormattedHttpHeader(OAuth::Http::Post, getAccessTokenUrl, "", true);
    //oAuthQueryString = oauth.getURLQueryString( OAuth::Http::Get, access_token_url, std::string( "" ), true );

    //std::cout << "Enter the following in your browser to get the final access token & secret: " << std::endl;
    //std::cout << access_token_url << "?" << oAuthQueryString;
    //std::cout << std::endl;

    std::string accessTokenResponse = mOauth->performPost(getAccessTokenUrl, oAuthHeader);

    std::cout << "Access token returned: " << accessTokenResponse << std::endl;

    // Once they've come back from the browser, extract the token and token_secret from the response
    //std::string access_token_resp = getUserString("Enter the response:");
    // On this extractToken, we do the parsing ourselves (via the library) so we
    // can extract additional keys that are sent back, in the case of twitter,
    // the screen_name
    OAuth::KeyValuePairs access_token_resp_data = OAuth::ParseKeyValuePairs(accessTokenResponse);
    OAuth::Token access_token = OAuth::Token::extract(access_token_resp_data);

    std::cout << "Access token:" << std::endl;
    std::cout << "    - oauth_token        = " << access_token.key() << std::endl;
    std::cout << "    - oauth_token_secret = " << access_token.secret() << std::endl;
    std::cout << std::endl;
    std::cout << "You may now access protected resources using the access tokens above." << std::endl;
    std::cout << std::endl;

    // now we can save the access token and discard the request token and use the access token for future requests
    mOauth->accessToken = new OAuth::Token(access_token.key(), access_token.secret());
    delete mOauth->client;
    mOauth->client = new OAuth::Client(mOauth->consumer, mOauth->accessToken);
    mAuthenticated = true;
    return true;
}

std::string OauthCurl::testGet()
{
    std::string testUrl = mInstanceUrl + "/pmr2-dashboard";
    std::string authHeader = "";
    if (mAuthenticated)
    {
        authHeader = mOauth->client->getFormattedHttpHeader(OAuth::Http::Get, testUrl);
    }
    std::string response = mOauth->performGet(testUrl, authHeader, pmr2JsonMimeType);
    return response;
}

std::string OauthCurl::getAuthenticationSettings() const
{
    Json::Value root;
    if (mAuthenticated)
    {
        root["access_token"] = mOauth->accessToken->key();
        root["access_secret"] = mOauth->accessToken->secret();
    }
    std::string response = Json::FastWriter().write(root);
    return response;
}

} // namespace gms
