# GLFW_FOUND
# GLFW_INCLUDE_DIR
# GLFW_LIBRARY

# set(FIND_GLFW_PATHS "F:\\C++\\ExternalLibraries\\GLFW")

find_path(GLFW_INCLUDE_DIR NAMES GLFW/glfw3 GLFW/glfw3.h PATH_SUFFIXES include)
find_library(GLFW_LIBRARY NAMES glfw3 glfw3.a libglfw3 libglfw3.a PATH_SUFFIXES lib-mingw)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLFW DEFAULT_MSG GLFW_LIBRARY GLFW_INCLUDE_DIR)

mark_as_advanced(GLFW_INCLUDE_DIR GLFW_LIBRARY)
