#ifndef CURLUTILS_H
#define CURLUTILS_H

#include <string>

namespace gms {

std::string getUrlContent(const std::string &url);
std::string getUrlWithHeaders(const std::string &url,
                              const std::string& httpHeader);

} // namespace gms

#endif // CURLUTILS_H
