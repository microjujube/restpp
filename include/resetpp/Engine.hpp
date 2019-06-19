//
// Created by linwei on 2019-06-18.
//

#ifndef RESTPP_ENGINE_HPP
#define RESTPP_ENGINE_HPP

#include <memory>

namespace resetpp {
    class Engine {
    public:
        typedef std::shared_ptr<Engine> sptr;

        virtual void Run() = 0;

        static sptr make();
    };

}
#endif //RESTPP_ENGINE_HPP
