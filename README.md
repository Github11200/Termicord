# Termicord
Discord in the terminal using raw sockets in C++.

# Features
- Logging in or signing up a new user
- Creating a server
- Joining servers
- Adding friends
- Adding friends to servers
- Chatting in a server

## How it works
There's 3 main components:
- **Server**:
  - Allows several clients to connect to it and send messages to each other
  - When it detects a message from one client it broadcasts it to several others
  - It boots up all the servers listed in the `servers` database so that users that are a part of that server can join it via a socket
- **HTTP Server**:
  - This creates a REST API so the server and client can interact with the database
  - It manages a json database where it reads and writes to json files
  - It routes requests to different functions to do something like write to the database or get some information 
- **Client**:
  - This createa TUI for the user to interact with
  - It sends requests to the HTTP server for getting this like user and server information
  - It allows users to perform operations such as creating servers, adding friends, and joining servers

Here's an [Excalidraw](https://excalidraw.com/#json=MWIcWPlZGN-EEXpAuesvz,c8Wu6_lDCktxkx7Pkgiuhg) document that has flowcharts for each of the components:
<img width="1660" height="1059" alt="image" src="https://github.com/user-attachments/assets/36952f69-ab7e-4a86-9f43-a49d4ed28eb2" />

## Libraries/Technologies used
- [Vcpkg](https://vcpkg.io/en/) for packagement
- [CPR](https://docs.libcpr.org/) for curl requests in C++
- [Spdlog](https://github.com/gabime/spdlog) for logging
- [JSON for Modern C++](https://json.nlohmann.me/) for parsing and stringifying JSON objects
- [NCurses](https://invisible-island.net/ncurses/announce.html) for the chat UI on the client side
- [CMake](https://cmake.org/) for building the project

## Questions/Feedback
Feel free to reach out to me via any of the contact methods below:
- [Email](mailto:jinayunity22@gmail.com)
- [Linkedin](https://www.linkedin.com/in/jinay-patel-6369002b4/)
