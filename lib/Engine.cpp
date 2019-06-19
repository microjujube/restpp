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
#include "RequestHandler.hpp"

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

class EngineSync : public Engine, public RequestHandler {
public:
    void Run(const std::string &args) override;

private:
    void do_session(boost::asio::ip::tcp::socket &socket, std::string const &doc_root);
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
    boost::asio::io_context ioc{4};

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

