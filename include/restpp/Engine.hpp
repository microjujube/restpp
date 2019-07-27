//
// Created by linwei on 2019-06-18.
//

#ifndef RESTPP_ENGINE_HPP
#define RESTPP_ENGINE_HPP

#include <memory>
#include <functional>
#include <thread>

#include "restpp/Config.h"
#include "restpp/Context.hpp"

namespace restpp {
    class RESTPP_API Engine {
    public:
        typedef std::shared_ptr<Engine> sptr;

        typedef std::string path_type;

        typedef std::function<void(Context::sptr &ctx)> Handler;

        virtual void Run(const std::string &args) = 0;

        virtual void GET(const path_type &path, Handler handler) = 0;

        virtual void POST(const path_type &path, Handler handler) = 0;

        virtual void FILES(const path_type &path, const std::string &dir) = 0;

        virtual void StaticFile(const path_type &path, const std::string &filename) = 0;

        static sptr make();
    };

}
#endif //RESTPP_ENGINE_HPP
