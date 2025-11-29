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

asio::awaitable<void>
receiver(connection& conn)
{
   flat_tree resp;
   conn.set_receive_response(resp);

   for (;;) {
      co_await conn.async_receive2();
      resp.clear();
   }
}

asio::awaitable<void>
session(connection& conn, request const& req)
{
   for (;;)
      co_await conn.async_exec(req);
}

void rethrow_on_error(std::exception_ptr p)
{
   if (p) {
      std::rethrow_exception(p);
   }
}

request make_reqs(std::size_t pings, std::string const& msg)
{
   request req;
   for (std::size_t i = 0u; i < n_pubs; ++i)
      req.push("PING", i);

   req.push("PUBLISH", "channel", msg);
   return req;
}

asio::awaitable<void>
co_main() 
{
   // Setup
   asio::io_context ctx;
   connection conn{ctx};

   request const pub_req = make_reqs(10u, 100u);

   // TODO: Use a unix socket.
   conn.async_run(logger{logger::level::crit}, [](auto) { });

   // Task that consumes the pushes.
   net::co_spawn(ctx, receiver(conn), rethrow_on_error);

   request sub_req;
   sub_req.push("SUBSCRIBE", "channel");

   conn.async_exec(sub_req, ignore, [&](error_code ec, std::size_t) {
      // TODO: Check error.

      for (std::size_t i = 0u; i < 1u; ++i)
         net::co_spawn(ctx, session(conn, session_req), rethrow_on_error);
   });

   ctx.run();
}

int main()
{
   try {
      asio::io_context ioc;
      asio::co_spawn(ioc, co_main(), [](std::exception_ptr p) {
         if (p)
            std::rethrow_exception(p);
      });
      ioc.run();

   } catch (std::exception const& e) {
      std::cerr << e.what() << std::endl;
      return 1;
   }
}
