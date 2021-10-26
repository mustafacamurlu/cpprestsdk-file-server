#ifndef _BASE_CONTROLLER_H_
#define _BASE_CONTROLLER_H_

#include <map>
#include <cpprest/http_listener.h>

using namespace std;
using namespace web;
using namespace http;
using namespace utility;

namespace mstfcmrl
{
    struct controller_request
    {
        http_request base_request;
        map<string_t, string_t> url_parameters;

        controller_request(const http_request& req, const map<string_t, string_t> url_params)
            :base_request(req), url_parameters(url_params)
            {}
    };

    class base_controller
    {
        public:
            virtual void mapping() = 0;

            virtual void resolve(const http_request& request, const string_t& trailing_uri) final;

            virtual void set_route(const string_t& uri, const http::method& method, std::function<void(controller_request request)> handler) final;
            
        private:
            struct controller_param
            {
                string_t orig_uri;
                http::method method;
                std::function<void(controller_request request)> handler;

                controller_param(const string_t& uri, const http::method& method, std::function<void(controller_request request)> handler)
                    :orig_uri(uri), method(method), handler(handler)
                {}

                controller_param& operator=(controller_param const& c_param)
                {
                    this->orig_uri = c_param.orig_uri;
                    this->method = c_param.method;
                    this->handler = c_param.handler;
                    return *this;
                }
            };

            vector<pair<vector<string_t>, controller_param>> controller_mapper;

            inline bool if_requested_param(const string_t& param);

            vector<string_t> get_flatten_uri(const string_t& uri);

            map<string_t, string_t> parse_url_parametesr(const vector<string_t>& route_rule, const vector<string_t>& flatten_uri);
    };
}

#endif /* _BASE_CONTROLLER_H_ */