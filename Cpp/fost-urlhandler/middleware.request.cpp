#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


namespace {


    const class request_middleware : public fostlib::urlhandler::view {
      public:
        request_middleware() : view("fost.middleware.request") {}

        std::pair<std::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &h) const {
            for (auto h = configuration["headers"].begin();
                 h != configuration["headers"].end(); ++h) {
                req.headers().set(
                        fostlib::coerce<fostlib::string>(h.key()), *h);
            }
            return execute(configuration, path, req, h);
        }
    } c_middleware_request;


}
