#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>
#include <fost/insert>
#include <fost/push_back>
#include <fost/unicode>


namespace {


    const class directory : public fostlib::urlhandler::view {
      public:
        directory(felspar::u8string name) : view{name} {}

        std::pair<std::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const {
            if (not configuration.has_key("root")) {
                throw fostlib::exceptions::not_implemented(
                        __PRETTY_FUNCTION__,
                        "Must specify a root folder when serving a directory");
            }
            std::filesystem::path root(
                    fostlib::coerce<std::filesystem::path>(
                            configuration["root"]));
            std::filesystem::path filename =
                    root / fostlib::coerce<std::filesystem::path>(path);
            if (std::filesystem::is_directory(filename)) {
                return serve_directory(
                        configuration, path, req, host, filename);
            } else {
                throw fostlib::exceptions::not_implemented(
                        __PRETTY_FUNCTION__,
                        "Told to serve a directory, but it turned out to be a "
                        "file");
            }
        }

        std::pair<std::shared_ptr<fostlib::mime>, int> serve_directory(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host,
                const std::filesystem::path &dirname) const {
            if (not static_cast<felspar::u8view>(req.file_spec())
                            .ends_with("/")) {
                fostlib::json redirect;
                fostlib::insert(redirect, "view", "fost.response.302");
                fostlib::insert(
                        redirect, "configuration", "location",
                        req.file_spec() + "/");
                return execute(redirect, path, req, host);
            }
            auto filename = dirname;
            if (configuration.has_key("index")) {
                filename /= fostlib::coerce<std::filesystem::path>(
                        configuration["index"]);
            } else {
                filename /= "index.html";
            }
            if (std::filesystem::exists(filename)) {
                return fostlib::urlhandler::serve_file(
                        configuration, req, filename);
            }
            /// There is no index file in this directory
            return listing(configuration, path, req, host, dirname);
        }

        virtual std::pair<std::shared_ptr<fostlib::mime>, int>
                listing(const fostlib::json &configuration,
                        const fostlib::string &path,
                        fostlib::http::server::request &req,
                        const fostlib::host &host,
                        const std::filesystem::path &) const {
            return execute(fostlib::json{"fost.response.403"}, path, req, host);
        }
    } c_directory{"fost.static.directory"};


    const class directory_json : public directory {
      public:
        directory_json() : directory("fost.static.directory.json") {}

        std::pair<std::shared_ptr<fostlib::mime>, int>
                listing(const fostlib::json &configuration,
                        const fostlib::string &path,
                        fostlib::http::server::request &req,
                        const fostlib::host &host,
                        const std::filesystem::path &directory) const {
            fostlib::json files = fostlib::json::object_t{};
            for (auto const &node :
                 std::filesystem::directory_iterator(directory)) {
                fostlib::json entry;
                if (std::filesystem::is_directory(node.path())) {
                    fostlib::insert(entry, "directory", true);
                    fostlib::insert(
                            entry, "path",
                            fostlib::coerce<fostlib::string>(
                                    node.path().filename())
                                    + "/");
                } else {
                    fostlib::insert(entry, "directory", false);
                    fostlib::insert(entry, "path", node.path().filename());
                }
                fostlib::insert(
                        files,
                        fostlib::coerce<fostlib::string>(node.path().filename()),
                        entry);
            }
            std::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                    fostlib::json::unparse(files, false),
                    fostlib::mime::mime_headers(), "application/json"));
            return std::make_pair(response, 200);
        }
    } c_directory_json;


    const class response_static : public fostlib::urlhandler::view {
      public:
        response_static() : view("fost.static") {}

        static bool allow_delete(const fostlib::json &conf) {
            return conf.has_key("verbs")
                    && fostlib::coerce<fostlib::nullable<bool>>(
                               conf["verbs"]["DELETE"])
                               .value_or(false);
        }

        std::pair<std::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const {
            std::filesystem::path root(
                    fostlib::coerce<std::filesystem::path>(
                            configuration["root"]));
            std::filesystem::path filename =
                    root / fostlib::coerce<std::filesystem::path>(path);
            if (std::filesystem::is_directory(filename)) {
                if (configuration.has_key("directory")) {
                    return execute(configuration["directory"], path, req, host);
                } else {
                    return c_directory.serve_directory(
                            configuration, path, req, host, filename);
                }
            } else {
                if (not std::filesystem::exists(filename)) {
                    return execute(
                            fostlib::json("fost.response.404"), path, req,
                            host);
                }
            }
            if (req.method() == "GET" || req.method() == "HEAD") {
                return fostlib::urlhandler::serve_file(
                        configuration, req, filename);
            } else if (allow_delete(configuration) && req.method() == "DELETE") {
                std::filesystem::remove(filename);
                std::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                        "<html><head><title>Resource deleted</title></head>"
                        "<body><h1>Resource deleted</h1></body></html>",
                        fostlib::mime::mime_headers(), "text/html"));
                return std::make_pair(response, 200);
            } else {
                fostlib::json c405;
                fostlib::insert(c405, "view", "fost.response.405");
                fostlib::push_back(c405, "configuration", "allow", "GET");
                fostlib::push_back(c405, "configuration", "allow", "HEAD");
                if (allow_delete(configuration))
                    fostlib::push_back(
                            c405, "configuration", "allow", "DELETE");
                return execute(c405, path, req, host);
            }
        }
    } c_response_static;


}
