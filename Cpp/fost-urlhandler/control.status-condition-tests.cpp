#include <fost/urlhandler>
#include <fost/test>


FSL_TEST_SUITE(control_status_condition);


FSL_TEST_FUNCTION(execute_then_if_less_than_400) {
    fostlib::json config;
    fostlib::insert(config, "view", "fost.control.status-condition");
    fostlib::insert(config, "configuration", "if", "fost.response.200");
    fostlib::insert(config, "configuration", "then", "fost.response.404");
    fostlib::http::server::request req;
    std::pair<std::shared_ptr<fostlib::mime>, int> response{
            fostlib::urlhandler::view::execute(
                    config, "/", req, fostlib::host())};
    FSL_CHECK_EQ(response.second, 404);
}

FSL_TEST_FUNCTION(return_if_default) {
    fostlib::json config;
    fostlib::insert(config, "view", "fost.control.status-condition");
    fostlib::insert(config, "configuration", "if", "fost.response.200");
    fostlib::http::server::request req;
    std::pair<std::shared_ptr<fostlib::mime>, int> response{
            fostlib::urlhandler::view::execute(
                    config, "/", req, fostlib::host())};
    FSL_CHECK_EQ(response.second, 200);
}


FSL_TEST_FUNCTION(return_specific_then) {
    fostlib::json config;
    fostlib::insert(config, "view", "fost.control.status-condition");
    fostlib::insert(config, "configuration", "if", "fost.response.200");
    fostlib::insert(config, "configuration", "then.200", "fost.response.404");
    fostlib::http::server::request req;
    std::pair<std::shared_ptr<fostlib::mime>, int> response{
            fostlib::urlhandler::view::execute(
                    config, "/", req, fostlib::host())};
    FSL_CHECK_EQ(response.second, 404);
}

FSL_TEST_FUNCTION(execute_else_if_more_than_400) {
    fostlib::json config;
    fostlib::insert(config, "view", "fost.control.status-condition");
    fostlib::insert(config, "configuration", "if", "fost.response.404");
    fostlib::insert(config, "configuration", "else", "fost.response.200");
    fostlib::http::server::request req;
    std::pair<std::shared_ptr<fostlib::mime>, int> response{
            fostlib::urlhandler::view::execute(
                    config, "/", req, fostlib::host())};
    FSL_CHECK_EQ(response.second, 200);
}

FSL_TEST_FUNCTION(return_response_if_more_than_400_without_else) {
    fostlib::json config;
    fostlib::insert(config, "view", "fost.control.status-condition");
    fostlib::insert(config, "configuration", "if", "fost.response.404");
    fostlib::http::server::request req;
    std::pair<std::shared_ptr<fostlib::mime>, int> response{
            fostlib::urlhandler::view::execute(
                    config, "/", req, fostlib::host())};
    FSL_CHECK_EQ(response.second, 404);
}
