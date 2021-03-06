# liboauthcpp
#
# OAUTH_FOUND
# OAUTH_INCLUDE_DIRS
# OAUTH_LIBRARIES

include(LibFindMacros)

message(STATUS "GMS_DEPENDENCY_DIR = ${GMS_DEPENDENCY_DIR}")
find_path(OAUTH_INCLUDE_DIR
  NAMES liboauthcpp/liboauthcpp.h
  PATHS ${GMS_DEPENDENCY_DIR}/include
)

find_library(OAUTH_LIBRARY
  NAMES oauthcpp
  PATHS ${GMS_DEPENDENCY_DIR}/lib
)

set(OAUTH_PROCESS_INCLUDES OAUTH_INCLUDE_DIR)
set(OAUTH_PROCESS_LIBS OAUTH_LIBRARY)
libfind_process(OAUTH)
