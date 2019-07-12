//
// Created by linwei on 2019-06-19.
//

#ifndef RESTPP_RESPONSE_HPP
#define RESTPP_RESPONSE_HPP

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <boost/noncopyable.hpp>

#include <restpp/Types.hpp>
#include <restpp/Utils.hpp>

namespace restpp {
    class Response : public boost::noncopyable {
    public:
        void JSON(uint64_t status, const json &j) {
            _status = status;
            _body = j.dump();
            _size = _body.size();
        }

        void File(const std::string &filename) {
            boost::beast::http::file_body::file_type body;
            boost::beast::error_code ec;
            body.open(filename.c_str(), boost::beast::file_mode::scan, ec);
            if (ec == boost::beast::errc::no_such_file_or_directory) {
                _status = 404;
                _body = std::string("no such file or directory");
                _size = _body.size();
                content_type = "text/html";
                return;
            } else if (ec) {
                _status = 500;
                _body = std::string("server error");
                _size = _body.size();
                return;
            }

            _status = 200;
            _size = body.size(ec);
            _body.resize(_size);
            body.write(_body.data(), _body.size(), ec);
            content_type = std::string(mime_type(filename));
        }

        const std::string &getBody() const {
            return _body;
        }

        uint16_t getStatus() const {
            return _status;
        }

        const std::string &getContentType() const {
            return content_type;
        }

        size_t getSize() const {
            return _size;
        }

    private:
        uint16_t _status;
        std::string content_type = "application/json";
        std::string _body = "{}";
        size_t _size = 2;
    };
}

#endif //RESTPP_RESPONSE_HPP
