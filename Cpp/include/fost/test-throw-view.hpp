#pragma once


#include <fost/core>
#include <functional>


namespace fostlib::urlhandler {


    using test_throw_plugin_fn = std::function<void(fostlib::string)>;


    struct test_throw_plugin {
        test_throw_plugin(fostlib::string name, test_throw_plugin_fn);
        ~test_throw_plugin();

      private:
        string name;
    };


}
