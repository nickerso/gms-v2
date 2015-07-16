#include "oauthcurl.h"

#include <iostream>
#include <curl/curl.h>
#include <vector>
#include <sstream>
#include <liboauthcpp/liboauthcpp.h>

namespace gms {

static int curlCallback(char* data, size_t size, size_t nmemb, OauthData* dataObj);

class OauthData {
public:
    OauthData()
    {
        std::cout << "Creating a persistent curl handle for an OauthData" << std::endl;
        mCurl = curl_easy_init();
        /* Set callback function to get response */
        curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, curlCallback);
        curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, this);
    }
    ~OauthData()
    {
        std::cout << "Destroying a persistent curl handle for an OauthData" << std::endl;
        if (mCurl) curl_easy_cleanup(mCurl);
    }

    OAuth::Consumer* consumer;
    OAuth::Client* client;
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

        /* Send http request */
        if (CURLE_OK == curl_easy_perform(mCurl))
        {
            if(headerList) curl_slist_free_all(headerList);
            return mCurlCallbackData;
        }
        if (headerList) curl_slist_free_all(headerList);
        return "";
    }

    int saveLastWebResponse(char*& data, size_t size)
    {
        if(data && size)
        {
            /* Append data in our internal buffer */
            mCurlCallbackData.append(data, size);
            return (int)size;
        }
        return 0;
    }

private:
    CURL* mCurl;
    std::string mCurlCallbackData;
    void resetCurl()
    {
        /* Restore any custom request we may have */
        curl_easy_setopt(mCurl, CURLOPT_CUSTOMREQUEST, NULL);
        /* All supported encodings */
        curl_easy_setopt(mCurl, CURLOPT_ENCODING, "");
        /* Clear callback and error buffers */
        mCurlCallbackData = "";
    }
};

static int curlCallback(char* data, size_t size, size_t nmemb, OauthData* dataObj)
{
    if(dataObj && data)
    {
        /* Save http response in our object's buffer */
        return dataObj->saveLastWebResponse(data, (size*nmemb));
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
    mConsumerSecret(consumerSecret), mAuthenticated(false)
{
    if (mInstanceUrl.back() == '/') mInstanceUrl.pop_back();
    mOauth = new OauthData();
    mOauth->consumer = new OAuth::Consumer(mConsumerKey, mConsumerSecret);
    mOauth->client = new OAuth::Client(mOauth->consumer);
}

OauthCurl::~OauthCurl()
{
    delete mOauth->client;
    delete mOauth->consumer;
    delete mOauth;
}

bool OauthCurl::authenticate()
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
    std::string request_token_query_args = "oauth_callback=oob";
    request_token_query_args += "&scope=" + OAuth::PercentEncode(
                collectionScope + "," + searchScope + "," +
                workspaceTempAuthScope + "," + workspaceFullScope);

    std::string requestTokenUrl = mInstanceUrl + "/OAuthRequestToken";
    std::string base_request_token_url = requestTokenUrl + "?"
            + request_token_query_args;
    std::string oAuthHeader = mOauth->client->getHttpHeader(OAuth::Http::Post, base_request_token_url);
    std::string requestTokenResponse = mOauth->performPost(base_request_token_url, oAuthHeader);

    std::cout << "Request token returned: " << requestTokenResponse << std::endl;

    // seems liboauth doesn't always check things, so we need to check the result ourselves
    if (requestTokenResponse.length() == 0)
    {
        std::cerr << "Unable to get get request token, so quitting." << std::endl;
        return false;
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

    return false;
}

} // namespace gms