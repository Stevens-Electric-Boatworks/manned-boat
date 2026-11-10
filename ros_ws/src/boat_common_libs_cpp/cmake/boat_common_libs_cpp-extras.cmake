# libsockcanpp is vendored via add_subdirectory and installed into this package's
# prefix. Load its exported targets directly so downstream packages can resolve
# sockcanpp::sockcanpp (find_package(libsockcanpp) would not search
# lib/cmake/sockcanpp by default, and a cached libsockcanpp_DIR could point elsewhere).
if(NOT TARGET sockcanpp::sockcanpp)
  include("${boat_common_libs_cpp_DIR}/../../../lib/cmake/sockcanpp/libsockcanppTargets.cmake")
endif()