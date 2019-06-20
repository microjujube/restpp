//
// Created by linwei on 2019-06-19.
//

#ifndef RESTPP_REQUEST_HPP
#define RESTPP_REQUEST_HPP

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>

#include "restpp/Types.hpp"

namespace restpp {
    struct Request {
        boost::string_view method;
        boost::string_view target;
        boost::string_view params;
        boost::string_view body;
    };
}


#endif //RESTPP_REQUEST_HPP
