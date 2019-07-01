//
// Created by linwei on 2019-06-19.
//

#include "restpp/Request.hpp"

#include <boost/algorithm/string.hpp>

std::string trim(const std::string &in) {
    return boost::algorithm::trim_copy(in);
}

using namespace restpp;

void Request::ParseGetParam() {
    for (const std::string &pair: tokenizer(_params, arg_delim)) {
        if (trim(pair).empty()) continue;
        std::vector<std::string> toks;
        for (const std::string &tok: tokenizer(pair, pair_delim)) {
            toks.push_back(tok);
        }
        if (toks.size() == 1) toks.emplace_back(""); //pad empty value
        if (toks.size() == 2 and not trim(toks[0]).empty()) { //only valid combination
            _get_dict.set(trim(toks[0]), trim(toks[1]));
        } else continue; //otherwise error
    }
}

const Dict<std::string, std::string> &Request::GetParams() const {
    return _get_dict;
}

void Request::BindJSON(restpp::json &j) {
    j = json::parse(getBody());
}
