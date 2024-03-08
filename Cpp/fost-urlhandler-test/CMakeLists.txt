if(TARGET stress)
    add_test_run(stress fost-urlhandler TESTS
            web-proxy.cpp
        )
endif()
