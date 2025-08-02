#include "windows/chat.h"

using namespace std;
using json = nlohmann::json;

// The origninal terminal settings
termios originalTermios;

// Reset the terminal settings and show the cursor
void disableRawMode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTermios); }

void enableRawMode() {
  tcgetattr(STDIN_FILENO,
            &originalTermios); // Store the current terminal settings
  atexit(disableRawMode); // It will call that function to clean up everything
                          // and disable raw mode

  termios raw = originalTermios;
  raw.c_lflag &=
      ~(ECHO | ICANON); // Doens't echo input characters and it doesn't wait for
                        // a new line because of ICANON
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); // Apply these settings
}

// Move the cursor to some position in the terminal
void moveCursor(int x, int y) { cout << "\033[" << y << ";" << x << "H"; }

namespace Chat {

vector<string> messages;
mutex mtx;
atomic<bool> running(true);

void redraw(string &input) {
  // Get the number of rows in the window
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  int row = w.ws_row - 1;

  mtx.lock();
  system("clear");

  // Go bottom up, so get the very last message and keep on printing them out
  // until you either run out of messages to print or run out of space to print
  // them
  for (int i = messages.size() - 1; row > 0 && i >= 0; --i) {
    moveCursor(1, row--);
    cout << messages[i];
  }

  // Move the cursor to the bottom and print out "> " for the next message
  moveCursor(1, w.ws_row);
  cout << "> " << input << flush;
  moveCursor(3 + input.size(), w.ws_row);
  mtx.unlock();
}

// This will continously listen for messages from the server in case someone
// else sends one
void receive(int socket, string &input) {
  while (Chat::running) {
    char buffer[1024] = {0};
    int bytesRecieved = recv(socket, buffer, sizeof(buffer), 0);

    // If the message isn't empty
    if (bytesRecieved > 0) {
      buffer[bytesRecieved] = '\0';

      // Sometimes TCP will bunch together messages and in that case just split
      // up with '\n' so you can process each on separately
      vector<string> seperatedMessageStrings =
          splitString(string(buffer), '\n');
      for (int i = 0; i < seperatedMessageStrings.size(); ++i) {
        string messageString = seperatedMessageStrings[i];

        // If the username sent is the same as the current user's then just
        // change it to [You] ex. "[Github11200] Hello" --> "[You] Hello",
        // assuming the current username is Github11200
        string messageUsername = "";
        int j = 1;
        for (; messageString[j] != ']'; ++j)
          messageUsername.push_back(messageString[j]);
        if (messageUsername == Auth::currentUser.username)
          messageString = "[You] " + messageString.substr(
                                         j + 2, messageString.size() - (j + 2));

        mtx.lock();
        messages.push_back(messageString);
        mtx.unlock();
        redraw(input);
      }
    }
  }
}

void join(int PORT) {
  system("clear");

  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(PORT);
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

  // Connect to the server
  if (connect(clientSocket, (struct sockaddr *)&serverAddress,
              sizeof(serverAddress)) == -1) {
    spdlog::error("Could not connect to the client.");
    return;
  }

  enableRawMode();

  string input = "";
  redraw(input);

  Chat::running = true;
  thread receivingThread(receive, clientSocket,
                         ref(input)); // Start the receiving thread to listen
                                      // for messages from other clients
  while (Chat::running) {
    char c;
    if (read(STDIN_FILENO, &c, 1) <= 0)
      break;

    // If the user presses enter
    if (c == '\n' && !input.empty()) {
      // If the user wants to exit then shutdown the client socket so the
      // thread can be joined
      if (input == "/exit") {
        Chat::running = false;
        shutdown(clientSocket, SHUT_RDWR);
        break;
      }

      // Append the username to the start
      string inputToBeSent = "[" + Auth::currentUser.username + "] " + input;
      ssize_t sent =
          send(clientSocket, inputToBeSent.data(), inputToBeSent.size(), 0);
      if (sent < 0) {
        spdlog::error("Could not send.");
        break;
      }

      mtx.lock();
      messages.push_back("[You] " + input);
      mtx.unlock();
      input = "";
      redraw(input);
    } else if ((c == 8 || c == 127) &&
               !input.empty()) { // They press backspace or delete
      input.pop_back();
      redraw(input);
    } else if (isprint(c)) { // If the user presses any other character then
                             // just update the string
      input += c;
      redraw(input);
    }
  }

  // Shutdown shenanigans
  disableRawMode();
  if (receivingThread.joinable())
    receivingThread.join();
  close(clientSocket);
  moveCursor(0, 0);
  system("clear");
}

} // namespace Chat
