message(STATUS "Configuring for platform Linux/GCC.")


# Enable C++11 support

execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
	OUTPUT_VARIABLE GCC_VERSION)

if(GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
elseif(GCC_VERSION VERSION_EQUAL 4.6)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()


set(LINUX_COMPILE_DEFS
	LINUX	                  # Linux system
        PIC		          # Position-independent code
	_REENTRANT                # Reentrant code
)
set(DEFAULT_COMPILE_DEFS_DEBUG
    ${LINUX_COMPILE_DEFS}
    _DEBUG	                  # Debug build
)
set(DEFAULT_COMPILE_DEFS_RELEASE
    ${LINUX_COMPILE_DEFS}
    NDEBUG	                  # Release build
)

if (OPTION_ERRORS_AS_EXCEPTION)
	set(EXCEPTION_FLAG "-fexceptions")
else()
	set(EXCEPTION_FLAG "-fno-exceptions")
endif()

set(LINUX_COMPILE_FLAGS "-fvisibility=hidden -pthread -pipe -fPIC -Wreturn-type -Wall -pedantic -Wextra -Wfloat-equal -Wcast-qual -Wcast-align -Wconversion -Werror -Wno-error=float-equal -Wno-error=switch ${EXCEPTION_FLAG}")
# pthread       -> use pthread library
# no-rtti       -> disable c++ rtti
# no-exceptions -> disable exception handling
# pipe          -> use pipes
# fPIC          -> use position independent code
# -Wreturn-type -Werror=return-type -> missing returns in functions and methods are handled as errors which stops the compilation
# -Wshadow -> e.g. when a parameter is named like a member, too many warnings, disabled for now
# -fvisibility=hidden -> only export symbols with __attribute__ ((visibility ("default")))

if(CMAKE_COMPILER_IS_GNUCXX)
	# gcc
	set(LINUX_COMPILE_FLAGS "${LINUX_COMPILE_FLAGS} -Wtrampolines")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	# clang
	set(LINUX_COMPILE_FLAGS "${LINUX_COMPILE_FLAGS} -Wno-mismatched-tags -Wno-unsequenced -Wno-sign-conversion -Wno-unused-function -Wno-missing-braces")
endif()

set(LINUX_LINKER_FLAGS "-pthread")

set(DEFAULT_COMPILE_FLAGS ${LINUX_COMPILE_FLAGS})

set(DEFAULT_LINKER_FLAGS_RELEASE ${LINUX_LINKER_FLAGS})
set(DEFAULT_LINKER_FLAGS_DEBUG ${LINUX_LINKER_FLAGS})
set(DEFAULT_LINKER_FLAGS ${LINUX_LINKER_FLAGS})

# Add platform specific libraries for linking
set(EXTRA_LIBS "")
