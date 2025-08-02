find_package(spdlog CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)

add_executable(client
  src/client.cpp
  src/utils/input.cpp
  src/utils/requests.cpp
  src/utils/utils.cpp
  src/windows/auth.cpp
  src/windows/home.cpp
  src/windows/chat.cpp
)

target_link_libraries(client PRIVATE spdlog::spdlog cpr::cpr nlohmann_json::nlohmann_json ${CURSES_LIBRARIES})
target_compile_options(client PRIVATE ${CURSES_CFLAGS})

target_include_directories(client
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
          ${CURSES_INCLUDE_DIRS}
)
