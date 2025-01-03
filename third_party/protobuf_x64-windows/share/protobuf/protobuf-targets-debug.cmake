#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "protobuf::libprotobuf" for configuration "Debug"
set_property(TARGET protobuf::libprotobuf APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(protobuf::libprotobuf PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/libprotobufd.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/libprotobufd.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS protobuf::libprotobuf )
list(APPEND _IMPORT_CHECK_FILES_FOR_protobuf::libprotobuf "${_IMPORT_PREFIX}/debug/lib/libprotobufd.lib" "${_IMPORT_PREFIX}/debug/bin/libprotobufd.dll" )

# Import target "protobuf::libprotoc" for configuration "Debug"
set_property(TARGET protobuf::libprotoc APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(protobuf::libprotoc PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/libprotocd.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/libprotocd.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS protobuf::libprotoc )
list(APPEND _IMPORT_CHECK_FILES_FOR_protobuf::libprotoc "${_IMPORT_PREFIX}/debug/lib/libprotocd.lib" "${_IMPORT_PREFIX}/debug/bin/libprotocd.dll" )

# Import target "protobuf::protoc" for configuration "Debug"
set_property(TARGET protobuf::protoc APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(protobuf::protoc PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/tools/protobuf/protoc.exe"
  )

list(APPEND _IMPORT_CHECK_TARGETS protobuf::protoc )
list(APPEND _IMPORT_CHECK_FILES_FOR_protobuf::protoc "${_IMPORT_PREFIX}/tools/protobuf/protoc.exe" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
