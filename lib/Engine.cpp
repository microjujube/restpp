//
// Created by linwei on 2019-06-18.
//

#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "resetpp/Engine.hpp"

using namespace resetpp;


class EngineSync : public Engine {
public:
    void Run() override;

private:
    void do_session(boost::asio::ip::tcp::socket &socket, std::string const &doc_root);

};

std::shared_ptr<Engine> resetpp::Engine::make() {
    return std::make_shared<EngineSync>();
}

void EngineSync::Run() {
    auto const address = boost::asio::ip::make_address("0.0.0.0");
    auto const port = 8080;
    std::string const doc_root = "./static";

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

void EngineSync::do_session(boost::asio::ip::tcp::socket &socket, std::string const &doc_root) {

}
