#include "register.h"

#include <iostream>
#include <json/json.h>
#include <json/json-forwards.h>
#include <liboauthcpp/liboauthcpp.h>

#include "../serverdata.h"
#include "../curlutils.h"

namespace gms {

static std::string consumer_key = "RZyfh0zasuRGvK_DsiiNhNuh"; // Key from Tommy
static std::string consumer_secret = "8-5bVfnChgJdGb8MaocGa2_A"; // Secret from Tommy

static const std::string RegisterRootPath = "/register";

bool Register::CompatiblePath(const std::string &path)
{
    if (path.compare(0, RegisterRootPath.length(), RegisterRootPath) == 0) return true;
    return false;
}

Register::Register()
{
    // nothing to do
}

Register::~Register()
{
    // nothing to do
}

std::string Register::execute(const std::string& path, const std::map<std::string, std::string>& argvals,
                              ServerData* data)
{
    // initialisation
    OAuth::Consumer consumer(consumer_key, consumer_secret);
    OAuth::Client oauth(&consumer);

    // Step 1: Get a request token. This is a temporary token that is used for
    // having the user authorize an access token and to sign the request to
    // obtain said access token.

    // add in the scope of our token to define the limits of the access token
    // FIXME: this is all the available features - do we need them all?
    std::string scopeBaseUrl = data->getRepositoryUrl() + "/oauth_scope/";
    std::string collectionScope = scopeBaseUrl + "collection";
    std::string searchScope =  scopeBaseUrl + "search";
    std::string workspaceTempAuthScope =  scopeBaseUrl + "workspace_tempauth";
    std::string workspaceFullScope = scopeBaseUrl + "workspace_full";
    std::string request_token_query_args = "oauth_callback=oob";
    request_token_query_args += "&scope=" + OAuth::PercentEncode(
                collectionScope + "," + searchScope + "," + workspaceTempAuthScope +
                "," + workspaceFullScope
                );

    std::string requestTokenUrl = data->getRepositoryUrl() + "/OAuthRequestToken";
    std::string base_request_token_url = requestTokenUrl + (request_token_query_args.empty() ? std::string("") : (std::string("?")+request_token_query_args) );
    std::string oAuthQueryString =
        oauth.getURLQueryString( OAuth::Http::Get, base_request_token_url);
    std::string oAuthHttpHeader =
        oauth.getFormattedHttpHeader(OAuth::Http::Get, base_request_token_url);

    //requestTokenUrl += "?" + oAuthQueryString;
    std::string requestToken = getUrlContent(requestTokenUrl + "?" + oAuthQueryString);
    std::cout << "Request token returned: " << requestToken << std::endl;
    requestToken = getUrlWithHeaders(requestTokenUrl, oAuthHttpHeader);
    std::cout << "Request token returned: " << requestToken << std::endl;

    // seems liboauth doesn't always check things, so we need to check the result ourselves
    if (requestToken.length() == 0)
    {
        std::cerr << "Unable to get get request token, so quitting." << std::endl;
        return "MISSING REQUEST TOKEN";
    }

    // Extract the token and token_secret from the response
    // This time we pass the response directly and have the library do the
    // parsing (see next extractToken call for alternative)
    OAuth::Token request_token = OAuth::Token::extract(requestToken);

    // Get access token and secret from OAuth object
    std::cout << "Request Token:" << std::endl;
    std::cout << "    - oauth_token        = " << request_token.key() << std::endl;
    std::cout << "    - oauth_token_secret = " << request_token.secret() << std::endl;
    std::cout << std::endl;

    return "bob";
}


} // namespace gms
