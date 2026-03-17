/* Copyright (c) 2018-2025 Marcelo Zimbres Silva (mzimbres@gmail.com)
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE.txt)
 */

#include <boost/redis/connection.hpp>
#include <boost/redis/config.hpp>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/consign.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/use_awaitable.hpp>

#include <iostream>

// Parameters
constexpr char const* channel = "channel";
constexpr char const* payload = "payload";
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
using boost::redis::connection;
using boost::redis::usage;
using boost::redis::config;

std::ostream& operator<<(std::ostream& os, usage const& usg)
{
   os << "Number of commands sent: "                     << usg.commands_sent << "\n"
      << "Number of bytes sent: "                        << usg.bytes_sent << "\n"
      << "Number of responses received: "                << usg.responses_received << "\n"
      << "Number of pushes received: "                   << usg.pushes_received << "\n"
      << "Number of response-bytes received: "           << usg.response_bytes_received << "\n"
      << "Number of push-bytes received: "               << usg.push_bytes_received << "\n"
      << "Number of bytes rotated in the read buffer: "  << usg.bytes_rotated << "\n"
   ;

   return os;
}

void rethrow_on_error(std::exception_ptr p)
{
   if (p) {
      std::rethrow_exception(p);
   }
}

asio::awaitable<void> co_main(config cfg)
{
   auto ex = co_await asio::this_coro::executor;
   auto conn = std::make_shared<connection>(ex);
   conn->async_run(cfg, asio::consign(asio::detached, conn));

   request sub_req;
   sub_req.push("SUBSCRIBE", channel);
   co_await conn->async_exec(sub_req);

   auto session_req = std::make_shared<request>();
   for (std::size_t i = 0u; i < pings; ++i)
      session_req->push("PING");

   session_req->push("PUBLISH", channel, payload);

   for (auto i = 0u; i < sessions; ++i) {
      auto const session = [conn, session_req]() -> asio::awaitable<void> {
         for (auto i = 0u; i < repeat; ++i) {
            co_await conn->async_exec(*session_req);
         }
      };
      asio::co_spawn(ex, session, rethrow_on_error);
   }

   while (conn->get_usage().pushes_received < expected_pushes) {
      co_await conn->async_receive2();
   }

   conn->cancel();
   std::cout << "Usage data\n" << conn->get_usage() << std::endl;
}

int main()
{
   try {
      config cfg;
      asio::io_context ioc{};
      asio::co_spawn(ioc, co_main(cfg), rethrow_on_error);
      ioc.run();

   } catch (std::exception const& e) {
      std::cerr << e.what() << std::endl;
      return 1;
   }
}
