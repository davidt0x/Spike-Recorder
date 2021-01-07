cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(SpikeRecorder)

#list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(hidapi)
find_package(SDL2 REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)


# We need to fetch BASS audio library
function(download_file url filename)

	if(NOT EXISTS ${filename})
	  file(DOWNLOAD ${url} ${filename}
	       TIMEOUT 60  # seconds
	       TLS_VERIFY ON)
	endif()

endfunction(download_file)

# We need three packages for BASS, the main library and two plugins for AAC
# and ALAC formats. Can't figure out a nice cross platform way to do this
# so we need to hard code downloads of the libraries.
file(MAKE_DIRECTORY bass)
if (WIN32)
  download_file(http://us.un4seen.com/files/bass24.zip bass/bass24.zip)
  download_file(http://www.un4seen.com/files/bassalac24.zip bass/bassalac24.zip)
  download_file(http://www.un4seen.com/files/z/2/bass_aac24.zip bass/bass_aac24.zip)
  file(ARCHIVE_EXTRACT INPUT bass/bass24.zip DESTINATION bass PATTERNS c/bass.h c/x64/bass.lib x64/bass.dll) 
  file(ARCHIVE_EXTRACT INPUT bass/bassalac24.zip DESTINATION bass PATTERNS x64/bassalac.dll) 
  file(ARCHIVE_EXTRACT INPUT bass/bass_aac24.zip DESTINATION bass PATTERNS x64/bass_aac.dll) 
  add_library(BASS STATIC IMPORTED)
  set_target_properties(BASS PROPERTIES IMPORTED_LOCATION bass/c/x64/bass.lib)
  set_target_properties(BASS PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/bass/c")
elseif (APPLE)
  download_file(http://us.un4seen.com/files/bass24-osx.zip bass/bass24-osx.zip)
elseif (UNIX)
  download_file(http://us.un4seen.com/files/bass24-linux.zip bass/bass24.zip)
  download_file(http://www.un4seen.com/files/bassalac24-linux.zip bass/bassalac24.zip)
  download_file(http://www.un4seen.com/files/z/2/bass_aac24-linux.zip bass/bass_aac24.zip)
  file(ARCHIVE_EXTRACT INPUT bass/bass24.zip DESTINATION bass PATTERNS bass.h x64/libbass.so) 
  file(ARCHIVE_EXTRACT INPUT bass/bassalac24.zip DESTINATION bass PATTERNS x64/libbassalac.so) 
  file(ARCHIVE_EXTRACT INPUT bass/bass_aac24.zip DESTINATION bass PATTERNS x64/libbass_aac.so) 
  add_library(BASS SHARED IMPORTED)
  set_target_properties(BASS PROPERTIES IMPORTED_LOCATION bass/x64/libbass.so)
  set_target_properties(BASS PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/bass/")
endif()

# General include directories for the project
include_directories("src" "src/libraries" "support")

file(GLOB SOURCES "src/*.cpp" "src/widgets/*.cpp" "src/engine/*.cpp" "src/libraries/*.cpp")

# Need to remove some firmware upgrade source, this is disabled for all builds for now
list(FILTER SOURCES EXCLUDE REGEX "FirmwareUpdateView.cpp")

# Add OS specific code
if (WIN32)
	list(APPEND SOURCES "src/native/PathsWin.cpp" "src/widgets/native/FileDialogWin.cpp" "src/defaults/DefaultConfig.cpp")
	list(APPEND PLATFORM_LIBS "setupapi.lib")
endif (WIN32)

if (UNIX)
	list(APPEND SOURCES "src/native/PathsLinux.cpp" "src/widgets/native/FileDialogLinux.cpp")
endif (UNIX)

if (APPLE)
	list(APPEND SOURCES "src/native/PathsMac.cpp" "src/widgets/native/FileDialogMac.cpp" "src/default/DefaultConfig.cpp")
endif (APPLE)

add_executable(SpikeRecorder ${SOURCES})
target_link_libraries(SpikeRecorder PUBLIC SDL2::SDL2-static SDL2::SDL2_image hidapi::hidapi OpenGL::GL BASS ${PLATFORM_LIBS})

target_compile_definitions(SpikeRecorder PUBLIC -DSIGSLOT_PURE_ISO)

if (WIN32)
  target_compile_definitions(SpikeRecorder PUBLIC -D_USE_MATH_DEFINES -DNOMINMAX)

  #--------------------------------------------------------------------
    # Hide the console window in visual studio projects
    #--------------------------------------------------------------------
  if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
  endif()

  #--------------------------------------------------------------------
  # Hide the console window in visual studio projects - Release
  #--------------------------------------------------------------------
  if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
  endif()
endif (WIN32)

add_definitions(-DSDL_MAIN_HANDLED)

# We need to copy data/ to the build directory too
add_custom_command(TARGET SpikeRecorder POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_CURRENT_LIST_DIR}/data
    $<TARGET_FILE_DIR:SpikeRecorder>/data
)

# Post build commands to copy BASS runtime libraries and data

# Add a post build command to copy bass runtime libs
if (WIN32)
  add_custom_command(TARGET SpikeRecorder POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    "${CMAKE_CURRENT_BINARY_DIR}/bass/x64/bass.dll" 
    "${CMAKE_CURRENT_BINARY_DIR}/bass/x64/bass_aac.dll" 
    "${CMAKE_CURRENT_BINARY_DIR}/bass/x64/bassalac.dll" 
    $<TARGET_FILE_DIR:SpikeRecorder>)
endif()
if (UNIX)
  add_custom_command(TARGET SpikeRecorder POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    "${CMAKE_CURRENT_BINARY_DIR}/bass/x64/libbass.so" 
    "${CMAKE_CURRENT_BINARY_DIR}/bass/x64/libbass_aac.so" 
    "${CMAKE_CURRENT_BINARY_DIR}/bass/x64/libbassalac.so" 
    $<TARGET_FILE_DIR:SpikeRecorder>)
endif()
