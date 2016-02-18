#pragma once
#include <string>
#include <unordered_map>
#include "json.h"
#include "http_request.h"
#include "ci_map.h"
#include "socket_adaptors.h"
#include "logging.h"

namespace crow
{

    template <typename Adaptor, typename Handler, typename ... Middlewares>
    class Connection;

    template <typename Adaptor>
    struct AdaptorWrapper{
        Adaptor* adaptor_;
    };

    struct response
    {
        template <typename Adaptor, typename Handler, typename ... Middlewares>
        friend class crow::Connection;


        std::string body;
        json::wvalue json_value;
        int code{200};

        // `headers' stores HTTP headers.
        ci_map headers;

        void set_header(std::string key, std::string value)
        {
            headers.erase(key);
            headers.emplace(std::move(key), std::move(value));
        }
        void add_header(std::string key, std::string value)
        {
            headers.emplace(std::move(key), std::move(value));
        }

        const std::string& get_header_value(const std::string& key)
        {
            return crow::get_header_value(headers, key);
        }


        response() {}
        explicit response(int code) : code(code) {}
        response(std::string body) : body(std::move(body)) {}
        response(json::wvalue&& json_value) : json_value(std::move(json_value)) 
        {
            json_mode();    
        }
        response(int code, std::string body) : body(std::move(body)), code(code) {}
        response(const json::wvalue& json_value) : body(json::dump(json_value)) 
        {
            json_mode();
        }
        response(int code, const json::wvalue& json_value) : code(code), body(json::dump(json_value))
        {
            json_mode();
        }

        response(response&& r)
        {
            *this = std::move(r);
        }

        response& operator = (const response& r) = delete;

        response& operator = (response&& r) noexcept
        {
            body = std::move(r.body);
            json_value = std::move(r.json_value);
            code = r.code;
            headers = std::move(r.headers);
            completed_ = r.completed_;
            return *this;
        }

        bool is_completed() const noexcept
        {
            return completed_;
        }

        void clear()
        {
            body.clear();
            json_value.clear();
            code = 200;
            headers.clear();
            completed_ = false;
        }

        void write(const std::string& body_part)
        {
            body += body_part;
        }

        void end()
        {
            if (!completed_)
            {
                completed_ = true;
                
                if (complete_request_handler_)
                {
                    complete_request_handler_();
                }
            }
        }

        void end(const std::string& body_part)
        {
            body += body_part;
            end();
        }

        bool is_alive()
        {
            return is_alive_helper_ && is_alive_helper_();
        }
        SocketAdaptor* adaptor;
        template <typename Adaptor>
        //void testKek(Adaptor& adaptor_){
        decltype(std::declval<Adaptor>().raw_socket())& testKek(Adaptor& adaptor_) {
            CROW_LOG_INFO << "AAAAAAAAAAAAAAAAAAAAAAA";
            //this->adaptor.adaptor_=adaptor_;
            return adaptor_.raw_socket();
        }


        private:
            bool completed_{};
            std::function<void()> complete_request_handler_;
            std::function<bool()> is_alive_helper_;
            
            //In case of a JSON object, set the Content-Type header
            void json_mode()
            {
                set_header("Content-Type", "application/json");
            }
    };
}
