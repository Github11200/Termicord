find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)

add_executable(server
  src/server.cpp
  src/servers.cpp
  src/utils/requests.cpp
)
target_link_libraries(server PRIVATE spdlog::spdlog nlohmann_json::nlohmann_json cpr::cpr)
target_include_directories(server
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

