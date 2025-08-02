#ifndef TYPES_H
#define TYPES_H

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct User {
  std::vector<std::string> friends;
  std::vector<std::string> servers;
  std::string password;
  std::string username;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(User, friends, servers, password, username);

struct Server {
  std::string owner;
  std::vector<std::string> users;
  int port;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Server, owner, users, port);

#endif
