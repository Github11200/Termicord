#include "utils/requests.h"

using namespace std;
using json = nlohmann::json;

namespace Requests {

string defaultUrl;

APIResult sendRequest(Request requestObject) {
  cpr::Response r = cpr::Get(cpr::Url{requestObject.url + requestObject.route},
                             cpr::Header{{"Content-Type", "application/json"}},
                             cpr::Body{requestObject.body.dump()});

  return APIResult(json::parse(r.text),
                   static_cast<ResponseCode>(r.status_code));
}

} // namespace Requests
