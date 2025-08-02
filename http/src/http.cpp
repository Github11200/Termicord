#include "routes/servers.h"
#include "routes/users.h"
#include "routing.h"
#include "types.h"
#include <arpa/inet.h>
#include <atomic>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <spdlog/spdlog.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <uuid/uuid.h>

using namespace std;

atomic<bool> running(true);
int serverSocket = 0;

int startServerSocket(int PORT) {
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    spdlog::error("Failed to create the socket.");
    return -1;
  }

  spdlog::info("Created the socket on port {}", PORT);

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
    return -1;
  }

  spdlog::info("Binded the socket to the server address.");

  // Start listening for incoming connections
  if (listen(serverSocket, 5) == -1) {
    spdlog::error("Failed to listen on the socket.");
    return -1;
  }

  spdlog::info("Server listening on port {}.", PORT);
  return serverSocket;
}

string readFromSocket(int clientSocket) {
  string clientMessage(4096, '\0');
  ssize_t bytesRead =
      read(clientSocket, clientMessage.data(), clientMessage.size());

  if (bytesRead > 0)
    clientMessage.resize(bytesRead);
  else if (bytesRead == 0)
    spdlog::info("Client closed connection");
  else
    spdlog::error("Read error");

  return clientMessage;
}

void signalCallbackFunction(int signum) {
  spdlog::info("Stopping the servers...");
  running = false;
  shutdown(serverSocket, SHUT_RDWR);
  exit(signum);
}

int main(int argc, char *argv[]) {
  int PORT = atoi(argv[1]);

  // Add all the routes
  Routing routing;
  routing.addRoute(Route("/getUser", getUser));
  routing.addRoute(Route("/getAllUsers", getAllUsers));
  routing.addRoute(Route("/addUser", addUser));
  routing.addRoute(Route("/addFriend", addFriend));
  routing.addRoute(Route("/addServerToUser", addServerToUser));

  routing.addRoute(Route("/getServer", getServer));
  routing.addRoute(Route("/getAllServers", getAllServers));
  routing.addRoute(Route("/addServer", addServer));
  routing.addRoute(Route("/addUserToServer", addUserToServer));

  serverSocket = startServerSocket(PORT);

  sockaddr_in clientAddress;
  socklen_t clientAddressLen = sizeof(clientAddress);
  int clientSocket = 0;

  // This is if the user presses CTRL + C to exit, then it will shutdown
  // everything
  signal(SIGINT, signalCallbackFunction);
  while (running) {
    clientSocket = accept(serverSocket, nullptr, nullptr);

    if (clientSocket == -1) {
      spdlog::error("Failed to accept the connection.");
      return -1;
    }

    // Split the message up
    string clientMessage = readFromSocket(clientSocket);
    string header = clientMessage.substr(0, clientMessage.find('\n'));
    string method = clientMessage.substr(0, clientMessage.find(' '));
    string json =
        clientMessage.substr(clientMessage.find('{') - 1, clientMessage.size());

    spdlog::info("Request for {}", header);

    // Get the route that the user is requesting and call the callback provided
    // for that route
    Result callbackResult =
        routing.getRoute(routing.getRouteNameFromHeader(header)).callback(json);
    string httpHeader = "";
    string message = "";

    // Send back a response code
    if (callbackResult.responseCode == 200) {
      httpHeader = "HTTP/1.1 200 OK\r\n\r\n";
      message = callbackResult.json;
    } else if (callbackResult.responseCode == 500) {
      httpHeader = "HTTP/1.1 500 Interal Server Error\r\n\r\n";
      message = callbackResult.json;
    }
    httpHeader += message;

    send(clientSocket, httpHeader.c_str(), httpHeader.size(), 0);
    close(clientSocket);
  }

  close(serverSocket);
  return 0;
}
