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

#include <nlohmann/json.hpp>

namespace restpp {
    struct Response {
        typedef nlohmann::json json;
        uint16_t _status;
        std::string content_type = "application/json";
        std::string body = "{}";
        size_t size = 2;

        void JSON(uint64_t status,const json &j) {
            _status = status;
            body = j.dump();
            size = body.size();
        }
    };
}

#endif //RESTPP_RESPONSE_HPP
