#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


const class response_303 : public fostlib::urlhandler::view {
  public:
    response_303() : view("fost.response.303") {}

    std::pair<std::shared_ptr<fostlib::mime>, int> operator()(
            const fostlib::json &config,
            const fostlib::string &,
            fostlib::http::server::request &req,
            const fostlib::host &) const {
        if (!config.has_key("location"))
            throw fostlib::exceptions::not_implemented(
                    "Configuration 'location' must give a full URL to redirect "
                    "to");
        fostlib::string location =
                fostlib::coerce<fostlib::string>(config["location"]);
        if (location.empty())
            throw fostlib::exceptions::not_implemented(
                    "Configuration 'location' must give a full URL to redirect "
                    "to");
        fostlib::mime::mime_headers headers;
        headers.set("Location", location);
        std::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                "<html><head><title>See Other</title></head>"
                "<body><h1>See Other</h1></body></html>",
                headers, "text/html"));
        return std::make_pair(response, 303);
    }
} c_response_303;
