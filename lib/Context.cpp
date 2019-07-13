//
// Created by linwei on 2019-07-13.
//

#include "restpp/Context.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

#define tokenizer(inp, sep) \
    boost::tokenizer<boost::char_separator<char> > \
    (inp, boost::char_separator<char>(sep))

namespace restpp {

    static const char *arg_delim = "&";
    static const char *pair_delim = "=";

    class ContextImpl : public Context {
    public:
        ContextImpl() = default;

        void JSON(int status, const json &json) override;

        void File(const std::string &filename) override;

        boost_beast_response response() override;

        void ParseGetParam() override;

        Dict<std::string, std::string> &GetParams() override;

    private:
        void set_parameter(boost::string_view &param) override;

        void set_target(boost::string_view &target) override;

    private:
        std::string _target;
        std::string _parameter;
        Dict<std::string, std::string> _get_dict;
    };

    void ContextImpl::JSON(int status, const json &json) {

    }

    void ContextImpl::File(const std::string &filename) {

    }

    Context::boost_beast_response ContextImpl::response() {
        return restpp::Context::boost_beast_response();
    }

    void ContextImpl::set_parameter(boost::string_view &param) {
        _parameter = std::string(param);
    }

    void ContextImpl::set_target(boost::string_view &target) {
        _target = std::string(target);
    }

    Dict<std::string, std::string> &ContextImpl::GetParams() {
        return _get_dict;
    }

    void ContextImpl::ParseGetParam() {
        for (const std::string &pair: tokenizer(_parameter, arg_delim)) {
            if (boost::algorithm::trim_copy(pair).empty()) continue;
            std::vector<std::string> toks;
            for (const std::string &tok: tokenizer(pair, pair_delim)) {
                toks.push_back(tok);
            }
            if (toks.size() == 1) toks.emplace_back(""); //pad empty value
            if (toks.size() == 2 and not boost::algorithm::trim_copy(toks[0]).empty()) { //only valid combination
                _get_dict.set(boost::algorithm::trim_copy(toks[0]), boost::algorithm::trim_copy(toks[1]));
            } else continue; //otherwise error
        }
    }

    Context::sptr Context::make() {
        return Context::sptr(new ContextImpl());
    }

}
