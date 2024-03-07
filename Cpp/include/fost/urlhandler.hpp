#ifndef FOST_URLHANDLER_HPP
#define FOST_URLHANDLER_HPP
#pragma once


#include <fost/internet>
#include <fost/http.server.hpp>


namespace fostlib {


    extern const module c_fost_web;
    extern const module c_fost_web_urlhandler;


    /// URL routing for requests
    namespace urlhandler {


        /// The host configuration
        FOST_URLHANDLER_DECLSPEC
        extern const fostlib::setting<fostlib::json> c_hosts;
        /// The view configuration
        FOST_URLHANDLER_DECLSPEC
        extern const fostlib::setting<fostlib::json> c_views;

        /// The prime routing for web sites
        FOST_URLHANDLER_DECLSPEC
        bool service(fostlib::http::server::request &req);

        /// Load MIME configuration data from the specified file
        FOST_URLHANDLER_DECLSPEC
        void load_mime_configuration(const fostlib::string &filename);

        /// Return the MIME type for the given file extension
        FOST_URLHANDLER_DECLSPEC
        fostlib::string mime_type(const std::filesystem::path &filename);

        /// A view class
        class FOST_URLHANDLER_DECLSPEC view {
          protected:
            /// The name of the configuration that the handler should tie to
            view(const fostlib::string &name);
            /// Allow sub-classing to work properly
            virtual ~view();

          public:
            /// Handle the request.
            virtual std::pair<std::shared_ptr<fostlib::mime>, int> operator()(
                    const fostlib::json &configuration,
                    const fostlib::string &path,
                    fostlib::http::server::request &request,
                    const fostlib::host &host) const = 0;

            /// Trace down the JSON for the view function and its configuration
            static std::pair<fostlib::string, fostlib::json> find_view(
                    const fostlib::string &view_name,
                    const fostlib::json &view_config = fostlib::json());

            /// Return the view that matches the provided name
            static const view &view_for(const fostlib::string &name);

            /// Execute a subview
            static std::pair<std::shared_ptr<fostlib::mime>, int>
                    execute(const fostlib::json &configuration,
                            const fostlib::string &path,
                            fostlib::http::server::request &request,
                            const fostlib::host &host);
        };

        /// Serves a file from disk providing etag, modified etc. and
        /// properly handling `If-None-Match` headers.
        std::pair<std::shared_ptr<fostlib::mime>, int> serve_file(
                const fostlib::json &configuration,
                fostlib::http::server::request &req,
                const std::filesystem::path &filename);


        /**
         * Helpers for responses that require particular handling
         */

        /// 405 Response builder
        template<typename... Args>
        inline std::pair<std::shared_ptr<fostlib::mime>, int> response_405(
                const fostlib::string &path,
                fostlib::http::server::request &request,
                const fostlib::host &host,
                Args... method) {
            fostlib::json::array_t methods;
            (methods.push_back(fostlib::coerce<fostlib::json>(method)), ...);
            fostlib::json::object_t config;
            config["allow"] = fostlib::json{std::move(methods)};
            fostlib::json::object_t view;
            view["view"] = fostlib::json{"fost.response.405"};
            view["configuration"] = fostlib::json{std::move(config)};
            return view::execute(view, path, request, host);
        }


    }


}


#endif // FOST_URLHANDLER_HPP
