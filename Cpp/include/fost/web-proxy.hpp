/**
    Copyright 2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once


#include <fost/urlhandler.hpp>


namespace fostlib::web_proxy {


    /// ## `base`
    /**
        Abstract class that defines the basics of the different proxy types.
    */
    class base : public fostlib::urlhandler::view {
      protected:
        base(f5::u8view const n) : view(n) {}

        /// Customisation point for determining the new URL that is to be used
        virtual url upstream(
                json const &configuration,
                url const &base,
                string const &path,
                http::server::request &request,
                host const &host) const;

      private:
        std::pair<boost::shared_ptr<mime>, int> operator()(
                json const &configuration,
                string const &path,
                http::server::request &request,
                host const &host) const override;
    };


}
