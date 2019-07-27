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
#include <boost/lexical_cast.hpp>
#include <boost/config.hpp>
#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>

#include "restpp/Logger.hpp"
#include "restpp/Engine.hpp"
#include "restpp/Exception.hpp"
#include "RequestHandler.hpp"

using namespace restpp;


class EngineImpl : public Engine, public RequestHandler {
public:
    void GET(const path_type &path, Handler handler) override {
        LOG(INFO) << "Register GET [" << path << "]";
        _routes[boost::beast::http::verb::get][path] = handler;
    }

    void POST(const path_type &path, Handler handler) override {
        LOG(INFO) << "Register POST [" << path << "]";
        _routes[boost::beast::http::verb::post][path] = handler;
    }

    void Static(const path_type &path, const std::string &dir) override {
        boost::filesystem::path p = dir;
        boost::filesystem::directory_iterator it{p};
        while (it != boost::filesystem::directory_iterator{}) {
            auto file = it->path();
            std::string get_path = path + "/" + file.filename().string();
            auto handler = [=](Context::sptr &ctx) {
                ctx->File(file.string());
            };
            _routes[boost::beast::http::verb::get][get_path] = handler;
            LOG(INFO) << "Register GET [" << get_path << "] to " << file.string();
            it++;
        }
    }

    void StaticFile(const path_type &path, const std::string &filename) override {
        auto handler = [=](Context::sptr &ctx) {
            ctx->File(filename);
        };
        LOG(INFO) << "Register GET [" << path << "] to " << filename;
        _routes[boost::beast::http::verb::get][path] = handler;
    }

private:
};


class EngineSync : public EngineImpl {
public:
    void Run(const std::string &args) override;

private:
    void do_session(boost::asio::ip::tcp::socket &socket);
};

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
    auto const port = boost::lexical_cast<unsigned short>(args_v[1]);

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
                std::move(socket))}.detach();
    }
}


// Report a failure
void
fail(boost::system::error_code ec, char const *what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

void EngineSync::do_session(boost::asio::ip::tcp::socket &socket) {
    bool close = false;
    boost::system::error_code ec;

    // This buffer is required to persist across reads
    boost::beast::flat_buffer buffer;

    for (;;) {
        // Read a request
        boost::beast::http::request<boost::beast::http::string_body> req;
        boost::beast::http::read(socket, buffer, req, ec);
        if (ec == boost::beast::http::error::end_of_stream)
            break;
        if (ec)
            return fail(ec, "read");

        // Send the response
        handle_request(std::move(req), socket, close, ec);
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


std::shared_ptr<Engine> restpp::Engine::make() {
    return std::make_shared<EngineSync>();
}