#ifndef _REST_SERVER_H_
#define _REST_SERVER_H_

#include <cpprest/http_listener.h>

#include "base_controller.hpp"

using namespace mstfcmrl;
using namespace utility;
using namespace web::http::experimental::listener;

namespace mstfcmrl
{
    class rest_server
    {
        http_listener listener;
        map<string_t, base_controller&> route_map;

        public:

            rest_server(uri uri);
            
            void register_route(const string_t &route, base_controller& controller);

            void router(http_request request);

            pplx::task<void> start();

            pplx::task<void> shutdown();
    };
}


#endif /* _REST_SERVER_H_ */