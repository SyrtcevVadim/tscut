//
// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.hpp"
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>
#include <string>

#include "logger.hpp"

int main(int argc, char* argv[]) {
    try {
        // Check command line arguments.
        if (argc != 4) {
            std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
            std::cerr << "  For IPv4, try:\n";
            std::cerr << "    receiver 0.0.0.0 80 .\n";
            std::cerr << "  For IPv6, try:\n";
            std::cerr << "    receiver 0::0 80 .\n";
            return 1;
        }

        // Initialise the server.
        http::server::server s(argv[1], argv[2], argv[3]);
        Logger::get_instance().info("Server has been started at address {}:{} with document root: {}", argv[1], argv[2], argv[3]);
        
        // Run the server until stopped.
        s.run();
        
    } catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << "\n";
    }

    Logger::get_instance().info("Server has been stopped");
    return 0;
}
