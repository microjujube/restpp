//
// Created by linwei on 2019-06-19.
//

#ifndef RESTPP_REQUEST_HPP
#define RESTPP_REQUEST_HPP

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <boost/any.hpp>
#include <boost/tokenizer.hpp>
#include <boost/regex.hpp>
#include <nlohmann/json.hpp>

#include <restpp/Types.hpp>
#include <restpp/Dict.hpp>

static const char *arg_delim = "&";
static const char *pair_delim = "=";

static std::string trim(const std::string &in);

#define tokenizer(inp, sep) \
    boost::tokenizer<boost::char_separator<char> > \
    (inp, boost::char_separator<char>(sep))


namespace restpp {
    class Request : boost::noncopyable {
    public:
        Request(std::string &&method,
                std::string &&target,
                std::string &&params,
                std::string &&body) : _method(std::move(method)), _target(std::move(target)),
                                      _params(std::move(params)), _body(std::move(body)) {

        }

        void ParseGetParam();

        void BindJSON(nlohmann::json &json);

        const std::string &getMethod() const {
            return _method;
        }

        const std::string &getTarget() const {
            return _target;
        }

        const std::string &getParams() const {
            return _params;
        }

        const std::string &getBody() const {
            return _body;
        }


    private:
        Dict<std::string, std::string> _get_dict;
    public:
        const Dict<std::string, std::string> &GetParams() const;

    private:
        std::string _method;
        std::string _target;
        std::string _params;
        std::string _body;


    };
}


#endif //RESTPP_REQUEST_HPP
