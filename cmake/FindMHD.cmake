# libmicrohttpd
#
# MHD_FOUND
# MHD_INCLUDE_DIRS
# MHD_LIBRARIES

include(LibFindMacros)

find_path(MHD_INCLUDE_DIR
  NAMES microhttpd.h
  PATHS ${GMS_DEPENDENCIES_DIR}
)

find_library(MHD_LIBRARY
  NAMES microhttpd
  PATHS ${GMS_DEPENDENCIES_DIR}
)

set(MHD_PROCESS_INCLUDES MHD_INCLUDE_DIR)
set(MHD_PROCESS_LIBS MHD_LIBRARY)
libfind_process(MHD)
