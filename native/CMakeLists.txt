include(GNUInstallDirs)

if (CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo" AND WITH_VLD)
  find_library(VLD vld HINTS "C:/Program Files (x86)/Visual Leak Detector/lib/Win64" "C:/Program Files/Visual Leak Detector/lib/Win64" REQUIRED)
  find_path(VLD_H vld.h HINTS "C:/Program Files (x86)/Visual Leak Detector/include" "C:/Program Files/Visual Leak Detector/include" REQUIRED)
endif()

set(GCC_CLANG_SHARED_C_FLAGS "-fno-builtin;-std=gnu2x")
set(GCC_CLANG_DEBUG_C_FLAGS
    "-ggdb;\
-O0;\
-pedantic;\
-Wall;\
-Wdeprecated-declarations;\
-Wempty-body;\
-Werror;\
-Werror=conversion;\
-Werror=float-conversion;\
-Werror=implicit-function-declaration;\
-Werror=incompatible-pointer-types;\
-Werror=int-conversion;\
-Werror=return-type;\
-Werror=sign-conversion;\
-Wmissing-braces;\
-Wmissing-field-initializers;\
-Wno-error=unused-value;\
-Wno-missing-prototypes;\
-Wno-strict-prototypes;\
-Wno-trigraphs;\
-Wno-unknown-pragmas;\
-Wno-unused-value;\
-Wparentheses;\
-Wpointer-sign;\
-Wshadow;\
-Wsign-compare;\
-Wswitch;\
-Wuninitialized;\
-Wunknown-pragmas;\
-Wunreachable-code;\
-Wunused-function;\
-Wunused-label;\
-Wunused-parameter;\
-Wunused-variable")

add_executable(
  winprefs
  arg.h
  constants.c
  constants.h
  main.c
  reg_command.c
  reg_command.h
  shell.c
  shell.h
  msvc.h)
target_compile_definitions(winprefs PRIVATE UNICODE _UNICODE)
target_link_libraries(winprefs PRIVATE shlwapi)
if (CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo" AND WITH_VLD)
  target_compile_definitions(winprefs PRIVATE ENABLE_VLD)
  target_include_directories(winprefs PRIVATE "${VLD_H}")
  target_link_libraries(winprefs PRIVATE "${VLD}")
endif()
if(CMAKE_COMPILER_IS_GNUCC OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
  target_compile_options(
    winprefs PRIVATE ${GCC_CLANG_SHARED_C_FLAGS}
                     $<$<CONFIG:Debug>:${GCC_CLANG_DEBUG_C_FLAGS}>)
  target_link_libraries(winprefs PRIVATE ole32 shell32)
  target_link_options(winprefs PRIVATE -municode)
else()
  target_compile_options(winprefs PRIVATE /Zc:wchar_t /Zc:forScope /Zc:inline)
  target_link_options(winprefs PRIVATE /DYNAMICBASE /NXCOMPAT /TLBID:1)
endif()
install(TARGETS winprefs DESTINATION ${CMAKE_INSTALL_BINDIR})
