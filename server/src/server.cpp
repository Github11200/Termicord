#include "servers.h"
#include "types.h"
#include "utils/requests.h"
#include <chrono>
#include <cpr/cpr.h>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <thread>
#include <utility>

using namespace std;
using json = nlohmann::json;

void signalCallbackFunction(int signum) {
  spdlog::info("Stopping all the servers...");
  Servers::running = false;
  Servers::stop();
  exit(signum);
}

int main(int argc, char *argv[]) {
  // This is if the user presses CTRL + C, then it will safely shutdown
  // everything
  signal(SIGINT, signalCallbackFunction);

  int PORT = atoi(argv[1]);
  Requests::defaultUrl = "http://localhost:" + to_string(PORT);

  Requests::Request request("/getAllServers", json::parse("{}"));
  Requests::APIResult result = Requests::sendRequest(request);

  // Get a list of all the servers with the name and the server data
  vector<pair<string, Server>> servers;
  for (auto &[key, value] : result.data.items()) {
    pair<string, Server> server;
    server.first = key;
    server.second = value.get<Server>();
    servers.push_back(server);
  }

  // Boot up each server
  for (auto server : servers)
    Servers::create(server.first, server.second.port);

  // Wait until the serves stop running
  while (Servers::running) {
  }
  Servers::stop();
  return 0;
}
