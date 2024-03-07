#pragma once


#include <fost/urlhandler.hpp>


namespace fostlib::web_proxy {


    /// ## `base`
    /**
        Abstract class that defines the basics of the different proxy types.
    */
    class base : public fostlib::urlhandler::view {
      protected:
        base(felspar::u8view const n) : view(n) {}

        /// Customisation point for determining the new URL that is to be used
        virtual url upstream(
                json const &configuration,
                url const &base,
                string const &path,
                http::server::request const &request,
                host const &host) const;

        /// Return the request that is to be used by the user agent making
        /// talking to the upstream web server.
        virtual http::user_agent::request ua_request(
                json const &configuration,
                url location,
                string const &path,
                http::server::request &request,
                host const &host) const;

        /// All the proxy to alter any aspect of the response it wishes
        virtual std::pair<std::shared_ptr<mime>, int>
                respond(json const &configuration,
                        url const &location,
                        string const &path,
                        http::server::request const &request,
                        host const &host,
                        std::shared_ptr<mime> body,
                        int status) const;

      private:
        std::pair<std::shared_ptr<mime>, int> operator()(
                json const &configuration,
                string const &path,
                http::server::request &request,
                host const &host) const override;
    };


}
