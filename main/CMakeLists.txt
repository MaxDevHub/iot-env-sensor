file(GLOB_RECURSE SOURCES src/*.c)

idf_component_register(
  SRCS
  ${SOURCES}
  INCLUDE_DIRS
  "include"
  REQUIRES
  driver
  mbedtls
  json
  nvs_flash
  bt
  EMBED_FILES
  "device_info.json"
  "onboarding_config.json")

set(stdk_config ${CMAKE_CURRENT_LIST_DIR}/stdkconfig.cmake)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/iotcore iotcore)
target_link_libraries(${COMPONENT_LIB} PUBLIC iotcore)
