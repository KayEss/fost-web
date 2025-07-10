#include <fost/dynlib>
#include <fost/log>
#include <fost/main>
#include <fost/threading>
#include <fost/unicode>
#include <fost/urlhandler>

#include <future>


using namespace std::literals;


namespace {
    const fostlib::setting<fostlib::string>
            c_cwd("webserver.cpp", "webserver", "Change directory", "");
    const fostlib::setting<fostlib::string>
            c_host("webserver.cpp", "webserver", "Bind to", "localhost");
    const fostlib::setting<int>
            c_port("webserver.cpp", "webserver", "Port", 8001);
    const fostlib::setting<fostlib::string>
            c_mime("webserver.cpp",
                   "webserver",
                   "MIME types",
                   "Configuration/mime-types.json");
    const fostlib::setting<fostlib::json> c_load(
            "webserver.cpp", "webserver", "Load", fostlib::json::array_t());

    const fostlib::setting<fostlib::json> c_logger(
            "webserver.cpp", "webserver", "logging", fostlib::json(), true);
    // Take out the Fost logger configuration so we don't end up with both
    const fostlib::setting<fostlib::json> c_fost_logger(
            "webserver.cpp",
            "webserver",
            "Logging sinks",
            fostlib::json::parse("{\"sinks\":[]}"));

    bool simple_logging(fostlib::log::message const &m) {
        fostlib::simple_logger(m, 0, true, std::cout);
        return true;
    }
}


FSL_MAIN("webserver", "Threaded HTTP server")
(fostlib::ostream &o, fostlib::arguments &args) {
    auto const started = std::chrono::steady_clock::now();

    args.commandSwitch("C", c_cwd.section(), c_cwd.name());
    if (not c_cwd.value().empty()) {
        std::filesystem::current_path(
                fostlib::coerce<std::filesystem::path>(c_cwd.value()));
    }
    // Load the configuration files we've been given on the command line
    std::vector<std::pair<std::filesystem::path, std::future<fostlib::json>>>
            loads;
    loads.reserve(args.size());
    for (std::size_t arg{1}; arg != args.size(); ++arg) {
        o << "Loading config " << fostlib::json(args[arg].value());
        auto filename =
                fostlib::coerce<std::filesystem::path>(args[arg].value());
        loads.push_back(
                {filename,
                 std::async(
                         [](auto filename) {
                             return fostlib::json::sloppy_parse(
                                     fostlib::utf::load_file(filename));
                         },
                         filename)});
    }
    std::map<std::filesystem::path, fostlib::json> configs;
    while (loads.size()) {
        std::erase_if(loads, [&](auto &f) mutable {
            if (f.second.wait_for(10ms) == std::future_status::ready) {
                configs[f.first] = f.second.get();
                std::cout << "Loaded \"" << f.first << "\"\n";
                return true;
            } else {
                return false;
            }
        });
    }
    std::vector<fostlib::settings> configurations;
    for (std::size_t arg{1}; arg != args.size(); ++arg) {
        configurations.push_back(
                fostlib::settings{
                        args[arg].value(),
                        configs[fostlib::coerce<std::filesystem::path>(
                                args[arg].value())]});
    }

    // Load any shared objects
    fostlib::json so(c_load.value());
    std::vector<std::shared_ptr<fostlib::dynlib>> dynlibs;
    for (fostlib::json::const_iterator p(so.begin()); p != so.end(); ++p) {
        o << "Loading code plugin " << *p;
        dynlibs.push_back(
                std::make_shared<fostlib::dynlib>(
                        fostlib::coerce<fostlib::string>(*p)));
    }

    // Process command line arguments last
    args.commandSwitch("p", c_port.section(), c_port.name());
    args.commandSwitch("h", c_host.section(), c_host.name());
    args.commandSwitch("m", c_mime.section(), c_mime.name());

    // Set up the logging options
    fostlib::log::logging_function(&simple_logging);

    // Load MIME types
    fostlib::urlhandler::load_mime_configuration(c_mime.value());

    // Bind server to host and port
    fostlib::http::server server(fostlib::host(c_host.value()), c_port.value());

    auto const ended = std::chrono::steady_clock::now();
    o << "Loading took "
      << std::chrono::duration_cast<std::chrono::milliseconds>(ended - started)
                    .count()
      << "ms\nAnswering requests on http://" << server.binding() << ":"
      << server.port() << "/" << std::endl;

    // Service requests
    server(fostlib::urlhandler::service);

    // It will never get this far
    return 0;
}
