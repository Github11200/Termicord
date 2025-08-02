#include "servers.h"
#include <sys/socket.h>

using namespace std;

namespace Servers {

unordered_map<string, thread *> runningServers;
mutex receivingThreadMutex;
set<int> connectedClientSockets;
set<int> connectedServerSockets;
vector<string> messages;
atomic<bool> running(true);

// This will take a message and send it to all the connected clients excluding
// the one that initially sent it
void broadcastMessage(string message, int excludedClient) {
  messages.push_back(message);
  for (int client : connectedClientSockets) {
    if (client != excludedClient) {
      ssize_t sent = send(client, message.data(), message.size(), 0);
      if (sent < 0)
        spdlog::error("Could not send.");
    }
  }
}

void receivingThreadCallback(int clientSocket) {
  char buffer[1024] = {0};
  string username = "";

  // The moment the client connects send them the latest messages so far, so if
  // 2 clients were talking and a third joins then send it all the messages from
  // the first 2 clients
  for (const string &message : messages) {
    spdlog::info("Sending the following message to the client, {}", message);
    string messageToBeSent = message + "\n";
    size_t sent =
        send(clientSocket, messageToBeSent.data(), messageToBeSent.size(), 0);
    if (sent < 0)
      spdlog::error("Could not send the initial messages");
  }

  // Just receive messages and continously broadcast them
  while (Servers::running) {
    int bytesRecieved = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRecieved <= 0) {
      spdlog::info("The client disconnected.");
      connectedClientSockets.erase(clientSocket);
      break;
    }
    buffer[bytesRecieved] = '\0';
    spdlog::info("Message from the client: {}", buffer);

    receivingThreadMutex.lock();
    broadcastMessage(string(buffer), clientSocket);
    receivingThreadMutex.unlock();
  }
}

void serverSocket(int PORT) {
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    spdlog::error("Failed to create the socket.");
    return;
  }

  spdlog::info("Created the socket.");

  // Prepare the server address
  sockaddr_in serverAddress;
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(PORT);

  // Make the port reusable so you don't have to change it every time you run
  // the server
  int yes = 1;
  setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  // Bind the socket to this address
  if (bind(serverSocket, (sockaddr *)&serverAddress, sizeof(serverAddress)) ==
      -1) {
    spdlog::error("Failed to bind the server to the server address.");
    return;
  }

  spdlog::info("Binded the socket to the server address.");

  socklen_t addressLength = sizeof(serverAddress);

  if (getsockname(serverSocket, (struct sockaddr *)&serverAddress,
                  &addressLength) == -1) {
    spdlog::error("whoops...");
  } else {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(serverAddress.sin_addr), ip, sizeof(ip));
    int port = ntohs(serverAddress.sin_port);

    spdlog::info("IP address: {}", ip);
    spdlog::info("Port: {}", port);
  }

  // Start listening for incoming connections
  if (listen(serverSocket, 5) == -1) {
    spdlog::error("Failed to listen on the socket.");
    return;
  }

  // Add this server socket to the list of the connected ones
  connectedServerSockets.insert(serverSocket);
  vector<thread *> clientThreads;

  while (Servers::running) {
    sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);
    int clientSocket =
        accept(serverSocket, (sockaddr *)&clientAddress, &clientAddressLen);
    if (clientSocket == -1) {
      if (Servers::running)
        spdlog::error("Failed to accept the connection.");
      return;
    }

    // Create a seperate thread for each new client that joins
    thread *clientThread = new thread(receivingThreadCallback, clientSocket);
    clientThreads.push_back(clientThread);
    connectedClientSockets.insert(clientSocket);
    spdlog::info("Accepted the connection from {} : {}.",
                 inet_ntoa(clientAddress.sin_addr),
                 ntohs(clientAddress.sin_port));
  }

  close(serverSocket);
}

void create(string threadName, int PORT) {
  // Create a seperate thread for each running server
  thread *serverSocketThread = new thread(serverSocket, PORT);
  runningServers[threadName] = serverSocketThread;
}

void stop() {
  Servers::running = false;

  // Shutdown all the client sockets
  for (const auto &runningSocket : connectedClientSockets)
    shutdown(runningSocket, SHUT_RDWR);

  // Shutdown all the server sockets
  for (const auto &runningSocket : connectedServerSockets)
    shutdown(runningSocket, SHUT_RDWR);

  // Now that all the sockets are shutdown the threads can be stopped and joined
  // back into the main thread
  for (const auto &server : runningServers) {
    spdlog::info("Shutting down {}", server.first);
    server.second->join();
    delete server.second;
  }
}

} // namespace Servers
