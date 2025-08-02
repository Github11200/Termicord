#ifndef SERVERS_H
#define SERVERS_H

#include "types.h"
#include <arpa/inet.h>
#include <atomic>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <set>
#include <spdlog/spdlog.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

namespace Servers {

extern std::unordered_map<std::string, std::thread *> runningServers;
extern std::set<int> connectedClientSockets;
extern std::set<int> connectedServerSockets;
extern std::mutex receivingThreadMutex;
extern std::vector<std::string> messages;
extern std::atomic<bool> running;

void broadcastMessage(std::string message, int excludedClient);
void receivingThreadCallback(int clientSocket);
void serverSocket(int PORT);
void create(std::string threadName, int PORT);
void stop();

} // namespace Servers

#endif
