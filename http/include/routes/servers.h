#ifndef GET_SERVER_H
#define GET_SERVER_H

#include "database/database.h"
#include "types.h"
#include <any>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <spdlog/spdlog.h>
#include <string>
#include <uuid/uuid.h>
#include <vector>

Result getServer(std::string rawJson);
Result getAllServers(std::string rawJson);
Result addServer(std::string rawJson);
Result addUserToServer(std::string rawJson);

#endif
