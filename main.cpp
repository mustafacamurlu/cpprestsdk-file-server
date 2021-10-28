#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdlib>
#include <cstdio>
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

    vector<string> get_file_names_in_base()
    {
        struct dirent *entry;
        vector<string> files;
        
        DIR *dir = opendir(base.c_str());

        if (!dir) 
        {
            return files;
        }

        while (entry = readdir(dir)) 
        {
            files.push_back(entry->d_name);
        }

        closedir(dir);
        
        return files;
    }

    public:

        file_controller(string_t base_path)
            :base(base_path)
        {}

        void list(controller_request req)
        {
            string out;
            for (const string &file_name : get_file_names_in_base())
            {
                out += file_name + "\n";
            }
            
            req.base_request.reply(status_codes::OK, out);
        }

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
            ofstream ofs(base + req.url_parameters.at("filename"));
            ofs << req.base_request.extract_string().get();
            ofs.close();
            req.base_request.reply(status_codes::OK);
        }

        char* formatdate(char* str, time_t val)
        {
                strftime(str, 36, "%d.%m.%Y %H:%M:%S", localtime(&val));
                return str;
        }
        
        void fstat(controller_request req)
        {
            time_t tv;
            char date[36];
            struct stat buf;
            json::value output;

            if (stat((base + req.url_parameters.at("filename")).c_str(), &buf) != 0)
            {
                req.base_request.reply(status_codes::NotFound);
            }

            output[U("stats")][U("size")] = json::value::number(buf.st_size);

            tv = buf.st_atime;
            strftime(date, 36, "%d.%m.%Y %H:%M:%S", localtime(&tv));
            output[U("stats")][U("access")] = json::value::string(date);

            tv = buf.st_mtime;
            strftime(date, 36, "%d.%m.%Y %H:%M:%S", localtime(&tv));
            output[U("stats")][U("modify")] = json::value::string(date);
            
            tv = buf.st_ctime;
            strftime(date, 36, "%d.%m.%Y %H:%M:%S", localtime(&tv));
            output[U("stats")][U("change")] = json::value::string(date);
            
            req.base_request.reply(status_codes::OK, output);
        }

        void del(controller_request req)
        {
            if (remove((base + req.url_parameters.at("filename") ).c_str()))
            {
                req.base_request.reply(status_codes::NotFound);
            }
            else
            {
                req.base_request.reply(status_codes::OK);
            }
        }

        void mapping()
        {
            set_route(U("/list"),            web::http::methods::GET,  bind(&file_controller::list,  this, placeholders::_1));
            set_route(U("/{filename}"),      web::http::methods::GET,  bind(&file_controller::read,  this, placeholders::_1));
            set_route(U("/stat/{filename}"), web::http::methods::GET,  bind(&file_controller::fstat, this, placeholders::_1));
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