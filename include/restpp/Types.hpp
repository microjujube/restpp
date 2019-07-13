//
// Created by linwei on 2019-06-20.
//

#ifndef RESTPP_TYPES_HPP
#define RESTPP_TYPES_HPP

#include <nlohmann/json.hpp>
#include <boost/beast/version.hpp>

namespace restpp {
    typedef nlohmann::json json;
    enum class Method {
        unknown,
        get,
        post,
        put,
        delete_,
        head,
    };

    using string_body = boost::beast::http::response<boost::beast::http::string_body>;
    using string_view = boost::beast::string_view;

    struct bad_request {
        unsigned int version_;
        bool keep_alive_;

        bad_request(unsigned int version, bool keep_alive) : version_(version), keep_alive_(keep_alive) {

        }

        string_body operator()(string_view why) {
            boost::beast::http::response<boost::beast::http::string_body> res{
                    boost::beast::http::status::bad_request, version_};
            res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(boost::beast::http::field::content_type, "text/html");
            res.keep_alive(keep_alive_);
            res.body() = why.to_string();
            res.prepare_payload();
            return res;
        }

    };

    struct server_error {
        unsigned int version_;
        bool keep_alive_;

        server_error(unsigned int version, bool keep_alive) : version_(version), keep_alive_(keep_alive) {

        }

        string_body operator()(string_view what) {
            boost::beast::http::response<boost::beast::http::string_body> res{
                    boost::beast::http::status::internal_server_error, version_};
            res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(boost::beast::http::field::content_type, "text/html");
            res.keep_alive(keep_alive_);
            res.body() = "An error occurred: '" + what.to_string() + "'";
            res.prepare_payload();
            return res;
        };
    };


    struct not_found {
        unsigned int version_;
        bool keep_alive_;

        not_found(unsigned int version, bool keep_alive) : version_(version), keep_alive_(keep_alive) {

        }

        string_body operator()(string_view target) {
            boost::beast::http::response<boost::beast::http::string_body> res{
                    boost::beast::http::status::not_found,
                    version_};
            res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(boost::beast::http::field::content_type, "text/html");
            res.keep_alive(keep_alive_);
            res.body() = "The resource '" + target.to_string() + "' was not found.";
            res.prepare_payload();
            return res;
        }

    };

}

#endif //RESTPP_TYPES_HPP
