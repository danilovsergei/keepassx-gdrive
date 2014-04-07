# - Try to find ImageMagick++
# Once done, this will define
#
#  Magick++_FOUND - system has Magick++
#  Magick++_INCLUDE_DIRS - the Magick++ include directories
#  Magick++_LIBRARIES - link these to use Magick++

include(LibFindMacros)

# Dependencies
libfind_package(qtdrive)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(qtdrive_PKGCONF)

# Include dir
find_path(qtdrive_INCLUDE_DIR
  NAMES gdrive.h
  PATHS ${qtdrive_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(qtdrive_LIBRARY
  NAMES qtdrive
  PATHS ${Magick++_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(Magick++_PROCESS_INCLUDES Magick++_INCLUDE_DIR Magick_INCLUDE_DIRS)
set(Magick++_PROCESS_LIBS Magick++_LIBRARY Magick_LIBRARIES)
libfind_process(Magick++) 
