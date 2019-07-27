//
// Created by linwei on 2019-06-18.
//
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
#include "EngineSync.hpp"
#include "EngineAsync.hpp"
#include "RequestHandler.hpp"

using namespace restpp;

std::shared_ptr<Engine> restpp::Engine::make() {
    return std::make_shared<EngineSync>();
}