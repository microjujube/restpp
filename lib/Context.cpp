//
// Created by linwei on 2019-07-13.
//

#include "restpp/Context.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/http.hpp>
#include <restpp/Utils.hpp>

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

        boost_beast_response &response() override;

        void ParseGetParam() override;

        Dict<std::string, std::string> &GetParams() override;

    private:
        void set_parameter(boost::string_view &param) override;

        void set_target(boost::string_view &target) override;

        void set_version(unsigned int version) override;

        void set_keep_alive(bool keep_alive) override;

        void set_body(boost::string_view &body) override;

    private:
        std::string _target;
        std::string _parameter;
        bool _keep_alive{};
        unsigned int _version{};
        Dict<std::string, std::string> _get_dict;
        boost_beast_response _response;
    };

    void ContextImpl::JSON(int status, const json &json) {
        //set http status
        _response.result(boost::beast::http::status(status));
        //set content type
        _response.set(boost::beast::http::field::content_type, "application/json");
        //write stream for json
        boost::beast::ostream(_response.body())
                << json.dump();

    }

    void ContextImpl::File(const std::string &filename) {
        boost::beast::http::file_body::file_type body;
        boost::beast::error_code ec;
        boost::beast::http::status status = boost::beast::http::status::ok;
        body.open(filename.c_str(), boost::beast::file_mode::scan, ec);
        if (ec == boost::beast::errc::no_such_file_or_directory) {
            status = boost::beast::http::status::not_found;
            boost::beast::ostream(_response.body()) << "not found";
        } else if (ec) {
            status = boost::beast::http::status::internal_server_error;
            boost::beast::ostream(_response.body()) << "internal server error";
        } else {
            boost::beast::ostream(_response.body());
            
        }
        //set http status as http ok
        _response.result(status);
        //set content type
        _response.set(boost::beast::http::field::content_type, mime_type(filename));

    }

    void ContextImpl::set_body(boost::string_view &body) {

    }

    Context::boost_beast_response &ContextImpl::response() {
        _response.set(boost::beast::http::field::access_control_allow_headers, "*");
        _response.set(boost::beast::http::field::access_control_allow_origin, "*");
        _response.set(boost::beast::http::field::cache_control, "private");

        _response.version(_version);
        _response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        _response.keep_alive(_keep_alive);

        _response.prepare_payload();
        return _response;
    }

    void ContextImpl::set_parameter(boost::string_view &param) {
        _parameter = std::string(param);
    }

    void ContextImpl::set_target(boost::string_view &target) {
        _target = std::string(target);
    }

    void ContextImpl::set_version(unsigned int version) {
        _version = version;
    }

    void ContextImpl::set_keep_alive(bool keep_alive) {
        _keep_alive = keep_alive;
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
