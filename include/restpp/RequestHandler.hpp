//
// Created by linwei on 2019-06-19.
//

#ifndef RESTPP_REQUESTHANDLER_HPP
#define RESTPP_REQUESTHANDLER_HPP

#include <memory>

namespace restpp {
    class RequestHandler {
    public:
        std::shared_ptr<RequestHandler> sptr;

    };
}

#endif //RESTPP_REQUESTHANDLER_HPP
