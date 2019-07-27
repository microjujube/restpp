//
// Created by linwei on 2019-07-27.
//

#ifndef RESTPP_ENGINESYNC_HPP
#define RESTPP_ENGINESYNC_HPP

#include "EnginImpl.hpp"

namespace restpp{
    class EngineSync : public EngineImpl {
    public:
        void Run(const std::string &args) override;

    private:
        void do_session(boost::asio::ip::tcp::socket &socket);
    };
}

#endif //RESTPP_ENGINESYNC_HPP
