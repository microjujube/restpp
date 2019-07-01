//
// Created by linwei on 2019-06-20.
//

#ifndef RESTPP_TYPES_HPP
#define RESTPP_TYPES_HPP

#include <nlohmann/json.hpp>

namespace restpp {
    typedef nlohmann::json json;
    enum class Method {
        unknown,
        get,
        post,
        put,
        delete_,
        head,
    };
}

#endif //RESTPP_TYPES_HPP
