//
// Created by linwei on 2019-06-19.
//
#include <restpp/Logger.hpp>
#include <restpp/Engine.hpp>
#include <string>

int main(int argc, char *argv[]) {
    auto engine = restpp::Engine::make();

    engine->GET("/", [](restpp::Request &req, restpp::Response &resp) {
        LOG(INFO) << req.getMethod();
        LOG(INFO) << req.getTarget();
        LOG(INFO) << req.getParams();
        LOG(INFO) << req.getBody();

        req.ParseGetParam();

        resp.JSON(200, {
                {"status", "ok"},
        });
    });

    engine->Run("0.0.0.0:8080");
    return 0;
}