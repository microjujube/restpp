//
// Created by locate on 7/12/19.
//

#include "restpp/Utils.hpp"

namespace restpp {
    boost::beast::string_view mime_type(boost::beast::string_view path) {
        boost::beast::string_view result = "application/text";
        using boost::beast::iequals;
        auto const ext = [&path] {
            auto const pos = path.rfind(".");
            return pos == boost::beast::string_view::npos ? boost::beast::string_view{} : path.substr(pos);
        }();

        if (iequals(ext, ".html")) result = "text/html";
        else if (iequals(ext, ".htm")) result = "text/html";
        else if (iequals(ext, ".css")) result = "text/css";
        else if (iequals(ext, ".txt")) result = "text/plain";
        else if (iequals(ext, ".js")) result = "application/javascript";
        else if (iequals(ext, ".json")) result = "application/json";
        else if (iequals(ext, ".xml")) result = "application/xml";
        else if (iequals(ext, ".swf")) result = "application/x-shockwave-flash";
        else if (iequals(ext, ".flv")) result = "video/x-flv";
        else if (iequals(ext, ".png")) result = "image/png";
        else if (iequals(ext, ".jpe")) result = "image/jpeg";
        else if (iequals(ext, ".jpeg")) result = "image/jpeg";
        else if (iequals(ext, ".jpg")) result = "image/jpeg";
        else if (iequals(ext, ".gif")) result = "image/gif";
        else if (iequals(ext, ".bmp")) result = "image/bmp";
        else if (iequals(ext, ".ico")) result = "image/vnd.microsoft.icon";
        else if (iequals(ext, ".tiff")) result = "image/tiff";
        else if (iequals(ext, ".tif")) result = "image/tiff";
        else if (iequals(ext, ".svg")) result = "image/svg+xml";
        else if (iequals(ext, ".svgz")) result = "image/svg+xml";

        return result;
    }

}