//
// Created by linwei on 2019-07-13.
//

#ifndef RESTPP_CONTEXT_HPP
#define RESTPP_CONTEXT_HPP

#include <memory>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/dynamic_body.hpp>
#include "restpp/Types.hpp"
#include "restpp/Dict.hpp"

namespace restpp {
    template<class Stream>
    struct send_lambda {
        Stream &stream_;
        bool &close_;
        boost::system::error_code &ec_;

        explicit
        send_lambda(
                Stream &stream,
                bool &close,
                boost::system::error_code &ec)
                : stream_(stream), close_(close), ec_(ec) {
        }

        template<bool isRequest, class Body, class Fields>
        void
        operator()(boost::beast::http::message<isRequest, Body, Fields> &&msg) const {
            // Determine if we should close the connection after
            close_ = msg.need_eof();

            // We need the serializer here because the serializer requires
            // a non-const file_body, and the message oriented version of
            // http::write only works with const messages.
            boost::beast::http::serializer<isRequest, Body, Fields> sr{msg};
            boost::beast::http::write(stream_, sr, ec_);
        }
    };

    class Context {
    public:
        typedef std::shared_ptr<Context> sptr;

        static sptr make(boost::asio::ip::tcp::socket &socket,
                         bool &close,
                         boost::system::error_code &ec);

        virtual void ParseGetParam() = 0;

        virtual Dict<std::string, std::string> &GetParams() = 0;

        //api for user
        virtual void JSON(int status, const json &json) = 0;

        virtual void File(const std::string &filename) = 0;

        friend class RequestHandler;

    private:
        //local api
        virtual void set_parameter(boost::string_view &param) = 0;

        virtual void set_target(boost::string_view &target) = 0;

        virtual void set_version(unsigned int version) = 0;

        virtual void set_keep_alive(bool keep_alive) = 0;

        virtual void set_body(boost::string_view &body) = 0;
    };
}

#endif //RESTPP_CONTEXT_HPP
