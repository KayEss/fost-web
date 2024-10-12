#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


namespace {


    class response : public fostlib::urlhandler::view {
        int status;
        felspar::u8string message;

      public:
        response()
        : view{"fost.response"},
          status{200},
          message{fostlib::http::server::status_text(200)} {}
        response(int s)
        : view{"fost.response." + std::to_string(s)},
          status{s},
          message{fostlib::http::server::status_text(s)} {}
        response(int s, felspar::u8string m) : response(s) { message = m; }

        std::shared_ptr<fostlib::mime> html(felspar::u8string msg) const {
            felspar::u8string resp_msg = "<html><head><title>" + msg
                    + "</title></head><body><h1>" + msg + "</h1></body></html>";
            std::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                    resp_msg, fostlib::mime::mime_headers(), "text/html"));
            return response;
        }

        std::shared_ptr<fostlib::mime> json(fostlib::json json) const {
            std::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                    fostlib::json::unparse(json, true),
                    fostlib::mime::mime_headers(), "application/json"));
            return response;
        }

        std::pair<std::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &config,
                const fostlib::string &,
                fostlib::http::server::request &,
                const fostlib::host &) const {
            int response_status{status};
            if (config.has_key("status")) {
                response_status = fostlib::coerce<int>(config["status"]);
            }
            if (config.has_key("message")) {
                return std::make_pair(
                        html(fostlib::coerce<felspar::u8string>(
                                config["message"])),
                        response_status);
            }
            if (config.has_key("json")) {
                return std::make_pair(json(config["json"]), response_status);
            }

            return std::make_pair(html(message), response_status);
        }
    };


    /**
     * ## List of generic responses
     */

    const response response_generic = response{};

    const response response_200 = response{200};
    const response response_202 = response{202};

    const response response_403 = response{403};
    const response response_404 = response{404, "Resource not found"};
    const response response_410 = response{410};
    const response response_412 = response{412};
    const response response_422 = response{422};

    const response response_500 = response{500};
    const response response_501 = response{501};
    const response response_503 = response{503};


}
