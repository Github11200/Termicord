#include "routes/servers.h"

using namespace std;
using json = nlohmann::json;

const string NAME = "servers";

Result getServer(string rawJson) {
  Database database(NAME);
  json httpRequest = json::parse(rawJson);

  // Search the database for the name and return that
  optional<string> searchResult =
      database.searchDatabase(httpRequest["server_name"].get<string>());
  if (searchResult)
    return Result(200, *searchResult);

  spdlog::error("Could not find the requested server in the database");
  return Result(
      500,
      "{ \"message\": \"Error, could not find the server in the database.\" }");
}

Result getAllServers(string rawJson) {
  Database database(NAME);
  return Result(200, database.getAllJsonData());
}

Result addServer(string rawJson) {
  Database database(NAME);
  json httpRequest = json::parse(rawJson);

  string key = httpRequest["server_name"].get<string>();

  // Remove the server_name property as that is the key but not a part of the
  // values
  httpRequest.erase("server_name");
  if (database.addEntry(key, httpRequest))
    return Result(200, "{ \"message\": \"Added the server to the database\" }");
  return Result(500, "{ \"message\": \"The server already exists\" }");
}

Result addUserToServer(std::string rawJson) {
  Database database(NAME);
  json httpRequest = json::parse(rawJson);

  string username = httpRequest["username"];
  string serverName = httpRequest["server_name"];

  // Get the server
  optional<string> searchResult = database.searchDatabase(serverName);
  if (!searchResult)
    return Result(500, "{ \"message\": \"The server does not exist in the "
                       "database, so the user could not be added.\" }");

  // Add the user to the list of users in that particular server
  json serverJson = json::parse(*searchResult);
  serverJson["users"].push_back(username);

  if (database.updateEntry(serverName, serverJson))
    return Result(200, "{ \"message\": \"Added the user to the server\" }");
  return Result(500,
                "{ \"message\": \"Could not add the user to the server.\" }");
}
