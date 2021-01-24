cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(SpikeRecorder)

if(UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(hidapi)
find_package(SDL2 REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(cppzmq REQUIRED)
find_package(BASS REQUIRED)
find_package(nlohmann_json REQUIRED)

# General include directories for the project
include_directories("src" "src/libraries" "support")

file(GLOB SOURCES "src/*.cpp" "src/widgets/*.cpp" "src/engine/*.cpp" "src/libraries/*.cpp")

# Need to remove some firmware upgrade source, this is disabled for all builds for now
list(FILTER SOURCES EXCLUDE REGEX "FirmwareUpdateView.cpp")

# Add OS specific source files and libs
if (WIN32)
	list(APPEND SOURCES 
			"src/native/PathsWin.cpp" 
			"src/widgets/native/FileDialogWin.cpp" 
			"src/defaults/DefaultConfig.cpp")
	
	# setupapi.lib is needed for some functions in HID code on windows it seems. MSVC knows where this lib is so just need
	# to add it by name.
	list(APPEND PLATFORM_LIBS "setupapi.lib")
elseif (LINUX)
	list(APPEND SOURCES 
			"src/native/PathsLinux.cpp" 
			"src/widgets/native/FileDialogLinux.cpp")
elseif (APPLE)
	list(APPEND SOURCES 
		"src/native/PathsMac.cpp" 
		"src/native/KeyboardGeneratorMac.cpp"
		"src/native/SerialPortsScanMac.cpp"
		"src/widgets/native/FileDialogMac.cpp" 
		"src/widgets/native/FileDialogMac.mm" 
		"src/defaults/DefaultConfig.cpp" 
		)
endif ()

add_executable(SpikeRecorder ${SOURCES})

target_link_libraries(SpikeRecorder PUBLIC 
	SDL2::SDL2-static
	SDL2::SDL2_image
	hidapi::hidapi
	OpenGL::GL 
	BASS
	cppzmq-static
	nlohmann_json::nlohmann_json
	${PLATFORM_LIBS})

target_compile_definitions(SpikeRecorder PUBLIC -DSIGSLOT_PURE_ISO -DSDL_MAIN_HANDLED)

if (WIN32)
	target_compile_definitions(SpikeRecorder PUBLIC -D_USE_MATH_DEFINES -DNOMINMAX)
	
	# Need this to enable WinMain and turn off debug console
	set_target_properties(SpikeRecorder PROPERTIES WIN32_EXECUTABLE TRUE)
	
	# We need to set these extra linker flags to overide WinMain to main from the above
	# property. There is probably a better way to do this but this is all I could get to
	# work.
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	
	set_property(TARGET SpikeRecorder PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif (APPLE)
	# Lets bundle it:
	set_target_properties(SpikeRecorder PROPERTIES
	  MACOSX_BUNDLE TRUE
	  MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/macosx-Info.plist
	  XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS ${CMAKE_CURRENT_SOURCE_DIR}/SpikeRecorder.entitlements
	)
endif ()

# We need to copy data/ to the build directory too
add_custom_command(TARGET SpikeRecorder POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_CURRENT_LIST_DIR}/data
    $<TARGET_FILE_DIR:SpikeRecorder>/data
)

# Post build commands to copy BASS runtime libraries and data to
# executable directory

# Add a post build command to copy bass runtime libs
if (WIN32)
  add_custom_command(TARGET SpikeRecorder POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    "${CMAKE_CURRENT_BINARY_DIR}/bass/x64/bass.dll" 
    "${CMAKE_CURRENT_BINARY_DIR}/bass/x64/bass_aac.dll" 
    "${CMAKE_CURRENT_BINARY_DIR}/bass/x64/bassalac.dll" 
    $<TARGET_FILE_DIR:SpikeRecorder>)
endif()
if (LINUX)
  add_custom_command(TARGET SpikeRecorder POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    "${CMAKE_CURRENT_BINARY_DIR}/bass/x64/libbass.so" 
    "${CMAKE_CURRENT_BINARY_DIR}/bass/x64/libbass_aac.so" 
    "${CMAKE_CURRENT_BINARY_DIR}/bass/x64/libbassalac.so" 
    $<TARGET_FILE_DIR:SpikeRecorder>)
endif()
if (APPLE)
  add_custom_command(TARGET SpikeRecorder POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    "${CMAKE_CURRENT_BINARY_DIR}/bass/libbass.dylib" 
    $<TARGET_FILE_DIR:SpikeRecorder>)
endif()
