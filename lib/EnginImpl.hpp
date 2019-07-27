//
// Created by linwei on 2019-07-27.
//

#ifndef RESTPP_ENGINIMPL_HPP
#define RESTPP_ENGINIMPL_HPP

#include <iostream>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/config.hpp>
#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>

#include "restpp/Logger.hpp"
#include "restpp/Engine.hpp"
#include "restpp/Exception.hpp"
#include "RequestHandler.hpp"


namespace restpp{

    class EngineImpl : public Engine, public RequestHandler {
    public:
        void GET(const path_type &path, Handler handler) override {
            LOG(INFO) << "Register GET [" << path << "]";
            _routes[boost::beast::http::verb::get][path] = handler;
        }

        void POST(const path_type &path, Handler handler) override {
            LOG(INFO) << "Register POST [" << path << "]";
            _routes[boost::beast::http::verb::post][path] = handler;
        }

        void Static(const path_type &path, const std::string &dir) override {
            boost::filesystem::path p = dir;
            boost::filesystem::directory_iterator it{p};
            while (it != boost::filesystem::directory_iterator{}) {
                auto file = it->path();
                std::string get_path = path + "/" + file.filename().string();
                auto handler = [=](Context::sptr &ctx) {
                    ctx->File(file.string());
                };
                _routes[boost::beast::http::verb::get][get_path] = handler;
                LOG(INFO) << "Register GET [" << get_path << "] to " << file.string();
                it++;
            }
        }

        void StaticFile(const path_type &path, const std::string &filename) override {
            auto handler = [=](Context::sptr &ctx) {
                ctx->File(filename);
            };
            LOG(INFO) << "Register GET [" << path << "] to " << filename;
            _routes[boost::beast::http::verb::get][path] = handler;
        }

    private:
    };
}

#endif //RESTPP_ENGINIMPL_HPP
