//
// Created by linwei on 2019-07-13.
//

#ifndef RESTPP_CONTEXT_HPP
#define RESTPP_CONTEXT_HPP

#include <memory>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/dynamic_body.hpp>
#include "Types.hpp"
#include "restpp/Dict.hpp"

namespace restpp {
    class Context {
    public:
        typedef std::shared_ptr<Context> sptr;

        typedef boost::beast::http::response<boost::beast::http::dynamic_body> boost_beast_response;

        static sptr make();

        virtual void ParseGetParam() = 0;

        virtual Dict <std::string, std::string> &GetParams() = 0;

        //api for user
        virtual void JSON(int status, const json &json) = 0;

        virtual void File(const std::string &filename) = 0;

        friend class RequestHandler;

    private:
        //local api
        virtual boost_beast_response response() = 0;

        virtual void set_parameter(boost::string_view &param) = 0;

        virtual void set_target(boost::string_view &target) = 0;
    };
}

#endif //RESTPP_CONTEXT_HPP
