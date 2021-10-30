#include "base_controller.hpp"

#include <map>
#include <regex>
#include <string>
#include <cpprest/http_listener.h>
#include <cpprest/http_client.h>

#include "jwt_validator.hpp"

using namespace std;
using namespace web;
using namespace http;
using namespace client;
using namespace utility;
using namespace mstfcmrl;

mstfcmrl::base_controller::base_controller(string public_key_uri)
    :validator(public_key_uri)
{
}

void mstfcmrl::base_controller::resolve(const http_request& request, const string_t& trailing_uri)
{
    status_code code;
    vector<string_t> flatten_uri;
    pair<vector<string_t>, endpoint_parameters>* endp_pair;

    flatten_uri = get_flatten_uri(trailing_uri);
    endp_pair = find_endpoint(flatten_uri, request.method(), true);
    endp_pair = endp_pair ? endp_pair : find_endpoint(flatten_uri, request.method());

    if (!endp_pair)
    {
        return request.reply(status_codes::NotFound).get();
    }

    if (endp_pair->second.validate_jwt)
    {
        code = validate_token(request);
        if (status_codes::OK != code)
        {
            request.reply(code);
            return;
        }
    }

    return endp_pair->second.handler(endpoint_request(request, parse_url_parameters(endp_pair->first, flatten_uri)));
}

void mstfcmrl::base_controller::set_route(const string_t& uri,
                                          const http::method& method,
                                          std::function<void(endpoint_request request)> handler,
                                          bool validate_jwt)
{
    endpoint_mapper.push_back(make_pair(get_flatten_uri(uri), endpoint_parameters(uri, method, handler, validate_jwt)));
    sort(endpoint_mapper.begin(), endpoint_mapper.end(),
        [](const pair<vector<string_t>, endpoint_parameters> &a, const pair<vector<string_t>, endpoint_parameters> &b) -> bool
        {
            return a.first.size() > b.first.size();
        });
}

status_code mstfcmrl::base_controller::validate_token(const http_request& request)
{
    auto headers = request.headers();
    string token = headers[header_names::authorization];
    token.erase(0, 7);

    switch (validator.validate(token))
    {
        case jwt_validator::jwt_validator_error_codes::OK:
            return status_codes::OK;
            break;
        case jwt_validator::jwt_validator_error_codes::VERIFICATION_FALURE:
            return status_codes::Unauthorized;
            break;
    }

    return status_codes::InternalError;
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

pair<vector<string_t>, mstfcmrl::base_controller::endpoint_parameters>* mstfcmrl::base_controller::find_endpoint(vector<string_t> flatten_uri, const web::http::method& method, bool exact)
{
    for (pair<vector<string_t>, endpoint_parameters>& p : endpoint_mapper)
    {
        bool flag = true;

        if (p.first.size() > flatten_uri.size())
        {
            continue;
        }

        for (unsigned int i = 0; (i < p.first.size() && i < flatten_uri.size()); ++i)
        {
            if (if_requested_param(p.first[i]) && !exact)
            {
                continue;
            }

            if ( p.first[i].compare(flatten_uri[i]) )
            {
                flag = false;
                break;
            }
        }

        if ( flag && (p.second.method == method) )
        {
            return &p;
        }
    }

    return NULL;
}

map<string_t, string_t> mstfcmrl::base_controller::parse_url_parameters(const vector<string_t>& route_rule, const vector<string_t>& flatten_uri)
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
