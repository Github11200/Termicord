#ifndef HOME_H
#define HOME_H

#include "types.h"
#include "utils/input.h"
#include "utils/requests.h"
#include "windows/auth.h"
#include "windows/chat.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <set>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

namespace Home {

enum class InitOptions {
  LIST_SERVERS = 1,
  ADD_FRIEND = 2,
  CREATE_SERVER = 3,
  EXIT = 4
};

enum class ServerOperation { JOIN_SERVER = 1, ADD_USER = 2, CANCEL = 3 };

enum class NextStep { RETRY = 1, EXIT = 2 };

void init();
void listServers();
void createServer();
void selectedServer(std::string serverName);
void addFriend();

} // namespace Home

#endif
