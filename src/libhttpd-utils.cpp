#include <microhttpd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <iostream>
#include <map>

#include "serverdata.h"
#include "serverapi.h"

// we manage the registration process here.
#include "api/register.h"

#define PAGE "<html><head><title>libmicrohttpd demo</title>"\
             "</head><body>libmicrohttpd demo</body></html>"

namespace gms {

static int ahc_echo(void * cls, struct MHD_Connection * connection,
        const char * url, const char * method, const char * version,
        const char * upload_data, size_t * upload_data_size, void ** ptr)
{
    static int dummy;
    const char * page = (char*) cls;
    struct MHD_Response * response;
    int ret;

    if (0 != strcmp(method, "GET"))
        return MHD_NO; /* unexpected method */
    if (&dummy != *ptr)
    {
        /* The first time only the headers are valid,
         do not respond in the first round... */
        *ptr = &dummy;
        return MHD_YES;
    }
    if (0 != *upload_data_size)
        return MHD_NO; /* upload data in a GET!? */
    *ptr = NULL; /* clear context pointer */
    printf("url: '%s'\n", url);
    response = MHD_create_response_from_data(strlen(page), (void*) page, MHD_NO,
            MHD_NO);
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

int print_out_key(void *cls, enum MHD_ValueKind kind, const char *key,
        const char *value)
{
    printf("%s: %s\n", key, value);
    return MHD_YES;
}

static int answer_to_connection(void *cls, struct MHD_Connection *connection,
        const char *url, const char *method, const char *version,
        const char *upload_data, size_t *upload_data_size, void **con_cls)
{
    printf("New %s request for %s using version %s\n", method, url, version);
    printf("\nMHD_RESPONSE_HEADER_KIND:\n");
    MHD_get_connection_values(connection, MHD_RESPONSE_HEADER_KIND,
            &print_out_key, NULL);
    printf("\nMHD_HEADER_KIND:\n");
    MHD_get_connection_values(connection, MHD_HEADER_KIND, &print_out_key,
            NULL);
    printf("\nMHD_COOKIE_KIND:\n");
    MHD_get_connection_values(connection, MHD_COOKIE_KIND, &print_out_key,
            NULL);
    printf("\nMHD_POSTDATA_KIND:\n");
    MHD_get_connection_values(connection, MHD_POSTDATA_KIND, &print_out_key,
            NULL);
    printf("\nMHD_GET_ARGUMENT_KIND:\n");
    MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, &print_out_key,
            NULL);
    printf("\nMHD_FOOTER_KIND:\n");
    MHD_get_connection_values(connection, MHD_FOOTER_KIND, &print_out_key,
            NULL);
    return MHD_NO;
}

#define ERROR_PAGE "<html><head><title>Error</title></head><body>Bad data</body></html>"

static int send_bad_response(struct MHD_Connection *connection)
{
    static char *bad_response = (char *) ERROR_PAGE;
    int bad_response_len = strlen(bad_response);
    int ret;
    struct MHD_Response *response;

    response = MHD_create_response_from_buffer(bad_response_len, bad_response,
            MHD_RESPMEM_PERSISTENT);
    if (response == 0)
    {
        return MHD_NO;
    }
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

static int send_authenticated_response(struct MHD_Connection *connection)
{
    static char *authResponse = (char *)("Successfully authenticated");
    int response_len = strlen(authResponse);
    int ret;
    struct MHD_Response *response;

    response = MHD_create_response_from_buffer(response_len, authResponse,
            MHD_RESPMEM_PERSISTENT);
    if (response == 0)
    {
        return MHD_NO;
    }
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

/*
 *The Cross-Origin Resource Sharing standard works by adding new HTTP headers
 *that allow servers to describe the set of origins that are permitted to read
 *that information using a web browser. Additionally, for HTTP request methods
 *that can cause side-effects on user data (in particular, for HTTP methods
 *other than GET, or for POST usage with certain MIME types), the specification
 *mandates that browsers "preflight" the request, soliciting supported methods
 *from the server with an HTTP OPTIONS request header, and then, upon
 *"approval" from the server, sending the actual request with the actual HTTP
 *request method. Servers can also notify clients whether "credentials"
 *(including Cookies and HTTP Authentication data) should be sent with
 *requests.
 */
static int sendAccessControl(struct MHD_Connection *connection, const char *url,
        const char *method, const char *version)
{
    int ret;
    struct MHD_Response *response;

    std::cout << "Sending CORS accept header for the request: " << std::endl;

    /*answer_to_connection(NULL, connection, url, method, version, NULL, NULL,
            NULL);*/

    response = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
    if (response == 0)
    {
        return MHD_NO;
    }
    // not too fussed with who is trying to use us :)
    MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
    // only allow GET (and OPTIONS) requests, no need for PUSH yet...now there is a need for post
    MHD_add_response_header(response, "Access-Control-Allow-Methods",
                            "GET, OPTIONS, POST"); // see http://stackoverflow.com/questions/107390/whats-the-difference-between-a-post-and-a-put-http-request
    // we simply 'allow' all requested headers
    const char* val = MHD_lookup_connection_value(connection, MHD_HEADER_KIND,
            "Access-Control-Request-Headers");
    MHD_add_response_header(response, "Access-Control-Allow-Headers", val);
    // these seem to be needed?
    MHD_add_response_header(response, "Access-Control-Expose-Headers",
            "Content-Range");

    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

static int get_url_args(void *cls, MHD_ValueKind kind, const char *key,
        const char* value)
{
    std::cout << "Get Arg with key: '" << key << "'" << std::endl;
    std::map<std::string, std::string> * url_args = static_cast<std::map<std::string, std::string> *>(cls);
    if (url_args->find(key) == url_args->end())
    {
        if (!value)
            (*url_args)[key] = "";
        else
            (*url_args)[key] = value;
    }
    else
    {
        std::cerr << "Duplicate URL argument?\n";
    }
    return MHD_YES;
}

/*
 * Based on information from:
 * https://gnunet.org/svn/libmicrohttpd/doc/examples/simplepost.c
 * http://www.gnu.org/software/libmicrohttpd/tutorial.html#Processing-POST-data
 *
 * to handle POST. But libmicrohttpd only has convenience methods for handling "form" style POSTs, so
 * here we check for JSON content in the uploaded data and handle that ourselves.
 *
 */
struct connection_info
{
    std::string url;
    char* biomapsId;
    int id;
    int connectiontype;
    char* data;
    size_t dataSize;
};
#define JSON_CONTENT_TYPE "application/json"

#define GET             0
#define POST            1

static int handleRegistration(struct MHD_Connection *connection, const char *url,
        std::map<std::string, std::string>& url_args, ServerData* data)
{
    int ret;
    struct MHD_Response *response;

    std::cout << "Handling registration URL: " << url << std::endl;

    if (data->isAuthenticated())
    {
        if (url_args.count("test"))
        {
            std::string testResponse = data->testGet();
            std::cout << "Test Get: " << testResponse << std::endl;
        }
        // already authenticated, so do nothing?
        return send_authenticated_response(connection);
    }

    // first we need to redirect the user to the PMR authorisation page, if we haven't already done so
    if (!data->isAuthenticationUrlSet())
    {
        std::string authorisationUrl = data->getAuthenticationUrl();
        response = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
        if (response == 0)
        {
            return MHD_NO;
        }
        MHD_add_response_header(response, "Location", authorisationUrl.c_str());
        ret = MHD_queue_response(connection, MHD_HTTP_TEMPORARY_REDIRECT, response);
        MHD_destroy_response(response);
        return ret;
    }

    // now if we have the verifier and token set in the URL we can continue the authentication process
    if (url_args.count("oauth_verifier"))
    {
        if (data->authenticate(url_args["oauth_verifier"]))
        {
            std::cout << "whoo hoo?!" << std::endl;
            return send_authenticated_response(connection);
        }
        else
        {
            std::cerr << "Error authenticating?" << std::endl;
        }
    }
    return MHD_NO;
}

static void
request_completed (void *cls, struct MHD_Connection *connection,
                   void **con_cls, enum MHD_RequestTerminationCode toe)
{
    ServerData* data = static_cast<ServerData*>(cls);
    struct connection_info *con_info = (connection_info*)(*con_cls);
    if (NULL == con_info) return;
    std::cout << "Request completed, so destroy connection information object: " << con_info->id << std::endl;
    free (con_info);
    *con_cls = NULL;
}

static int url_handler(void *cls, struct MHD_Connection *connection,
        const char *url, const char *method, const char *version,
        const char *upload_data, size_t *upload_data_size, void **con_cls)
{
    ServerData* data = static_cast<ServerData*>(cls);
    char *me;
    struct MHD_Response *response;
    int ret;
    std::map<std::string, std::string> url_args;
    std::string respdata;

    // HTTP access control (CORS)
    // https://developer.mozilla.org/en-US/docs/HTTP/Access_control_CORS?redirectlocale=en-US&redirectslug=HTTP_access_control
    // some times a preflight check is required which uses the OPTIONS HTTP method to check for permission to
    // call cross-domain requests
    if (0 == strcmp(method, MHD_HTTP_METHOD_OPTIONS))
        return sendAccessControl(connection, url, method, version);

    // we only need to deal with GET requests
    /*  FIXME -- don't need this since the HTTP options/preflight will ensure non-supported methods are rejected?
    if (0 != strcmp(method, MHD_HTTP_METHOD_GET))
        return MHD_NO;*/

    // set up out connection information on the first pass through.
    if (NULL == *con_cls)
    {
        struct connection_info *con_info;
        con_info = (connection_info*) malloc (sizeof (struct connection_info));
        if (NULL == con_info) return MHD_NO;
        static int idCounter = 1;
        con_info->id = idCounter++;
        con_info->url = std::string(url);
        con_info->data = NULL;
        con_info->biomapsId = NULL;
        con_info->dataSize = 0;
        if (0 == strcmp (method, MHD_HTTP_METHOD_POST))
        {
            std::cout << "Setting up con_cls for POST: " << con_info->id << std::endl;
            std::cout << " - with url: " << url << std::endl;
            std::string contentType(MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "Content-type"));
            if (contentType.find(JSON_CONTENT_TYPE) == std::string::npos)
            {
                std::cerr << "Error creating POST processor?! Unhandled content type: "
                          << MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "Content-type")
                          << std::endl;
                free (con_info);
                return MHD_NO;
            }
            con_info->connectiontype = POST;
        }
        else con_info->connectiontype = GET;
        *con_cls = (void *) con_info;
        return MHD_YES;
    }

    // intercept POST requests for now to test stuff
    if (0 == strcmp(method, MHD_HTTP_METHOD_POST))
    {
        // post recieved, do stuff.
        struct connection_info *con_info = (connection_info*)(*con_cls);
        std::cout << "Received a POST for connection: " << con_info->id << std::endl;
        if (*upload_data_size != 0)
        {
            std::cout << "Processed some data: " << *upload_data_size << std::endl;
            //std::cout << "Data: " << upload_data << std::endl;
            con_info->data = (char*)realloc(con_info->data, con_info->dataSize + *upload_data_size);
            memcpy(con_info->data + con_info->dataSize, upload_data, *upload_data_size);
            con_info->dataSize += *upload_data_size;
            //std::string bob(upload_data, *upload_data_size);
            //con_info->data += bob.c_str();
            //std::cout << "con_info->data: " << con_info->data << std::endl;
            *upload_data_size = 0; // set to 0 to indicate all data considered/handled.
            return MHD_YES;
        }
        else
        {

        }
    }
    // need to handle registration/authorisation separately
    else if (Register::CompatiblePath(url))
    {
        if (MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND,
                get_url_args, &url_args) < 0)
        {
            return MHD_NO;
        }
        return handleRegistration(connection, url, url_args, data);
    }

    else if (0 == strcmp(method, MHD_HTTP_METHOD_GET))
    {
        if (MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND,
                get_url_args, &url_args) < 0)
        {
            return MHD_NO;
        }
        respdata = executeAPI(url, url_args, data);
    }

    if (respdata == "BAD RESPONSE")
    {
        return send_bad_response(connection);
    }
    //val = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "q");
    me = (char *) malloc(respdata.size() + 1);
    if (me == 0)
        return MHD_NO;
    strncpy(me, respdata.c_str(), respdata.size() + 1);
    response = MHD_create_response_from_buffer(strlen(me), me,
            MHD_RESPMEM_MUST_FREE);
    if (response == 0)
    {
        free(me);
        return MHD_NO;
    }

    /*it = url_args.find("type");
     if (it != url_args.end() && strcasecmp(it->second.c_str(), "xml") == 0)
     type = typexml;*/

    MHD_add_response_header(response, "Content-Type", "application/json");
    MHD_add_response_header(response, "Content-Range", "items 0-5/5");
    MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
    // need to make sure we always expose the (non-simple) headers that we use
    MHD_add_response_header(response, "Access-Control-Expose-Headers",
            "Content-Range");
    //MHD_add_response_header(response, "OurHeader", type);

    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

int startServer(int port, ServerData* data)
{
    struct MHD_Daemon * d = 0;

    d = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, port, NULL, NULL,
                         &url_handler, static_cast<void*>(data),
                         MHD_OPTION_NOTIFY_COMPLETED, request_completed,
                         static_cast<void*>(data), MHD_OPTION_END);
    if (d == NULL)
        return 1;
    (void) getchar();
    MHD_stop_daemon(d);
    return 0;
}

} // namespace gms
