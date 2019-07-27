//
// Created by linwei on 2019-07-27.
//

#include "EngineSync.hpp"

using namespace restpp;

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
