#include "routing.h"

using namespace std;

Routing::Routing() {}

void Routing::addRoute(Route route) { this->routes[route.route] = route; }

Route Routing::getRoute(string route) { return this->routes.at(route); }

string Routing::getRouteNameFromHeader(string header) {
  /***************************************************************************************
  GET /random/route HTTP/1.1 <-- That is what an http header looks like, we
  split this into spaces and then get the second item which contains the route
  and use that to get the Route object.
  ***************************************************************************************/
  istringstream iss(header);
  vector<string> tokens;
  string word;

  while (iss >> word)
    tokens.push_back(word);

  return tokens[1];
}
