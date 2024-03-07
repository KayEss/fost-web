#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


const class response_405 : public fostlib::urlhandler::view {
  public:
    response_405() : view("fost.response.405") {}

    std::pair<std::shared_ptr<fostlib::mime>, int> operator()(
            const fostlib::json &config,
            const fostlib::string &,
            fostlib::http::server::request &req,
            const fostlib::host &) const {
        if (!config.has_key("allow"))
            throw fostlib::exceptions::not_implemented(
                    "No 'allow' key in the configuration so can't build the "
                    "required Allow header");
        fostlib::string allow;
        for (fostlib::json::const_iterator m(config["allow"].begin());
             m != config["allow"].end(); ++m)
            if (allow.empty())
                allow = fostlib::coerce<fostlib::string>(*m);
            else
                allow += ", " + fostlib::coerce<fostlib::string>(*m);
        fostlib::mime::mime_headers headers;
        headers.set("Allow", allow);
        std::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                "<html><head><title>Method Not Allowed</title></head>"
                "<body><h1>Method Not Allowed</h1></body></html>",
                headers, "text/html"));
        return std::make_pair(response, 405);
    }
} c_response_405;
