include(GNUInstallDirs)

# -Wunused-value;\
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

add_executable(winprefs constants.c main.c reg_command.c shell.c)
if(CMAKE_COMPILER_IS_GNUCC OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
  target_compile_definitions(winprefs PRIVATE UNICODE _UNICODE)
  target_compile_options(
    winprefs PRIVATE ${GCC_CLANG_SHARED_C_FLAGS}
                     $<$<CONFIG:Debug>:${GCC_CLANG_DEBUG_C_FLAGS}>)
  target_link_libraries(winprefs PUBLIC ole32 pathcch shell32 shlwapi)
  target_link_options(winprefs PRIVATE -municode)

endif()
# install(TARGETS winprefs DESTINATION ${CMAKE_INSTALL_BINDIR})
