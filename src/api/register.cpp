#include "register.h"

#include <iostream>
#include <json/json.h>
#include <json/json-forwards.h>
#include <liboauthcpp/liboauthcpp.h>

#include "../serverdata.h"

namespace gms {

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

} // namespace gms
