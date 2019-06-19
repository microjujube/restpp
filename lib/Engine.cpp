//
// Created by linwei on 2019-06-18.
//
#include <iostream>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <boost/tokenizer.hpp>

#include "restpp/Logger.hpp"
#include "restpp/Engine.hpp"
#include "restpp/Exception.hpp"

using namespace restpp;

std::string
path_cat(
        boost::beast::string_view base,
        boost::beast::string_view path) {
    if (base.empty())
        return path.to_string();
    std::string result = base.to_string();

    char constexpr path_separator = '/';
    if (result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
    return result;
}

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

class EngineSync : public Engine {
public:
    void Run(const std::string &args) override;

private:
    void do_session(boost::asio::ip::tcp::socket &socket, std::string const &doc_root);

    template<
            class Body, class Allocator,
            class Send>
    void
    handle_request(
            boost::beast::string_view doc_root,
            boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> &&req,
            Send &&send);
};

std::shared_ptr<Engine> restpp::Engine::make() {
    return std::make_shared<EngineSync>();
}

void EngineSync::Run(const std::string &args) {
    //TODO Parse arguments
    boost::char_separator<char> sep(":");
    boost::tokenizer tokens(args, sep);
    auto iter = tokens.begin();
    std::vector<std::string> args_v(2);
    for (int i = 0; i < 2; ++i) {
        if (iter == tokens.end())
            throw Exception(Exception::ERR_DEFAULT, std::string(""));
        args_v[i] = *iter;
        iter++;
    }

    auto const address = boost::asio::ip::make_address(args_v[0]);
    auto const port = (unsigned short) std::atoi(args_v[1].c_str());
    std::string const doc_root = "./static";

    LOG(INFO) << "Server on: " << address << ":" << port;

    // The io_context is required for all I/O
    boost::asio::io_context ioc{1};

    // The acceptor receives incoming connections
    boost::asio::ip::tcp::acceptor acceptor{ioc, {address, port}};
    for (;;) {
        // This will receive the new connection
        boost::asio::ip::tcp::socket socket{ioc};

        // Block until we get a connection
        acceptor.accept(socket);

        // Launch the session, transferring ownership of the socket
        std::thread{std::bind(
                &EngineSync::do_session, this,
                std::move(socket),
                doc_root)}.detach();
    }
}

// This is the C++11 equivalent of a generic lambda.
// The function object is used to send an HTTP message.
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


// Report a failure
void
fail(boost::system::error_code ec, char const *what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

void EngineSync::do_session(boost::asio::ip::tcp::socket &socket, std::string const &doc_root) {
    bool close = false;
    boost::system::error_code ec;

    // This buffer is required to persist across reads
    boost::beast::flat_buffer buffer;

    // This lambda is used to send messages
    send_lambda<boost::asio::ip::tcp::socket> lambda{socket, close, ec};

    for (;;) {
        // Read a request
        boost::beast::http::request<boost::beast::http::string_body> req;
        boost::beast::http::read(socket, buffer, req, ec);
        if (ec == boost::beast::http::error::end_of_stream)
            break;
        if (ec)
            return fail(ec, "read");

        // Send the response
        handle_request(doc_root, std::move(req), lambda);
        if (ec)
            return fail(ec, "write");
        if (close) {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            break;
        }
    }

    // Send a TCP shutdown
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
}

template<class Body, class Allocator, class Send>
void EngineSync::handle_request(boost::beast::string_view doc_root,
                                boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> &&req,
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
                boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::not_found,
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

    LOG(INFO) << "handle request";

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
    std::string path = path_cat(doc_root, req.target());
    if (req.target().back() == '/')
        path.append("index.html");

    // Attempt to open the file
    boost::beast::error_code ec;
    boost::beast::http::file_body::value_type body;
    body.open(path.c_str(), boost::beast::file_mode::scan, ec);

    // Handle the case where the file doesn't exist
    if (ec == boost::system::errc::no_such_file_or_directory)
        return send(not_found(req.target()));

    // Handle an unknown error
    if (ec)
        return send(server_error(ec.message()));

    // Respond to HEAD request
    if (req.method() == boost::beast::http::verb::head) {
        boost::beast::http::response<boost::beast::http::empty_body> res{boost::beast::http::status::ok, req.version()};
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, mime_type(path));
        res.content_length(body.size());
        res.keep_alive(req.keep_alive());
        return send(std::move(res));
    }

    // Respond to GET request
    boost::beast::http::response<boost::beast::http::file_body> res{
            std::piecewise_construct,
            std::make_tuple(std::move(body)),
            std::make_tuple(boost::beast::http::status::ok, req.version())};
    res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(boost::beast::http::field::content_type, mime_type(path));
    res.content_length(body.size());
    res.keep_alive(req.keep_alive());
    return send(std::move(res));
}
