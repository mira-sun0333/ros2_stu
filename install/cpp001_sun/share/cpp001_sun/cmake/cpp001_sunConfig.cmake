# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_cpp001_sun_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED cpp001_sun_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(cpp001_sun_FOUND FALSE)
  elseif(NOT cpp001_sun_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(cpp001_sun_FOUND FALSE)
  endif()
  return()
endif()
set(_cpp001_sun_CONFIG_INCLUDED TRUE)

# output package information
if(NOT cpp001_sun_FIND_QUIETLY)
  message(STATUS "Found cpp001_sun: 0.0.0 (${cpp001_sun_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'cpp001_sun' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT ${cpp001_sun_DEPRECATED_QUIET})
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(cpp001_sun_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${cpp001_sun_DIR}/${_extra}")
endforeach()
