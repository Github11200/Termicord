#include "utils/requests.h"
#include "windows/auth.h"
#include "windows/home.h"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>

using namespace std;
using json = nlohmann::json;

int main(int argc, char *argv[]) {
  curl_global_init(CURL_GLOBAL_DEFAULT);
  int PORT = atoi(argv[1]);
  Requests::defaultUrl = "http://localhost:" + to_string(PORT);

  Auth::init();
  Home::init();

  curl_global_cleanup();

  return 0;
}
