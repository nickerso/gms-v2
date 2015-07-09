#ifndef LIBHTTPDUTILS_HPP
#define LIBHTTPDUTILS_HPP

namespace gms {

class ServerData;

// start the webserver, returns when server terminates (hit enter key)
int startServer(int port, ServerData* data);

} // namespace gms

#endif // LIBHTTPDUTILS_HPP
