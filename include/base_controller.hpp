#ifndef _BASE_CONTROLLER_H_
#define _BASE_CONTROLLER_H_

#include <map>
#include <string>
#include <cpprest/http_listener.h>

#include "jwt_validator.hpp"

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace mstfcmrl;

namespace mstfcmrl
{
    struct endpoint_request
    {
        http_request base_request;
        map<string_t, string_t> url_parameters;

        endpoint_request(const http_request& req, const map<string_t, string_t> url_params)
            :base_request(req), url_parameters(url_params)
            {}
    };

    class base_controller
    {
        public:
            base_controller(string public_key_uri = string(""));

            virtual void endpoint_mapping() = 0;

            virtual void resolve(const http_request& request, const string_t& trailing_uri) final;

            virtual void set_route(const string_t& uri,
                                   const http::method& method,
                                   std::function<void(endpoint_request request)> handler,
                                   bool validate_jwt = false) final;

        private:
            struct endpoint_parameters
            {
                string_t orig_uri;
                bool validate_jwt;
                http::method method;
                std::function<void(endpoint_request request)> handler;

                endpoint_parameters(const string_t& uri, const http::method& method, std::function<void(endpoint_request request)> handler, bool validate_jwt)
                    :orig_uri(uri), method(method), handler(handler), validate_jwt(validate_jwt)
                {}

                endpoint_parameters& operator=(const endpoint_parameters& endpoint_param)
                {
                    this->orig_uri = endpoint_param.orig_uri;
                    this->method = endpoint_param.method;
                    this->handler = endpoint_param.handler;
                    this->validate_jwt = endpoint_param.validate_jwt;
                    return *this;
                }
            };

            jwt_validator validator;
            vector<pair<vector<string_t>, endpoint_parameters>> endpoint_mapper;

            status_code validate_token(const http_request& request);

            inline bool if_requested_param(const string_t& param);

            vector<string_t> get_flatten_uri(const string_t& uri);

            pair<vector<string_t>, endpoint_parameters>* find_endpoint(vector<string_t> flatten_uri, const web::http::method& method, bool exact = false);

            map<string_t, string_t> parse_url_parameters(const vector<string_t>& route_rule, const vector<string_t>& flatten_uri);
    };
}

#endif /* _BASE_CONTROLLER_H_ */