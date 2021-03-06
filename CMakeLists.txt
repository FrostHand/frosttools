cmake_minimum_required(VERSION 2.6)
PROJECT(FrostTools)

set(FROSTTOOLS_MAJOR_VERSION 1)
set(FROSTTOOLS_MINOR_VERSION 0)
set(FROSTTOOLS_PATCH_VERSION 3)
set(FROSTTOOLS_VERSION ${FROSTTOOLS_MAJOR_VERSION}.${FROSTTOOLS_MINOR_VERSION}.${FROSTTOOLS_PATCH_VERSION})

if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake/FrostTools)
endif()

# Does it work good for yocto?
set(INSTALL_LIB_DIR lib)# CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin)# CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include)# CACHE PATH "Installation directory for header files")
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR})# CACHE PATH "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

add_subdirectory(src)

option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
	add_subdirectory(tests)
endif(BUILD_TESTS)

option(BUILD_DOCUMENTATION "Use Doxygen to create documentation" OFF)
if(BUILD_DOCUMENTATION)
	find_package(Doxygen)
	if(NOT DOXYGEN_FOUND)
		message(FATAL_ERROR "Doxygen should be installed to build the documentation")
	endif(NOT DOXYGEN_FOUND)
	configure_file(Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)
	add_custom_target (Docs ALL 
		COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
		SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)
endif(BUILD_DOCUMENTATION)


# The interesting stuff goes here
# ===============================
 
## Add all targets to the build-tree export set
#export(TARGETS frosttools FILE "${PROJECT_BINARY_DIR}/FrostToolsTargets.cmake") 
## Export the package for use from the build-tree
## (this registers the build-tree with a global CMake-registry)
#export(PACKAGE FrostTools)
 
# Create the FrostToolsConfig.cmake and FrostToolsConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(FrostToolsConfig.cmake.in "${PROJECT_BINARY_DIR}/FrostToolsConfig.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${FROSTTOOLS_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(FrostToolsConfig.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/FrostToolsConfig.cmake" @ONLY)
# ... for both
configure_file(FrostToolsConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/FrostToolsConfigVersion.cmake" @ONLY)
 
# Install the FrostToolsConfig.cmake and FrostToolsConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/FrostToolsConfig.cmake"
  "${PROJECT_BINARY_DIR}/FrostToolsConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}")

# Install the export set for use with the install-tree
#install(EXPORT FrostToolsTargets DESTINATION "${INSTALL_CMAKE_DIR}")
install(DIRECTORY include/frosttools DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY include/frosttools DESTINATION include FILES_MATCHING PATTERN "*.hpp")

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

if(USE_CPACK)
# CPack installation
set(CPACK_PACKAGE_NAME "frosttools")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_VENDOR "SyboTech LLC")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FrostTools development library.\n Contains numerous header-only libraries, like 3dmath, timers, threading.")

set(CPACK_PACKAGE_VERSION_MAJOR ${FROSTTOOLS_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${FROSTTOOLS_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${FROSTTOOLS_PATCH_VERSION})
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
#set(CPACK_PACKAGE_INSTALL_DIRECTORY "CPack Component Example")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/description.txt")
set(CPACK_PACKAGE_FILE_NAME "frosttools")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/licence.txt")
set(CPACK_PACKAGE_FILE_NAME "frosttools-${CPACK_PACKAGE_VERSION}")
set(CPACK_COMPONENT_SDK_DISPLAY_NAME "SDK Contents")
set(CPACK_COMPONENT_SAMPLES_DISPLAY_NAME "Examples")

# for debian
SET(CPACK_DEBIAN_PACKAGE_NAME "libfrosttools-dev")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Dmitry Kargin dv_frost@mail.ru")
SET(CPACK_DEBIAN_PACKAGE_PROVIDES "libfrosttools-dev")
SET(CPACK_DEBIAN_PACKAGE_REPLACES "libfrosttools-dev")

# for NSIS
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_DISPLAY_NAME "FrostTools development library")
include(CPack)
endif(USE_CPACK)