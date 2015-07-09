#include "serverdata.h"

namespace gms {

ServerData::ServerData(const std::string& repositoryUrl, const std::string& workingFolder) :
    mRepositoryUrl(repositoryUrl), mWorkingFolder(workingFolder)
{
}

ServerData::~ServerData()
{
    // nothing to do yet
}

const std::string& ServerData::getRepositoryUrl() const
{
    return mRepositoryUrl;
}

const std::string& ServerData::getWorkingFolder() const
{
    return mWorkingFolder;
}


} // namespace gms
