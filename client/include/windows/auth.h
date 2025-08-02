#ifndef AUTH_H
#define AUTH_H

#include "types.h"
#include "utils/input.h"
#include "utils/requests.h"
#include <cpr/cpr.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

namespace Auth {

extern User currentUser;

enum class AuthType { LOGIN = 1, SIGN_UP = 2 };

enum class NextStep { RETRY = 1, EXIT = 2 };

void init();
void login();
void signUp();

} // namespace Auth

#endif
