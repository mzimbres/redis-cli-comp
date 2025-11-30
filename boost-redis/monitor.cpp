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
#include <boost/asio/signal_set.hpp>
#include <boost/asio/redirect_error.hpp>

#include <iostream>

namespace asio = boost::asio;
using namespace std::chrono_literals;

using boost::asio::signal_set;
using asio::redirect_error;
using boost::redis::connection;
using boost::redis::ignore;
using boost::redis::request;
using boost::redis::resp3::flat_tree;
using boost::system::error_code;
using boost::redis::config;

void rethrow_on_error(std::exception_ptr p)
{
   if (p) {
      std::rethrow_exception(p);
   }
}

asio::awaitable<void>
co_monitor(std::shared_ptr<connection> conn)
{
   flat_tree resp;
   conn->set_receive_response(resp);

   std::size_t events = 0;
   for (error_code ec;;) {
      co_await conn->async_receive2(redirect_error(ec));
      if (ec)
         break;

      events += resp.get_total_msgs();

      //for (auto const& node: resp.get_view()) {
      //   std::cout
      //      << "events: " << events << "\n"
      //      << "     type: " << node.data_type << "\n"
      //      << "agg. size: " << node.aggregate_size << "\n"
      //      << "    depth: " << node.depth << "\n"
      //      << "    value: " << node.value << "\n"
      //      << std::endl;
      //}

      resp.clear();
   }

   std::cout << "Events received: " << events << std::endl;
}

asio::awaitable<void>
co_main() 
{
   auto ex = co_await asio::this_coro::executor;
   auto conn = std::make_shared<connection>(ex);

   config cfg;
   cfg.health_check_interval = std::chrono::seconds{0};
   conn->async_run(cfg, asio::consign(asio::detached, conn));

   request monitor_req;
   monitor_req.push("MONITOR");
   co_await conn->async_exec(monitor_req);

   asio::co_spawn(ex, co_monitor(conn), asio::detached);

   signal_set sig_set(ex, SIGINT, SIGTERM);
   co_await sig_set.async_wait();
   conn->cancel();
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
