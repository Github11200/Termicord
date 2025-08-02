#ifndef ROUTING_H
#define ROUTING_H

#include "types.h"
#include <any>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct Route {
  std::string route;
  std::function<Result(const std::string &rawJson)> callback;

  Route() = default;
  Route(std::string route,
        std::function<Result(const std::string &rawJson)> callback)
      : route(route), callback(callback) {}
};

class Routing {
private:
  std::unordered_map<std::string, Route> routes;

public:
  Routing();
  void addRoute(Route route);
  Route getRoute(std::string route);
  std::string getRouteNameFromHeader(std::string header);
};

#endif
