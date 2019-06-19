//
// Created by linwei on 2019-06-19.
//
#include <restpp/Engine.hpp>
#include <string>

int main(int argc, char *argv[]) {
    auto engine = restpp::Engine::make();
    engine->Run("0.0.0.0:8080");

    return 0;
}