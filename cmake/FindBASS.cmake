# - Cmake script to download BASS audio library 2.4 from un4seen.com/files/bass24
# - and extract relevant libaries. Also includes ALAC and AAC codecs for non MacOS
# - systems. 
# from https://github.com/davidt0x/Spike-Recorder
#
# Creates target: BASS
#
# Original Author:
# 2020 David Turner <davidt0x@gmail.com>
# SPDX short identifier: BSD-3-Clause


# We need to fetch BASS audio library
function(download_file url filename)

	if(NOT EXISTS ${filename})
	  file(DOWNLOAD ${url} ${filename}
	       TIMEOUT 60  # seconds
	       TLS_VERIFY ON)
	endif()

endfunction(download_file)

if (WIN32)
  download_file(http://us.un4seen.com/files/bass24.zip ${CMAKE_CURRENT_BINARY_DIR}/bass/bass24.zip)
  download_file(http://www.un4seen.com/files/bassalac24.zip ${CMAKE_CURRENT_BINARY_DIR}/bass/bassalac24.zip)
  download_file(http://www.un4seen.com/files/z/2/bass_aac24.zip ${CMAKE_CURRENT_BINARY_DIR}/bass/bass_aac24.zip)
  file(ARCHIVE_EXTRACT INPUT ${CMAKE_CURRENT_BINARY_DIR}/bass/bass24.zip DESTINATION bass PATTERNS c/bass.h c/x64/bass.lib x64/bass.dll) 
  file(ARCHIVE_EXTRACT INPUT ${CMAKE_CURRENT_BINARY_DIR}/bass/bassalac24.zip DESTINATION bass PATTERNS x64/bassalac.dll) 
  file(ARCHIVE_EXTRACT INPUT ${CMAKE_CURRENT_BINARY_DIR}/bass/bass_aac24.zip DESTINATION bass PATTERNS x64/bass_aac.dll) 
  add_library(BASS STATIC IMPORTED)
  set_target_properties(BASS PROPERTIES IMPORTED_LOCATION bass/c/x64/bass.lib)
  set_target_properties(BASS PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/bass/c")
elseif (APPLE)
  download_file(http://us.un4seen.com/files/bass24-osx.zip ${CMAKE_CURRENT_BINARY_DIR}/bass/bass24.zip)
  file(ARCHIVE_EXTRACT INPUT ${CMAKE_CURRENT_BINARY_DIR}/bass/bass24.zip DESTINATION bass PATTERNS bass.h libbass.dylib) 
  add_library(BASS SHARED IMPORTED)
  set_target_properties(BASS PROPERTIES IMPORTED_LOCATION bass/libbass.dylib)
  set_target_properties(BASS PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/bass/")
elseif (LINUX)
  download_file(http://us.un4seen.com/files/bass24-linux.zip ${CMAKE_CURRENT_BINARY_DIR}/bass/bass24.zip)
  download_file(http://www.un4seen.com/files/bassalac24-linux.zip ${CMAKE_CURRENT_BINARY_DIR}/bass/bassalac24.zip)
  download_file(http://www.un4seen.com/files/z/2/bass_aac24-linux.zip ${CMAKE_CURRENT_BINARY_DIR}/bass/bass_aac24.zip)
  file(ARCHIVE_EXTRACT INPUT ${CMAKE_CURRENT_BINARY_DIR}/bass/bass24.zip DESTINATION bass PATTERNS bass.h x64/libbass.so) 
  file(ARCHIVE_EXTRACT INPUT ${CMAKE_CURRENT_BINARY_DIR}/bass/bassalac24.zip DESTINATION bass PATTERNS x64/libbassalac.so) 
  file(ARCHIVE_EXTRACT INPUT ${CMAKE_CURRENT_BINARY_DIR}/bass/bass_aac24.zip DESTINATION bass PATTERNS x64/libbass_aac.so) 
  add_library(BASS SHARED IMPORTED)
  set_target_properties(BASS PROPERTIES IMPORTED_LOCATION bass/x64/libbass.so)
  set_target_properties(BASS PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/bass/")
endif()
