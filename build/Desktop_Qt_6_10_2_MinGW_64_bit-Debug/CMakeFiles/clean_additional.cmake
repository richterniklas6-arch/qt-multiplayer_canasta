# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Multiplayer_Canasta_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Multiplayer_Canasta_autogen.dir\\ParseCache.txt"
  "Multiplayer_Canasta_autogen"
  )
endif()
