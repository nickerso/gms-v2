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
#include <curl/curl.h>

#include "gms_config.h"
#include "serverdata.h"
#include "libhttpd-utils.h"

static void printInformation()
{
    std::cout << "GET Model Server version: " << gms::versionString() << "\n"
              << "Documentation: " << gms::GMS_DOCUMENTATION_URL << std::endl;
}

static void printUsage(const char* progName)
{
    std::cerr << "\nUsage: " << progName << " <port> <working folder> [PMR2 instance]\n"
              << "\n  port - (required) the port number GMS should listen on, e.g., 1234.\n"
              << "\n  working folder - (required) the full path to the local folder in which\n"
              << "    to store working copies of repository workspaces.\n"
              << "\n  PMR2 instance - (optional) the instance of PMR2 to use as the repository.\n"
              << "    Will not override the existing origin in any working copies already\n"
              << "    in the <working folder>. Available options are:\n"
              << "    * staging - (default) http://staging.physiomeproject.org\n"
              << "    * models - https://models.physiomeproject.org\n"
              << std::endl;
}

int main(int argc, char *argv[])
{
    printInformation();
    curl_global_init(CURL_GLOBAL_DEFAULT);
    if (argc < 3)
    {
        printUsage(argv[0]);
        return -1;
    }
    std::string workingFolder(argv[2]);
    std::string repositoryUrl = "http://staging.physiomeproject.org/";
    if (argc > 3)
    {
        std::string tmp(argv[3]);
        if (tmp == "staging");// default, do nothing
        else if (tmp == "models") repositoryUrl = "https://models.physiomeproject.org/";
        else
        {
            std::cerr << "Invalid/unknown PMR2 instance: " << tmp << std::endl;
            printUsage(argv[0]);
            return -2;
        }
    }
    gms::ServerData* data = new gms::ServerData(repositoryUrl, workingFolder);
    startServer(atoi(argv[1]), data);
    delete data;
    curl_global_cleanup();
    return 0;
}
