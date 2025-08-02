#ifndef CHAT_H
#define CHAT_H

#include "types.h"
#include "utils/input.h"
#include "utils/requests.h"
#include "utils/utils.h"
#include "windows/auth.h"
#include "windows/home.h"
#include <arpa/inet.h>
#include <asm-generic/ioctls.h>
#include <atomic>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <curses.h>
#include <functional>
#include <iostream>
#include <mutex>
#include <ncurses.h>
#include <netinet/in.h>
#include <nlohmann/json.hpp>
#include <ostream>
#include <spdlog/spdlog.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <term.h>
#include <termios.h>
#include <thread>
#include <unistd.h>
#include <vector>

void disableRawMode();
void enableRawMode();
void moveCursor(int x, int y);

namespace Chat {

extern std::vector<std::string> messages;
extern std::mutex mtx;
extern std::atomic<bool> running;

void receive(int socket, string &input);
void redraw();
void join(int PORT);

} // namespace Chat

#endif
