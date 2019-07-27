//
// Created by linwei on 2019-07-27.
//

#include "EnginImpl.hpp"

using namespace restpp;
// Report a failure
void
fail(boost::system::error_code ec, char const *what) {
    std::cerr << what << ": " << ec.message() << "\n";
}