/*
Copyright 2015 University of Auckland

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.Some license of other
*/

#include "version.h"

#include <iostream>

#include "gms_config.h"

static void printInformation()
{
    std::cout << "GET Model Server version: " << gms::versionString() << "\n"
              << "Documentation: " << gms::GMS_DOCUMENTATION_URL << std::endl;
}

static void printUsage(const char* progName)
{
    std::cerr << "Usage: " << progName << " <PORT>\n"
              << "  PORT - the port number GMS should listen on."
              << std::endl;
}

int main(int argc, char *argv[])
{
    printInformation();
    if (argc < 2)
    {
        printUsage(argv[0]);
        return -1;
    }
    return 0;
}
