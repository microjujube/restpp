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

namespace restpp {
    // Return a reasonable mime type based on the extension of a file.
    boost::beast::string_view mime_type(boost::beast::string_view path) {
        boost::beast::string_view result = "application/text";
        using boost::beast::iequals;
        auto const ext = [&path] {
            auto const pos = path.rfind(".");
            return pos == boost::beast::string_view::npos ? boost::beast::string_view{} : path.substr(pos);
        }();

        if (iequals(ext, ".html")) result = "text/html";
        else if (iequals(ext, ".htm")) result = "text/html";
        else if (iequals(ext, ".css")) result = "text/css";
        else if (iequals(ext, ".txt")) result = "text/plain";
        else if (iequals(ext, ".js")) result = "application/javascript";
        else if (iequals(ext, ".json")) result = "application/json";
        else if (iequals(ext, ".xml")) result = "application/xml";
        else if (iequals(ext, ".swf")) result = "application/x-shockwave-flash";
        else if (iequals(ext, ".flv")) result = "video/x-flv";
        else if (iequals(ext, ".png")) result = "image/png";
        else if (iequals(ext, ".jpe")) result = "image/jpeg";
        else if (iequals(ext, ".jpeg")) result = "image/jpeg";
        else if (iequals(ext, ".jpg")) result = "image/jpeg";
        else if (iequals(ext, ".gif")) result = "image/gif";
        else if (iequals(ext, ".bmp")) result = "image/bmp";
        else if (iequals(ext, ".ico")) result = "image/vnd.microsoft.icon";
        else if (iequals(ext, ".tiff")) result = "image/tiff";
        else if (iequals(ext, ".tif")) result = "image/tiff";
        else if (iequals(ext, ".svg")) result = "image/svg+xml";
        else if (iequals(ext, ".svgz")) result = "image/svg+xml";

        return result;
    }

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
                req.method() != boost::beast::http::verb::head)
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
            LOG(INFO) << req.method() << " " << req.target();
            if (!_routes.count(req.method())) {
                return send(bad_request("Unknown HTTP-method"));
            }

            if (!_routes[req.method()].count(Engine::path_type(req.target()))) {
                return send(bad_request("Not Found"));
            }



            // Attempt to open the file
            boost::beast::error_code ec;
            boost::beast::http::string_body::value_type body;
//    body.open(path.c_str(), boost::beast::file_mode::scan, ec);
            body = R"({"name":"hello world"})";

            // Handle the case where the file doesn't exist
            if (ec == boost::system::errc::no_such_file_or_directory)
                return send(not_found(req.target()));

            // Handle an unknown error
            if (ec)
                return send(server_error(ec.message()));

            // Respond to HEAD request
            if (req.method() == boost::beast::http::verb::head) {
                boost::beast::http::response<boost::beast::http::empty_body> res{boost::beast::http::status::ok,
                                                                                 req.version()};
                res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(boost::beast::http::field::content_type, mime_type(".json"));
//        res.content_length(body.size());
                res.keep_alive(req.keep_alive());
                return send(std::move(res));
            }


            // Respond to GET request
            boost::beast::http::response<boost::beast::http::string_body> res;
            res.version(req.version());
            res.result(boost::beast::http::status::ok);
            res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(boost::beast::http::field::content_type, mime_type(".json"));
            res.content_length(body.size());
            res.keep_alive(req.keep_alive());
            res.body() = body;
            res.prepare_payload();

            return send(std::move(res));
        }

    protected:
        std::unordered_map<boost::beast::http::verb, std::unordered_map<Engine::path_type, Engine::Handler> > _routes;
//        std::unordered_map<Engine::path_type, Engine::Handler> _put_routes;
//        std::unordered_map<Engine::path_type, Engine::Handler> _delete_routes;
//        std::unordered_map<Engine::path_type, Engine::Handler> _post_routes;
//        std::unordered_map<Engine::path_type, Engine::Handler> _get_routes;

    };
}

#endif //RESTPP_REQUESTHANDLER_HPP
