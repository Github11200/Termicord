find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-libuuid CONFIG REQUIRED)

add_executable(http
  src/http.cpp
  src/routing.cpp
  src/database/database.cpp
  src/routes/users.cpp
  src/routes/servers.cpp
)
target_link_libraries(http PRIVATE spdlog::spdlog unofficial::UUID::uuid)
target_include_directories(http
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

