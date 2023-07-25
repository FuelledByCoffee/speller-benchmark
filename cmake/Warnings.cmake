# from here:
#
# https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md

function(enable_warnings)
   option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" FALSE)

   set(CLANG_WARNINGS
      -Wall
      -Wextra
      -Wshadow
   )

   if(WARNINGS_AS_ERRORS)
      set(CLANG_WARNINGS ${CLANG_WARNINGS} -Werror)
   endif()
   
   if(NOT WIN32)
      add_compile_options(${CLANG_WARNINGS})
   endif()
endfunction()
