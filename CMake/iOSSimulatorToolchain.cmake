set(LFL_APPLE_DEVELOPER "/Applications/Xcode.app/Contents/Developer")
set(LFL_IOS_ROOT "${LFL_APPLE_DEVELOPER}/Platforms/iPhoneSimulator.platform/Developer")
set(LFL_IOS_SDK "${LFL_IOS_ROOT}/SDKs/iPhoneSimulator.sdk")
set(LFL_IOS_ARCH "x86_64")
set(LFL_IOS_ARCH_FLAGS "-arch x86_64")
set(LFL_IOS TRUE)
set(LFL_IOS_SIM TRUE)
set(LFL_USE_LIBCPP ON)
set(IOS_VERSION_MIN "8.0")
set(IOS_VERSION_MIN_FLAGS "-miphoneos-version-min=8.0")
set(IOS_VERSION_MIN_FULL_FLAGS "-miphoneos-version-min=8.0 -D__IPHONE_OS_VERSION_MIN_REQUIRED=80000")

set(CMAKE_C_COMPILER /usr/bin/clang CACHE STRING "iOS C Compiler" FORCE)
set(CMAKE_CXX_COMPILER /usr/bin/clang++ CACHE STRING "iOS C++ Compuiler" FORCE)
set(CMAKE_C_COMPILER_ID Apple CACHE STRING "iOS C Compiler ID" FORCE)
set(CMAKE_CXX_COMPILER_ID Apple CACHE STRING "iOS C++ Compuiler ID" FORCE)
set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_AR ar CACHE FILEPATH "" FORCE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_OSX_SYSROOT ${LFL_IOS_SDK} CACHE PATH "iOS sysroot")
set(CMAKE_OSX_ARCHITECTURES ${LFL_IOS_ARCH} CACHE STRING "iOS arch")
set(CMAKE_OSX_DEPLOYMENT_TARGET "" CACHE STRING "iOS deploy" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -isysroot ${LFL_IOS_SDK} -F${LFL_IOS_SDK}/System/Library/Frameworks ${IOS_VERSION_MIN_FLAGS}" CACHE STRING "iOS linker flags" FORCE)
set(CMAKE_SYSTEM_FRAMEWORK_PATH ${LFL_IOS_SDK}/System/Library/Frameworks)
set(CMAKE_SIZEOF_VOID_P 8)
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(CMAKE_FIND_FRAMEWORK FIRST)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_C_CREATE_STATIC_LIBRARY   "/usr/bin/libtool -static -o <TARGET> <LINK_FLAGS> <OBJECTS> ")
set(CMAKE_CXX_CREATE_STATIC_LIBRARY "/usr/bin/libtool -static -o <TARGET> <LINK_FLAGS> <OBJECTS> ")

add_definitions("-isysroot ${LFL_IOS_SDK} ${IOS_VERSION_MIN_FULL_FLAGS}")

if(CMAKE_GENERATOR MATCHES Xcode)
  set(LFL_XCODE TRUE)
  set(ENV_CPP "/usr/bin/clang -E")
else()
  set(ZLIB_INCLUDE_DIR ${LFL_IOS_SDK}/usr/include)
  set(ZLIB_LIBRARY ${LFL_IOS_SDK}/usr/lib/libz.dylib)
  set(M_LIBRARY ${LFL_IOS_SDK}/usr/lib/libm.dylib)
endif()

set(ENV_CFLAGS   "${LFL_IOS_ARCH_FLAGS} -isysroot ${LFL_IOS_SDK} ${IOS_VERSION_MIN_FLAGS}")
set(ENV_CXXFLAGS "${LFL_IOS_ARCH_FLAGS} -isysroot ${LFL_IOS_SDK} ${IOS_VERSION_MIN_FLAGS}")
set(ENV_LDFLAGS  "${LFL_IOS_ARCH_FLAGS} -isysroot ${LFL_IOS_SDK} ${IOS_VERSION_MIN_FLAGS}")
set(CONFIGURE_ENV CC=/usr/bin/clang CXX=/usr/bin/clang++ CPP=${ENV_CPP}
    CFLAGS=${ENV_CFLAGS} CXXFLAGS=${ENV_CXXFLAGS} LDFLAGS=${ENV_LDFLAGS})
