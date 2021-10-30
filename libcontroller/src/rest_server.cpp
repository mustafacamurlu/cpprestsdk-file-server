#include "rest_server.hpp"

#include <regex>
#include <cpprest/http_listener.h>

#include "base_controller.hpp"

using namespace std;
using namespace mstfcmrl;
using namespace utility;
using namespace web::http::experimental::listener;

mstfcmrl::rest_server::rest_server(uri uri)
{
    listener = http_listener(uri);
    listener.support(bind(&rest_server::router, this, placeholders::_1));
}

void mstfcmrl::rest_server::register_route(const string_t &route, base_controller& controller)
{
    controller.endpoint_mapping();
    route_map.insert({route, controller});
}

void mstfcmrl::rest_server::router(http_request request)
{
    vector<string> routes;
    string route = string("");
    std::regex route_regex("/");
    string_t request_uri = uri::decode(request.relative_uri().path());

    for (string &s: vector<string>(std::sregex_token_iterator(request_uri.begin(), request_uri.end(), route_regex, -1), std::sregex_token_iterator()))
    {
        route += (route.size() == 1 ? "" : "/") + s;
        routes.push_back(route);
    }

    for (vector<string>::reverse_iterator i = routes.rbegin(); i != routes.rend(); ++i )
    {
        auto controller_it = route_map.find(*i);

        if ( controller_it != route_map.end() )
        {
            return controller_it->second.resolve(request, request_uri.substr(i->size()));
        }
    }

    request.reply(status_codes::NotFound);
    return;
}

pplx::task<void> mstfcmrl::rest_server::start()
{
    return listener.open();
}

pplx::task<void> mstfcmrl::rest_server::shutdown()
{
    return listener.close();
}
