//
// Created by linwei on 2019-06-18.
//

#ifndef RESTPP_ENGINE_HPP
#define RESTPP_ENGINE_HPP

#include <memory>
#include <functional>
#include <thread>

#include "restpp/Request.hpp"
#include "restpp/Response.hpp"

namespace restpp {
    class Engine {
    public:
        typedef std::shared_ptr<Engine> sptr;

        typedef std::string path_type;

        typedef std::function<void(const Request &req, Response &resp)> Handler;

        virtual void Run(const std::string &args) = 0;

        virtual void GET(const path_type &path, Handler handler) = 0;

        static sptr make();
    };

}
#endif //RESTPP_ENGINE_HPP
