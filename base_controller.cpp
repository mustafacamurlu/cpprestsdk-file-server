#include "base_controller.hpp"

#include <map>
#include <regex>
#include <cpprest/http_listener.h>

using namespace std;
using namespace web;
using namespace http;
using namespace utility;


void mstfcmrl::base_controller::resolve(const http_request& request, const string_t& trailing_uri)
{
    vector<string_t> flatten_uri = get_flatten_uri(trailing_uri);

    for (pair<vector<string_t>, controller_param>& p : controller_mapper)
    {
        bool flag = true;

        if (p.first.size() > flatten_uri.size())
        {
            continue;
        }

        for (unsigned int i = 0; (i < p.first.size() && i < flatten_uri.size()); ++i)
        {
            if (if_requested_param(p.first[i]))
            {
                continue;
            }

            if ( p.first[i].compare(flatten_uri[i]) )
            {
                flag = false;
                break;
            }
        }

        if ( flag && (p.second.method == request.method()) )
        {
            return p.second.handler(controller_request(request, parse_url_parametesr(p.first, flatten_uri)));
        }
    }

    request.reply(status_codes::NotFound);
}

void mstfcmrl::base_controller::set_route(const string_t& uri, const http::method& method, std::function<void(controller_request request)> handler)
{
    controller_mapper.push_back(make_pair(get_flatten_uri(uri), controller_param(uri, method, handler)));
    sort(controller_mapper.begin(), controller_mapper.end(), 
        [](const pair<vector<string_t>, controller_param> &a, const pair<vector<string_t>, controller_param> &b) -> bool
        { 
            return a.first.size() > b.first.size(); 
        });
}

inline bool mstfcmrl::base_controller::if_requested_param(const string_t& param)
{
    return (param.size() > 2) && (param.front() == '{') && (param.back() == '}');
}

vector<string_t> mstfcmrl::base_controller::get_flatten_uri(const string_t& uri)
{
    std::regex route_regex("/");
    vector<string_t> routes = vector<string>(std::sregex_token_iterator(uri.begin(), uri.end(), route_regex, -1), std::sregex_token_iterator());
    routes.erase(routes.begin());
    return routes;
}

map<string_t, string_t> mstfcmrl::base_controller::parse_url_parametesr(const vector<string_t>& route_rule, const vector<string_t>& flatten_uri)
{
    map<string_t, string_t> r_map;
    for (unsigned int i = 0; i < route_rule.size(); ++i)
    {
        if (if_requested_param(route_rule[i]))
        {
            r_map.insert({route_rule[i].substr(1, route_rule[i].size() - 2), flatten_uri[i]});
        }
    }

    return r_map;
}
