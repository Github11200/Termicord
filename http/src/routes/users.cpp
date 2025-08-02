#include "routes/users.h"

using namespace std;
using json = nlohmann::json;

const string NAME = "users";

Result getUser(string rawJson) {
  Database database(NAME);
  json httpRequest = json::parse(rawJson);

  optional<string> searchResult =
      database.searchDatabase(httpRequest["username"].get<string>());
  if (searchResult)
    return Result(200, *searchResult);

  spdlog::error("Could not find the requested user in the database");
  return Result(
      500,
      "{ \"message\": \"Error, could not find the user in the database.\" }");
}

Result getAllUsers(string rawJson) {
  Database database(NAME);
  return Result(200, database.getAllJsonData());
}

Result addUser(string rawJson) {
  Database database(NAME);
  json httpRequest = json::parse(rawJson);

  string key = httpRequest["username"].get<string>();

  // Remove the username key and then pass the rest of the json object
  if (database.addEntry(key, httpRequest))
    return Result(200, "{ \"message\": \"Added the user to the database\" }");
  return Result(500, "{ \"message\": \"The user already exists\" }");
}

Result addFriend(string rawJson) {
  Database database(NAME);
  json httpRequest = json::parse(rawJson);

  string username = httpRequest["username"];
  string friendUsername = httpRequest["friend"];

  // Check if that friend exists in the database
  optional<string> searchResult = database.searchDatabase(friendUsername);
  if (!searchResult)
    return Result(500, "{ \"message\": \"The friend's username does not exist "
                       "in the database.\" }");

  optional<string> result = database.searchDatabase(username);
  json currentUserJson = json::parse(*result);

  // Add it to the list of friends
  currentUserJson["friends"].push_back(friendUsername);
  if (database.updateEntry(username, currentUserJson))
    return Result(200, "{ \"message\": \"The friend was successfully added to "
                       "the database.\" }");
  return Result(
      500,
      "{ \"message\": \"The friend could not be added to the database.\" }");
}

Result addServerToUser(string rawJson) {
  Database database(NAME);
  json httpRequest = json::parse(rawJson);

  string username = httpRequest["username"];
  string serverName = httpRequest["server_name"];

  optional<string> result = database.searchDatabase(username);
  json currentUserJson = json::parse(*result);

  // Add the server name to the user's list of servers
  currentUserJson["servers"].push_back(serverName);
  if (database.updateEntry(username, currentUserJson))
    return Result(200, "{ \"message\": \"The server was successfully added to "
                       "the user's list of servers.\" }");
  return Result(500, "{ \"message\": \"The server could not be added to the "
                     "user's list of servers.\" }");
}
