# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\MacroPad_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\MacroPad_autogen.dir\\ParseCache.txt"
  "MacroPad_autogen"
  )
endif()
