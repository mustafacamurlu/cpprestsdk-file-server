
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <boost/asio.hpp>

#include "base_controller.hpp"
#include "rest_server.hpp"

using namespace std;
using namespace web;
using namespace http;
using namespace boost;
using namespace utility;
using namespace mstfcmrl;

class file_controller : public base_controller
{
    string_t base;
    public:

        file_controller(string_t base_path)
            :base(base_path)
        {}

        void read(controller_request req)
        {
            std::string content; 
            ifstream ifs(base + req.url_parameters.at("filename"));

            if ( !ifs )
            {
                req.base_request.reply(status_codes::NotFound);
            }

            content.assign( (std::istreambuf_iterator<char>(ifs) ), (std::istreambuf_iterator<char>()));

            auto res = conversions::to_utf8string(content);
            
            req.base_request.reply(status_codes::OK, res);
        }

        void write(controller_request req)
        {
            /* @todo: file post body as file */
            req.base_request.reply(status_codes::OK);
        }

        void stat(controller_request req)
        {
            /* @todo: return some file stat */
            req.base_request.reply(status_codes::OK);
        }

        void del(controller_request req)
        {
            /* @todo: delete file*/
            req.base_request.reply(status_codes::OK);
        }

        void mapping()
        {
            set_route(U("/{filename}"),      web::http::methods::GET,  bind(&file_controller::read,  this, placeholders::_1));
            set_route(U("/stat/{filename}"), web::http::methods::GET,  bind(&file_controller::stat,  this, placeholders::_1));
            set_route(U("/{filename}"),      web::http::methods::POST, bind(&file_controller::write, this, placeholders::_1));
            set_route(U("/{filename}"),      web::http::methods::DEL,  bind(&file_controller::del,   this, placeholders::_1));
        }
};

void interrupt_handler( const boost::system::error_code& error , int signal_number ) 
{
    cout << "Cancelled!" << endl;
}

int main() 
{
    rest_server server(uri("http://0.0.0.0:8081"));
    file_controller f_ctrl(getenv("FILE_SERVER_BASE"));
    server.register_route(U("/file"), f_ctrl);
    server.start();

    boost::asio::io_service handler_context;
    boost::asio::signal_set signals(handler_context, SIGINT);
    signals.async_wait(interrupt_handler);
    handler_context.run();

    return 0;
}