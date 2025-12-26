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

// Parameters
constexpr char const* channel = "channel";
constexpr char const* payload = "payload";
constexpr char const* uds = "/run/redis/redis-server.sock";
constexpr std::size_t pings = 5;
constexpr std::size_t sessions = 1000;
constexpr std::size_t repeat = 10000;

// Number of events expected 
constexpr auto expected_pushes = 1 + sessions * repeat;

// Number of responses expected 
constexpr auto expected_resps = sessions * pings;

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

asio::awaitable<void>
co_session(
   std::shared_ptr<connection> conn,
   std::shared_ptr<const request> req)
{
   for (auto i = 0u; i < repeat; ++i) {
      co_await conn->async_exec(*req);
   }
}

void rethrow_on_error(std::exception_ptr p)
{
   if (p) {
      std::rethrow_exception(p);
   }
}

std::shared_ptr<request>
make_reqs()
{
   auto req = std::make_shared<request>();
   for (std::size_t i = 0u; i < pings; ++i)
      req->push("PING");

   req->push("PUBLISH", channel, payload);
   return req;
}

asio::awaitable<void>
co_main() 
{
   auto ex = co_await asio::this_coro::executor;
   auto conn = std::make_shared<connection>(ex);

   config cfg;
   cfg.health_check_interval = std::chrono::seconds{0};
   cfg.unix_socket = uds;

   flat_tree resp;
   conn->set_receive_response(resp);
   conn->async_run(cfg, asio::consign(asio::detached, conn));

   request sub_req;
   sub_req.push("SUBSCRIBE", channel);
   co_await conn->async_exec(sub_req);

   auto const session_req = make_reqs();
   for (auto i = 0u; i < sessions; ++i)
      asio::co_spawn(ex, co_session(conn, session_req), rethrow_on_error);

   // The number of expected events
   auto n = 0u;
   while (n < expected_pushes) {
      co_await conn->async_receive2();
      auto const total_msgs = resp.get_total_msgs();
      n += total_msgs;
      resp.clear();
   }

   conn->cancel();
}

int main()
{
   try {
      std::cout
         << "Expects:\n"
         << "  - Responses: " << expected_resps << "\n"
         << "  - Pushes: " << expected_pushes << "\n"
         << "  - Total: " << (expected_resps + expected_pushes) << "\n"
         << std::endl
      ;

      asio::io_context ioc{BOOST_ASIO_CONCURRENCY_HINT_UNSAFE};
      asio::co_spawn(ioc, co_main(), rethrow_on_error);
      ioc.run();

   } catch (std::exception const& e) {
      std::cerr << e.what() << std::endl;
      return 1;
   }
}
