//
// Created by linwei on 2019-06-19.
//
#include <restpp/Logger.hpp>
#include <restpp/Engine.hpp>
#include <string>

int main(int argc, char *argv[]) {
    try {
        auto engine = restpp::Engine::make();

        engine->StaticFile("/", "index.html");
        engine->Static("/static", "static");

        engine->GET("/hello", [](restpp::Context::sptr &ctx) {
            ctx->ParseGetParam();
            auto dict = ctx->GetParams();
            for (auto &key : dict.keys()) {
                LOG(INFO) << key << " = " << dict[key];
            }

            ctx->JSON(200, {
                    {"status", "ok"},
            });
        });

        engine->POST("/hello", [](restpp::Context::sptr &ctx) {
            restpp::json json;
            ctx->ShouldBindJSON(json);
            LOG(INFO) << json.dump();
            ctx->JSON(200, {
                    {"status", "ok"},
            });
        });

        engine->Run("0.0.0.0:8080");
    } catch (std::exception &e) {
        LOG(ERROR) << e.what();
    }

    return 0;
}