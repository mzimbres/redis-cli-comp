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
#include <boost/asio/steady_timer.hpp>

#include <iostream>

// Parameters
constexpr char const* channel = "channel";
constexpr char const* payload = "payload";
constexpr std::size_t pings = 5;
constexpr std::size_t sessions = 1000;
constexpr std::size_t repeat = 30000;

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

//std::ostream& operator<<(std::ostream& os, usage const& u)
//{
//   os << "Commands sent: "           << u.commands_sent << "\n"
//      << "Bytes sent: "              << u.bytes_sent << "\n"
//      << "Responses received: "      << u.responses_received << "\n"
//      << "Pushes received: "         << u.pushes_received << "\n"
//      << "Response-bytes received: " << u.response_bytes_received << "\n"
//      << "Push-bytes received: "     << u.push_bytes_received << "\n"
//      << "Bytes rotated: "           << u.bytes_rotated << "\n"
//      << "Socket reads: "            << u.socket_reads << "\n"
//      << "Socket writes: "           << u.socket_writes << "\n"
//      << "Time reading: "            << std::chrono::duration_cast<std::chrono::milliseconds>(u.time_reading) << "\n"
//      << "Time writing: "            << std::chrono::duration_cast<std::chrono::milliseconds>(u.time_writing)
//   ;
//
//   return os;
//}

// Commands sent
// Bytes sent
// Responses received
// Pushes received
// Response-bytes received
// Push-bytes received
// Bytes rotated
// Socket reads
// Socket writes
// Time reading
// Time writing

std::ostream& operator<<(std::ostream& os, usage const& u)
{
   os << u.commands_sent << ":"
      << u.bytes_sent << ":"
      << u.responses_received << ":"
      << u.pushes_received << ":"
      << u.response_bytes_received << ":"
      << u.push_bytes_received << ":"
      << u.bytes_rotated << ":"
      << u.socket_reads << ":"
      << u.socket_writes << ":"
      << std::chrono::duration_cast<std::chrono::milliseconds>(u.time_reading) << ":"
      << std::chrono::duration_cast<std::chrono::milliseconds>(u.time_writing)
   ;

   return os;
}
void rethrow_on_error(std::exception_ptr p)
{
   if (p) {
      std::rethrow_exception(p);
   }
}

usage make_diff(usage const& a, usage const& b)
{
   usage ret;

   ret.commands_sent           =  b.commands_sent           - a.commands_sent;
   ret.bytes_sent              =  b.bytes_sent              - a.bytes_sent;
   ret.responses_received      =  b.responses_received      - a.responses_received;
   ret.pushes_received         =  b.pushes_received         - a.pushes_received;
   ret.response_bytes_received =  b.response_bytes_received - a.response_bytes_received;
   ret.push_bytes_received     =  b.push_bytes_received     - a.push_bytes_received;
   ret.bytes_rotated           =  b.bytes_rotated           - a.bytes_rotated;
   ret.socket_reads            =  b.socket_reads            - a.socket_reads;
   ret.socket_writes           =  b.socket_writes           - a.socket_writes;
   ret.time_reading            =  b.time_reading            - a.time_reading;
   ret.time_writing            =  b.time_writing            - a.time_writing;

   return ret;
}

asio::awaitable<void> log_usage(std::shared_ptr<connection> conn)
{
   auto ex = co_await asio::this_coro::executor;

   asio::steady_timer st{ex};

   for (;;) {
      st.expires_after(std::chrono::seconds{1});
      auto const before = conn->get_usage();
      co_await st.async_wait();
      auto const after = conn->get_usage();
      auto const diff = make_diff(before, after);
      std::cout << diff << std::endl;
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

   asio::co_spawn(ex, log_usage(conn), rethrow_on_error);
   while (conn->get_usage().pushes_received < expected_pushes) {
      co_await conn->async_receive2();
   }

   conn->cancel();
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
