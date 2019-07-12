//
// Created by locate on 7/12/19.
//

#ifndef RESTPP_UTILS_HPP
#define RESTPP_UTILS_HPP


#include <boost/beast/core/string.hpp>

namespace restpp {
    boost::beast::string_view mime_type(boost::beast::string_view path);
}


#endif //RESTPP_UTILS_HPP
