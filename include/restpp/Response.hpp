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

namespace restpp {
    class Response : public boost::noncopyable {
    public:
        void JSON(uint64_t status, const json &j) {
            _status = status;
            _body = j.dump();
            _size = _body.size();
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
