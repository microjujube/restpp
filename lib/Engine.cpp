//
// Created by linwei on 2019-06-18.
//

#include <Engine.hpp>

#include "Engine.hpp"

using namespace resetpp;


class EngineSync : public Engine {
public:
    void Run() override;

public:

};

std::shared_ptr <Engine> resetpp::Engine::make() {
    return std::make_shared<EngineSync>();
}

void EngineSync::Run() {

}
