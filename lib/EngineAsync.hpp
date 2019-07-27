//
// Created by linwei on 2019-07-27.
//

#ifndef RESTPP_ENGINEASYNC_HPP
#define RESTPP_ENGINEASYNC_HPP

#include "EnginImpl.hpp"

namespace restpp {
    class EngineAsync : public EngineImpl {
    public:
        void Run(const std::string &args) override;
    };
}


#endif //RESTPP_ENGINEASYNC_HPP
