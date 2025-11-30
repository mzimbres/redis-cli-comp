/* Copyright (c) 2018-2025 Marcelo Zimbres Silva (mzimbres@gmail.com)
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE.txt)
 */

#include <boost/redis/connection.hpp>
#include <boost/redis/logger.hpp>
#include <boost/redis/config.hpp>
#include <boost/redis/connection.hpp>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/consign.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/use_awaitable.hpp>

#include <iostream>

#include <boost/redis/src.hpp>

namespace asio = boost::asio;
using namespace std::chrono_literals;

using boost::system::error_code;
using boost::redis::request;
using boost::redis::resp3::flat_tree;
using boost::redis::ignore;
using boost::redis::ignore_t;
using boost::redis::logger;
using boost::redis::connection;
using boost::redis::usage;
using boost::redis::error;
using boost::redis::config;

void rethrow_on_error(std::exception_ptr p)
{
   if (p) {
      std::rethrow_exception(p);
   }
}

asio::awaitable<void>
co_main() 
{
   auto ex = co_await asio::this_coro::executor;
   auto conn = std::make_shared<connection>(ex);
   conn->async_run({}, asio::consign(asio::detached, conn));

   request monitor_req;
   monitor_req.push("MONITOR");
   co_await conn->async_exec(monitor_req);

   flat_tree resp;
   conn->set_receive_response(resp);

   int counter = 0;
   for (;; ++counter) {
      co_await conn->async_receive2();
      for (auto const& node: resp.get_view()) {
         std::cout
            << "     type: " << node.data_type << "\n"
            << "agg. size: " << node.aggregate_size << "\n"
            << "    depth: " << node.depth << "\n"
            << "    value: " << node.value << "\n"
            << std::endl;
         resp.clear();
      }
   }

   std::cout << "Number of events: " << counter << std::endl;
}

int main()
{
   try {

      asio::io_context ioc;
      asio::co_spawn(ioc, co_main(), rethrow_on_error);
      ioc.run();

   } catch (std::exception const& e) {
      std::cerr << e.what() << std::endl;
      return 1;
   }
}
