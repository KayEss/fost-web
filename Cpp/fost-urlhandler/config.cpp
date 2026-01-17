#include "fost-urlhandler.hpp"
#include <fost/urlhandler.hpp>


const fostlib::setting<fostlib::json> fostlib::urlhandler::c_hosts(
        "urlhandler/routing.cpp",
        "webserver",
        "hosts",
        fostlib::json::object_t(),
        true);
const fostlib::setting<fostlib::json> fostlib::urlhandler::c_views(
        "urlhandler/routing.cpp",
        "webserver",
        "views",
        fostlib::json::object_t(),
        true);
