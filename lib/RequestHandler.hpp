//
// Created by linwei on 2019-06-19.
//

#ifndef RESTPP_REQUESTHANDLER_HPP
#define RESTPP_REQUESTHANDLER_HPP

#include <memory>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>

#include <unordered_map>
#include "restpp/Engine.hpp"
#include "restpp/Logger.hpp"
#include "restpp/Context.hpp"

namespace restpp {
    // Return a reasonable mime type based on the extension of a file.

    class RequestHandler {
    public:
        std::shared_ptr<RequestHandler> sptr;

        template<class Body, class Allocator, class Send>
        void handle_request(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> &&req,
                            Send &&send) {
            // Returns a bad request response
            auto const bad_request =
                    [&req](boost::beast::string_view why) {
                        boost::beast::http::response<boost::beast::http::string_body> res{
                                boost::beast::http::status::bad_request, req.version()};
                        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
                        res.set(boost::beast::http::field::content_type, "text/html");
                        res.keep_alive(req.keep_alive());
                        res.body() = why.to_string();
                        res.prepare_payload();
                        return res;
                    };

            // Returns a not found response
            auto const not_found =
                    [&req](boost::beast::string_view target) {
                        boost::beast::http::response<boost::beast::http::string_body> res{
                                boost::beast::http::status::not_found,
                                req.version()};
                        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
                        res.set(boost::beast::http::field::content_type, "text/html");
                        res.keep_alive(req.keep_alive());
                        res.body() = "The resource '" + target.to_string() + "' was not found.";
                        res.prepare_payload();
                        return res;
                    };

            // Returns a server error response
            auto const server_error =
                    [&req](boost::beast::string_view what) {
                        boost::beast::http::response<boost::beast::http::string_body> res{
                                boost::beast::http::status::internal_server_error, req.version()};
                        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
                        res.set(boost::beast::http::field::content_type, "text/html");
                        res.keep_alive(req.keep_alive());
                        res.body() = "An error occurred: '" + what.to_string() + "'";
                        res.prepare_payload();
                        return res;
                    };

            // Make sure we can handle the method
            if (req.method() != boost::beast::http::verb::get &&
                req.method() != boost::beast::http::verb::head &&
                req.method() != boost::beast::http::verb::post)
                return send(bad_request("Unknown HTTP-method"));

            // Request path must be absolute and not contain "..".
            if (req.target().empty() ||
                req.target()[0] != '/' ||
                req.target().find("..") != boost::beast::string_view::npos)
                return send(bad_request("Illegal request-target"));

            // Build the path to the requested file
            // std::string path = path_cat(doc_root, req.target());
            // if (req.target().back() == '/')
            // path.append("index.html");

            if (!_routes.count(req.method())) {
                return send(bad_request("Unknown HTTP-method"));
            }

            //get params
            size_t index = req.target().find("?");
            boost::string_view target = req.target().substr(0, index);
            boost::string_view get_param = "";
            if (index != boost::string_view::npos)
                get_param = req.target().substr(index + 1);


            DLOG(INFO) << req.method() << " " << target << " " << get_param;

            if (!_routes[req.method()].count(Engine::path_type(target))) {
                return send(bad_request("Not Found"));
            }

            auto ctx = Context::make();
            ctx->set_parameter(get_param);
            ctx->set_target(target);

            _routes[req.method()][Engine::path_type(target)](ctx);

            // Respond to GET request
//            boost::beast::http::response<boost::beast::http::string_body> res;
//            res.version(req.version());
//            res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
//            res.keep_alive(req.keep_alive());
//
//            res.body() = response.getBody();
//            res.content_length(response.getSize());
//            res.set(boost::beast::http::field::content_type, response.getContentType());
//            res.result(boost::beast::http::status(response.getStatus()));
//            res.set(boost::beast::http::field::access_control_allow_headers, "*");
//            res.set(boost::beast::http::field::access_control_allow_origin, "*");
//            res.set(boost::beast::http::field::cache_control, "private");
//
//            res.prepare_payload();

            return send(std::move(ctx->response()));
        }

    protected:
        std::unordered_map<boost::beast::http::verb,
                std::unordered_map<Engine::path_type,
                        Engine::Handler> > _routes;

    };
}

#endif //RESTPP_REQUESTHANDLER_HPP
