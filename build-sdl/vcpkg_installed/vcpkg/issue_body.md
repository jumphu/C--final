Package: sdl2:x64-windows@2.32.10

**Host Environment**

- Host: x64-windows
- Compiler: MSVC 19.44.35217.0
- CMake Version: 4.2.0-rc4
-    vcpkg-tool version: 2025-10-16-71538f2694db93da4668782d094768ba74c45991
    vcpkg-scripts version: 2ad7bd0612 2025-11-21 (5 hours ago)

**To Reproduce**

`vcpkg install `

**Failure logs**

```
-- Using cached libsdl-org-SDL-release-2.32.10.tar.gz
-- Cleaning sources at C:/vcpkg/buildtrees/sdl2/src/se-2.32.10-3b143ac573.clean. Use --editable to skip cleaning for the packages you specify.
-- Extracting source C:/vcpkg/downloads/libsdl-org-SDL-release-2.32.10.tar.gz
-- Applying patch deps.patch
-- Applying patch alsa-dep-fix.patch
-- Applying patch cxx-linkage-pkgconfig.diff
-- Using source at C:/vcpkg/buildtrees/sdl2/src/se-2.32.10-3b143ac573.clean
-- Configuring x64-windows
CMake Error at scripts/cmake/vcpkg_execute_required_process.cmake:127 (message):
    Command failed: C:/vcpkg/downloads/tools/ninja/1.13.1-windows/ninja.exe -v
    Working Directory: C:/vcpkg/buildtrees/sdl2/x64-windows-rel/vcpkg-parallel-configure
    Error code: 1
    See logs for more information:
      C:\vcpkg\buildtrees\sdl2\config-x64-windows-dbg-CMakeCache.txt.log
      C:\vcpkg\buildtrees\sdl2\config-x64-windows-rel-CMakeCache.txt.log
      C:\vcpkg\buildtrees\sdl2\config-x64-windows-dbg-CMakeConfigureLog.yaml.log
      C:\vcpkg\buildtrees\sdl2\config-x64-windows-rel-CMakeConfigureLog.yaml.log
      C:\vcpkg\buildtrees\sdl2\config-x64-windows-out.log

Call Stack (most recent call first):
  C:/project-C/build-sdl/vcpkg_installed/x64-windows/share/vcpkg-cmake/vcpkg_cmake_configure.cmake:269 (vcpkg_execute_required_process)
  buildtrees/versioning_/versions/sdl2/af36a2c0ace7069e28bded20480ebbaf4e9c6e98/portfile.cmake:42 (vcpkg_cmake_configure)
  scripts/ports.cmake:206 (include)



```

<details><summary>C:\vcpkg\buildtrees\sdl2\config-x64-windows-out.log</summary>

```
[1/2] "C:/Program Files/CMake/bin/cmake.exe" -E chdir ".." "C:/Program Files/CMake/bin/cmake.exe" "C:/vcpkg/buildtrees/sdl2/src/se-2.32.10-3b143ac573.clean" "-G" "Ninja" "-DCMAKE_BUILD_TYPE=Release" "-DCMAKE_INSTALL_PREFIX=C:/vcpkg/packages/sdl2_x64-windows" "-DFETCHCONTENT_FULLY_DISCONNECTED=ON" "-DSDL_ALSA=OFF" "-DSDL_DBUS=OFF" "-DSDL_IBUS=OFF" "-DSDL_LIBSAMPLERATE=OFF" "-DSDL_VULKAN=OFF" "-DSDL_WAYLAND=OFF" "-DSDL_X11=OFF" "-DSDL_STATIC=0" "-DSDL_SHARED=1" "-DSDL_FORCE_STATIC_VCRT=0" "-DSDL_LIBC=ON" "-DSDL_TEST=OFF" "-DSDL_INSTALL_CMAKEDIR=cmake" "-DCMAKE_DISABLE_FIND_PACKAGE_Git=ON" "-DSDL_LIBSAMPLERATE_SHARED=OFF" "-DCMAKE_MAKE_PROGRAM=C:/vcpkg/downloads/tools/ninja/1.13.1-windows/ninja.exe" "-DBUILD_SHARED_LIBS=ON" "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=C:/vcpkg/scripts/toolchains/windows.cmake" "-DVCPKG_TARGET_TRIPLET=x64-windows" "-DVCPKG_SET_CHARSET_FLAG=ON" "-DVCPKG_PLATFORM_TOOLSET=v143" "-DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY=ON" "-DCMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP=TRUE" "-DCMAKE_VERBOSE_MAKEFILE=ON" "-DVCPKG_APPLOCAL_DEPS=OFF" "-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake" "-DCMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION=ON" "-DVCPKG_CXX_FLAGS=" "-DVCPKG_CXX_FLAGS_RELEASE=" "-DVCPKG_CXX_FLAGS_DEBUG=" "-DVCPKG_C_FLAGS=" "-DVCPKG_C_FLAGS_RELEASE=" "-DVCPKG_C_FLAGS_DEBUG=" "-DVCPKG_CRT_LINKAGE=dynamic" "-DVCPKG_LINKER_FLAGS=" "-DVCPKG_LINKER_FLAGS_RELEASE=" "-DVCPKG_LINKER_FLAGS_DEBUG=" "-DVCPKG_TARGET_ARCHITECTURE=x64" "-DCMAKE_INSTALL_LIBDIR:STRING=lib" "-DCMAKE_INSTALL_BINDIR:STRING=bin" "-D_VCPKG_ROOT_DIR=C:/vcpkg" "-D_VCPKG_INSTALLED_DIR=C:/project-C/build-sdl/vcpkg_installed" "-DVCPKG_MANIFEST_INSTALL=OFF"
FAILED: [code=1] ../CMakeCache.txt 
"C:/Program Files/CMake/bin/cmake.exe" -E chdir ".." "C:/Program Files/CMake/bin/cmake.exe" "C:/vcpkg/buildtrees/sdl2/src/se-2.32.10-3b143ac573.clean" "-G" "Ninja" "-DCMAKE_BUILD_TYPE=Release" "-DCMAKE_INSTALL_PREFIX=C:/vcpkg/packages/sdl2_x64-windows" "-DFETCHCONTENT_FULLY_DISCONNECTED=ON" "-DSDL_ALSA=OFF" "-DSDL_DBUS=OFF" "-DSDL_IBUS=OFF" "-DSDL_LIBSAMPLERATE=OFF" "-DSDL_VULKAN=OFF" "-DSDL_WAYLAND=OFF" "-DSDL_X11=OFF" "-DSDL_STATIC=0" "-DSDL_SHARED=1" "-DSDL_FORCE_STATIC_VCRT=0" "-DSDL_LIBC=ON" "-DSDL_TEST=OFF" "-DSDL_INSTALL_CMAKEDIR=cmake" "-DCMAKE_DISABLE_FIND_PACKAGE_Git=ON" "-DSDL_LIBSAMPLERATE_SHARED=OFF" "-DCMAKE_MAKE_PROGRAM=C:/vcpkg/downloads/tools/ninja/1.13.1-windows/ninja.exe" "-DBUILD_SHARED_LIBS=ON" "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=C:/vcpkg/scripts/toolchains/windows.cmake" "-DVCPKG_TARGET_TRIPLET=x64-windows" "-DVCPKG_SET_CHARSET_FLAG=ON" "-DVCPKG_PLATFORM_TOOLSET=v143" "-DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY=ON" "-DCMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP=TRUE" "-DCMAKE_VERBOSE_MAKEFILE=ON" "-DVCPKG_APPLOCAL_DEPS=OFF" "-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake" "-DCMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION=ON" "-DVCPKG_CXX_FLAGS=" "-DVCPKG_CXX_FLAGS_RELEASE=" "-DVCPKG_CXX_FLAGS_DEBUG=" "-DVCPKG_C_FLAGS=" "-DVCPKG_C_FLAGS_RELEASE=" "-DVCPKG_C_FLAGS_DEBUG=" "-DVCPKG_CRT_LINKAGE=dynamic" "-DVCPKG_LINKER_FLAGS=" "-DVCPKG_LINKER_FLAGS_RELEASE=" "-DVCPKG_LINKER_FLAGS_DEBUG=" "-DVCPKG_TARGET_ARCHITECTURE=x64" "-DCMAKE_INSTALL_LIBDIR:STRING=lib" "-DCMAKE_INSTALL_BINDIR:STRING=bin" "-D_VCPKG_ROOT_DIR=C:/vcpkg" "-D_VCPKG_INSTALLED_DIR=C:/project-C/build-sdl/vcpkg_installed" "-DVCPKG_MANIFEST_INSTALL=OFF"
-- The C compiler identification is MSVC 19.44.35217.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - failed
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe - broken
CMake Error at C:/Program Files/CMake/share/cmake-4.2/Modules/CMakeTestCCompiler.cmake:67 (message):
  The C compiler

...
Skipped 10 lines
...
    [2/2] C:\Windows\system32\cmd.exe /C "cd . && "C:\Program Files\CMake\bin\cmake.exe" -E vs_link_exe --msvc-ver=1944 --intdir=CMakeFiles\cmTC_ad155.dir --rc=rc --mt=CMAKE_MT-NOTFOUND --manifests  -- C:\PROGRA~1\MICROS~4\2022\COMMUN~1\VC\Tools\MSVC\1444~1.352\bin\Hostx64\x64\link.exe /nologo CMakeFiles\cmTC_ad155.dir\testCCompiler.c.obj  /out:cmTC_ad155.exe /implib:cmTC_ad155.lib /pdb:cmTC_ad155.pdb /version:0.0 /machine:x64 /nologo /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
    FAILED: [code=4294967295] cmTC_ad155.exe 
    C:\Windows\system32\cmd.exe /C "cd . && "C:\Program Files\CMake\bin\cmake.exe" -E vs_link_exe --msvc-ver=1944 --intdir=CMakeFiles\cmTC_ad155.dir --rc=rc --mt=CMAKE_MT-NOTFOUND --manifests  -- C:\PROGRA~1\MICROS~4\2022\COMMUN~1\VC\Tools\MSVC\1444~1.352\bin\Hostx64\x64\link.exe /nologo CMakeFiles\cmTC_ad155.dir\testCCompiler.c.obj  /out:cmTC_ad155.exe /implib:cmTC_ad155.lib /pdb:cmTC_ad155.pdb /version:0.0 /machine:x64 /nologo /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
    RC Pass 1: command "rc /fo CMakeFiles\cmTC_ad155.dir/manifest.res CMakeFiles\cmTC_ad155.dir/manifest.rc" failed (exit code 0) with the following output:
    no such file or directory
    ninja: build stopped: subcommand failed.
    
    



  CMake will not be able to correctly generate this project.
Call Stack (most recent call first):
  CMakeLists.txt:9 (project)


-- Configuring incomplete, errors occurred!
[2/2] "C:/Program Files/CMake/bin/cmake.exe" -E chdir "../../x64-windows-dbg" "C:/Program Files/CMake/bin/cmake.exe" "C:/vcpkg/buildtrees/sdl2/src/se-2.32.10-3b143ac573.clean" "-G" "Ninja" "-DCMAKE_BUILD_TYPE=Debug" "-DCMAKE_INSTALL_PREFIX=C:/vcpkg/packages/sdl2_x64-windows/debug" "-DFETCHCONTENT_FULLY_DISCONNECTED=ON" "-DSDL_ALSA=OFF" "-DSDL_DBUS=OFF" "-DSDL_IBUS=OFF" "-DSDL_LIBSAMPLERATE=OFF" "-DSDL_VULKAN=OFF" "-DSDL_WAYLAND=OFF" "-DSDL_X11=OFF" "-DSDL_STATIC=0" "-DSDL_SHARED=1" "-DSDL_FORCE_STATIC_VCRT=0" "-DSDL_LIBC=ON" "-DSDL_TEST=OFF" "-DSDL_INSTALL_CMAKEDIR=cmake" "-DCMAKE_DISABLE_FIND_PACKAGE_Git=ON" "-DSDL_LIBSAMPLERATE_SHARED=OFF" "-DCMAKE_MAKE_PROGRAM=C:/vcpkg/downloads/tools/ninja/1.13.1-windows/ninja.exe" "-DBUILD_SHARED_LIBS=ON" "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=C:/vcpkg/scripts/toolchains/windows.cmake" "-DVCPKG_TARGET_TRIPLET=x64-windows" "-DVCPKG_SET_CHARSET_FLAG=ON" "-DVCPKG_PLATFORM_TOOLSET=v143" "-DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY=ON" "-DCMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP=TRUE" "-DCMAKE_VERBOSE_MAKEFILE=ON" "-DVCPKG_APPLOCAL_DEPS=OFF" "-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake" "-DCMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION=ON" "-DVCPKG_CXX_FLAGS=" "-DVCPKG_CXX_FLAGS_RELEASE=" "-DVCPKG_CXX_FLAGS_DEBUG=" "-DVCPKG_C_FLAGS=" "-DVCPKG_C_FLAGS_RELEASE=" "-DVCPKG_C_FLAGS_DEBUG=" "-DVCPKG_CRT_LINKAGE=dynamic" "-DVCPKG_LINKER_FLAGS=" "-DVCPKG_LINKER_FLAGS_RELEASE=" "-DVCPKG_LINKER_FLAGS_DEBUG=" "-DVCPKG_TARGET_ARCHITECTURE=x64" "-DCMAKE_INSTALL_LIBDIR:STRING=lib" "-DCMAKE_INSTALL_BINDIR:STRING=bin" "-D_VCPKG_ROOT_DIR=C:/vcpkg" "-D_VCPKG_INSTALLED_DIR=C:/project-C/build-sdl/vcpkg_installed" "-DVCPKG_MANIFEST_INSTALL=OFF"
FAILED: [code=1] ../../x64-windows-dbg/CMakeCache.txt 
"C:/Program Files/CMake/bin/cmake.exe" -E chdir "../../x64-windows-dbg" "C:/Program Files/CMake/bin/cmake.exe" "C:/vcpkg/buildtrees/sdl2/src/se-2.32.10-3b143ac573.clean" "-G" "Ninja" "-DCMAKE_BUILD_TYPE=Debug" "-DCMAKE_INSTALL_PREFIX=C:/vcpkg/packages/sdl2_x64-windows/debug" "-DFETCHCONTENT_FULLY_DISCONNECTED=ON" "-DSDL_ALSA=OFF" "-DSDL_DBUS=OFF" "-DSDL_IBUS=OFF" "-DSDL_LIBSAMPLERATE=OFF" "-DSDL_VULKAN=OFF" "-DSDL_WAYLAND=OFF" "-DSDL_X11=OFF" "-DSDL_STATIC=0" "-DSDL_SHARED=1" "-DSDL_FORCE_STATIC_VCRT=0" "-DSDL_LIBC=ON" "-DSDL_TEST=OFF" "-DSDL_INSTALL_CMAKEDIR=cmake" "-DCMAKE_DISABLE_FIND_PACKAGE_Git=ON" "-DSDL_LIBSAMPLERATE_SHARED=OFF" "-DCMAKE_MAKE_PROGRAM=C:/vcpkg/downloads/tools/ninja/1.13.1-windows/ninja.exe" "-DBUILD_SHARED_LIBS=ON" "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=C:/vcpkg/scripts/toolchains/windows.cmake" "-DVCPKG_TARGET_TRIPLET=x64-windows" "-DVCPKG_SET_CHARSET_FLAG=ON" "-DVCPKG_PLATFORM_TOOLSET=v143" "-DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY=ON" "-DCMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP=TRUE" "-DCMAKE_VERBOSE_MAKEFILE=ON" "-DVCPKG_APPLOCAL_DEPS=OFF" "-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake" "-DCMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION=ON" "-DVCPKG_CXX_FLAGS=" "-DVCPKG_CXX_FLAGS_RELEASE=" "-DVCPKG_CXX_FLAGS_DEBUG=" "-DVCPKG_C_FLAGS=" "-DVCPKG_C_FLAGS_RELEASE=" "-DVCPKG_C_FLAGS_DEBUG=" "-DVCPKG_CRT_LINKAGE=dynamic" "-DVCPKG_LINKER_FLAGS=" "-DVCPKG_LINKER_FLAGS_RELEASE=" "-DVCPKG_LINKER_FLAGS_DEBUG=" "-DVCPKG_TARGET_ARCHITECTURE=x64" "-DCMAKE_INSTALL_LIBDIR:STRING=lib" "-DCMAKE_INSTALL_BINDIR:STRING=bin" "-D_VCPKG_ROOT_DIR=C:/vcpkg" "-D_VCPKG_INSTALLED_DIR=C:/project-C/build-sdl/vcpkg_installed" "-DVCPKG_MANIFEST_INSTALL=OFF"
-- The C compiler identification is MSVC 19.44.35217.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - failed
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe - broken
CMake Error at C:/Program Files/CMake/share/cmake-4.2/Modules/CMakeTestCCompiler.cmake:67 (message):
  The C compiler

    "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe"

  is not able to compile a simple test program.

  It fails with the following output:

    Change Dir: 'C:/vcpkg/buildtrees/sdl2/x64-windows-dbg/CMakeFiles/CMakeScratch/TryCompile-zyqhmt'
    
    Run Build Command(s): C:/vcpkg/downloads/tools/ninja/1.13.1-windows/ninja.exe -v cmTC_e7576
    [1/2] C:\PROGRA~1\MICROS~4\2022\COMMUN~1\VC\Tools\MSVC\1444~1.352\bin\Hostx64\x64\cl.exe  /nologo -D_MBCS  /nologo /DWIN32 /D_WINDOWS /utf-8 /MP   /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\cmTC_e7576.dir\testCCompiler.c.obj /FdCMakeFiles\cmTC_e7576.dir\ /FS -c C:\vcpkg\buildtrees\sdl2\x64-windows-dbg\CMakeFiles\CMakeScratch\TryCompile-zyqhmt\testCCompiler.c
    [2/2] C:\Windows\system32\cmd.exe /C "cd . && "C:\Program Files\CMake\bin\cmake.exe" -E vs_link_exe --msvc-ver=1944 --intdir=CMakeFiles\cmTC_e7576.dir --rc=rc --mt=CMAKE_MT-NOTFOUND --manifests  -- C:\PROGRA~1\MICROS~4\2022\COMMUN~1\VC\Tools\MSVC\1444~1.352\bin\Hostx64\x64\link.exe /nologo CMakeFiles\cmTC_e7576.dir\testCCompiler.c.obj  /out:cmTC_e7576.exe /implib:cmTC_e7576.lib /pdb:cmTC_e7576.pdb /version:0.0 /machine:x64 /nologo /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
    FAILED: [code=4294967295] cmTC_e7576.exe 
    C:\Windows\system32\cmd.exe /C "cd . && "C:\Program Files\CMake\bin\cmake.exe" -E vs_link_exe --msvc-ver=1944 --intdir=CMakeFiles\cmTC_e7576.dir --rc=rc --mt=CMAKE_MT-NOTFOUND --manifests  -- C:\PROGRA~1\MICROS~4\2022\COMMUN~1\VC\Tools\MSVC\1444~1.352\bin\Hostx64\x64\link.exe /nologo CMakeFiles\cmTC_e7576.dir\testCCompiler.c.obj  /out:cmTC_e7576.exe /implib:cmTC_e7576.lib /pdb:cmTC_e7576.pdb /version:0.0 /machine:x64 /nologo /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
    RC Pass 1: command "rc /fo CMakeFiles\cmTC_e7576.dir/manifest.res CMakeFiles\cmTC_e7576.dir/manifest.rc" failed (exit code 0) with the following output:
    no such file or directory
    ninja: build stopped: subcommand failed.
    
    



  CMake will not be able to correctly generate this project.
Call Stack (most recent call first):
  CMakeLists.txt:9 (project)


-- Configuring incomplete, errors occurred!
ninja: build stopped: subcommand failed.
```
</details>

<details><summary>C:\vcpkg\buildtrees\sdl2\config-x64-windows-rel-CMakeCache.txt.log</summary>

```
# This is the CMakeCache file.
# For build in directory: c:/vcpkg/buildtrees/sdl2/x64-windows-rel
# It was generated by CMake: C:/Program Files/CMake/bin/cmake.exe
# You can edit this file to change values found and used by cmake.
# If you do not want to change any of the values, simply exit the editor.
# If you do want to change a value, simply edit, save, and exit the editor.
# The syntax for the file is as follows:
# KEY:TYPE=VALUE
# KEY is the name of a variable in the cache.
# TYPE is a hint to GUIs for the type of VALUE, DO NOT EDIT TYPE!.
# VALUE is the current value for the KEY.

########################
# EXTERNAL cache entries
########################

//No help, variable specified on the command line.
BUILD_SHARED_LIBS:UNINITIALIZED=ON

//Path to a program.
CMAKE_AR:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/lib.exe

//Choose the type of build, options are: None Debug Release RelWithDebInfo
// MinSizeRel ...
CMAKE_BUILD_TYPE:STRING=Release

CMAKE_CROSSCOMPILING:STRING=OFF

CMAKE_CXX_FLAGS:STRING=' /nologo /DWIN32 /D_WINDOWS /utf-8 /GR /EHsc /MP '

CMAKE_CXX_FLAGS_DEBUG:STRING='/MDd /Z7 /Ob0 /Od /RTC1 '

CMAKE_CXX_FLAGS_RELEASE:STRING='/MD /O2 /Oi /Gy /DNDEBUG /Z7 '

//C compiler
CMAKE_C_COMPILER:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe

CMAKE_C_FLAGS:STRING=' /nologo /DWIN32 /D_WINDOWS /utf-8 /MP '

CMAKE_C_FLAGS_DEBUG:STRING='/MDd /Z7 /Ob0 /Od /RTC1 '

//Flags used by the C compiler during MINSIZEREL builds.
CMAKE_C_FLAGS_MINSIZEREL:STRING=/O1 /Ob1 /DNDEBUG

CMAKE_C_FLAGS_RELEASE:STRING='/MD /O2 /Oi /Gy /DNDEBUG /Z7 '

//Flags used by the C compiler during RELWITHDEBINFO builds.
CMAKE_C_FLAGS_RELWITHDEBINFO:STRING=/Zi /O2 /Ob1 /DNDEBUG

//Libraries linked by default with all C applications.
CMAKE_C_STANDARD_LIBRARIES:STRING=kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib

//No help, variable specified on the command line.
CMAKE_DISABLE_FIND_PACKAGE_Git:UNINITIALIZED=ON

//No help, variable specified on the command line.
CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION:UNINITIALIZED=ON

//Flags used by the linker during all build types.
CMAKE_EXE_LINKER_FLAGS:STRING=/machine:x64

//Flags used by the linker during DEBUG builds.
CMAKE_EXE_LINKER_FLAGS_DEBUG:STRING=/nologo    /debug /INCREMENTAL

//Flags used by the linker during MINSIZEREL builds.
CMAKE_EXE_LINKER_FLAGS_MINSIZEREL:STRING=/INCREMENTAL:NO

CMAKE_EXE_LINKER_FLAGS_RELEASE:STRING='/nologo /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF  '

//Flags used by the linker during RELWITHDEBINFO builds.
CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO:STRING=/debug /INCREMENTAL

//Enable/Disable output of build database during the build.
CMAKE_EXPORT_BUILD_DATABASE:BOOL=

//Enable/Disable output of compile commands during generation.
CMAKE_EXPORT_COMPILE_COMMANDS:BOOL=

//No help, variable specified on the command line.
CMAKE_EXPORT_NO_PACKAGE_REGISTRY:UNINITIALIZED=ON

//No help, variable specified on the command line.
CMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY:UNINITIALIZED=ON

//No help, variable specified on the command line.
CMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY:UNINITIALIZED=ON

//Value Computed by CMake.
CMAKE_FIND_PACKAGE_REDIRECTS_DIR:STATIC=C:/vcpkg/buildtrees/sdl2/x64-windows-rel/CMakeFiles/pkgRedirects

//No help, variable specified on the command line.
CMAKE_INSTALL_BINDIR:STRING=bin

//No help, variable specified on the command line.
CMAKE_INSTALL_LIBDIR:STRING=lib

//Install path prefix, prepended onto install directories.
CMAKE_INSTALL_PREFIX:PATH=C:/vcpkg/packages/sdl2_x64-windows

//No help, variable specified on the command line.
CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP:UNINITIALIZED=TRUE

//Path to a program.
CMAKE_LINKER:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/link.exe

//No help, variable specified on the command line.
CMAKE_MAKE_PROGRAM:UNINITIALIZED=C:/vcpkg/downloads/tools/ninja/1.13.1-windows/ninja.exe
...
Skipped 224 lines
...
// and CMAKE_FIND_ROOT_PATH so that vcpkg libraries/packages are
// found after toolchain/system libraries/packages.
VCPKG_PREFER_SYSTEM_LIBS:BOOL=OFF

//Enable the setup of CMAKE_PROGRAM_PATH to vcpkg paths
VCPKG_SETUP_CMAKE_PROGRAM_PATH:BOOL=ON

//No help, variable specified on the command line.
VCPKG_SET_CHARSET_FLAG:UNINITIALIZED=ON

//No help, variable specified on the command line.
VCPKG_TARGET_ARCHITECTURE:UNINITIALIZED=x64

//Vcpkg target triplet (ex. x86-windows)
VCPKG_TARGET_TRIPLET:STRING=x64-windows

//Trace calls to find_package()
VCPKG_TRACE_FIND_PACKAGE:BOOL=OFF

//Enables messages from the VCPKG toolchain for debugging purposes.
VCPKG_VERBOSE:BOOL=OFF

//(experimental) Automatically copy dependencies into the install
// target directory for executables. Requires CMake 3.14.
X_VCPKG_APPLOCAL_DEPS_INSTALL:BOOL=OFF

//(experimental) Add USES_TERMINAL to VCPKG_APPLOCAL_DEPS to force
// serialization.
X_VCPKG_APPLOCAL_DEPS_SERIALIZED:BOOL=OFF

//The directory which contains the installed libraries for each
// triplet
_VCPKG_INSTALLED_DIR:PATH=C:/project-C/build-sdl/vcpkg_installed

//No help, variable specified on the command line.
_VCPKG_ROOT_DIR:UNINITIALIZED=C:/vcpkg


########################
# INTERNAL cache entries
########################

//ADVANCED property for variable: CMAKE_AR
CMAKE_AR-ADVANCED:INTERNAL=1
//This is the directory where this CMakeCache.txt was created
CMAKE_CACHEFILE_DIR:INTERNAL=c:/vcpkg/buildtrees/sdl2/x64-windows-rel
//Major version of cmake used to create the current loaded cache
CMAKE_CACHE_MAJOR_VERSION:INTERNAL=4
//Minor version of cmake used to create the current loaded cache
CMAKE_CACHE_MINOR_VERSION:INTERNAL=2
//Patch version of cmake used to create the current loaded cache
CMAKE_CACHE_PATCH_VERSION:INTERNAL=0
//Path to CMake executable.
CMAKE_COMMAND:INTERNAL=C:/Program Files/CMake/bin/cmake.exe
//Path to cpack program executable.
CMAKE_CPACK_COMMAND:INTERNAL=C:/Program Files/CMake/bin/cpack.exe
//Path to ctest program executable.
CMAKE_CTEST_COMMAND:INTERNAL=C:/Program Files/CMake/bin/ctest.exe
//ADVANCED property for variable: CMAKE_C_COMPILER
CMAKE_C_COMPILER-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS
CMAKE_C_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_DEBUG
CMAKE_C_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_MINSIZEREL
CMAKE_C_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_RELEASE
CMAKE_C_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_RELWITHDEBINFO
CMAKE_C_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_STANDARD_LIBRARIES
CMAKE_C_STANDARD_LIBRARIES-ADVANCED:INTERNAL=1
//Path to cache edit program executable.
CMAKE_EDIT_COMMAND:INTERNAL=C:/Program Files/CMake/bin/cmake-gui.exe
//Executable file format
CMAKE_EXECUTABLE_FORMAT:INTERNAL=Unknown
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS
CMAKE_EXE_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_DEBUG
CMAKE_EXE_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_MINSIZEREL
CMAKE_EXE_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_RELEASE
CMAKE_EXE_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXPORT_BUILD_DATABASE
CMAKE_EXPORT_BUILD_DATABASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXPORT_COMPILE_COMMANDS
CMAKE_EXPORT_COMPILE_COMMANDS-ADVANCED:INTERNAL=1
//Name of external makefile project generator.
CMAKE_EXTRA_GENERATOR:INTERNAL=
//Name of generator.
CMAKE_GENERATOR:INTERNAL=Ninja
//Generator instance identifier.
CMAKE_GENERATOR_INSTANCE:INTERNAL=
//Name of generator platform.
CMAKE_GENERATOR_PLATFORM:INTERNAL=
//Name of generator toolset.
CMAKE_GENERATOR_TOOLSET:INTERNAL=
//Source directory with the top level CMakeLists.txt file for this
// project
CMAKE_HOME_DIRECTORY:INTERNAL=C:/vcpkg/buildtrees/sdl2/src/se-2.32.10-3b143ac573.clean
//ADVANCED property for variable: CMAKE_LINKER
CMAKE_LINKER-ADVANCED:INTERNAL=1
//Name of CMakeLists files to read
CMAKE_LIST_FILE_NAME:INTERNAL=CMakeLists.txt
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS
CMAKE_MODULE_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_DEBUG
CMAKE_MODULE_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL
CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_RELEASE
CMAKE_MODULE_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MT
CMAKE_MT-ADVANCED:INTERNAL=1
//number of local generators
CMAKE_NUMBER_OF_MAKEFILES:INTERNAL=1
//Platform information initialized
CMAKE_PLATFORM_INFO_INITIALIZED:INTERNAL=1
//ADVANCED property for variable: CMAKE_POLICY_VERSION_MINIMUM
CMAKE_POLICY_VERSION_MINIMUM-ADVANCED:INTERNAL=1
//noop for ranlib
CMAKE_RANLIB:INTERNAL=:
//ADVANCED property for variable: CMAKE_RC_COMPILER
CMAKE_RC_COMPILER-ADVANCED:INTERNAL=1
CMAKE_RC_COMPILER_WORKS:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS
CMAKE_RC_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_DEBUG
CMAKE_RC_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_MINSIZEREL
CMAKE_RC_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_RELEASE
CMAKE_RC_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_RELWITHDEBINFO
CMAKE_RC_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//Path to CMake installation.
CMAKE_ROOT:INTERNAL=C:/Program Files/CMake/share/cmake-4.2
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS
CMAKE_SHARED_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_DEBUG
CMAKE_SHARED_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL
CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_RELEASE
CMAKE_SHARED_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SKIP_INSTALL_RPATH
CMAKE_SKIP_INSTALL_RPATH-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SKIP_RPATH
CMAKE_SKIP_RPATH-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS
CMAKE_STATIC_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_DEBUG
CMAKE_STATIC_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL
CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_RELEASE
CMAKE_STATIC_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_TOOLCHAIN_FILE
CMAKE_TOOLCHAIN_FILE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_VERBOSE_MAKEFILE
CMAKE_VERBOSE_MAKEFILE-ADVANCED:INTERNAL=1
//Install the dependencies listed in your manifest:
//\n    If this is off, you will have to manually install your dependencies.
//\n    See https://github.com/microsoft/vcpkg/tree/master/docs/specifications/manifests.md
// for more info.
//\n
VCPKG_MANIFEST_INSTALL:INTERNAL=OFF
//ADVANCED property for variable: VCPKG_VERBOSE
VCPKG_VERBOSE-ADVANCED:INTERNAL=1
//Making sure VCPKG_MANIFEST_MODE doesn't change
Z_VCPKG_CHECK_MANIFEST_MODE:INTERNAL=OFF
//Vcpkg root directory
Z_VCPKG_ROOT_DIR:INTERNAL=C:/vcpkg

```
</details>

<details><summary>C:\vcpkg\buildtrees\sdl2\config-x64-windows-dbg-CMakeCache.txt.log</summary>

```
# This is the CMakeCache file.
# For build in directory: c:/vcpkg/buildtrees/sdl2/x64-windows-dbg
# It was generated by CMake: C:/Program Files/CMake/bin/cmake.exe
# You can edit this file to change values found and used by cmake.
# If you do not want to change any of the values, simply exit the editor.
# If you do want to change a value, simply edit, save, and exit the editor.
# The syntax for the file is as follows:
# KEY:TYPE=VALUE
# KEY is the name of a variable in the cache.
# TYPE is a hint to GUIs for the type of VALUE, DO NOT EDIT TYPE!.
# VALUE is the current value for the KEY.

########################
# EXTERNAL cache entries
########################

//No help, variable specified on the command line.
BUILD_SHARED_LIBS:UNINITIALIZED=ON

//Path to a program.
CMAKE_AR:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/lib.exe

//Choose the type of build, options are: None Debug Release RelWithDebInfo
// MinSizeRel ...
CMAKE_BUILD_TYPE:STRING=Debug

CMAKE_CROSSCOMPILING:STRING=OFF

CMAKE_CXX_FLAGS:STRING=' /nologo /DWIN32 /D_WINDOWS /utf-8 /GR /EHsc /MP '

CMAKE_CXX_FLAGS_DEBUG:STRING='/MDd /Z7 /Ob0 /Od /RTC1 '

CMAKE_CXX_FLAGS_RELEASE:STRING='/MD /O2 /Oi /Gy /DNDEBUG /Z7 '

//C compiler
CMAKE_C_COMPILER:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe

CMAKE_C_FLAGS:STRING=' /nologo /DWIN32 /D_WINDOWS /utf-8 /MP '

CMAKE_C_FLAGS_DEBUG:STRING='/MDd /Z7 /Ob0 /Od /RTC1 '

//Flags used by the C compiler during MINSIZEREL builds.
CMAKE_C_FLAGS_MINSIZEREL:STRING=/O1 /Ob1 /DNDEBUG

CMAKE_C_FLAGS_RELEASE:STRING='/MD /O2 /Oi /Gy /DNDEBUG /Z7 '

//Flags used by the C compiler during RELWITHDEBINFO builds.
CMAKE_C_FLAGS_RELWITHDEBINFO:STRING=/Zi /O2 /Ob1 /DNDEBUG

//Libraries linked by default with all C applications.
CMAKE_C_STANDARD_LIBRARIES:STRING=kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib

//No help, variable specified on the command line.
CMAKE_DISABLE_FIND_PACKAGE_Git:UNINITIALIZED=ON

//No help, variable specified on the command line.
CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION:UNINITIALIZED=ON

//Flags used by the linker during all build types.
CMAKE_EXE_LINKER_FLAGS:STRING=/machine:x64

//Flags used by the linker during DEBUG builds.
CMAKE_EXE_LINKER_FLAGS_DEBUG:STRING=/nologo    /debug /INCREMENTAL

//Flags used by the linker during MINSIZEREL builds.
CMAKE_EXE_LINKER_FLAGS_MINSIZEREL:STRING=/INCREMENTAL:NO

CMAKE_EXE_LINKER_FLAGS_RELEASE:STRING='/nologo /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF  '

//Flags used by the linker during RELWITHDEBINFO builds.
CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO:STRING=/debug /INCREMENTAL

//Enable/Disable output of build database during the build.
CMAKE_EXPORT_BUILD_DATABASE:BOOL=

//Enable/Disable output of compile commands during generation.
CMAKE_EXPORT_COMPILE_COMMANDS:BOOL=

//No help, variable specified on the command line.
CMAKE_EXPORT_NO_PACKAGE_REGISTRY:UNINITIALIZED=ON

//No help, variable specified on the command line.
CMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY:UNINITIALIZED=ON

//No help, variable specified on the command line.
CMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY:UNINITIALIZED=ON

//Value Computed by CMake.
CMAKE_FIND_PACKAGE_REDIRECTS_DIR:STATIC=C:/vcpkg/buildtrees/sdl2/x64-windows-dbg/CMakeFiles/pkgRedirects

//No help, variable specified on the command line.
CMAKE_INSTALL_BINDIR:STRING=bin

//No help, variable specified on the command line.
CMAKE_INSTALL_LIBDIR:STRING=lib

//Install path prefix, prepended onto install directories.
CMAKE_INSTALL_PREFIX:PATH=C:/vcpkg/packages/sdl2_x64-windows/debug

//No help, variable specified on the command line.
CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP:UNINITIALIZED=TRUE

//Path to a program.
CMAKE_LINKER:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/link.exe

//No help, variable specified on the command line.
...
Skipped 226 lines
...
// found after toolchain/system libraries/packages.
VCPKG_PREFER_SYSTEM_LIBS:BOOL=OFF

//Enable the setup of CMAKE_PROGRAM_PATH to vcpkg paths
VCPKG_SETUP_CMAKE_PROGRAM_PATH:BOOL=ON

//No help, variable specified on the command line.
VCPKG_SET_CHARSET_FLAG:UNINITIALIZED=ON

//No help, variable specified on the command line.
VCPKG_TARGET_ARCHITECTURE:UNINITIALIZED=x64

//Vcpkg target triplet (ex. x86-windows)
VCPKG_TARGET_TRIPLET:STRING=x64-windows

//Trace calls to find_package()
VCPKG_TRACE_FIND_PACKAGE:BOOL=OFF

//Enables messages from the VCPKG toolchain for debugging purposes.
VCPKG_VERBOSE:BOOL=OFF

//(experimental) Automatically copy dependencies into the install
// target directory for executables. Requires CMake 3.14.
X_VCPKG_APPLOCAL_DEPS_INSTALL:BOOL=OFF

//(experimental) Add USES_TERMINAL to VCPKG_APPLOCAL_DEPS to force
// serialization.
X_VCPKG_APPLOCAL_DEPS_SERIALIZED:BOOL=OFF

//The directory which contains the installed libraries for each
// triplet
_VCPKG_INSTALLED_DIR:PATH=C:/project-C/build-sdl/vcpkg_installed

//No help, variable specified on the command line.
_VCPKG_ROOT_DIR:UNINITIALIZED=C:/vcpkg


########################
# INTERNAL cache entries
########################

//ADVANCED property for variable: CMAKE_AR
CMAKE_AR-ADVANCED:INTERNAL=1
//This is the directory where this CMakeCache.txt was created
CMAKE_CACHEFILE_DIR:INTERNAL=c:/vcpkg/buildtrees/sdl2/x64-windows-dbg
//Major version of cmake used to create the current loaded cache
CMAKE_CACHE_MAJOR_VERSION:INTERNAL=4
//Minor version of cmake used to create the current loaded cache
CMAKE_CACHE_MINOR_VERSION:INTERNAL=2
//Patch version of cmake used to create the current loaded cache
CMAKE_CACHE_PATCH_VERSION:INTERNAL=0
//Path to CMake executable.
CMAKE_COMMAND:INTERNAL=C:/Program Files/CMake/bin/cmake.exe
//Path to cpack program executable.
CMAKE_CPACK_COMMAND:INTERNAL=C:/Program Files/CMake/bin/cpack.exe
//Path to ctest program executable.
CMAKE_CTEST_COMMAND:INTERNAL=C:/Program Files/CMake/bin/ctest.exe
//ADVANCED property for variable: CMAKE_C_COMPILER
CMAKE_C_COMPILER-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS
CMAKE_C_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_DEBUG
CMAKE_C_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_MINSIZEREL
CMAKE_C_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_RELEASE
CMAKE_C_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_RELWITHDEBINFO
CMAKE_C_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_STANDARD_LIBRARIES
CMAKE_C_STANDARD_LIBRARIES-ADVANCED:INTERNAL=1
//Path to cache edit program executable.
CMAKE_EDIT_COMMAND:INTERNAL=C:/Program Files/CMake/bin/cmake-gui.exe
//Executable file format
CMAKE_EXECUTABLE_FORMAT:INTERNAL=Unknown
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS
CMAKE_EXE_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_DEBUG
CMAKE_EXE_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_MINSIZEREL
CMAKE_EXE_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_RELEASE
CMAKE_EXE_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXPORT_BUILD_DATABASE
CMAKE_EXPORT_BUILD_DATABASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXPORT_COMPILE_COMMANDS
CMAKE_EXPORT_COMPILE_COMMANDS-ADVANCED:INTERNAL=1
//Name of external makefile project generator.
CMAKE_EXTRA_GENERATOR:INTERNAL=
//Name of generator.
CMAKE_GENERATOR:INTERNAL=Ninja
//Generator instance identifier.
CMAKE_GENERATOR_INSTANCE:INTERNAL=
//Name of generator platform.
CMAKE_GENERATOR_PLATFORM:INTERNAL=
//Name of generator toolset.
CMAKE_GENERATOR_TOOLSET:INTERNAL=
//Source directory with the top level CMakeLists.txt file for this
// project
CMAKE_HOME_DIRECTORY:INTERNAL=C:/vcpkg/buildtrees/sdl2/src/se-2.32.10-3b143ac573.clean
//ADVANCED property for variable: CMAKE_LINKER
CMAKE_LINKER-ADVANCED:INTERNAL=1
//Name of CMakeLists files to read
CMAKE_LIST_FILE_NAME:INTERNAL=CMakeLists.txt
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS
CMAKE_MODULE_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_DEBUG
CMAKE_MODULE_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL
CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_RELEASE
CMAKE_MODULE_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MT
CMAKE_MT-ADVANCED:INTERNAL=1
//number of local generators
CMAKE_NUMBER_OF_MAKEFILES:INTERNAL=1
//Platform information initialized
CMAKE_PLATFORM_INFO_INITIALIZED:INTERNAL=1
//ADVANCED property for variable: CMAKE_POLICY_VERSION_MINIMUM
CMAKE_POLICY_VERSION_MINIMUM-ADVANCED:INTERNAL=1
//noop for ranlib
CMAKE_RANLIB:INTERNAL=:
//ADVANCED property for variable: CMAKE_RC_COMPILER
CMAKE_RC_COMPILER-ADVANCED:INTERNAL=1
CMAKE_RC_COMPILER_WORKS:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS
CMAKE_RC_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_DEBUG
CMAKE_RC_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_MINSIZEREL
CMAKE_RC_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_RELEASE
CMAKE_RC_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_RELWITHDEBINFO
CMAKE_RC_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//Path to CMake installation.
CMAKE_ROOT:INTERNAL=C:/Program Files/CMake/share/cmake-4.2
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS
CMAKE_SHARED_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_DEBUG
CMAKE_SHARED_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL
CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_RELEASE
CMAKE_SHARED_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SKIP_INSTALL_RPATH
CMAKE_SKIP_INSTALL_RPATH-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SKIP_RPATH
CMAKE_SKIP_RPATH-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS
CMAKE_STATIC_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_DEBUG
CMAKE_STATIC_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL
CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_RELEASE
CMAKE_STATIC_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_TOOLCHAIN_FILE
CMAKE_TOOLCHAIN_FILE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_VERBOSE_MAKEFILE
CMAKE_VERBOSE_MAKEFILE-ADVANCED:INTERNAL=1
//Install the dependencies listed in your manifest:
//\n    If this is off, you will have to manually install your dependencies.
//\n    See https://github.com/microsoft/vcpkg/tree/master/docs/specifications/manifests.md
// for more info.
//\n
VCPKG_MANIFEST_INSTALL:INTERNAL=OFF
//ADVANCED property for variable: VCPKG_VERBOSE
VCPKG_VERBOSE-ADVANCED:INTERNAL=1
//Making sure VCPKG_MANIFEST_MODE doesn't change
Z_VCPKG_CHECK_MANIFEST_MODE:INTERNAL=OFF
//Vcpkg root directory
Z_VCPKG_ROOT_DIR:INTERNAL=C:/vcpkg

```
</details>

<details><summary>C:\vcpkg\buildtrees\sdl2\config-x64-windows-rel-CMakeConfigureLog.yaml.log</summary>

```

---
events:
  -
    kind: "message-v1"
    backtrace:
      - "C:/Program Files/CMake/share/cmake-4.2/Modules/CMakeDetermineSystem.cmake:212 (message)"
      - "CMakeLists.txt:9 (project)"
    message: |
      The system is: Windows - 10.0.26100 - AMD64
  -
    kind: "find-v1"
    backtrace:
      - "C:/Program Files/CMake/share/cmake-4.2/Modules/CMakeDetermineCompiler.cmake:63 (find_program)"
      - "C:/Program Files/CMake/share/cmake-4.2/Modules/CMakeDetermineCCompiler.cmake:64 (_cmake_find_compiler)"
      - "CMakeLists.txt:9 (project)"
    mode: "program"
    variable: "CMAKE_C_COMPILER"
    description: "C compiler"
    settings:
      SearchFramework: "LAST"
      SearchAppBundle: "LAST"
      CMAKE_FIND_USE_CMAKE_PATH: false
      CMAKE_FIND_USE_CMAKE_ENVIRONMENT_PATH: false
      CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH: true
      CMAKE_FIND_USE_CMAKE_SYSTEM_PATH: false
      CMAKE_FIND_USE_INSTALL_PREFIX: true
    names:
      - "cc"
      - "gcc"
      - "cl"
      - "bcc"
      - "xlc"
      - "icx"
      - "clang"
    candidate_directories:
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/VC/vcpackages/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/TestWindow/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/TeamFoundation/Team Explorer/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/Roslyn/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Program Files (x86)/Microsoft SDKs/Windows/v10.0A/bin/NETFX 4.8 Tools/x64/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/FSharp/Tools/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Program Files/Microsoft Visual Studio/2022/Community/Team Tools/DiagnosticsHub/Collector/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/amd64/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Windows/Microsoft.NET/Framework64/v4.0.30319/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Program Files/Microsoft Visual Studio/2022/Community/Common7/Tools/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/vcpkg/downloads/tools/powershell-core-7.5.3-windows/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Windows/System32/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Windows/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Windows/System32/wbem/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Windows/System32/WindowsPowerShell/v1.0/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Windows/System32/OpenSSH/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/VC/Linux/bin/ConnectionManagerExe/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/vcpkg/downloads/tools/ninja/1.13.1-windows/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/VC/vcpackages/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/TestWindow/"
...
Skipped 2882 lines
...
        - "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\VC\\Linux\\bin\\ConnectionManagerExe"
        - "C:\\vcpkg\\downloads\\tools\\ninja\\1.13.1-windows"
      CMAKE_INSTALL_PREFIX: "C:/vcpkg/packages/sdl2_x64-windows"
      CMAKE_SYSTEM_PREFIX_PATH:
        - "C:/Program Files"
        - "C:/Program Files (x86)"
        - "C:/Program Files/CMake"
        - "C:/vcpkg/packages/sdl2_x64-windows"
      CMAKE_FIND_ROOT_PATH: "C:/project-C/build-sdl/vcpkg_installed/x64-windows;C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug"
  -
    kind: "try_compile-v1"
    backtrace:
      - "C:/Program Files/CMake/share/cmake-4.2/Modules/CMakeDetermineCompilerABI.cmake:83 (try_compile)"
      - "C:/Program Files/CMake/share/cmake-4.2/Modules/CMakeTestCCompiler.cmake:26 (CMAKE_DETERMINE_COMPILER_ABI)"
      - "CMakeLists.txt:9 (project)"
    checks:
      - "Detecting C compiler ABI info"
    directories:
      source: "C:/vcpkg/buildtrees/sdl2/x64-windows-rel/CMakeFiles/CMakeScratch/TryCompile-p4fsto"
      binary: "C:/vcpkg/buildtrees/sdl2/x64-windows-rel/CMakeFiles/CMakeScratch/TryCompile-p4fsto"
    cmakeVariables:
      CMAKE_C_FLAGS: " /nologo /DWIN32 /D_WINDOWS /utf-8 /MP "
      CMAKE_C_FLAGS_DEBUG: "/MDd /Z7 /Ob0 /Od /RTC1 "
      CMAKE_EXE_LINKER_FLAGS: "/machine:x64"
      CMAKE_MSVC_DEBUG_INFORMATION_FORMAT: ""
      CMAKE_MSVC_RUNTIME_LIBRARY: "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<STREQUAL:dynamic,dynamic>:DLL>"
      VCPKG_CHAINLOAD_TOOLCHAIN_FILE: "C:/vcpkg/scripts/toolchains/windows.cmake"
      VCPKG_CRT_LINKAGE: "dynamic"
      VCPKG_CXX_FLAGS: ""
      VCPKG_CXX_FLAGS_DEBUG: ""
      VCPKG_CXX_FLAGS_RELEASE: ""
      VCPKG_C_FLAGS: ""
      VCPKG_C_FLAGS_DEBUG: ""
      VCPKG_C_FLAGS_RELEASE: ""
      VCPKG_INSTALLED_DIR: "C:/project-C/build-sdl/vcpkg_installed"
      VCPKG_LINKER_FLAGS: ""
      VCPKG_LINKER_FLAGS_DEBUG: ""
      VCPKG_LINKER_FLAGS_RELEASE: ""
      VCPKG_PLATFORM_TOOLSET: "v143"
      VCPKG_PREFER_SYSTEM_LIBS: "OFF"
      VCPKG_SET_CHARSET_FLAG: "ON"
      VCPKG_TARGET_ARCHITECTURE: "x64"
      VCPKG_TARGET_TRIPLET: "x64-windows"
      Z_VCPKG_ROOT_DIR: "C:/vcpkg"
    buildResult:
      variable: "CMAKE_C_ABI_COMPILED"
      cached: true
      stdout: |
        Change Dir: 'C:/vcpkg/buildtrees/sdl2/x64-windows-rel/CMakeFiles/CMakeScratch/TryCompile-p4fsto'
        
        Run Build Command(s): C:/vcpkg/downloads/tools/ninja/1.13.1-windows/ninja.exe -v cmTC_3de8d
        [1/2] C:\\PROGRA~1\\MICROS~4\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1444~1.352\\bin\\Hostx64\\x64\\cl.exe  /nologo -D_MBCS  /nologo /DWIN32 /D_WINDOWS /utf-8 /MP   /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\\cmTC_3de8d.dir\\CMakeCCompilerABI.c.obj /FdCMakeFiles\\cmTC_3de8d.dir\\ /FS -c "C:\\Program Files\\CMake\\share\\cmake-4.2\\Modules\\CMakeCCompilerABI.c"
        [2/2] C:\\Windows\\system32\\cmd.exe /C "cd . && "C:\\Program Files\\CMake\\bin\\cmake.exe" -E vs_link_exe --msvc-ver=1944 --intdir=CMakeFiles\\cmTC_3de8d.dir --rc=rc --mt=CMAKE_MT-NOTFOUND --manifests  -- C:\\PROGRA~1\\MICROS~4\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1444~1.352\\bin\\Hostx64\\x64\\link.exe /nologo CMakeFiles\\cmTC_3de8d.dir\\CMakeCCompilerABI.c.obj  /out:cmTC_3de8d.exe /implib:cmTC_3de8d.lib /pdb:cmTC_3de8d.pdb /version:0.0 /machine:x64 /nologo /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
        FAILED: [code=4294967295] cmTC_3de8d.exe 
        C:\\Windows\\system32\\cmd.exe /C "cd . && "C:\\Program Files\\CMake\\bin\\cmake.exe" -E vs_link_exe --msvc-ver=1944 --intdir=CMakeFiles\\cmTC_3de8d.dir --rc=rc --mt=CMAKE_MT-NOTFOUND --manifests  -- C:\\PROGRA~1\\MICROS~4\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1444~1.352\\bin\\Hostx64\\x64\\link.exe /nologo CMakeFiles\\cmTC_3de8d.dir\\CMakeCCompilerABI.c.obj  /out:cmTC_3de8d.exe /implib:cmTC_3de8d.lib /pdb:cmTC_3de8d.pdb /version:0.0 /machine:x64 /nologo /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
        RC Pass 1: command "rc /fo CMakeFiles\\cmTC_3de8d.dir/manifest.res CMakeFiles\\cmTC_3de8d.dir/manifest.rc" failed (exit code 0) with the following output:
        no such file or directory
        ninja: build stopped: subcommand failed.
        
      exitCode: -1
  -
    kind: "try_compile-v1"
    backtrace:
      - "C:/Program Files/CMake/share/cmake-4.2/Modules/CMakeTestCCompiler.cmake:56 (try_compile)"
      - "CMakeLists.txt:9 (project)"
    checks:
      - "Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe"
    directories:
      source: "C:/vcpkg/buildtrees/sdl2/x64-windows-rel/CMakeFiles/CMakeScratch/TryCompile-ctt9yz"
      binary: "C:/vcpkg/buildtrees/sdl2/x64-windows-rel/CMakeFiles/CMakeScratch/TryCompile-ctt9yz"
    cmakeVariables:
      CMAKE_C_FLAGS: " /nologo /DWIN32 /D_WINDOWS /utf-8 /MP "
      CMAKE_C_FLAGS_DEBUG: "/MDd /Z7 /Ob0 /Od /RTC1 "
      CMAKE_EXE_LINKER_FLAGS: "/machine:x64"
      CMAKE_MSVC_DEBUG_INFORMATION_FORMAT: ""
      CMAKE_MSVC_RUNTIME_LIBRARY: "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<STREQUAL:dynamic,dynamic>:DLL>"
      VCPKG_CHAINLOAD_TOOLCHAIN_FILE: "C:/vcpkg/scripts/toolchains/windows.cmake"
      VCPKG_CRT_LINKAGE: "dynamic"
      VCPKG_CXX_FLAGS: ""
      VCPKG_CXX_FLAGS_DEBUG: ""
      VCPKG_CXX_FLAGS_RELEASE: ""
      VCPKG_C_FLAGS: ""
      VCPKG_C_FLAGS_DEBUG: ""
      VCPKG_C_FLAGS_RELEASE: ""
      VCPKG_INSTALLED_DIR: "C:/project-C/build-sdl/vcpkg_installed"
      VCPKG_LINKER_FLAGS: ""
      VCPKG_LINKER_FLAGS_DEBUG: ""
      VCPKG_LINKER_FLAGS_RELEASE: ""
      VCPKG_PLATFORM_TOOLSET: "v143"
      VCPKG_PREFER_SYSTEM_LIBS: "OFF"
      VCPKG_SET_CHARSET_FLAG: "ON"
      VCPKG_TARGET_ARCHITECTURE: "x64"
      VCPKG_TARGET_TRIPLET: "x64-windows"
      Z_VCPKG_ROOT_DIR: "C:/vcpkg"
    buildResult:
      variable: "CMAKE_C_COMPILER_WORKS"
      cached: true
      stdout: |
        Change Dir: 'C:/vcpkg/buildtrees/sdl2/x64-windows-rel/CMakeFiles/CMakeScratch/TryCompile-ctt9yz'
        
        Run Build Command(s): C:/vcpkg/downloads/tools/ninja/1.13.1-windows/ninja.exe -v cmTC_ad155
        [1/2] C:\\PROGRA~1\\MICROS~4\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1444~1.352\\bin\\Hostx64\\x64\\cl.exe  /nologo -D_MBCS  /nologo /DWIN32 /D_WINDOWS /utf-8 /MP   /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\\cmTC_ad155.dir\\testCCompiler.c.obj /FdCMakeFiles\\cmTC_ad155.dir\\ /FS -c C:\\vcpkg\\buildtrees\\sdl2\\x64-windows-rel\\CMakeFiles\\CMakeScratch\\TryCompile-ctt9yz\\testCCompiler.c
        [2/2] C:\\Windows\\system32\\cmd.exe /C "cd . && "C:\\Program Files\\CMake\\bin\\cmake.exe" -E vs_link_exe --msvc-ver=1944 --intdir=CMakeFiles\\cmTC_ad155.dir --rc=rc --mt=CMAKE_MT-NOTFOUND --manifests  -- C:\\PROGRA~1\\MICROS~4\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1444~1.352\\bin\\Hostx64\\x64\\link.exe /nologo CMakeFiles\\cmTC_ad155.dir\\testCCompiler.c.obj  /out:cmTC_ad155.exe /implib:cmTC_ad155.lib /pdb:cmTC_ad155.pdb /version:0.0 /machine:x64 /nologo /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
        FAILED: [code=4294967295] cmTC_ad155.exe 
        C:\\Windows\\system32\\cmd.exe /C "cd . && "C:\\Program Files\\CMake\\bin\\cmake.exe" -E vs_link_exe --msvc-ver=1944 --intdir=CMakeFiles\\cmTC_ad155.dir --rc=rc --mt=CMAKE_MT-NOTFOUND --manifests  -- C:\\PROGRA~1\\MICROS~4\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1444~1.352\\bin\\Hostx64\\x64\\link.exe /nologo CMakeFiles\\cmTC_ad155.dir\\testCCompiler.c.obj  /out:cmTC_ad155.exe /implib:cmTC_ad155.lib /pdb:cmTC_ad155.pdb /version:0.0 /machine:x64 /nologo /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
        RC Pass 1: command "rc /fo CMakeFiles\\cmTC_ad155.dir/manifest.res CMakeFiles\\cmTC_ad155.dir/manifest.rc" failed (exit code 0) with the following output:
        no such file or directory
        ninja: build stopped: subcommand failed.
        
      exitCode: -1
...
```
</details>

<details><summary>C:\vcpkg\buildtrees\sdl2\config-x64-windows-dbg-CMakeConfigureLog.yaml.log</summary>

```

---
events:
  -
    kind: "message-v1"
    backtrace:
      - "C:/Program Files/CMake/share/cmake-4.2/Modules/CMakeDetermineSystem.cmake:212 (message)"
      - "CMakeLists.txt:9 (project)"
    message: |
      The system is: Windows - 10.0.26100 - AMD64
  -
    kind: "find-v1"
    backtrace:
      - "C:/Program Files/CMake/share/cmake-4.2/Modules/CMakeDetermineCompiler.cmake:63 (find_program)"
      - "C:/Program Files/CMake/share/cmake-4.2/Modules/CMakeDetermineCCompiler.cmake:64 (_cmake_find_compiler)"
      - "CMakeLists.txt:9 (project)"
    mode: "program"
    variable: "CMAKE_C_COMPILER"
    description: "C compiler"
    settings:
      SearchFramework: "LAST"
      SearchAppBundle: "LAST"
      CMAKE_FIND_USE_CMAKE_PATH: false
      CMAKE_FIND_USE_CMAKE_ENVIRONMENT_PATH: false
      CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH: true
      CMAKE_FIND_USE_CMAKE_SYSTEM_PATH: false
      CMAKE_FIND_USE_INSTALL_PREFIX: true
    names:
      - "cc"
      - "gcc"
      - "cl"
      - "bcc"
      - "xlc"
      - "icx"
      - "clang"
    candidate_directories:
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/VC/vcpackages/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/TestWindow/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/TeamFoundation/Team Explorer/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/Roslyn/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Program Files (x86)/Microsoft SDKs/Windows/v10.0A/bin/NETFX 4.8 Tools/x64/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/FSharp/Tools/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Program Files/Microsoft Visual Studio/2022/Community/Team Tools/DiagnosticsHub/Collector/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/amd64/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Windows/Microsoft.NET/Framework64/v4.0.30319/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Program Files/Microsoft Visual Studio/2022/Community/Common7/Tools/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/vcpkg/downloads/tools/powershell-core-7.5.3-windows/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Windows/System32/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Windows/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Windows/System32/wbem/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Windows/System32/WindowsPowerShell/v1.0/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Windows/System32/OpenSSH/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/VC/Linux/bin/ConnectionManagerExe/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug/vcpkg/downloads/tools/ninja/1.13.1-windows/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/"
      - "C:/project-C/build-sdl/vcpkg_installed/x64-windows/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/VC/vcpackages/"
...
Skipped 2884 lines
...
        - "C:\\vcpkg\\downloads\\tools\\ninja\\1.13.1-windows"
      CMAKE_INSTALL_PREFIX: "C:/vcpkg/packages/sdl2_x64-windows/debug"
      CMAKE_SYSTEM_PREFIX_PATH:
        - "C:/Program Files"
        - "C:/Program Files (x86)"
        - "C:/Program Files/CMake"
        - "C:/vcpkg/packages/sdl2_x64-windows/debug"
      CMAKE_FIND_ROOT_PATH: "C:/project-C/build-sdl/vcpkg_installed/x64-windows/debug;C:/project-C/build-sdl/vcpkg_installed/x64-windows"
  -
    kind: "try_compile-v1"
    backtrace:
      - "C:/Program Files/CMake/share/cmake-4.2/Modules/CMakeDetermineCompilerABI.cmake:83 (try_compile)"
      - "C:/Program Files/CMake/share/cmake-4.2/Modules/CMakeTestCCompiler.cmake:26 (CMAKE_DETERMINE_COMPILER_ABI)"
      - "CMakeLists.txt:9 (project)"
    checks:
      - "Detecting C compiler ABI info"
    directories:
      source: "C:/vcpkg/buildtrees/sdl2/x64-windows-dbg/CMakeFiles/CMakeScratch/TryCompile-hoc0ea"
      binary: "C:/vcpkg/buildtrees/sdl2/x64-windows-dbg/CMakeFiles/CMakeScratch/TryCompile-hoc0ea"
    cmakeVariables:
      CMAKE_C_FLAGS: " /nologo /DWIN32 /D_WINDOWS /utf-8 /MP "
      CMAKE_C_FLAGS_DEBUG: "/MDd /Z7 /Ob0 /Od /RTC1 "
      CMAKE_EXE_LINKER_FLAGS: "/machine:x64"
      CMAKE_MSVC_DEBUG_INFORMATION_FORMAT: ""
      CMAKE_MSVC_RUNTIME_LIBRARY: "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<STREQUAL:dynamic,dynamic>:DLL>"
      VCPKG_CHAINLOAD_TOOLCHAIN_FILE: "C:/vcpkg/scripts/toolchains/windows.cmake"
      VCPKG_CRT_LINKAGE: "dynamic"
      VCPKG_CXX_FLAGS: ""
      VCPKG_CXX_FLAGS_DEBUG: ""
      VCPKG_CXX_FLAGS_RELEASE: ""
      VCPKG_C_FLAGS: ""
      VCPKG_C_FLAGS_DEBUG: ""
      VCPKG_C_FLAGS_RELEASE: ""
      VCPKG_INSTALLED_DIR: "C:/project-C/build-sdl/vcpkg_installed"
      VCPKG_LINKER_FLAGS: ""
      VCPKG_LINKER_FLAGS_DEBUG: ""
      VCPKG_LINKER_FLAGS_RELEASE: ""
      VCPKG_PLATFORM_TOOLSET: "v143"
      VCPKG_PREFER_SYSTEM_LIBS: "OFF"
      VCPKG_SET_CHARSET_FLAG: "ON"
      VCPKG_TARGET_ARCHITECTURE: "x64"
      VCPKG_TARGET_TRIPLET: "x64-windows"
      Z_VCPKG_ROOT_DIR: "C:/vcpkg"
    buildResult:
      variable: "CMAKE_C_ABI_COMPILED"
      cached: true
      stdout: |
        Change Dir: 'C:/vcpkg/buildtrees/sdl2/x64-windows-dbg/CMakeFiles/CMakeScratch/TryCompile-hoc0ea'
        
        Run Build Command(s): C:/vcpkg/downloads/tools/ninja/1.13.1-windows/ninja.exe -v cmTC_a03a0
        [1/2] C:\\PROGRA~1\\MICROS~4\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1444~1.352\\bin\\Hostx64\\x64\\cl.exe  /nologo -D_MBCS  /nologo /DWIN32 /D_WINDOWS /utf-8 /MP   /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\\cmTC_a03a0.dir\\CMakeCCompilerABI.c.obj /FdCMakeFiles\\cmTC_a03a0.dir\\ /FS -c "C:\\Program Files\\CMake\\share\\cmake-4.2\\Modules\\CMakeCCompilerABI.c"
        [2/2] C:\\Windows\\system32\\cmd.exe /C "cd . && "C:\\Program Files\\CMake\\bin\\cmake.exe" -E vs_link_exe --msvc-ver=1944 --intdir=CMakeFiles\\cmTC_a03a0.dir --rc=rc --mt=CMAKE_MT-NOTFOUND --manifests  -- C:\\PROGRA~1\\MICROS~4\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1444~1.352\\bin\\Hostx64\\x64\\link.exe /nologo CMakeFiles\\cmTC_a03a0.dir\\CMakeCCompilerABI.c.obj  /out:cmTC_a03a0.exe /implib:cmTC_a03a0.lib /pdb:cmTC_a03a0.pdb /version:0.0 /machine:x64 /nologo /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
        FAILED: [code=4294967295] cmTC_a03a0.exe 
        C:\\Windows\\system32\\cmd.exe /C "cd . && "C:\\Program Files\\CMake\\bin\\cmake.exe" -E vs_link_exe --msvc-ver=1944 --intdir=CMakeFiles\\cmTC_a03a0.dir --rc=rc --mt=CMAKE_MT-NOTFOUND --manifests  -- C:\\PROGRA~1\\MICROS~4\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1444~1.352\\bin\\Hostx64\\x64\\link.exe /nologo CMakeFiles\\cmTC_a03a0.dir\\CMakeCCompilerABI.c.obj  /out:cmTC_a03a0.exe /implib:cmTC_a03a0.lib /pdb:cmTC_a03a0.pdb /version:0.0 /machine:x64 /nologo /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
        RC Pass 1: command "rc /fo CMakeFiles\\cmTC_a03a0.dir/manifest.res CMakeFiles\\cmTC_a03a0.dir/manifest.rc" failed (exit code 0) with the following output:
        no such file or directory
        ninja: build stopped: subcommand failed.
        
      exitCode: -1
  -
    kind: "try_compile-v1"
    backtrace:
      - "C:/Program Files/CMake/share/cmake-4.2/Modules/CMakeTestCCompiler.cmake:56 (try_compile)"
      - "CMakeLists.txt:9 (project)"
    checks:
      - "Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe"
    directories:
      source: "C:/vcpkg/buildtrees/sdl2/x64-windows-dbg/CMakeFiles/CMakeScratch/TryCompile-zyqhmt"
      binary: "C:/vcpkg/buildtrees/sdl2/x64-windows-dbg/CMakeFiles/CMakeScratch/TryCompile-zyqhmt"
    cmakeVariables:
      CMAKE_C_FLAGS: " /nologo /DWIN32 /D_WINDOWS /utf-8 /MP "
      CMAKE_C_FLAGS_DEBUG: "/MDd /Z7 /Ob0 /Od /RTC1 "
      CMAKE_EXE_LINKER_FLAGS: "/machine:x64"
      CMAKE_MSVC_DEBUG_INFORMATION_FORMAT: ""
      CMAKE_MSVC_RUNTIME_LIBRARY: "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<STREQUAL:dynamic,dynamic>:DLL>"
      VCPKG_CHAINLOAD_TOOLCHAIN_FILE: "C:/vcpkg/scripts/toolchains/windows.cmake"
      VCPKG_CRT_LINKAGE: "dynamic"
      VCPKG_CXX_FLAGS: ""
      VCPKG_CXX_FLAGS_DEBUG: ""
      VCPKG_CXX_FLAGS_RELEASE: ""
      VCPKG_C_FLAGS: ""
      VCPKG_C_FLAGS_DEBUG: ""
      VCPKG_C_FLAGS_RELEASE: ""
      VCPKG_INSTALLED_DIR: "C:/project-C/build-sdl/vcpkg_installed"
      VCPKG_LINKER_FLAGS: ""
      VCPKG_LINKER_FLAGS_DEBUG: ""
      VCPKG_LINKER_FLAGS_RELEASE: ""
      VCPKG_PLATFORM_TOOLSET: "v143"
      VCPKG_PREFER_SYSTEM_LIBS: "OFF"
      VCPKG_SET_CHARSET_FLAG: "ON"
      VCPKG_TARGET_ARCHITECTURE: "x64"
      VCPKG_TARGET_TRIPLET: "x64-windows"
      Z_VCPKG_ROOT_DIR: "C:/vcpkg"
    buildResult:
      variable: "CMAKE_C_COMPILER_WORKS"
      cached: true
      stdout: |
        Change Dir: 'C:/vcpkg/buildtrees/sdl2/x64-windows-dbg/CMakeFiles/CMakeScratch/TryCompile-zyqhmt'
        
        Run Build Command(s): C:/vcpkg/downloads/tools/ninja/1.13.1-windows/ninja.exe -v cmTC_e7576
        [1/2] C:\\PROGRA~1\\MICROS~4\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1444~1.352\\bin\\Hostx64\\x64\\cl.exe  /nologo -D_MBCS  /nologo /DWIN32 /D_WINDOWS /utf-8 /MP   /MDd /Z7 /Ob0 /Od /RTC1  -MDd /showIncludes /FoCMakeFiles\\cmTC_e7576.dir\\testCCompiler.c.obj /FdCMakeFiles\\cmTC_e7576.dir\\ /FS -c C:\\vcpkg\\buildtrees\\sdl2\\x64-windows-dbg\\CMakeFiles\\CMakeScratch\\TryCompile-zyqhmt\\testCCompiler.c
        [2/2] C:\\Windows\\system32\\cmd.exe /C "cd . && "C:\\Program Files\\CMake\\bin\\cmake.exe" -E vs_link_exe --msvc-ver=1944 --intdir=CMakeFiles\\cmTC_e7576.dir --rc=rc --mt=CMAKE_MT-NOTFOUND --manifests  -- C:\\PROGRA~1\\MICROS~4\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1444~1.352\\bin\\Hostx64\\x64\\link.exe /nologo CMakeFiles\\cmTC_e7576.dir\\testCCompiler.c.obj  /out:cmTC_e7576.exe /implib:cmTC_e7576.lib /pdb:cmTC_e7576.pdb /version:0.0 /machine:x64 /nologo /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
        FAILED: [code=4294967295] cmTC_e7576.exe 
        C:\\Windows\\system32\\cmd.exe /C "cd . && "C:\\Program Files\\CMake\\bin\\cmake.exe" -E vs_link_exe --msvc-ver=1944 --intdir=CMakeFiles\\cmTC_e7576.dir --rc=rc --mt=CMAKE_MT-NOTFOUND --manifests  -- C:\\PROGRA~1\\MICROS~4\\2022\\COMMUN~1\\VC\\Tools\\MSVC\\1444~1.352\\bin\\Hostx64\\x64\\link.exe /nologo CMakeFiles\\cmTC_e7576.dir\\testCCompiler.c.obj  /out:cmTC_e7576.exe /implib:cmTC_e7576.lib /pdb:cmTC_e7576.pdb /version:0.0 /machine:x64 /nologo /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib && cd ."
        RC Pass 1: command "rc /fo CMakeFiles\\cmTC_e7576.dir/manifest.res CMakeFiles\\cmTC_e7576.dir/manifest.rc" failed (exit code 0) with the following output:
        no such file or directory
        ninja: build stopped: subcommand failed.
        
      exitCode: -1
...
```
</details>

**Additional context**

<details><summary>vcpkg.json</summary>

```
{
  "name": "project-c",
  "version": "1.0.0",
  "description": "A C++ shapes library and demonstration",
  "dependencies": [
    "sdl2"
  ],
  "builtin-baseline": "2ad7bd06128280e02bfe02361d8ffd7d465cfcf0"
}

```
</details>
