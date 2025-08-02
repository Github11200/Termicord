#include "windows/home.h"
#include "utils/input.h"
#include "utils/requests.h"
#include "utils/utils.h"
#include "windows/auth.h"
#include <unistd.h>

using namespace std;
using json = nlohmann::json;

namespace Home {

void init() {
  while (true) {
    system("clear");
    cout << "Welcome " << Auth::currentUser.username << "!\n\n";

    InitOptions initOption;
    Input::getOption<InitOptions>(
        "What would you like to do?",
        vector<string>(
            {"List all servers", "Add a friend", "Create a server", "Exit"}),
        initOption);
    if (initOption == InitOptions::LIST_SERVERS)
      listServers();
    else if (initOption == InitOptions::ADD_FRIEND)
      addFriend();
    else if (initOption == InitOptions::CREATE_SERVER)
      createServer();
    else if (initOption == InitOptions::EXIT) {
      cout << "Goodbye!" << endl;
      return;
    }
  }
}

void createServer() {
  // Get all the ports that are in use so far
  cpr::Response getAllServersResponse =
      cpr::Post(cpr::Url{"http://localhost:8000/getAllServers"},
                cpr::Header{{"Content-Type", "application/json"}},
                cpr::Body{json::parse("{}").dump()});
  json allServersJson = json::parse(getAllServersResponse.text);

  set<int> ports;
  for (auto &[key, value] : allServersJson.items()) {
    Server server;
    server = value.get<Server>();
    ports.insert(server.port);
  }

  while (true) {
    system("clear");

    string serverName = "";
    Input::getStringInput("What would you like the name of the server to be? ",
                          serverName);

    // Continue choosing a port number until it finds one that isn't in use
    int port = 0;
    while (true) {
      port = randomInteger(1024, 65535);
      if (ports.count(port) == 0)
        break;
    }

    // Add the server to the database
    json addServerJson = {
        {"server_name", serverName},
        {"owner", Auth::currentUser.username},
        {"port", port},
        {"users", vector<string>{Auth::currentUser.username}}};
    json updateUserServerListJson = {{"server_name", serverName},
                                     {"username", Auth::currentUser.username}};

    Requests::Request addServerRequest("/addServer", addServerJson);
    Requests::APIResult addServerResult =
        Requests::sendRequest(addServerRequest);

    // Add the server to the user's list of servers so they can actually access
    // it
    Requests::Request updateUserServerListRequest("/addServerToUser",
                                                  updateUserServerListJson);
    Requests::APIResult updateUserServerListResponse =
        Requests::sendRequest(updateUserServerListRequest);

    if (addServerResult.code == Requests::ResponseCode::SUCCESS &&
        updateUserServerListResponse.code == Requests::ResponseCode::SUCCESS) {
      json getUserJson = {{"username", Auth::currentUser.username}};
      Requests::Request getUserRequest("/getUser", getUserJson);
      Requests::APIResult getUserResult = Requests::sendRequest(getUserRequest);

      // Update the user again to get a refreshed list of servers that they're a
      // part of
      Auth::currentUser = getUserResult.data.get<User>();

      cout << "The server was added successfully. Please press enter to "
              "continue";
      cin.ignore();
      cin.get();
      return;
    } else {
      NextStep nextStep;
      Input::getOption<NextStep>(
          "Could not add the server. What would you like to do?",
          vector<string>{"Try again", "Exit"}, nextStep);
      if (nextStep == NextStep::RETRY)
        continue;
      else if (nextStep == NextStep::EXIT)
        return;
    }
  }
}

void listServers() {
  while (true) {
    system("clear");

    // Display all the servers
    vector<string> servers;
    for (int i = 0; i < Auth::currentUser.servers.size(); ++i) {
      string currentServer = Auth::currentUser.servers[i];
      cout << "(" << i + 1 << ") " << currentServer << endl;
      servers.push_back(currentServer);
    }

    int serverIndex = 0;
    Input::getBoundedNumericInput<int>(
        "Which server would you like to join and/or modify (1 - " +
            to_string(servers.size()) + "). Input 0 to exit: ",
        serverIndex, 0, servers.size());
    if (serverIndex == 0)
      return;
    selectedServer(servers[serverIndex - 1]);
  }
}

void selectedServer(string serverName) {
  system("clear");

  // Get the specific server's information
  json j = {{"server_name", serverName}};
  Requests::Request request("/getServer", j);
  Requests::APIResult result = Requests::sendRequest(request);

  if (result.code == Requests::ResponseCode::SUCCESS) {
    while (true) {
      Server server = result.data.get<Server>();
      ServerOperation serverOperation;
      Input::getOption<ServerOperation>(
          "What would you like to do with the following server: " + serverName,
          vector<string>({"Join server", "Add user", "Cancel"}),
          serverOperation);

      if (serverOperation == ServerOperation::JOIN_SERVER) {
        Chat::join(server.port);
      } else if (serverOperation == ServerOperation::ADD_USER) {
        system("clear");

        // The username can only add users to the server if they are the owner
        if (server.owner != Auth::currentUser.username) {
          cout << "You are not the owner of this server and cannot add users. "
                  "Please press enter to continue"
               << endl;
          cin.get();
          continue;
        }
        string username = "";
        Input::getStringInput(
            "What is the username of the person you'd like to add? ", username);

        json addUserJson = {{"server_name", serverName},
                            {"username", username}};

        // Update the servers and users database with the new information
        cpr::Response addUserToServerResponse =
            cpr::Post(cpr::Url{"http://localhost:8000/addUserToServer"},
                      cpr::Header{{"Content-Type", "application/json"}},
                      cpr::Body{addUserJson.dump()});
        cpr::Response addServerToUserResponse =
            cpr::Post(cpr::Url{"http://localhost:8000/addServerToUser"},
                      cpr::Header{{"Content-Type", "application/json"}},
                      cpr::Body{addUserJson.dump()});

        system("clear");
        if (addUserToServerResponse.status_code ==
                Requests::ResponseCode::SUCCESS &&
            addServerToUserResponse.status_code ==
                Requests::ResponseCode::SUCCESS)
          cout << "The user was added successfully! Please press enter to "
                  "continue";
        else
          cout << "There was an error adding the user. Please press enter to "
                  "continue";

        cin.ignore();
        cin.get();
      } else if (serverOperation == ServerOperation::CANCEL) {
        return;
      }
    }
  } else if (result.code == Requests::ResponseCode::INTERNAL_SERVER_ERROR) {
    cout << "Could fetch the server :(" << endl;
  }
}

void addFriend() {
  system("clear");

  // Get the username
  string friendUsername = "";
  Input::getStringInput("What is the username of the friend? ", friendUsername);

  // Update the database
  json j = {{"username", Auth::currentUser.username},
            {"friend", friendUsername}};
  Requests::Request request("/addFriend", j);
  Requests::APIResult result = Requests::sendRequest(request);

  system("clear");
  if (result.code == Requests::ResponseCode::SUCCESS)
    cout << "Your new friend is ready to chat with :). Please press enter to "
            "continue.";
  else if (result.code == Requests::ResponseCode::INTERNAL_SERVER_ERROR)
    cout << "There was an error adding this friend. It's okay though, you can "
            "still talk in real life :). Please press enter to continue.";
  cin.ignore();
  cin.get();
}

} // namespace Home
