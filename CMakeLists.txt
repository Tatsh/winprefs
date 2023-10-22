cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
project(
  winprefs
  DESCRIPTION "Dump a registry path to a series of 'reg add' commands."
  HOMEPAGE_URL "https://github.com/Tatsh/winprefs"
  LANGUAGES C
  VERSION 0.2.1)

add_subdirectory(native)

set(CPACK_PACKAGE_NAME "winprefs")
set(CPACK_PACKAGE_VENDOR "Tatsh")
# FIXME include license file in zip FIXME install path should not have version number

# FIXME may need Visual C++ runtime

# FIXME missing license in installer

# FIXME NSIS does not create shortcut to app

# FIXME Set publisher

# FIXME Set 'file description'?

# FIXME Set Product name

# FIXME Set 'file version'?

# FIXME Set Product version

# FIXME Set Copyright

# FIXME Set language

# FIXME No UNIX-style packaging. Just have a single root.
set(CPACK_GENERATOR "NSIS;ZIP")
include(CPack)
