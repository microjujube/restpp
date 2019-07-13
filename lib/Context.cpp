//
// Created by linwei on 2019-07-13.
//

#include "restpp/Context.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <restpp/Utils.hpp>

#define tokenizer(inp, sep) \
    boost::tokenizer<boost::char_separator<char> > \
    (inp, boost::char_separator<char>(sep))

namespace restpp {

    static const char *arg_delim = "&";
    static const char *pair_delim = "=";

    class ContextImpl : public Context {
    public:
        ContextImpl(boost::asio::ip::tcp::socket &socket,
                    bool &close,
                    boost::system::error_code &ec) : _socket(socket), _close(close), _ec(ec) {

        };

        void JSON(int status, const json &json) override;

        void File(const std::string &filename) override;

        void ParseGetParam() override;

        Dict<std::string, std::string> &GetParams() override;

    private:
        void set_parameter(boost::string_view &param) override;

        void set_target(boost::string_view &target) override;

        void set_version(unsigned int version) override;

        void set_keep_alive(bool keep_alive) override;

        void set_body(boost::string_view &body) override;

        template<class T>
        void set_access_control(T &response) {
            response.set(boost::beast::http::field::access_control_allow_headers, "*");
            response.set(boost::beast::http::field::access_control_allow_origin, "*");
            response.set(boost::beast::http::field::cache_control, "private");
        }

    private:
        std::string _target;
        std::string _parameter;
        bool _keep_alive{};
        unsigned int _version{};
        Dict<std::string, std::string> _get_dict;

        boost::asio::ip::tcp::socket &_socket;
        bool &_close;
        boost::system::error_code &_ec;
    };

    void ContextImpl::JSON(int status, const json &json) {
        send_lambda send{_socket, _close, _ec};
        boost::beast::http::response<boost::beast::http::string_body> res{
                boost::beast::http::status(status),
                _version};
        //set http status
        res.result(boost::beast::http::status(status));
        //set content type
        res.set(boost::beast::http::field::content_type, "application/json");
        //write stream for json
        res.body() = json.dump();

        set_access_control(res);
        return send(std::move(res));
    }

    void ContextImpl::File(const std::string &filename) {
        send_lambda send{_socket, _close, _ec};
        boost::beast::http::file_body::value_type body;
        boost::beast::error_code ec;
        body.open(filename.c_str(), boost::beast::file_mode::scan, ec);
        if (ec == boost::beast::errc::no_such_file_or_directory) {
            send(not_found(_version, _keep_alive)(filename));
        } else if (ec) {
            send(server_error(_version, _keep_alive)("Internal Server Error"));
        } else {
            const size_t size = body.size();
            boost::beast::http::response<boost::beast::http::file_body> res{
                    std::piecewise_construct,
                    std::make_tuple(std::move(body)),
                    std::make_tuple(boost::beast::http::status::ok, _version)};

            res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(boost::beast::http::field::content_type, mime_type(filename));
            res.content_length(size);
            res.keep_alive(_keep_alive);
            return send(std::move(res));
        }
    }

    void ContextImpl::set_body(boost::string_view &body) {

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

    Context::sptr Context::make(boost::asio::ip::tcp::socket &socket,
                                bool &close,
                                boost::system::error_code &ec) {
        return Context::sptr(new ContextImpl(socket, close, ec));
    }

}
