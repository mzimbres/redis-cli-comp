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

constexpr char const* channel = "channel";

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
   // TODO: Handle error.
   for (;;)
      co_await conn->async_exec(*req);
}

void rethrow_on_error(std::exception_ptr p)
{
   if (p) {
      std::rethrow_exception(p);
   }
}

std::shared_ptr<request>
make_reqs(std::size_t pings, std::string const& msg)
{
   auto req = std::make_shared<request>();
   for (std::size_t i = 0u; i < pings; ++i)
      req->push("PING", i);

   req->push("PUBLISH", channel, msg);
   return req;
}

asio::awaitable<void>
co_main() 
{
   // Parameters
   auto const pings = 10u;
   auto const payload_size = 1000u;
   auto const sessions = 1000u;
   char const* uds = "/run/redis/redis-server.sock";

   auto ex = co_await asio::this_coro::executor;
   auto conn = std::make_shared<connection>(ex);
   config cfg;
   cfg.health_check_interval = std::chrono::seconds{0};
   cfg.unix_socket = uds;
   conn->async_run(cfg, asio::consign(asio::detached, conn));

   request sub_req;
   sub_req.push("SUBSCRIBE", channel);
   co_await conn->async_exec(sub_req);

   auto const session_req = make_reqs(pings, std::string(payload_size, 'a'));
   for (auto i = 0u; i < sessions; ++i)
      asio::co_spawn(ex, co_session(conn, session_req), rethrow_on_error);

   flat_tree resp;
   conn->set_receive_response(resp);

   for (;;) {
      co_await conn->async_receive2();
      resp.clear();
   }
}

int main()
{
   try {

      // TODO: Use concurrency hint for single threaded.
      asio::io_context ioc;
      asio::co_spawn(ioc, co_main(), rethrow_on_error);
      ioc.run();

   } catch (std::exception const& e) {
      std::cerr << e.what() << std::endl;
      return 1;
   }
}
