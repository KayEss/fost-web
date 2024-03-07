#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


namespace {


    const class response_middleware : public fostlib::urlhandler::view {
      public:
        response_middleware() : view("fost.middleware.response") {}

        std::pair<std::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &h) const {
            auto response = execute(configuration, path, req, h);
            for (auto h = configuration["headers"].begin();
                 h != configuration["headers"].end(); ++h) {
                response.first->headers().set(
                        fostlib::coerce<fostlib::string>(h.key()), *h);
            }
            return response;
        }
    } c_middleware_request;


}
