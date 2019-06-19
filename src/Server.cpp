//
// Created by linwei on 2019-06-19.
//
#include <restpp/Engine.hpp>
#include <string>

int main(int argc, char *argv[]) {
    auto engine = restpp::Engine::make();

    engine->GET("/hello", [](const restpp::Request &req, restpp::Response &resp) {

    });

    engine->Run("0.0.0.0:8080");
    return 0;
}