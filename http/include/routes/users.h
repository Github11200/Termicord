#ifndef GET_USER_H
#define GET_USER_H

#include "database/database.h"
#include "types.h"
// #include "utils.h"
#include <any>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <spdlog/spdlog.h>
#include <string>
#include <uuid/uuid.h>
#include <vector>

Result getUser(std::string rawJson);
Result getAllUsers(std::string rawJson);
Result addUser(std::string rawJson);
Result addFriend(std::string rawJson);
Result addServerToUser(std::string rawJson);

#endif
