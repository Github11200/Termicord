#ifndef REQUESTS_H
#define REQUESTS_H

#include <cpr/cpr.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>

namespace Requests {

extern std::string defaultUrl;

enum ResponseCode {
  SUCCESS = 200,
  NOT_FOUND = 404,
  INTERNAL_SERVER_ERROR = 500
};

struct APIResult {
  nlohmann::json data;
  ResponseCode code;

  APIResult() = default;
  APIResult(nlohmann::json data, ResponseCode code) : data(data), code(code) {}
};

struct Request {
  std::string url;
  std::string route;
  nlohmann::json body;

  Request() = default;
  Request(std::string route, nlohmann::json body)
      : url(url), route(route), body(body) {
    this->url = Requests::defaultUrl;
  }
};

APIResult sendRequest(Request requestObject);

} // namespace Requests

#endif
