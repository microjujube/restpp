//
// Created by linwei on 2019-06-19.
//

#ifndef RESTPP_EXCEPTION_HPP
#define RESTPP_EXCEPTION_HPP

#include <exception>
#include <string>

namespace restpp {
    class Exception : public std::exception {
    public:
        enum ERRCODE {
            ERR_DEFAULT,
            ERR_VALUE_ERROR
        };

        Exception(ERRCODE code, std::string str)
                : _code(code), _str(std::move(str)) {

        };

        virtual const char *what() noexcept {
            return std::string(_str).c_str();
        }

    private:
        std::string _str;
        ERRCODE _code;
    };

    class KeyError : Exception {
    public:
        explicit KeyError(std::string str) : Exception(ERR_VALUE_ERROR, str) {

        }
    };


    class ValueError : Exception {
    public:
        explicit ValueError(std::string str) : Exception(ERR_VALUE_ERROR, str) {

        }
    };

}
#endif //RESTPP_EXCEPTION_HPP
